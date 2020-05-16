//
//  KarplusString.h -- CSL "physical model" string class
//	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
//

#include "KarplusString.h"

using namespace csl;

// Method implementations

// Trivial constructors

KarplusString::KarplusString(float frequency) : Scalable(), Phased(frequency) {
	mIndex = 0;
	mEnergy = 0;
	mDelayLength = (int) (CGestalt::frameRate() / frequency);
	mFrequency = frequency;
	this->initDelayLine();
}

/// Reset time to 0.0 to restart envelope

void KarplusString::trigger() {
	WhiteNoise noise;
	noise.nextBuffer(mDelayLine, 0);	// write noise into the delay line
	mDelayLine.removeDC();
										// estimate decay time for freq
	mEnergy = 600 * (5 - (freqToKey(mFrequency) / 25));
//	logMsg("\tKarplusString::trigger @ %5.2f Hz", mFrequency);
}

void KarplusString::dump() {
	logMsg("a KarplusString @ %5.2f Hz", mFrequency);
	Scalable::dump();
}

/// initialize and fill (with noise) the delay line

void KarplusString::initDelayLine() {
	mDelayLine.setSize(1, mDelayLength); // set up and allocate space for the delay line
	mDelayLine.allocateBuffers();
	mEnergy = 0;
	mIndex = 0;
}

void KarplusString::setFrequency(float frequency) {
	Phased::setFrequency(frequency);
	mEnergy = 0;
	mFrequency = frequency;
	mDelayLength = (int) (CGestalt::frameRate() / frequency);
	this->initDelayLine();
}

/// The mono_next_buffer method does all the work

void KarplusString::nextBuffer(Buffer &outputBuffer, unsigned outBufNum) throw (CException) {
	
	sample samp, lastSample;
	unsigned numFrames = outputBuffer.mNumFrames;	
	sample *outPtr = outputBuffer.buffer(outBufNum); // get the pointer to the output buffer to write into
	sample *delayPtr = mDelayLine.buffer(0); // get the pointer to the delay line storage
	if ( ! mEnergy)
		return;
	DECLARE_SCALABLE_CONTROLS;
#ifdef CSL_DEBUG
	logMsg("Karplus Strong String nextBuffer");
#endif
	LOAD_SCALABLE_CONTROLS;
					
	for (unsigned i = 0; i < numFrames; i++) { // now do the KS recirculating noise filter
		samp = delayPtr[mIndex];	// get the current sample in  the delay line
		if (mIndex > 0)				// get the previous sample
			lastSample = delayPtr[mIndex - 1];
		else
			lastSample = delayPtr[mDelayLength - 1];
		samp += lastSample;			// average the last 2 samples (cheap LPF)
		samp *= 0.5;
		UPDATE_SCALABLE_CONTROLS;		// update the dynamic scale/offset
		*outPtr++ = samp * scaleValue + offsetValue;			// write to output
		delayPtr[mIndex] = samp;	// write into the delay line
		mIndex++;					// increment and wrap the index
		if (mIndex >= mDelayLength)	
			mIndex = 0;
	}
	if (mEnergy)
		mEnergy--;
}
