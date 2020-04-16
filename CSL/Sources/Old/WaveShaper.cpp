//
//  WaveShaper.h -- CSL wave-shaping oscillator class
//	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
//

#include "WaveShaper.h"		// my superclass

using namespace csl;

// Trivial constructors

WaveShaper::WaveShaper() : Sine() {
	initWaveTable();
}

// Size is optional and defaults to 8192 bytes.
WaveShaper::WaveShaper(float frequency, unsigned size) : Sine(frequency), mTableSize(size) {
	initWaveTable();
}

// initialize and fill in the default wavetable (a stringht line)

void WaveShaper::initWaveTable() {
	mTransferFunction.set_size(1, mTableSize);
	mTransferFunction.allocateBuffers();
	sample * sampPtr = mTransferFunction.monoBuffer(0);
	sample val = -1.0;
	sample incr = 2.0 / mTableSize;
	for (unsigned i = 0; i < mTableSize; i++) {
		*sampPtr++ = val;
		val += incr;
	}
					// use this for a clipping wave shape
//#define clipit
#ifdef clipit
	for (unsigned i = 0; i < mTableSize/4; i++)
		*sampPtr++ = -0.5;
	for (unsigned i = mTableSize/4; i < 3*mTableSize/4; i++) {
		*sampPtr++ = val;
		val += incr;
	}
	for (unsigned i = 3*mTableSize/4; i < mTableSize; i++)
		*sampPtr++ = 0.5;
#endif
}

// The mono_next_buffer method does all the work

status WaveShaper::mono_next_buffer(Buffer & inputBuffer, Buffer & outputBuffer, unsigned inBufNum, unsigned outBufNum) {
					// get the sine wave
	Sine::mono_next_buffer(inputBuffer, outputBuffer, inBufNum, outBufNum);
					// get the pointers to the output buffer to write into
	sample * buffer = outputBuffer._monoBuffers[outBufNum];
	unsigned numFrames = outputBuffer._numFrames;
					// get the pointer to the transfer function
	sample * function = mTransferFunction.mBuffers[0];
	unsigned fcnLength = mTransferFunction.mNumFrames;
	unsigned fcnLengthHalf = fcnLength / 2;
	
	sample samp;
	
					// now do the wave-shaping table look-up
	for (unsigned i = 0; i < numFrames; i++) {
		samp = *buffer;									// get a sample (assumed to be in the range +- 1.0)
		unsigned index = (samp + 1.0) * fcnLengthHalf;	// convert to a table index
		if (index < 0) index = 0;						// make sure it's in the right range
		if (index >= fcnLength) index = fcnLength - 1;
		samp = function[index];							// scale the sample
		*buffer++ = samp;								// store it in the buffer
	}
	return cslOk;
}

