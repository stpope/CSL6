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

WaveShaper::WaveShaper(float frequency, unsigned sel, unsigned size) 
		: Sine(frequency), mTableSize(size) {
	initWaveTable(sel);
}

// initialize and fill in the default wavetable (a straight line)

void WaveShaper::initWaveTable(unsigned sel) {
	mTransferFunction.setSize(1, mTableSize);
	mTransferFunction.allocateBuffers();
	sample * sampPtr = mTransferFunction.buffer(0);
	sample val = -1.0;
	sample incr = 2.0 / mTableSize;
	switch (sel) {
	case 0:				// straight table
		for (unsigned i = 0; i < mTableSize; i++) {
			*sampPtr++ = val;
			val += incr;
		}
		break;
	case 1:				// use this for a clipping wave shape
		for (unsigned i = 0; i < mTableSize/6; i++)
			*sampPtr++ = -0.8333;
		for (unsigned i = mTableSize/6; i < 5*mTableSize/6; i++) {
			*sampPtr++ = val;
			val += incr;
		}
		for (unsigned i = 5*mTableSize/6; i < mTableSize; i++)
			*sampPtr++ = 0.8333;
		break;
	case 2:			// cube table
		for (unsigned i = 0; i < mTableSize; i++) {
			*sampPtr++ = val * val * val;
			val += incr;
		}
		break;
	case 3:			// other table
		for (unsigned i = 0; i < mTableSize/4; i++)
			*sampPtr++ = -0.5;
		for (unsigned i = mTableSize/4; i < 3*mTableSize/4; i++) {
			*sampPtr++ = val;
			val += incr;
		}
		for (unsigned i = 3*mTableSize/4; i < mTableSize; i++)
			*sampPtr++ = 0.5;	
		break;
	}
}

// The mono_next_buffer method does all the work

void WaveShaper::nextBuffer(Buffer & outputBuffer, unsigned outBufNum) throw (CException) {
					// get the sine wave
	Sine::nextBuffer(outputBuffer, outBufNum);
					// get the pointer to the output buffer to write into
	sample * buffer = outputBuffer.buffer(outBufNum);
	unsigned numFrames = outputBuffer.mNumFrames;
					// get the pointer to the transfer function
	sample * function = mTransferFunction.buffer(0);
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
}

