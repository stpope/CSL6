//
//  InOut.h -- implementation of the class that copies the input buffer to the output buffer
//	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
//

#include "InOut.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

using namespace csl;

// Generic InOut implementation

// Arguments passed to the constructor are optional. Defaults to 2 channels in and out.

InOut::InOut(IO * anIO, unsigned inChans, unsigned outChans, InOutFlags f) 
		: mIO(anIO), mMap(outChans,  CGestalt::blockSize()),
			mInChans(inChans), mOutChans(outChans), 
			mFlags(f) {
	anIO->mInterleaved = false;
	mGains = (float *) malloc(mOutChans * sizeof(float));
}

InOut::InOut(IO * anIO, unsigned inChans, unsigned outChans, InOutFlags f, ...) 
		: mIO(anIO), mMap(outChans,  CGestalt::blockSize()),
			mInChans(inChans), mOutChans(outChans), 
			mFlags(f) {
	anIO->mInterleaved = false;
	mGains = (float *) malloc(mOutChans * sizeof(float));
	va_list ap;
	va_start(ap, f);
	for (unsigned i = 0; i < outChans; i++) 
		mMap.mChannelMap.push_back(va_arg(ap, unsigned));
	va_end(ap);
}

InOut::InOut(UnitGenerator & myInput, unsigned inChans, unsigned outChans, InOutFlags f)
		: mIO(NULL), mMap(outChans, CGestalt::blockSize()), 
			mInChans(inChans), mOutChans(outChans), 
			mFlags(f) {
	this->addInput(CSL_INPUT, myInput);
	mGains = (float *) malloc(mOutChans * sizeof(float));
}

InOut::InOut(UnitGenerator & myInput, unsigned inChans, unsigned outChans, InOutFlags f, ...)
		: mIO(NULL), mMap(outChans,  CGestalt::blockSize()),
			mInChans(inChans), mOutChans(outChans), 
			mFlags(f) {
	this->addInput(CSL_INPUT, myInput);
	mGains = (float *) malloc(mOutChans * sizeof(float));
	va_list ap;
	va_start(ap, f);
	for (unsigned i = 0; i < outChans; i++) 
		mMap.mChannelMap[i] = va_arg(ap, unsigned);
	va_end(ap);
}

InOut::~InOut() { }

void InOut::setChanMap(unsigned * chans) {					///< set channel map
	for (unsigned i = 0; i < mOutChans; i++)
		mMap.mChannelMap[i] = chans[i];
}

void InOut::setChanGains(float * values) {					///< set gain array
	for (unsigned i = 0; i < mOutChans; i++)
		mGains[i] = values[i];
}

void InOut::setGain(unsigned index, float tvalue) {			///< set gain value at index
		mGains[index] = tvalue;
}

// nextBuffer simply grabs the IO's input buffer

void InOut::nextBuffer(Buffer & outputBuffer) throw (CException) {
	unsigned numFrames = outputBuffer.mNumFrames;
	Buffer *inputBuffer;

	if (mIO) {						// either grab the mic input, or my effect in chain
		mIO->getInput(outputBuffer.mNumFrames, outputBuffer.mNumChannels);
		inputBuffer = &(mIO->mInputBuffer);
	} else {
		Effect::pullInput(numFrames);
		Port * tinPort = mInputs[CSL_INPUT];
		inputBuffer = tinPort->mBuffer;
	}

	switch (mFlags) {
//        case kNoProc:
//            for (unsigned i = 0; i < mOutChans; i++)
//                memcpy(outputBuffer.buffer(i), inputBuffer->buffer(i % mInChans), outputBuffer.mMonoBufferByteSize);
//            break;
		case kLR2M:
			for (unsigned i = 0; i < mOutChans; i++) {
				sample * outPtr =  outputBuffer.buffer(i);
				sample * inPtr1 =  inputBuffer->buffer(i % mInChans);
				sample * inPtr2 =  inputBuffer->buffer((i+1) % mInChans);
				for (unsigned j = 0; j < numFrames; j++)
					*outPtr++ = (*inPtr1++ * mGains[i]) + (*inPtr2++* mGains[i]);
			}
			break;
		case kL2M:
		case kR2M:
			break;
		case kN2M:					// N-to-M-channel mapping with bufferCMap
			for (unsigned i = 0; i < mOutChans; i++) {
				sample * outPtr =  outputBuffer.buffer(i);
				int which = mMap.mChannelMap[i];
				if (which < 0) continue;
				sample * inPtr1 =  inputBuffer->buffer(which);
				for (unsigned j = 0; j < numFrames; j++)
					*outPtr++ = *inPtr1++ * mGains[i];
			}
			break;
	}
}

