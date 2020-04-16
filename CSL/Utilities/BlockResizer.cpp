//
//  BlockResizer.cpp -- Regularizes the amount of data called for,
//
//	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
//

#include "BlockResizer.h"
#include <string.h>			// memcpy
 
using namespace csl;

// Constructor - set up cache buffer for 1 block at the input's block size

BlockResizer::BlockResizer(UnitGenerator & input, unsigned blockSize) 
			: mInput(&input), mBufferSize(blockSize), mFramePointer(-1) {
	mNumChannels = mInput->numChannels();		// remember client's # channels
												// set up & allocate my cache buffer
	mInputBuffer.setSize(mNumChannels, mBufferSize);
	mInputBuffer.allocateBuffers();				// make space for my input
}

// Destructor free cache buffer

BlockResizer::~BlockResizer() {
	mInputBuffer.freeBuffers();
}

// nextBuffer() calls the up-hill graph as needed

void BlockResizer::nextBuffer(Buffer & outBuffer) throw(CException) {
	unsigned numFrames = outBuffer.mNumFrames;
	unsigned copiedSoFar = 0;
	
//	if (outBuffer.mNumChannels != mNumChannels) {			// I handle channel mis-match now
//		logMsg(kLogError, "BlockResizer channel # mismatch: %d : %d", outBuffer.mNumChannels, mNumChannels);
//		throw RunTimeError("BlockResizer channel # mismatch");
//		return;
//	}
//	outBuffer.zeroBuffers();					// necessary?

	while (1) {									// loop for multiple input reads if necessary
		if (mFramePointer == -1) {				// if it's time to get more input
//			mInputBuffer.zeroBuffers();			// necessary?
//			logMsg("		Read %d", mInputBuffer.mNumFrames);
			mInput->nextBuffer(mInputBuffer);	// grab more input
			mFramePointer = 0;					// set ptr to the start of the buffer
		}
												// now we know there's data available
		unsigned toCopy = csl_min((mBufferSize - mFramePointer), 
								  (numFrames - copiedSoFar));
												// channel buffer copy loop
		for (unsigned i = 0; i < outBuffer.mNumChannels; i++)	{
			unsigned whichIn = csl_min(i,  (mNumChannels - 1));		// handle mono/stereo
			SampleBuffer src = mInputBuffer.buffer(whichIn) + mFramePointer;
			SampleBuffer dest = outBuffer.buffer(i) + copiedSoFar;
			memcpy (dest, src, toCopy * sizeof(sample));
		}
//		logMsg("r %d s %d p %d", toCopy, copiedSoFar, mFramePointer);
		
		mFramePointer += toCopy;				// now update pointers
		if (mFramePointer >= (int) mBufferSize)
			mFramePointer = -1;					// time to grab more input
		copiedSoFar += toCopy;
		if (copiedSoFar >= numFrames)			// return if we're done
			return;								// maybe without reading at all
	}											// while loop
}
