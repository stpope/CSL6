//
//  RingBuffer.cpp -- the ring buffer class implementation
//	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
//

#include "RingBuffer.h"

#include <string.h>		// for memcpy

using namespace csl;

// ~~~~~~~~~~~~~ RingBufferTap implementation ~~~~~~~~~~~~~~~~ //

// Tap constructors

RingBufferTap::RingBufferTap(RingBuffer *parent, int offset) : UnitGenerator(), Seekable(), 
					mLoopStartFrame(0), mLoopEndFrame(0), mParentBuffer(parent) {
	setOffset(offset);
}

// Tap utilities.

unsigned RingBufferTap::duration() {
	return mParentBuffer->mBuffer.mNumFrames;
}

void RingBufferTap::setOffset(int offset) {
	int offsetFrame = offset;
	if (mParentBuffer) {
		unsigned bufferSize = mParentBuffer->mBuffer.mNumFrames;
		offsetFrame = mParentBuffer->mCurrentWriteFrame + offset;
		while (offsetFrame < 0)
			offsetFrame += bufferSize;
		while ((unsigned) offsetFrame > bufferSize)
			offsetFrame -= bufferSize;
	}
	mCurrentFrame = (unsigned) offsetFrame;
}

// Override next_buffer as well to store the local state

void RingBufferTap::nextBuffer(Buffer &outputBuffer) throw(CException){

	UnitGenerator::nextBuffer(outputBuffer); // Call the super version

	mCurrentFrame = mTempCurrentFrame; // remember the state
	return;
}

// Tap private nextBuffer: copy frames from the buffer, wrapping around at end

void RingBufferTap::nextBuffer(Buffer &outputBuffer, unsigned outBufNum) throw(CException) {
	// get everything into registers
	unsigned numFrames = outputBuffer.mNumFrames;
	unsigned currentFrame = mCurrentFrame;
	unsigned ringBufferStartFrame = mLoopStartFrame;
	unsigned ringBufferEndFrame = mParentBuffer->mBuffer.mNumFrames - mLoopEndFrame; // get the end frame from the begining of the buffer.	
	unsigned currentBufNum = outBufNum % (mParentBuffer->mBuffer.mNumChannels);	// if out of buffers then recycle from buffer 0.	
	unsigned framesWritten = 0;
	bool canCopyToEnd;
		
	// Check bounds, and if outside of loop points, then reset to start frame of tap loop.
	if (currentFrame > ringBufferEndFrame)
		currentFrame = ringBufferStartFrame;
	else if (currentFrame < ringBufferStartFrame)
		currentFrame = ringBufferStartFrame;

	// Get a pointer to the buffers (read and write).
	SampleBuffer currentBufferPtr = mParentBuffer->mBuffer.buffer(currentBufNum) + currentFrame;
	SampleBuffer outputBufferPtr = outputBuffer.buffer(outBufNum);

	for (; numFrames > 0; numFrames -= framesWritten) {
		canCopyToEnd = (currentFrame + numFrames) > ringBufferEndFrame;
		framesWritten = canCopyToEnd ? (ringBufferEndFrame - currentFrame) : numFrames;
		memcpy(outputBufferPtr, currentBufferPtr, framesWritten * sizeof(sample*));
		if (canCopyToEnd)
			currentFrame = ringBufferStartFrame;
		else
			currentFrame += framesWritten;
		// increment buffer pointers
		currentBufferPtr = mParentBuffer->mBuffer.buffer(currentBufNum) + currentFrame;
		outputBufferPtr += framesWritten;
	}
	mTempCurrentFrame = currentFrame; // remember the state
	
	return;
}

void RingBufferTap::destructiveNextBuffer(Buffer &outputBuffer) throw(CException) {
	unsigned numOutputBuffers = outputBuffer.mNumChannels;
	unsigned i;
	
	for (i = 0; i < numOutputBuffers; i++)
		destructiveNextBuffer(outputBuffer, i);
				
	mCurrentFrame = mTempCurrentFrame; // remember the state
	return;
}

// this is the same as next buffer, but I zero out things as I go along

void RingBufferTap::destructiveNextBuffer(Buffer &outputBuffer, unsigned outBufNum) throw(CException) {
	// get everything into registers
	unsigned numFrames = outputBuffer.mNumFrames;
	unsigned currentFrame = mCurrentFrame;
	unsigned ringBufferStartFrame = mLoopStartFrame;	
	unsigned ringBufferEndFrame = mParentBuffer->mBuffer.mNumFrames - mLoopEndFrame;	
	
	if (currentFrame > ringBufferEndFrame)
		currentFrame = ringBufferStartFrame;
	else if (currentFrame < ringBufferStartFrame)
		currentFrame = ringBufferStartFrame;
	unsigned currentBufNum = outBufNum % (mParentBuffer->mBuffer.mNumChannels);		
	SampleBuffer currentBufferPtr = mParentBuffer->mBuffer.buffer(currentBufNum) + currentFrame;
	SampleBuffer outputBufferPtr = outputBuffer.buffer(outBufNum);
		
	unsigned framesWritten = 0;
	for (; numFrames > 0; numFrames -= framesWritten) {
		bool copyToEnd = (currentFrame + numFrames) > ringBufferEndFrame;
		framesWritten = copyToEnd ? (ringBufferEndFrame - currentFrame) : numFrames;
		memcpy (outputBufferPtr, currentBufferPtr, framesWritten * sizeof(sample*));
		memset (currentBufferPtr, 0, framesWritten * sizeof(sample*));
		if (copyToEnd)
			currentFrame = ringBufferStartFrame;
		else
			currentFrame += framesWritten;
				// increment buffer pointers
		currentBufferPtr = mParentBuffer->mBuffer.buffer(currentBufNum) + currentFrame;
		outputBufferPtr += framesWritten;
	}
				// remember the state
	mTempCurrentFrame = currentFrame;
	return;	
}

// Taps are also Seekable

unsigned RingBufferTap::seekTo(int position, SeekPosition whence) throw(CException){
	switch(whence) {
		case kPositionStart:
			setOffset((int) position);
			break;
		case kPositionCurrent: {
				int offsetFrame = mCurrentFrame + position;
				int bufferSize = mParentBuffer->mBuffer.mNumFrames;
				while (offsetFrame < 0)
					offsetFrame += bufferSize;
				while (offsetFrame > bufferSize)
					offsetFrame -= bufferSize;
			}
			break;
		case kPositionEnd:
			setOffset(-1 * position);
			break;
	}
	return mCurrentFrame;
}

#pragma mark RingBuffer

// ~~~~~~~~~ RingBuffer implementation ~~~~~~~~~~~~~~

// Constructors

RingBuffer::RingBuffer() : 
	Effect(), Writeable(), mCurrentWriteFrame(0), 
	mBuffer(), mTempCurrentWriteFrame(0) { 
	mTap.setBuffer(this);
}

RingBuffer::RingBuffer(unsigned int nmChannels, unsigned int nmFrames) 
			: Effect(), Writeable(), mCurrentWriteFrame(0),
				mBuffer(nmChannels, nmFrames), 
				mTempCurrentWriteFrame(0) {
	mBuffer.allocateBuffers();
	mTap.setBuffer(this);
}

RingBuffer::RingBuffer(UnitGenerator & input, unsigned int nmChannels, unsigned int nmFrames)
			: Effect(input), Writeable(), mCurrentWriteFrame(0),
				mBuffer(nmChannels, nmFrames), 
				mTempCurrentWriteFrame(0) {
	mBuffer.allocateBuffers();
	mTap.setBuffer(this);
}

// Buffer nextBuffer reads from the default tap

void RingBuffer::nextBuffer(Buffer &outputBuffer) throw(CException) {
	if (mInputs[CSL_INPUT]) {
		Effect::pullInput(outputBuffer);
		writeBuffer(outputBuffer);
	}
	return mTap.nextBuffer(outputBuffer);
}

void RingBuffer::destructiveNextBuffer(Buffer &outputBuffer) throw(CException) {
	return mTap.destructiveNextBuffer(outputBuffer);
}

void RingBuffer::writeBuffer(Buffer &inputBuffer) throw(CException) {
	Writeable::writeBuffer(inputBuffer);
	mCurrentWriteFrame = mTempCurrentWriteFrame;
	return;
}

void RingBuffer::sumIntoBuffer(Buffer &inputBuffer) throw(CException) {
	unsigned numBufs = inputBuffer.mNumChannels;
	for (unsigned i = 0; i < numBufs; i++)
		sumIntoBuffer(inputBuffer, i);
	mCurrentWriteFrame = mTempCurrentWriteFrame;
	return;	
}

unsigned RingBuffer::seekTo(int position) throw(CException) {
	unsigned writeFrame = mCurrentWriteFrame;
	unsigned numFrames = mBuffer.mNumFrames;
	
	writeFrame += position;
	while (writeFrame > numFrames)
		writeFrame -= numFrames;
	mCurrentWriteFrame = writeFrame;
	return mCurrentWriteFrame;
}

// Buffer write method

void RingBuffer::writeBuffer(Buffer &inputBuffer, unsigned bufNum) throw(CException) {
	if (bufNum >= mNumChannels) return;
	unsigned numFrames = inputBuffer.mNumFrames;	// get everything into registers
	unsigned currentFrame = mCurrentWriteFrame;
	unsigned ringBufferStartFrame = mTap.mLoopStartFrame;
	unsigned ringBufferEndFrame = mBuffer.mNumFrames - mTap.mLoopEndFrame;
	
	if (currentFrame > ringBufferEndFrame)
		currentFrame = ringBufferStartFrame;
	else if (currentFrame < ringBufferStartFrame)
		currentFrame = ringBufferStartFrame;

	unsigned framesWritten = 0;
	unsigned currentBufNum = bufNum % mBuffer.mNumChannels;
	SampleBuffer currentBufferPtr = mBuffer.buffer(currentBufNum) + currentFrame;
	SampleBuffer inputBufferPtr = inputBuffer.buffer(bufNum);
//	printf("\tRB: %7.5f\n", *inputBufferPtr);
	for (; numFrames > 0; numFrames -= framesWritten) {
		bool copyToEnd = (currentFrame + numFrames) > ringBufferEndFrame;
		framesWritten = copyToEnd ? (ringBufferEndFrame - currentFrame) : numFrames;
		memcpy (currentBufferPtr, inputBufferPtr, framesWritten * sizeof(sample*));
		if (copyToEnd)
			currentFrame = ringBufferStartFrame;
		else
			currentFrame += framesWritten;
							// increment buffer pointers
		currentBufferPtr = mBuffer.buffer(currentBufNum) + currentFrame;
		inputBufferPtr += framesWritten;
	}
							// remember the state
	mTempCurrentWriteFrame = currentFrame;
	return;
}

void RingBuffer::sumIntoBuffer(Buffer &inputBuffer, unsigned bufNum) throw(CException) {
	// get everything into registers
	unsigned numFrames = inputBuffer.mNumFrames;
	unsigned currentFrame = mCurrentWriteFrame;
	unsigned ringBufferStartFrame = mTap.mLoopStartFrame;
	unsigned ringBufferEndFrame = mBuffer.mNumFrames - mTap.mLoopEndFrame;
	
	if (currentFrame > ringBufferEndFrame)
		currentFrame = ringBufferStartFrame;
	else if (currentFrame < ringBufferStartFrame)
		currentFrame = ringBufferStartFrame;

	unsigned framesWritten = 0;	
	unsigned currentBufNum = bufNum % mBuffer.mNumChannels;
	bool canCopyToEnd;
	
	SampleBuffer currentBufferPtr = mBuffer.buffer(currentBufNum) + currentFrame;
	SampleBuffer inputBufferPtr = inputBuffer.buffer(bufNum);
	
	for (; numFrames > 0; numFrames -= framesWritten) {
		canCopyToEnd = (currentFrame + numFrames) > ringBufferEndFrame;
		framesWritten = canCopyToEnd ? (ringBufferEndFrame - currentFrame) : numFrames;
		
		for (unsigned i = 0; i < framesWritten; i++)
			*currentBufferPtr++ += *inputBufferPtr++;

		if (canCopyToEnd) {
			currentFrame = ringBufferStartFrame;
			currentBufferPtr = mBuffer.buffer(currentBufNum) + currentFrame;
		} else
			currentFrame += framesWritten;
	}

	// remember the state
	mTempCurrentWriteFrame = currentFrame;
	
	return;
}

#pragma mark BufferStream

// ~~~~~~~~~ BufferStream implementation starts here ~~~~~~~~~~~~~~

unsigned BufferStream::seekTo(int position, SeekPosition whence) throw(CException) {
	switch(whence) {
		case kPositionStart:
			mCurrentFrame = position;
			break;
		case kPositionCurrent:
			mCurrentFrame += position;
			break;
		case kPositionEnd:
			mCurrentFrame = mBuffer->mNumFrames - position;
			break;
	}
	return mCurrentFrame;
}

void BufferStream::nextBuffer(Buffer & outputBuffer) throw(CException) {
	UnitGenerator::nextBuffer(outputBuffer);
	mCurrentFrame = mTempCurrentFrame;	// remember state
	return;
}

void BufferStream::writeBuffer(Buffer &inputBuffer) throw(CException) {
	Writeable::writeBuffer(inputBuffer);
	mCurrentFrame = mTempCurrentWriteFrame; // remember state
	return;
}

void BufferStream::nextBuffer(Buffer &outputBuffer, unsigned outBufNum) throw(CException) {
										// get everything into registers
	unsigned numFrames = outputBuffer.mNumFrames;
	unsigned currentFrame = mCurrentFrame;
	unsigned bufferEndFrame = mBuffer->mNumFrames;
	unsigned currentBufNum = outBufNum % (mBuffer->mNumChannels);
	SampleBuffer outputBufferPtr = outputBuffer.buffer(outBufNum);
	
										// if we are past the end
	if (currentFrame > bufferEndFrame) {
		memset(outputBufferPtr, 0, numFrames);
		return;
	}
	
	SampleBuffer currentBufPtr = mBuffer->buffer(currentBufNum) + currentFrame;	
	unsigned framesWritten = 0;
	bool copyToEnd = (currentFrame + numFrames) > bufferEndFrame;
	framesWritten = copyToEnd ? (bufferEndFrame - currentFrame) : numFrames;
	memcpy (outputBufferPtr, currentBufPtr, framesWritten * sizeof(sample*));
	currentFrame += framesWritten;
	if (framesWritten < numFrames) {
		outputBufferPtr += framesWritten;
		memset(outputBufferPtr, 0, numFrames - framesWritten);
	}									// remember the state
	mTempCurrentFrame = currentFrame;
	return;
}

void BufferStream::writeBuffer(Buffer &inputBuffer, unsigned bufNum) throw(CException) {
						// get everything into registers
	unsigned numFrames = inputBuffer.mNumFrames;
	unsigned currentFrame = mCurrentFrame;
	unsigned bufferEndFrame = mBuffer->mNumFrames;

	if (currentFrame > bufferEndFrame) {
		logMsg(kLogError, "Attempt to write into a buffer, past the end");
		return;
	}

	unsigned currentBufNum = bufNum % mBuffer->mNumChannels;
	SampleBuffer currentBufPtr = mBuffer->buffer(currentBufNum) + currentFrame;
	SampleBuffer inputBufferPtr = inputBuffer.buffer(bufNum);
	unsigned framesWritten = 0;
	bool copyToEnd = (currentFrame + numFrames) > bufferEndFrame;
	framesWritten = copyToEnd ? (bufferEndFrame - currentFrame) : numFrames;
	memcpy (currentBufPtr, inputBufferPtr, framesWritten * sizeof(sample*));
	currentFrame += framesWritten;	
							// remember the state
	mTempCurrentWriteFrame = currentFrame;
	return;
}
