//
//  BlockResizer.cpp -- Regularizes the amount of data called for
//	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
//

#include "BlockResizer.h"

using namespace csl;

BlockResizer::BlockResizer() : Effect() {
	setBlockSize(CGestalt::blockSize());
	mNumChannels = CGestalt::numOutChannels();
}

BlockResizer::BlockResizer(unsigned quantum) : Effect() {
	setBlockSize(quantum);
	mNumChannels = CGestalt::numOutChannels();
}

BlockResizer::BlockResizer(UnitGenerator &input, unsigned quantum) : Effect(input) {
	mNumChannels = input.numChannels();
	setBlockSize(quantum);
}

BlockResizer::~BlockResizer() {
	freeBuffers();
}

void BlockResizer::initBuffers() {
	mInputBuffer.setSize(mNumChannels, mFrameQuantum);
	mOutputBuffer.setSize(mNumChannels, mFrameQuantum);
	mInputRingBuffer.mBuffer.setSize(mNumChannels, CGestalt::maxBufferFrames());
	mOutputRingBuffer.mBuffer.setSize(mNumChannels, CGestalt::maxBufferFrames());
	
	mInputBuffer.allocateBuffers();
	mOutputBuffer.allocateBuffers();
	mInputRingBuffer.mBuffer.allocateBuffers();
	mOutputRingBuffer.mBuffer.allocateBuffers();
	
}

void BlockResizer::freeBuffers() {
	mInputRingBuffer.mBuffer.freeBuffers();
	mOutputRingBuffer.mBuffer.freeBuffers();
	mInputBuffer.freeBuffers();
	mOutputBuffer.freeBuffers();
}
	
void BlockResizer::setBlockSize(unsigned quantum) {
	mFrameQuantum = quantum;
	mNumAvailableFrames = 0;
	mNumAvailableInput = 0;
	freeBuffers();
	initBuffers();
}

void BlockResizer::resizeInternalBuffers() {
	mNumChannels = CGestalt::numOutChannels();
	mNumAvailableFrames = 0;
	mNumAvailableInput = 0;
	freeBuffers();
	initBuffers();	
}

void BlockResizer::bufferInput(Buffer & inputBuffer) {
	mInputRingBuffer.writeBuffer(inputBuffer);
	mNumAvailableInput += inputBuffer.mNumFrames;
	return;
}

void BlockResizer::readInput() {
	mInputRingBuffer.nextBuffer(mInputBuffer);
	mNumAvailableInput -= mFrameQuantum;
	return;
}

void BlockResizer::readInputAdvancing(unsigned numFramesToAdvance) {
	mInputRingBuffer.nextBuffer(mInputBuffer);
	mInputRingBuffer.mTap.seekTo((int) numFramesToAdvance - (int) mFrameQuantum, kPositionCurrent);
	mNumAvailableInput -= numFramesToAdvance;
#ifdef DEBUG_OLA
	printf("Reading %u, advancing %u\n\t Num Available Input  %i \tcurrent read frame  %u\n", 
				mFrameQuantum, numFramesToAdvance, mNumAvailableInput, mInputRingBuffer.mTap.mCurrentFrame);
#endif	
	return;
}

void BlockResizer::bufferOutput() {
	mOutputRingBuffer.writeBuffer(mOutputBuffer);
	mNumAvailableFrames += mFrameQuantum;
	
	return;
}

void BlockResizer::sumIntoOutputAdvancing(unsigned numFramesToAdvance) {
	mOutputRingBuffer.sumIntoBuffer(mOutputBuffer);
	mOutputRingBuffer.seekTo((int) numFramesToAdvance - (int) mFrameQuantum);
	mNumAvailableFrames += numFramesToAdvance;
	
#ifdef DEBUG_OLA
	printf("Writing %u, advancing %u\n\t Num Available Output %i \tcurrent write frame %u\n", 
				mFrameQuantum, numFramesToAdvance, mNumAvailableFrames, mOutputBuffer.mCurrentWriteFrame);
#endif	

	return;
}

void BlockResizer::readOutput(Buffer & outputBuffer) {
	if (mNumAvailableFrames > (int) outputBuffer.mNumFrames) {
		mOutputRingBuffer.nextBuffer(outputBuffer);
		mNumAvailableFrames -= outputBuffer.mNumFrames;
	} else
		outputBuffer.zeroBuffers();
		
	return;
}

void BlockResizer::readOutputAndZero(Buffer & outputBuffer) {
	if (mNumAvailableFrames > (int) outputBuffer.mNumFrames) {
		mOutputRingBuffer.destructiveNextBuffer(outputBuffer);
		mNumAvailableFrames -= outputBuffer.mNumFrames;
	} else
		outputBuffer.zeroBuffers();
#ifdef DEBUG_OLA		
	printf("Outputting %u\t Output position %u\t Num available Output %i\n-------------\n\n", 
				outputBuffer.mNumFrames, mOutputBuffer.mTap.mCurrentFrame, mNumAvailableFrames);	
#endif
	return;	
}

void BlockResizer::nextBuffer(Buffer & inputBuffer) throw(CException) {
	unsigned numFrames = inputBuffer.mNumFrames;
	unsigned frameQuantum = mFrameQuantum;
										// if I don't need to do any buffering
	if (numFrames == frameQuantum && 0 == mNumAvailableInput && 0 == mNumAvailableFrames) {
		return Effect::pullInput(inputBuffer);
	}

	bufferInput(inputBuffer);
	if (mNumAvailableInput >= (int) frameQuantum) {
		readInput();
		Effect::pullInput(mInputBuffer);
		bufferOutput();
	}
	readOutput(inputBuffer);
	return;
}

