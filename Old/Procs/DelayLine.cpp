//
// DLine.h -- an Interpolating Delay Line
//	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
//
// This is a typical circular buffer with one writer and multiple readers (tap instances)

#include "DelayLine.h"

using namespace csl;

// DelayLine implementation
DelayLine :: DelayLine(unsigned maxDelayInSamples) : Effect() {
	// add a little bit of wiggle room to the delay length this is necessary to make setDelayLength work
	mMaxDelayInSamples = maxDelayInSamples;
	mTotalDelayInSamples = maxDelayInSamples + CGestalt::maxBufferFrames();
	mRingBuffer.mBuffer.setSize(CGestalt::numOutChannels(), mTotalDelayInSamples); // Prepare the buffer to be used for storage.
	mRingBuffer.mBuffer.allocateBuffers(); // Allocate memory for my buffer.
	mRingBuffer.mTap.seekTo(mMaxDelayInSamples, kPositionEnd); // Cue the read head (tap) to the new position.
}

unsigned DelayLine::delayLength() {
	int length = mRingBuffer.mCurrentWriteFrame - mRingBuffer.mTap.mCurrentFrame;
	if (length < 0)
		length += mTotalDelayInSamples;
	return (unsigned)length;
}

float DelayLine::delayTime() {								///< Returns the delay amount in miliseconds.
	return(delayLength()  / mFrameRate * 1000);
} 

// change the delay length and return the new value
unsigned DelayLine::setDelayLength(unsigned delayInSamples) {

	if (delayInSamples > mMaxDelayInSamples) {
		logMsg(kLogError, "Cannot set delay line length: asked for %d in line of length %d", delayInSamples, mMaxDelayInSamples);
		return delayLength();
	}
	mRingBuffer.mTap.seekTo(delayInSamples, kPositionEnd); // Cue the read head (tap) to the new position.
	return delayLength();
}

// change the delay length and return the new value
float DelayLine::setDelayTime(float delayInMiliseconds) {
	unsigned delayInSamples = (unsigned)(delayInMiliseconds * mFrameRate / 1000); // Converting ms to samples.
	
	if (delayInSamples > mMaxDelayInSamples) {  // If larger than the maximum size specified, keep last value and print an error.
		logMsg(kLogError, "Cannot set delay line length: asked for %d in line of length %d", delayInSamples, mMaxDelayInSamples);
		return (delayLength() * (float)mFrameRate / 1000); // Converting back to miliseconds.
	}
	mRingBuffer.mTap.seekTo(delayInSamples, kPositionEnd); // Cue the read head (tap) to the new position.
	return delayInMiliseconds; // If everything went well, return the requested value.
}

// The next_buffer call writes, then reads
void DelayLine::nextBuffer(Buffer& output) throw(CException) {
//	getInput(output);
	Port * tinPort = mInputs[CSL_INPUT];
	Controllable::pullInput(tinPort, output.mNumFrames);

	mRingBuffer.writeBuffer(*(tinPort->mBuffer));
	mRingBuffer.nextBuffer(output);
}
