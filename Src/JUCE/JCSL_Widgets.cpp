//
//  JCSL_Widgets.cpp - JUCE widgets for audio by stp@heaveneverywhere.com
//	What's here:
//		AudioWaveformDisplay - simple oscilloscope based on the JUCE AudioDemo
//		AudioSpectrumDisplay - simple spectrogram
//		RangeSlider - interprets its two markers as base value and range, rather than min/max
//			i.e., the upper marker is the base value, and the lower one is the random range
//		VUMeter (based on killerbobjr's MeterComponent)

#include "JCSL_Widgets.h"
#include <math.h>

#define csl_max(a, b)	(((a) > (b)) ? (a) : (b))
#define csl_min(a, b)	(((a) < (b)) ? (a) : (b))

// #include "AnalogMeterImages.h"		// only needed for fancy analog meters

// oscilloscope class

AudioWaveformDisplay::AudioWaveformDisplay() {
	bufferPos = 0;
	bufferSize = CSL_mMaxBufferFrames;			// allocate buffer
	circularBuffer = (float*) calloc (1, sizeof (float) * bufferSize);
	currentOutputLevel = 0.0f;
	numSamplesIn = 0;
	setOpaque (true);
	whichChannel = -1;
	indicatorValue = 0;
}

AudioWaveformDisplay::~AudioWaveformDisplay() {
	free (circularBuffer);
}

// set up oscilloscope state (since constructor takes no args)

void AudioWaveformDisplay::initialise(int channel, unsigned rate, unsigned window, bool zeroX) {
	if (rate)
		delayInMsec = 1000/rate;		// refresh rate
	else
		delayInMsec = 0;
	samplesToAverage = window;			// avg range
	bufferSize = CSL_mMaxBufferFrames / 4;  // * window;			// allocate buffer
	circularBuffer = (float*) calloc (1, sizeof (float) * bufferSize);
	zeroCross = zeroX;					// whether to trigger on zero-X
	whichChannel = channel;
//	startTimer(delayInMsec);			// repaint every so-many msec
}

// start/stop display

void AudioWaveformDisplay::start() {
	startTimer(delayInMsec);
}

void AudioWaveformDisplay::stop() {
	stopTimer();
}

// main scope draw method

void AudioWaveformDisplay::paint (juce::Graphics& g) {
	g.fillAll (juce::Colours::black);
	g.setColour (juce::Colours::lightgreen);
	
	const float halfHeight = getHeight() * 0.5f;
	unsigned wid = getWidth();
									//this is handled in the add_samples call-back now
//	if ( ! samplesToAverage) {			// if no interpolation, subsample the samples
//		float skip = 1; //(float) samplesToAverage / (float) wid;
//		for (unsigned x = 0; x < wid; x++) {		// drawing loop
//			float index = (float) x * skip;
//			const float level = circularBuffer[(unsigned) index];
////			g.setPixel(x, (int) (halfHeight - (halfHeight * level)));
//			g.drawLine ((float) x, halfHeight,
//						(float) x, halfHeight + (halfHeight * level));
//		}
//		return;
//	}
	int x0;
	if (bufferPos > wid) 
		x0 = bufferPos - wid;
	else
		x0 = bufferSize - wid;

	if (zeroCross) {							// loop to find zeroX earlier than wid ago
		float y0, y1;
		y0 = circularBuffer [x0];
		while (x0-- > 0) {
			y1 = y0;
			y0 = circularBuffer[x0];
			if ((y0 >= 0.0) && (y1 < 0.0))		// if zero-crossing
				break;
		}
	}
	x0++;
	for (unsigned x = 0; x < wid; x++) {		// drawing loop
		if ((x0 + x) >= bufferSize)
			x0 = 0 - x;
		const float level = circularBuffer[x0 + x];
#ifndef WIN32
		if ( ! std::isnormal(level)) continue;
#endif
		//		g.setPixel(x, (int) (halfHeight - (halfHeight * level)));
		g.drawLine ((float) x, halfHeight,
                            (float) x, halfHeight + (halfHeight * level));
	}
//	bufferPos = 0;			// reset buffer writing pos
}

// timer just redraws

void AudioWaveformDisplay::timerCallback() {
	repaint();
}

// adding a sample accumulates values

void AudioWaveformDisplay::addSample (const float sample) {
	currentOutputLevel += sample;					// rather than fabs as in the other scope version
	if (++numSamplesIn >= samplesToAverage) {		// if at end of mini-window
		circularBuffer[bufferPos++] = currentOutputLevel / samplesToAverage;
		numSamplesIn = 0;
		currentOutputLevel = 0.0f;
	}
	if (bufferPos >= bufferSize)
		bufferPos = 0;
}

// callaback accumulates samples

void AudioWaveformDisplay::audioDeviceIOCallback (const float** inputChannelData, int totalNumInputChannels,
							float** outputChannelData, int totalNumOutputChannels, int numSamples) {
	if ( ! samplesToAverage) {			// if no interpolation, just hold onto the pointer
		circularBuffer = outputChannelData[0];
		numSamplesIn = numSamples;
		return;
	}
	if (whichChannel == -1) {			// merge all channels
		for (int i = 0; i < totalNumOutputChannels; ++i) {
			if (outputChannelData[i] != 0) {
				for (int j = 0; j < numSamples; ++j)
					addSample (outputChannelData[i][j]);
				break;
			}
		}
	} else {				// single channel
		for (int j = 0; j < numSamples; ++j)
			addSample (outputChannelData[whichChannel][j]);
	}
}

void AudioWaveformDisplay::audioDeviceAboutToStart (double sampleRate, int numSamplesPerBlock) {
	juce::zeromem (circularBuffer, sizeof (float) * bufferSize);
}

void AudioWaveformDisplay::audioDeviceStopped() {
	juce::zeromem (circularBuffer, sizeof (float) * bufferSize);
}

///////////////////////////////////////////////////////////////////////
//
// AudioSpectrumDisplay methods
//

AudioSpectrumDisplay::AudioSpectrumDisplay() : AudioWaveformDisplay() {
	bufferSize = 512;				// # windows to buffer
	spectrumBuffer = (float **) calloc (1, sizeof (float *) * bufferSize);
	bufferPos = 0;
	mLogDisplay = false;
	mSpectroDisplay = true;			// open in spectrogram display
	numWindows = bufferSize;
}

AudioSpectrumDisplay::~AudioSpectrumDisplay() {
	free (spectrumBuffer);
}

// callaback copies *mag* samples in spectrumBuffer
// outputChannelData is the complex spectrum, totalNumOutputChannels the index and 
// numSamples the buffer size
// FFTW half-complex format = r0, r1, r2, ..., rn/2, i(n+1)/2-1, ..., i2, i1

void AudioSpectrumDisplay::audioDeviceIOCallback (const float** inputChannelData, int totalNumInputChannels,
							float** outputChannelData, int totalNumOutputChannels, int numSamples) {
	float realV, imagV, pixV;

	bufferSize = numSamples;
	if ( ! spectrumBuffer[bufferPos])
		spectrumBuffer[bufferPos] = (float *) malloc(numSamples / 2 * sizeof(float));
	
	for (unsigned x = 0; x < numSamples / 2; x++) {		// calc/norm loop
		realV = outputChannelData[0][x + 1];
		imagV = outputChannelData[0][numSamples - (x + 1)];
		pixV = hypotf(realV, imagV) * 10.0;				// * 10 for scaling
		spectrumBuffer[bufferPos][x] = pixV;
	}
	bufferPos++;
	if (bufferPos == bufferSize)
		bufferPos = 0;
}

// spectrum draw method - draw a full 3D color spectrogram (if mSpectroDisplay)
// or just a single spectral slice

#define SetPixel(g, x, y)     g.fillRect(x, y, 1, 1);

void AudioSpectrumDisplay::paint (juce::Graphics& g) {
	float realV, maxV;
	g.fillAll (juce::Colours::black);

	unsigned hite = getHeight();
	unsigned wid = getWidth();
//	printf("Paint %d @ %d\n", wid, hite);
	
	if (mSpectroDisplay) {		// if spectrogram (vs spectral slice)
		float xScale = (float) wid / numWindows;
//		printf("Paint %d @ %d (%g)\n", wid, hite, xScale);
		maxV = 0.0f;	
		for (unsigned x = 0; x < wid; x++) {			// window loop
			if (x >= numWindows)
				continue;
			if ( ! spectrumBuffer[x])
				continue;
			for (unsigned y = 0; y < hite; y++) {			// log loop
				realV = logf(spectrumBuffer[x][y]);
				if (realV > maxV)
					maxV = realV;
				circularBuffer[y] = realV;
			}
			for (unsigned y = 0; y < hite; y++) {			// frame loop
				g.setColour (juce::Colour(circularBuffer[y] / maxV, 1.0f, 1.0f, 1.0f));	// HSVA
				unsigned yVal = hite - y;
				unsigned xVal = x * xScale;
				for (unsigned i = 0; i < xScale; i++) {		// draw loop
					SetPixel(g, xVal+i, yVal);
				}
			}
		}
	} else {							//spectral slices
		float minV, max2;
		g.setColour (juce::Colours::lightgreen);
		unsigned howWide = csl_min(wid, (bufferSize / 2));
		maxV = 0.0f;
		for (unsigned x = 0; x < howWide; x++) {			// calc/norm loop
			realV = spectrumBuffer[indicatorValue][x];
			circularBuffer[x] = realV;
			if (realV > maxV)
				maxV = realV;
		}
		if (maxV == 0.0f)
			return;
//		printf("\tmax = %5.3f\n", maxV);

		if (mLogDisplay) {
			max2 = 0.0;
			minV = 0.0;
			for (unsigned x = 0; x < howWide; x++) {		// log loop
				realV = logf(circularBuffer[x]);
				if (realV > max2)
					max2 = realV;
				if (realV < minV)
					minV = realV;
				circularBuffer[x] = realV;
			}
//			printf("\t\tmin = %5.3f, max = %5.3f\n", minV, max2);
			maxV = max2;
		}
		howWide = csl_min(wid, bufferSize);
		for (unsigned x = 0; x < howWide; x += 2) {		// drawing loop
			realV = hite - (hite * circularBuffer[x / 2] / maxV);
			g.drawLine ((float) x, hite, (float) x, realV);
			g.drawLine ((float) x+1, hite, (float) x+1, realV);
		}
	}
}

#ifdef RANGE_SLIDER

///////////////////////////////////////////////////////////////////////
//
// RangeSlider methods

// Answer the base value

double RangeSlider::getBaseValue() {
    return baseValue;
}

// answer the range (normalized to 0 - 1 -- not certain if there's a single way to do this, or if we need another state flag

double RangeSlider::getRangeValue() {
	return (rangeValue - minimum) / (maximum - minimum);
}

void RangeSlider::mouseDown (const MouseEvent& e) {
	float mousePos;
	float maxPosDistance;
	float minPosDistance;
    mouseWasHidden = false;
    incDecDragged = false;
	
    if (isEnabled()) {
		menuShown = false;
		if (valueBox != 0)
			valueBox->hideEditor (true);
		sliderBeingDragged = 0;
		mousePos = (float) (isVertical() ? e.y : e.x);
		maxPosDistance = fabsf (getLinearSliderPos (getMaxValue()) - 0.1f - mousePos);
		minPosDistance = fabsf (getLinearSliderPos (getMinValue()) + 0.1f - mousePos);
		if (maxPosDistance <= minPosDistance)
			sliderBeingDragged = 2;
		else
			sliderBeingDragged = 1;
	}
	minMaxDiff = getMaxValue() - getMinValue();
	mouseXWhenLastDragged = e.x;
	mouseYWhenLastDragged = e.y;
	if (sliderBeingDragged == 2)
		valueWhenLastDragged = getValue();
	else if (sliderBeingDragged == 1)
		valueWhenLastDragged = baseValue;
	valueOnMouseDown = valueWhenLastDragged;            
	
	sendDragStart();
	mouseDrag (e);
}

void RangeSlider::mouseDrag (const MouseEvent& e) {
    if (isEnabled() && ( ! menuShown)) {
		const int mousePos = (isHorizontal() || style == RotaryHorizontalDrag) ? e.x : e.y;
		double scaledMousePos = (mousePos - sliderRegionStart) / (double) sliderRegionSize;
		if (style == LinearVertical)
			scaledMousePos = 1.0 - scaledMousePos;
		valueWhenLastDragged = proportionOfLengthToValue (jlimit (0.0, 1.0, scaledMousePos));
		mouseXWhenLastDragged = e.x;
        mouseYWhenLastDragged = e.y;
        valueWhenLastDragged = jlimit (minimum, maximum, valueWhenLastDragged);
        if (sliderBeingDragged == 1)
            setBaseValue(valueWhenLastDragged, ! sendChangeOnlyOnRelease, false);
        else if (sliderBeingDragged == 2)
            setRangeValue(valueWhenLastDragged, ! sendChangeOnlyOnRelease, false);
        mouseXWhenLastDragged = e.x;
        mouseYWhenLastDragged = e.y;
    }
}

void RangeSlider::setBaseValue (double newValue, const bool sendUpdateMessage, const bool sendMessageSynchronously) {
//	newValue = jmin (valueMax, newValue);
//	newValue = jmax (valueMin, newValue);
//	printf("B: %5.3f\n", newValue);
    if (baseValue != newValue) {
        baseValue = newValue;
        valueMin = newValue;
        currentValue = newValue;
        repaint();
        if (sendUpdateMessage)
            triggerChangeMessage (sendMessageSynchronously);
    }
}

void RangeSlider::setRangeValue (double newValue, const bool sendUpdateMessage, const bool sendMessageSynchronously) {
//	newValue = jmin (valueMax, newValue);
//	newValue = jmax (valueMin, newValue);
//	printf("R: %5.3f\n", newValue);
    if (rangeValue != newValue) {
        rangeValue = newValue;
        valueMax = newValue;
        repaint();
        if (sendUpdateMessage)
            triggerChangeMessage (sendMessageSynchronously);
    }
//	printf("R: %5.3f\n", getRangeValue());
}

#endif

///////////////////////////////////////////////////////////////////////
//
// VUMeter methods

// Default constructor = 32-segment vertical VU meter

VUMeter::VUMeter() :
			Component("VU Meter"),
			m_img(0),
			m_value(0), 
			m_skew(1.0f),
			m_threshold(0.707f),
			m_meterType(VUMeter::Vertical),
			m_segments(32),
			m_markerWidth(2),
			m_inset(0),			
			m_channel(0),
			m_raised(false),
			m_minColour(juce::Colours::green),
			m_thresholdColour(juce::Colours::yellow),
			m_maxColour(juce::Colours::red),
			m_backgroundColour(juce::Colours::black),
			m_decayTime(33),
			m_decayPercent(0.707f),
			m_decayToValue(0),
			m_hold(2),
			m_monostable(0),
			m_background(0),
			m_overlay(0),
			m_minPosition(0.1f),
			m_maxPosition(0.9f),
			m_needleLength(0),
			m_needleWidth(2) {
	startTimer(m_decayTime);
}

VUMeter::VUMeter(int channel,
				int type, 
				int segs, 
				int markerWidth, 
				const juce::Colour& min,
				const juce::Colour& thresh,
				const juce::Colour& max,
				const juce::Colour& back,
				float threshold) : 
    Component("VU Meter"),
	m_img(0),
    m_value(0), 
    m_skew(1.0f),
    m_threshold(threshold),
    m_meterType(type),
    m_segments(segs),
    m_markerWidth(markerWidth),
    m_inset(0), 
    m_channel(channel),
    m_raised(false),
    m_minColour(min), 
    m_thresholdColour(thresh), 
    m_maxColour(max), 
    m_backgroundColour(back), 
    m_decayTime(50),
    m_decayPercent(0.707f),
    m_decayToValue(0),
    m_hold(4),
    m_monostable(0),
    m_background(0),
    m_overlay(0),
    m_minPosition(0.1f),
    m_maxPosition(0.9f),
	m_needleLength(0),
    m_needleWidth(markerWidth)
{
    if (m_meterType == Analog)
        if ( ! m_segments)		// Minimum analog meter wiper width
		    m_segments = 8;
    startTimer(m_decayTime);
}

VUMeter::VUMeter(int channel,
				juce::Image* background,
				juce::Image* overlay,
				float minPosition, 
				float maxPosition, 
				juce::Point<int>& needleCenter,
				int needleLength,
				int needleWidth,
				int arrowLength,
				int arrowWidth, 
				const juce::Colour& needleColour,
				int needleDropShadow, 
				int dropDistance) :
    Component("Meter Component"),
    m_img(0),
    m_value(0), 
    m_skew(1.0f),
    m_threshold(0.707f),
    m_meterType(Analog),
    m_segments(0),
    m_markerWidth(0),
    m_inset(0), 
    m_channel(channel),
    m_raised(false),
    m_minColour(0), 
    m_thresholdColour(0), 
    m_maxColour(0), 
    m_backgroundColour(0), 
    m_decayTime(50),
    m_decayPercent(0.707f),
    m_decayToValue(0),
    m_hold(4),
    m_monostable(0),
    m_background(0),
    m_overlay(0),
    m_minPosition(minPosition),
    m_maxPosition(maxPosition),
	m_needleLength(needleLength),
    m_needleWidth(needleWidth),
    m_arrowLength(arrowLength),
    m_arrowWidth(arrowWidth),
	m_needleDropShadow(needleDropShadow),
	m_dropDistance(dropDistance)
{
    m_background = background;
    m_overlay = overlay;
	m_needleCenter.setXY(needleCenter.getX(), needleCenter.getY());
	m_needleColour = needleColour;

    startTimer(m_decayTime);
}

VUMeter::~VUMeter() {    
	stopTimer(); 
    deleteAndZero(m_img);
    deleteAndZero(m_background);
    deleteAndZero(m_overlay);
}

void VUMeter::buildImage(void) {
    deleteAndZero(m_img);

    // Build our image in memory
    int w = getWidth() - m_inset*2;
    int h = getHeight() - m_inset*2;
	if ((w == 0) || (h == 0))
		return;
    if (m_meterType == Horizontal) {
		m_img = new juce::Image(juce::Image::RGB, w, h, false);
		juce::Graphics g(*m_img);

		juce::ColourGradient brLower(m_minColour, 0, 0, m_thresholdColour, w * m_threshold, 0, false);
        g.setGradientFill(brLower);
        g.fillRect(0, 0, int(w * m_threshold), h);

        juce::ColourGradient brUpper(m_thresholdColour, int(w * m_threshold), 0, m_maxColour, w, 0, false);
        g.setGradientFill(brUpper);
        g.fillRect(int(w * m_threshold), 0, w, h);

        if (m_segments)
        {
            // Break up our image into segments
            g.setColour(m_backgroundColour);
            g.fillRect (0, 0, w, m_markerWidth);
            g.fillRect (0, h-m_markerWidth, w, m_markerWidth);
            for (int i = 0; i <= m_segments; i++)
                g.fillRect(i * (w/m_segments), 0, m_markerWidth, h);
        }
    } else if (m_meterType == Vertical) {
		m_img = new juce::Image(juce::Image::RGB, w, h, false);
		juce::Graphics g(*m_img);

        int hSize = (int) (h * m_threshold);
        juce::ColourGradient brLower(m_minColour, 0, h, m_thresholdColour, 0, h - hSize, false);
        g.setGradientFill(brLower);
        g.fillRect(0, h - hSize, w, hSize);

        juce::ColourGradient brUpper(m_thresholdColour, 0, h - hSize, m_maxColour, 0, 0, false);
        g.setGradientFill(brUpper);
        g.fillRect(0, 0, w, h - hSize);

        if (m_segments) {
            // Break up our image into segments
            g.setColour(m_backgroundColour);
            g.fillRect (0, 0, m_markerWidth, h);
            g.fillRect (w-m_markerWidth, 0, m_markerWidth, h);
            for (int i = 0; i <= m_segments; i++)
                g.fillRect(0, i * (h/m_segments), w, m_markerWidth);
        }
    }
	// Only build if no other analog images exist
    else if (m_meterType == Analog && !(m_background || m_overlay || m_needleLength)) {
		m_img = new juce::Image(juce::Image::RGB, w, h, false);
		juce::Graphics g(*m_img);

        g.setColour(m_backgroundColour);
        g.fillRect(0, 0, w, h);

        const double left = 4.71238898;     // 270 degrees in radians
        const double right = 1.57079633;    // 90 degrees in radians
        double startPos = m_minPosition;    // Start at 10%
        double endPos = m_maxPosition;      // End at 90%

        float strokeWidth = m_segments;     // We get our wiper width from the segments amount
        double pos;
        float radius = csl_max (w/2, h/2) - strokeWidth/2;
        float angle;
        float x;
        float y;

        // Create an arc with lineTo's (works better than addArc)
        juce::Path p;
        for (pos = startPos; pos < endPos; pos += .02) {
            angle = left + pos * (right - left);
            x = sin(angle)*radius + w/2;
            y = cos(angle)*radius + h - m_segments;
            if (pos == startPos)
                p.startNewSubPath(x, y);
            else
                p.lineTo(x, y);
        }
        angle = left + pos * (right - left);
        p.lineTo(sin(angle)*radius + w/2, cos(angle)*radius + h - m_segments);

        // Create an image brush of our gradient
        juce::Image img(juce::Image::RGB, w, h, false);
        juce::Graphics g2(img);

        juce::ColourGradient brLower(m_minColour, 0, 0, m_thresholdColour, w * m_threshold, 0, false);
        g2.setGradientFill(brLower);
        g2.fillRect(0, 0, int(w * m_threshold), h);

        juce::ColourGradient brUpper(m_thresholdColour, int(w * m_threshold), 0, m_maxColour, w, 0, false);
        g2.setGradientFill(brUpper);
        g2.fillRect(int(w * m_threshold), 0, w, h);

        g.setTiledImageFill(img, 0, 0, 1.0);

        // Stroke the arc with the gradient
        g.strokePath(p, juce::PathStrokeType(strokeWidth));
    }
}

void VUMeter::setBounds (int x, int y, int width, int height) { 
	Component::setBounds(x, y, width, height); 
	buildImage(); 
}

void VUMeter::setFrame(int inset, bool raised) { 
	m_inset=inset; 
	m_raised=raised; 
	buildImage(); 
}

void VUMeter::setColours(juce::Colour& min, juce::Colour& threshold, juce::Colour& max, juce::Colour& back) {
	m_minColour = min; 
	m_thresholdColour = threshold; 
	m_maxColour = max; 
	m_backgroundColour = back; 
	buildImage(); 
}

void VUMeter::setValue(float v) { 
    float val = csl_min(csl_max(v, 0.0f), 1.0f);
    if (m_skew != 1.0 && val > 0.0)
        val = exp (log (val) / m_skew);

    if (m_decayTime) {
        if (m_value < val) {
            // Sample and hold
            m_monostable=m_hold;
            m_value = val;
//          repaint();
        }
        m_decayToValue = val;
    } else {
        if (m_value != val) {
			m_value = val;
//			repaint();
		}
    }
}

void VUMeter::setDecay(int decay, int hold, float percent) {
    m_decayPercent = percent;
    m_decayTime = decay;
    m_hold = hold;
    if (m_decayTime)
        // Start our decay
        startTimer(m_decayTime); 
    else
        stopTimer();
}

void VUMeter::timerCallback() {
    if (m_monostable)			// Wait for our hold period
        m_monostable--;
    else {
        m_value = m_decayToValue + (m_decayPercent * (m_value - m_decayToValue));
        if (m_value - m_decayToValue > 0.01f)
						// Only repaint if there's enough changes
            repaint();
		else if (m_value != m_decayToValue) {
						// Zero out
			m_value = m_decayToValue;
			repaint();
		}
    }
}

//	This needs to be made more efficient by only repainting when things actually
//	change, and only painting the area that has changed. Right now, this paints
//	everything on every repaint request.
//
void VUMeter::paint (juce::Graphics& g) {
    int h = getHeight() - m_inset*2;
    int w = getWidth() - m_inset*2;

    // Background
	if ( ! m_background && !m_needleLength) {
		g.setColour(m_backgroundColour);
		g.fillRect(m_inset, m_inset, w, h);
	}

    // Bevel outline for the entire draw area
	if (m_inset)
        juce::LookAndFeel_V1::drawBevel(g, 0, 0, getWidth(), getHeight(),
			m_inset, 
			m_raised ? juce::Colours::white.withAlpha(0.9f) : juce::Colours::black.withAlpha(0.9f),
			m_raised ? juce::Colours::black.withAlpha(0.9f) : juce::Colours::white.withAlpha(0.9f));

    // Blit our prebuilt image
    g.setOpacity(1.0f);
    if (m_meterType == Horizontal) {
        if (m_segments) {
            float val = float(int(m_value * m_segments)) / m_segments;
            g.drawImage(*m_img, m_inset, m_inset, w * val, h, 0, 0, w * val, h);
        } else
            g.drawImage(*m_img, m_inset, m_inset, w * m_value, h, 0, 0, w * m_value, h);
    } else if (m_meterType == Vertical) {
		int hSize;
        if (m_segments)
            hSize = h * float(int(m_value * m_segments)) / m_segments;
        else
            hSize = h * m_value;
        g.drawImage(*m_img, m_inset, m_inset + h - hSize, w, hSize, 0, h - hSize, m_img->getWidth(), hSize);
    } else if (m_meterType == Analog) {
		if (m_background)
			g.drawImage(*m_background, m_inset, m_inset, w, h, 0, 0, m_background->getWidth(), m_background->getHeight());
		else if (m_img)
			g.drawImage(*m_img, m_inset, m_inset, w, h, 0, 0, m_img->getWidth(), m_img->getHeight());
    
        float angle = 4.71238898 + (m_value * (m_maxPosition - m_minPosition) + m_minPosition) * -3.14159265;
		if (m_needleLength) {
			g.setColour(m_needleColour);
//			g.drawArrow(
//				m_needleCenter.getX() + m_inset, 
//				m_needleCenter.getY() + m_inset, 
//				sin(angle)*m_needleLength + m_needleCenter.getX() + m_inset, 
//				cos(angle)*m_needleLength + m_needleCenter.getY() + m_inset, 
//				m_needleWidth, 
//				m_arrowLength, 
//				m_arrowWidth);

			if (m_needleDropShadow) {
				int dropX, dropY, dropPointX, dropPointY;
				if (m_needleDropShadow == Left) {
					dropX = -m_dropDistance;
					dropY = 0;
					dropPointX = -m_dropDistance;
					dropPointY = 0;
				} else if (m_needleDropShadow == Right) {
					dropX = m_dropDistance;
					dropY = 0;
					dropPointX = m_dropDistance;
					dropPointY = 0;
				} else if (m_needleDropShadow == Above) {
					dropX = 0;
					dropY = 0;
					dropPointX = 0;
					dropPointY = -m_dropDistance;
				} else if (m_needleDropShadow == Below) {
					dropX = 0;
					dropY = m_dropDistance;
					dropPointX = 0;
					dropPointY = m_dropDistance;
				}
                g.setColour(juce::Colours::black.withAlpha(0.2f));
//				g.drawArrow(
//					m_needleCenter.getX() + m_inset + dropX, 
//					m_needleCenter.getY() + m_inset + dropY, 
//					sin(angle)*m_needleLength + m_needleCenter.getX() + m_inset + dropPointX, 
//					cos(angle)*m_needleLength + m_needleCenter.getY() + m_inset + dropPointY, 
//					m_needleWidth, 
//					m_arrowLength, 
//					m_arrowWidth);
			}
		} else {					// Contrasting arrow for built-in analog meter
			g.setColour(m_backgroundColour.contrasting(1.0f).withAlpha(0.9f));
//			g.drawArrow(
//				w/2 + m_inset, 
//				h - m_segments + m_inset, 
//				sin(angle)*jmax (w/2, h/2) + w/2 + m_inset, 
//				cos(angle)*jmax (w/2, h/2) + h - m_segments + m_inset, 
//				m_needleWidth, 
//				m_needleWidth*2, 
//				m_needleWidth*2);
		}
    	if (m_overlay) {
		    g.setOpacity(1.0f);
			g.drawImage(*m_overlay, m_inset, m_inset, w, h, 0, 0, m_overlay->getWidth(), m_overlay->getHeight());
		}
    }
}

// The callback simply sets the meter's value to the peak level (no rms yet - fix me)

void VUMeter::audioDeviceIOCallback (const float** inputChannelData,
							int totalNumInputChannels,
							float** outputChannelData,
							int totalNumOutputChannels,
							int numSamples) {
	float peak = 0.0;
	float samp = 0.0f;
	float * sampPtr = outputChannelData[m_channel];
	for (int j = 0; j < numSamples; j++) {
		samp = fabs(*sampPtr++);
		if (samp > peak)
			peak = samp;
	}
	this->setValue(peak * m_scale);
}
