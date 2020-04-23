//  FDN.cpp --  CSL feedback delay network class
//
//	TODO:
//		The delay lines are fixed. Add the possibility of changing delay lengths, number of taps, etc.
//		Add interpolation.


#include "CSL_Core.h"		// my superclass
#include "FDN.h"

using namespace csl;

// Method implementations
// Trivial constructors
FDN::FDN(UnitGenerator &op, unsigned int delayLineLengths[], unsigned int numDelayLines, sample inputGains[], 
		sample outputGains[], sample feedbackMatrix[], sample feedbackGains[] ) 
		: Effect(op), mInputGains(inputGains), mOutputGains(outputGains), mFeedbackMatrix(feedbackMatrix), mFeedbackGains(feedbackGains)  {

	mNumDelLines = numDelayLines;
	
	unsigned numElements = mNumDelLines * (CGestalt::numOutChannels());	
	mDelayLine = new Buffer[mNumDelLines];
	mIndex = new unsigned[numElements];
	mDelLength = new unsigned[numElements];
	mFeedbackVector = new sample[numElements];
	mOutputVector = new sample[numElements];

	for (unsigned int i = 0; i < numElements; i++) {
		mIndex[i] = 0;
		mDelLength[i] = delayLineLengths[i % mNumDelLines];
		mFeedbackVector[i] = 0;
	}

	this->initDelayLines();
}

// initialize and fill (with noise) the delay line
void FDN::initDelayLines() {

	for (unsigned int i = 0; i < mNumDelLines; i++) {
		// set up and allocate space for the delay line
		mDelayLine[i].setSize(CGestalt::numOutChannels(), mDelLength[i]);
		mDelayLine[i].allocateBuffers();
		mDelayLine[i].zeroBuffers();
	}

}

// Destructor
FDN::~FDN() {

	delete [] mDelayLine;
	delete [] mIndex;
	delete [] mDelLength;
	delete [] mFeedbackVector;
	delete [] mOutputVector;
}

// The nextBuffer method does all the work
void FDN::nextBuffer(Buffer &outputBuffer, unsigned outBufNum) throw (CException) {

	unsigned i, j, k;
	unsigned numFrames = outputBuffer.mNumFrames;
	sample *outBufferPtr = outputBuffer.mBuffers[outBufNum]; // get the pointers to the output buffer to write into
	sample *dLineBufferPtr; 
	sample accumulator;	
	sample *inputBufferPtr = mInputPtr;
	
	sample *outputVector = &mOutputVector[mNumDelLines * outBufNum];
    sample *feedbackVector = &mFeedbackVector[mNumDelLines * outBufNum];
	unsigned *index = &mIndex[mNumDelLines * outBufNum];
	unsigned *delLength= &mDelLength[mNumDelLines * outBufNum];

	for (k = 0; k < numFrames; k++) {
		accumulator = 0; // reset accumulator
		for (i = 0; i < mNumDelLines; i++) {
			dLineBufferPtr = mDelayLine[i].mBuffers[outBufNum]; // get pointer to the delay storage
			outputVector[i] = dLineBufferPtr[index[i]]; // get the current sample in  the delay line
			
			dLineBufferPtr[index[i]++] = (*inputBufferPtr++) * mInputGains[i] + feedbackVector[i];
			if (index[i] >= delLength[i]) 
				index[i] = 0;
			accumulator += mOutputGains[i] * outputVector[i];
		}

		*outBufferPtr++ = accumulator;

		//Calculate feedback for next poll
		for (i = 0; i < mNumDelLines; i++) {
			accumulator = 0; // reset accumulator.		
			for (j = 0; j < mNumDelLines; j++)
				accumulator += mFeedbackMatrix[i * mNumDelLines + j] * outputVector[j];
			
			feedbackVector[i] = mFeedbackGains[i] * accumulator;
		}
	}
	return;
}
