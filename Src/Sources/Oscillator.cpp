//
//  Oscillator.cpp -- implementation of the base oscillator class and most simple waveform generators
//	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
//

#include "Oscillator.h"
//#include "SHARC.h"
#include <math.h>

using namespace csl;

// Generic Oscillator implementation

// Constructor: Parameters are optional. Defaults to freq 220, amp 1 and phase & offset of 0

Oscillator::Oscillator(float frequency, float ampl, float offset, float phase) 
			: UnitGenerator(), 
			Phased(frequency, phase), 
			Scalable(ampl, offset) { /* no-op */ }

Oscillator::Oscillator(UnitGenerator & frequency, float ampl, float offset, float phase)
			: UnitGenerator(), 
			Phased(frequency, phase), 
			Scalable(ampl, offset) { /* no-op */ }
			
Oscillator::Oscillator(UnitGenerator & frequency, UnitGenerator & ampl, float offset, float phase)
			: UnitGenerator(), 
			Phased(frequency, phase), 
			Scalable(ampl, offset) { /* no-op */ }
			
Oscillator::~Oscillator() { }

void Oscillator::dump() {
	logMsg("an Oscillator");
	Scalable::dump();
}

// Wavetable oscillator methods
// Constructors -- default creates a sine wave table

//WavetableOscillator::WavetableOscillator() : Oscillator() {
//	mWavetable.setSize(1, DEFAULT_WAVETABLE_SIZE);
//	mInterpolate = kTruncate;
////	mWavetable.allocateBuffers();
////	fillSine();
//}

WavetableOscillator::WavetableOscillator(float frequency, float ampl, float offset, float phase)
				: Oscillator(frequency, ampl, offset, phase) {
	mWavetable.setSize(1, DEFAULT_WAVETABLE_SIZE);
//	mWavetable.allocateBuffers();
	mInterpolate = kTruncate;
//	fillSine();
}

//WavetableOscillator::WavetableOscillator(SampleBuffer samps, unsigned size) : Oscillator() {
//	mWavetable.setSize(1, size);
//	mInterpolate = kTruncate;
//	setWaveform(samps, size);	
//}

WavetableOscillator::WavetableOscillator(Buffer & wave) : Oscillator() {
	mWavetable.setSize(1, wave.mNumFrames);
	mInterpolate = kTruncate;
	setWaveform(wave);	
}

///< Destructor

WavetableOscillator::~WavetableOscillator() {
	mWavetable.freeBuffers();
}

// Plug in a waveform from a buffer or a simple sample array

void WavetableOscillator::setWaveform(Buffer & wave, bool freBufs) {
	if (freBufs)
		mWavetable.freeBuffers();
	mWavetable.mNumChannels = wave.mNumChannels;
	mWavetable.mNumFrames = wave.mNumFrames;
	if (mWavetable.buffers() == NULL)
		mWavetable.setBuffers(new SampleBuffer[mNumChannels]);
	for (unsigned i = 0; i < mWavetable.mNumChannels; i++)
		mWavetable.setBuffer(i, wave.buffer(i));
	mWavetable.mMonoBufferByteSize = wave.mMonoBufferByteSize;
	mWavetable.mIsPopulated = wave.mIsPopulated;
	mWavetable.mAreBuffersZero = wave.mAreBuffersZero;
    mWavetable.mAreBuffersAllocated = wave.mAreBuffersAllocated;
    mWavetable.mNumAlloc = wave.mNumAlloc;
    mWavetable.mDidIAllocateBuffers = false;
    mWavetable.mAreBuffersZero = false;
    mWavetable.mIsPopulated = true;
}

//void WavetableOscillator::setWaveform(SampleBuffer samps, unsigned size) {
//	mWavetable.freeBuffers();
//	mWavetable.setSize(1, size);
//	mWavetable.setBuffer(0, samps);
//	mWavetable.mMonoBufferByteSize = size * sizeof(sample);
//	mWavetable.mIsPopulated = true;
//	mWavetable.mAreBuffersZero = false;
//	mWavetable.mAreBuffersAllocated = true;	
//	mWavetable.mDidIAllocateBuffers = false;
//}

//// Shared buffer with a sine of length CGestalt::maxBufferFrames()

static Buffer * sSineTable = 0;

// Create the default wavetable -- 1 cycle of a sine

void WavetableOscillator::fillSine() {
	if ( ! sSineTable) {					// create shared sine table lazily
		sSineTable = new Buffer(1, DEFAULT_WTABLE_SIZE);
		sSineTable->allocateBuffers();		// make space
		SampleBuffer ptr = sSineTable->buffer(0);
		float incr = CSL_TWOPI / DEFAULT_WTABLE_SIZE;
		float accum = 0;					// sine fill loop
		for (unsigned i = 0; i < DEFAULT_WTABLE_SIZE; i++) {
			*ptr++ = sinf(accum);
			accum += incr;
		}
	}
	mWavetable.setSize(1, DEFAULT_WTABLE_SIZE);
	mWavetable.setBuffer(0, sSineTable->buffer(0));		// point to the shared sine waveform
	mWavetable.mAreBuffersAllocated = true;					// fib a bit
	mWavetable.mDidIAllocateBuffers = false;
}

// Oscillate a buffer-full of the stored waveform

void WavetableOscillator::nextBuffer(Buffer & outputBuffer, unsigned outBufNum) throw (CException) {
	SampleBuffer buffer = outputBuffer.buffer(outBufNum);	// get pointer to the selected output channel
	if ( ! mWavetable.mAreBuffersAllocated)					// lazy sine init
		fillSine();
	SampleBuffer waveform = mWavetable.buffer(0);
	unsigned tableLength = mWavetable.mNumFrames;
	float rateRecip = (float) tableLength / (float) mFrameRate;
	float phase = mPhase;									// get a local copy of the phase
	unsigned numFrames = outputBuffer.mNumFrames;			// the number of frames to fill
	DECLARE_PHASED_CONTROLS;								// declare the frequency buffer and value
	DECLARE_SCALABLE_CONTROLS;								// declare the scale/offset buffers and values
#ifdef CSL_DEBUG
	logMsg("WavetableOscillator nextBuffer");
#endif		
	if ( ! waveform) 
		return;
	LOAD_PHASED_CONTROLS;									// load the freqC from the constant or dynamic value
	LOAD_SCALABLE_CONTROLS;									// load the scaleC and offsetC from the constant or dynamic value
	unsigned int index;
	sample samp1, samp2;
	float fraction;
	switch (mInterpolate) {
	case kTruncate:											// truncating wavetable lookup
		for (unsigned i = 0; i < numFrames; i++) {			// sample loop
			while (phase >= tableLength)					// wrap-around phase
				phase -= tableLength;
			while (phase < 0)					// wrap-around phase
				phase += tableLength;
			index = (unsigned int) floorf(phase);					// truncate phase to an integer
			samp1 = waveform[index];						//// WAVE TABLE ACCESS ////
			*buffer++ = (samp1 * scaleValue) + offsetValue;	// get and scale the table item (truncating look-up)
			phase += (freqValue * rateRecip);
			UPDATE_PHASED_CONTROLS;							// update the dynamic frequency
			UPDATE_SCALABLE_CONTROLS;						// update the dynamic scale/offset
		}
		break;
	case kLinear:
		for (unsigned i = 0; i < numFrames; i++) {			// sample loop
			index = (unsigned int) floorf(phase);
			fraction = phase - (float) index;
			samp1 = waveform[index];						// get sample
			if (index < (tableLength - 1))
				samp2 = waveform[index+1];					// and next sample
			else
				samp2 = waveform[0];	
			samp1 += (samp2 - samp1) * fraction;			// and interpolate linear-wise
			*buffer++ = (samp1 * scaleValue) + offsetValue;	// get and scale the table item (truncating look-up)
			phase += (freqValue * rateRecip);
			while (phase >= tableLength)					// wrap-around phase
				phase -= tableLength;
			UPDATE_PHASED_CONTROLS;							// update the dynamic frequency
			UPDATE_SCALABLE_CONTROLS;						// update the dynamic scale/offset
		}
		break;
	default:
		throw LogicError("Unimplemented truncation policy");
	}
	mPhase = phase;											// store the temp phase back to the member variable
}

// CompOrCacheOscillator implementation

// Arguments are optional. Defaults to: Cache = false, freq = 220, phase = 0.0.

CompOrCacheOscillator::CompOrCacheOscillator(bool whether, float frequency, float phase)
				: WavetableOscillator(frequency, 1.0f, 0.0f, phase), 
				Cacheable(whether) { }

// Create the default wavetable cache

void CompOrCacheOscillator::createCache(void) {
	mUseCache = true;
	mWavetable.allocateBuffers();
	this->nextWaveInto(mWavetable.buffer(0), mWavetable.mNumFrames, true);
}

// nextBuffer either calls the inherited wavetable method, or does the computation on-demand here

void CompOrCacheOscillator::nextBuffer(Buffer & outputBuffer, unsigned outBufNum) throw (CException) {
	if (mUseCache)				// if we cache -- use the wavetable
		return WavetableOscillator::nextBuffer(outputBuffer, outBufNum);
								// otherwise do the synthesis on demand
	outputBuffer.zeroBuffers();
	SampleBuffer buffer = outputBuffer.buffer(outBufNum);
	this->nextWaveInto(buffer, outputBuffer.mNumFrames, false);
}

// Sine methods
	
// Constructor with variable number of arguments. Defaults to f = 220, amp = 1, offset & phase = 0.

Sine::Sine(float frequency, float ampl, float offset, float phase) 
			: Oscillator(frequency, ampl, offset, phase) { }

// the computed sine's nextBuffer

void Sine::nextBuffer(Buffer & outputBuffer, unsigned outBufNum) throw (CException) {
	SampleBuffer buffer = outputBuffer.buffer(outBufNum);		// get pointer to the selected output channel
	float rateRecip = CSL_TWOPI / mFrameRate;
	unsigned numFrames = outputBuffer.mNumFrames;			// the number of frames to fill
	float phase = mPhase;
	DECLARE_PHASED_CONTROLS;								// declare the frequency buffer and value
	DECLARE_SCALABLE_CONTROLS;								// declare the scale/offset buffers and values
#ifdef CSL_DEBUG
	logMsg("Sine nextBuffer");
#endif		
	LOAD_PHASED_CONTROLS;									// load the freqC from the constant or dynamic value
	LOAD_SCALABLE_CONTROLS;									// load the scaleC and offsetC from the constant or dynamic value

//	fprintf(stderr, " :%d: %5.3f", numFrames, scaleValue);
	for (unsigned i = 0; i < numFrames; i++) {					// sample loop
		*buffer++ = (sinf(phase) * scaleValue) + offsetValue;	// compute and store (scaled and offset) sine value
		phase += (freqValue * rateRecip);						// increment phase
		UPDATE_PHASED_CONTROLS;								// update the dynamic frequency
		UPDATE_SCALABLE_CONTROLS;							// update the dynamic scale/offset
	}	
	while (phase >= CSL_TWOPI)								// wraparound after 2pi radians
		phase -= CSL_TWOPI;
	mPhase = phase;
}

// FSine methods
	
// Constructor with variable number of arguments. Defaults to f = 220, amp = 1, offset & phase = 0.

FSine::FSine(float frequency, float ampl, float offset, float phase) 
			: Oscillator(frequency, ampl, offset, phase) { }

// the computed sine's nextBuffer

void FSine::nextBuffer(Buffer & outputBuffer, unsigned outBufNum) throw (CException) {
	SampleBuffer buffer = outputBuffer.buffer(outBufNum);		// get pointer to the selected output channel
	float rateRecip = CSL_TWOPI / mFrameRate;
	unsigned numFrames = outputBuffer.mNumFrames;			// the number of frames to fill
	float phase = mPhase;
	DECLARE_PHASED_CONTROLS;								// declare the frequency buffer and value
	DECLARE_SCALABLE_CONTROLS;								// declare the scale/offset buffers and values
#ifdef CSL_DEBUG
	logMsg("Sine nextBuffer");
#endif		
	LOAD_PHASED_CONTROLS;									// load the freqC from the constant or dynamic value
	LOAD_SCALABLE_CONTROLS;									// load the scaleC and offsetC from the constant or dynamic value

//	fprintf(stderr, " :%d: %5.3f", numFrames, scaleValue);
	for (unsigned i = 0; i < numFrames; i++) {					// sample loop
		*buffer++ = (sinf(phase) * scaleValue) + offsetValue;	// compute and store (scaled and offset) sine value
		phase += (freqValue * rateRecip);						// increment phase
		UPDATE_PHASED_CONTROLS;								// update the dynamic frequency
		UPDATE_SCALABLE_CONTROLS;							// update the dynamic scale/offset
	}	
	while (phase >= CSL_TWOPI)								// wraparound after 2pi radians
		phase -= CSL_TWOPI;
	mPhase = phase;
}

// Sawtooth methods

// Constructor with variable number of arguments. Defaults to f = 220, amp = 1, offset & phase = 0

Sawtooth::Sawtooth(float frequency, float ampl, float offset, float phase) 
			: Oscillator(frequency, ampl, offset, phase) { }

void Sawtooth::nextBuffer(Buffer & outputBuffer, unsigned outBufNum) throw (CException) {
	SampleBuffer buffer = outputBuffer.buffer(outBufNum);	// get pointer to the selected output channel
	float rateRecip = 1.0f / mFrameRate;
	unsigned numFrames = outputBuffer.mNumFrames;			// the number of frames to fill
	float phase = mPhase;
	DECLARE_PHASED_CONTROLS;								// declare the frequency buffer and value
	DECLARE_SCALABLE_CONTROLS;								// declare the scale/offset buffers and values
#ifdef CSL_DEBUG
	logMsg("Sine nextBuffer");
#endif		
	LOAD_PHASED_CONTROLS;									// load the freqC from the constant or dynamic value
	LOAD_SCALABLE_CONTROLS;									// load the scaleC and offsetC from the constant or dynamic value
	
	for (unsigned i = 0; i < numFrames; i++) {				// sample loop
		*buffer++ = phase * scaleValue + offsetValue;		// store the scaled and offset phase value
		phase += (freqValue * rateRecip);					// increment phase
		if (phase >= 1.0f)
			phase -= 2.0f;
		UPDATE_PHASED_CONTROLS;								// update the dynamic frequency
		UPDATE_SCALABLE_CONTROLS;							// update the dynamic scale/offset
	}	
	mPhase = phase;
}

// SquareSquare methods
	
// Constructor with variable number of arguments. Defaults to f = 220, amp = 1, offset & phase = 0

Square::Square(float frequency, float ampl, float offset, float phase) 
			: Oscillator(frequency, ampl, offset, phase) { }

void Square::nextBuffer(Buffer & outputBuffer, unsigned outBufNum) throw (CException) {
	SampleBuffer buffer = outputBuffer.buffer(outBufNum);	// get pointer to the selected output channel
	float rateRecip = 1.0f / mFrameRate;
	unsigned numFrames = outputBuffer.mNumFrames;			// the number of frames to fill
	float phase = mPhase;
	DECLARE_PHASED_CONTROLS;								// declare the frequency buffer and value
	DECLARE_SCALABLE_CONTROLS;								// declare the scale/offset buffers and values
#ifdef CSL_DEBUG
	logMsg("Sine nextBuffer");
#endif		
	LOAD_PHASED_CONTROLS;									// load the freqC from the constant or dynamic value
	LOAD_SCALABLE_CONTROLS;									// load the scaleC and offsetC from the constant or dynamic value
	
	for (unsigned i = 0; i < numFrames; i++) {				// sample loop
		*buffer++ = ((phase > 0.0) ? 1.0f : -1.0f) * scaleValue + offsetValue;		// store +- 1
		phase += (freqValue * rateRecip);					// increment phase
		if (phase >= 1.0f)
			phase -= 2.0f;
		UPDATE_PHASED_CONTROLS;								// update the dynamic frequency
		UPDATE_SCALABLE_CONTROLS;							// update the dynamic scale/offset
	}	
	mPhase = phase;
}

// Impulse methods
	
Impulse::Impulse() : Oscillator() { mCounter = 0; }

Impulse::Impulse(float delay) : Oscillator() { mCounter = (int) delay; }

Impulse::Impulse(float frequency, float ampl) 
	: Oscillator(frequency, ampl) { mCounter = 0; }

Impulse::Impulse(float frequency, float ampl, float offset) 
	: Oscillator(frequency, ampl, offset) { mCounter = 0; }

Impulse::Impulse(float frequency, float ampl, float offset, float phase) 
	: Oscillator(frequency, ampl, offset, phase) { mCounter = 0; }

// the computed Impulse's next_buffer

void Impulse::nextBuffer(Buffer & outputBuffer, unsigned outBufNum) throw (CException) {
	SampleBuffer buffer = outputBuffer.buffer(outBufNum);		// get pointer to the selected output channel
	unsigned numFrames = outputBuffer.mNumFrames;			// the number of frames to fill
	unsigned i, j;
	int count = mCounter;
	DECLARE_PHASED_CONTROLS;								// declare the frequency buffer and value as above
	DECLARE_SCALABLE_CONTROLS;								// declare the scale/offset buffers and values as above
#ifdef CSL_DEBUG
	logMsg("Sine nextBuffer");
#endif		
	LOAD_PHASED_CONTROLS;									// load the freqC from the constant or dynamic value
	LOAD_SCALABLE_CONTROLS;									// load the scaleC and offsetC from the constant or dynamic value
	
	if (count < 0) {										// Impulse is done. Output only zeros.
		for (i = 0; i < numFrames; i++)
			*buffer++ = 0;
	} else if (count < numFrames) {
		for (i = 0; i < count; i++)
			*buffer++ = 0;
		*buffer++ = scaleValue;
		for (j = count+1; j < numFrames; j++) 
			*buffer++ = 0;
	} else {
		for (i = 0; i < numFrames; i++) 
			*buffer++ = 0;
	}
	mCounter = count - numFrames;
}

// SumOfSines

SumOfSines::SumOfSines() : CompOrCacheOscillator() { }

SumOfSines::SumOfSines(float frequency) 
				: CompOrCacheOscillator(false, frequency) { }

SumOfSines::SumOfSines(PartialDescriptionMode format, unsigned partialCount, ...) 
				: CompOrCacheOscillator(false) {
	float num=0, amp=0, pha=0;
	Partial * p;
	va_list ap;
	va_start(ap, partialCount);
	for (unsigned i = 0; i < partialCount; i++) {
		switch (format) {
		case kFrequency:				// if 1 term per partial, it's the amplitude
			num = (float) (i + 1);
			amp = va_arg(ap, double);
			pha = 0.0;
			break;
		case kFreqAmp:				// if 2 terms per partial, they're num, ampl
			num = va_arg(ap, double);
			amp = va_arg(ap, double);
			pha = 0.0;
			break;
		case kFreqAmpPhase:			// if 3 terms per partial, they're num, ampl
			num = va_arg(ap, double);
			amp = va_arg(ap, double);
			pha = va_arg(ap, double);
			break;
		}
		p = new Partial;
		p->number = num;
		p->amplitude = amp;
		p->phase = pha;
		mPartials.push_back(p);
	}
	va_end(ap);
}

// given a SHARC spectrum
#include "SHARC.h"

SumOfSines::SumOfSines(SHARCSpectrum & spect) {
	Partial * harm;	
	for (unsigned i = 0; i < spect._num_partials; i++) {
		harm = spect._partials[i];
//		fprintf(stderr, "\t%g  @ %.5f @ %.5f\n", harm->number, harm->amplitude, harm->phase);
		this->addPartial(harm->number, harm->amplitude, harm->phase);
	}
	this->createCache();							// make the cached wavetable
}

// 1/f spectrum + noise

SumOfSines::SumOfSines(unsigned numHarms, float noise) {
	for (unsigned i = 0; i < numHarms; i++) {
		float ampl = fRandV(noise) / (float) (i + 2);
		float phas = fRandV(CSL_PI);
//		fprintf(stderr, "\t%d  @ %.5f @ %.5f\n", i, ampl, phas);
		this->addPartial(i, ampl, phas);
	}
	this->createCache();							// make the cached wavetable
}

SumOfSines::SumOfSines(float frequency, unsigned numHarms, float noise) 
			: CompOrCacheOscillator(false, frequency) {
	for (unsigned i = 0; i < numHarms; i++) {
		this->addPartial(i, fRandM(0.5f, 1.3f) / (float) i, fRandV(CSL_PI));
	}
	this->createCache();							// make the cached wavetable
}

// Methods to add partials

void SumOfSines::addPartial(float nu, float amp) {
	Partial * p = new Partial;
	p->number = nu;
	p->amplitude = amp;
	p->phase = 0.0;
	mPartials.push_back(p);
}

void SumOfSines::addPartial(float nu, float amp, float phase) {
	Partial * p = new Partial;
	p->number = nu;
	p->amplitude = amp;
	p->phase = phase;
	mPartials.push_back(p);
}

void SumOfSines::addPartial(Partial * pt) {
	mPartials.push_back(pt);
}

void SumOfSines::addPartials(unsigned num_p, Partial ** pt) {
	Partial ** pt_ptr = pt;
	for (unsigned i = 0; i < num_p; i++)
		mPartials.push_back(*pt_ptr++);
}


void SumOfSines::addPartials(int argc, void ** argv) {
	float ** v_ptr = (float ** ) argv;
	int i = 0;
	while (i < argc) {
		Partial * part = new Partial;
		part->number = * v_ptr[i++];
		part->amplitude = * v_ptr[i++];
		mPartials.push_back(part);
	}
}

void SumOfSines::clearPartials() {
	mPartials.clear();
}

// Do sum-of-sines additive synthesis into the given buffer

void SumOfSines::nextWaveInto(SampleBuffer dest, unsigned count, bool oneHz) {
	float incr, t_incr, phase, ampl;
	SampleBuffer out_ptr;
	unsigned numFrames = count;							// the number of frames to fill
	DECLARE_PHASED_CONTROLS;							// declare the frequency buffer and value
	DECLARE_SCALABLE_CONTROLS;							// declare the scale/offset buffers and values

	if (oneHz) {										// if we're creating a wavetable
		incr = CSL_TWOPI / (float) count;
	} else {											// otherwise compute phase increment scale
		incr = CSL_TWOPI / (float) mFrameRate;
		LOAD_PHASED_CONTROLS;							// load the freqC from the constant or dynamic value
		LOAD_SCALABLE_CONTROLS;							// load the scaleC and offsetC from the constant or dynamic value
	}
	for (unsigned i = 0; i < mPartials.size(); i++) {	// partials loop
		Partial * p = mPartials[i];
		out_ptr = dest;
		phase = p->phase;
		t_incr = incr * p->number;
		ampl = p->amplitude;
//		fprintf(stderr, "\t\tp%d = i %g : a %g : p %g\n", i, t_incr, ampl, phase);
		if (oneHz) {
			for (unsigned j = 0; j < numFrames; j++) {	// sample loop
				*out_ptr++ += (sinf(phase) * ampl);
				phase += t_incr;
			}
		} else {
			for (unsigned j = 0; j < numFrames; j++) {	// sample loop
				*out_ptr++ += (sinf(phase) * ampl * scaleValue) + offsetValue;
				phase += (t_incr * freqValue);
				UPDATE_PHASED_CONTROLS;					// update the dynamic frequency
				UPDATE_SCALABLE_CONTROLS;				// update the dynamic scale/offset
//				if (j == 1) fprintf(stderr, "\t\t\t%g : %g : %g\n", freqValue, scaleValue, offsetValue);
			}
		}
		if ( ! oneHz) {
			freqPort->resetPtr();						// reset control pointers after each loop through a partial
			scalePort->resetPtr();
			offsetPort->resetPtr();
			freqValue = freqPort->nextValue();
			scaleValue = scalePort->nextValue();
			offsetValue = offsetPort->nextValue();
		}
		while (phase > CSL_TWOPI)						// wrap phase here
			phase -= CSL_TWOPI;
		p->phase = phase;
	}
}

void SumOfSines::dump() {
	unsigned siz = mPartials.size();
	logMsg("a SumOfSines: %d partials", siz);
	for (unsigned i = 0; i < siz; i++)
		fprintf(stderr, "\tP: %g, %g, %g\n", mPartials[i]->number, mPartials[i]->amplitude, mPartials[i]->phase); 
	Scalable::dump();
}
