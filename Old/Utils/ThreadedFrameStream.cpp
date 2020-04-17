//
//  ThreadedFrameStream.cpp -- a frame stream that uses a separate thread
//	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
//

#include "ThreadedFrameStream.h"

using namespace csl;

void ThreadedReader::init(unsigned numChannels, unsigned numBufferFrames) {
	setSize(numChannels, numBufferFrames);
	mAvailableFrameThreshold = 1024;
	mBuffer.setSize(numChannels, CGestalt::maxBufferFrames());
	mSynch = Synch::MakeSynch();
	mThread = Thread::MakeThread();
}

void * ThreadedReader::FeederFunction(void * arg) {
	ThreadedReader * me = (ThreadedReader *) arg;
	unsigned ringBufferNumFrames = me->mNumBufferFrames;
	unsigned inputNumFrames = me->mBuffer.mNumFrames;
					// read as much as I can into the buffer
	while (me->mNumAvailableFrames < (int) (me->mNumBufferFrames - inputNumFrames))
		me->readIntoBuffer();
	while (me->mIsRunning) {
		Synch* synch = me->mSynch;	// get my sync object
		synch->lock();			// lock to let people know I want the resource
		synch->condWait();		// wait to be told I should fill the buffer
		while (me->mNumAvailableFrames < (int) (ringBufferNumFrames - inputNumFrames))
			me->readIntoBuffer();
		synch->unlock();
	}
	return NULL;
}

// General reader function

void ThreadedReader::readIntoBuffer() {
	pullInput(mBuffer);
	mRingBuffer.writeBuffer(mBuffer);
	mNumAvailableFrames += mBuffer.mNumFrames;
}

// Property setter
	
void ThreadUtil::setSize(unsigned numChannels, unsigned numBufferFrames) {
	mNumChannels = numChannels;
	mNumBufferFrames = numBufferFrames;
	mIsRunning = false;
	mNumAvailableFrames = 0;
	mRingBuffer.mBuffer.setSize(numChannels, numBufferFrames);
}

void ThreadUtil::setThreshold(unsigned threshold) {
	mAvailableFrameThreshold = threshold;
}

void ThreadUtil::setChannels(unsigned numChannels) {
	mNumChannels = numChannels;
}

// Read from the threaded reader

void ThreadedReader::nextBuffer(Buffer & outputBuffer, unsigned chan) throw (CException) {
	if (!mIsRunning) {					// if I'm not running, just get the input to give its next buffer
		Effect::pullInput(outputBuffer);
								//copy samples from mInputPtr
		return;
	}
									// otherwise, read from the ring buffer
	mRingBuffer.nextBuffer(outputBuffer);
	mNumAvailableFrames -= outputBuffer.mNumFrames;
	if (mNumAvailableFrames < (int) mAvailableFrameThreshold)
		mSynch->condSignal();		// signal the sync to start the other thread
}

// Start the co-process

void ThreadUtil::start() {
	if (!mBuffer.mAreBuffersAllocated)
		mBuffer.allocateBuffers();
	if (!mRingBuffer.mBuffer.mAreBuffersAllocated)
		mRingBuffer.mBuffer.allocateBuffers();
	mIsRunning = true;
}

void ThreadUtil::stop() {
	mIsRunning = false;				// the feeder function exits when this goes to false
	mSynch->condSignal();
}

void ThreadedReader::start() {
	ThreadUtil::start();	
	mThread->createRealtimeThread(FeederFunction, this);
}

void ThreadedWriter::start() {
	ThreadUtil::start();	
	mThread->createRealtimeThread(FeederFunction, this);
}

void ThreadedWriter::stop() {
	ThreadUtil::stop();	
	while (mNumAvailableFrames > 0)				// dump out what's remaining in the buffers
		writeToOutput();
}

// Constructors

ThreadedReader::ThreadedReader(UnitGenerator inp) : Effect(inp) {
	init(CGestalt::numOutChannels(), CGestalt::maxBufferFrames() * 10);
}

ThreadedReader::ThreadedReader() : Effect() {
	init(CGestalt::numOutChannels(), CGestalt::maxBufferFrames() * 10);
}

ThreadedReader::ThreadedReader(unsigned numChannels) : Effect() {
	init(numChannels, CGestalt::maxBufferFrames() * 10);
}

ThreadedReader::ThreadedReader(unsigned numChannels, unsigned numBufferFrames) {
	init(numChannels, numBufferFrames);
}

ThreadedReader::~ThreadedReader() {
	if (mBuffer.mAreBuffersAllocated)
		mBuffer.freeBuffers();
	delete mSynch;
	delete mThread;
}
	
void ThreadedWriter::init(unsigned numChannels, unsigned numBufferFrames) {
	setSize(numChannels, numBufferFrames);
	mAvailableFrameThreshold = 1024;
	mBuffer.setSize(mNumChannels, CGestalt::maxBufferFrames());
	mSynch = Synch::MakeSynch();
	mThread = Thread::MakeThread();
}

void* ThreadedWriter::FeederFunction(void* arg) {
	ThreadedWriter* me = (ThreadedWriter*) arg;
	unsigned outputNumFrames = me->mBuffer.mNumFrames;
	// write as much as I can
	while (me->mNumAvailableFrames > (int) outputNumFrames)
		me->writeToOutput();
		
	while (me->mIsRunning) {
		Synch* synch = me->mSynch;			// lock to let people know I want the resource
		synch->lock();						// wait to be told I should write out
		synch->condWait();
		if (me->mIsRunning) {
			while (me->mNumAvailableFrames > (int) outputNumFrames)
				me->writeToOutput();
		}
		synch->unlock();
	}
	return NULL;
}

void ThreadedWriter::writeToOutput() {
	mRingBuffer.destructiveNextBuffer(mBuffer);
	mOutput->writeBuffer(mBuffer);	
	mNumAvailableFrames -= mBuffer.mNumFrames;
}

void ThreadedWriter::writeBuffer(Buffer& inputBuffer) {
	// if I'm not running, just get the input to give its next buffer
	if (!mIsRunning)
		return mOutput->writeBuffer(inputBuffer);
		
	// otherwise, write to the ring buffer
	mRingBuffer.writemBuffer(inputBuffer);
	mNumAvailableFrames += inputBuffer.mNumFrames;
	if (mNumAvailableFrames > (int) mAvailableFrameThreshold)
		mSynch->condSignal();
	
	return cslOk;
}

ThreadedWriter::ThreadedWriter() : Writeable() {
	init(CGestalt::numOutChannels(), CGestalt::maxBufferFrames() * 10);
}

ThreadedWriter::ThreadedWriter(unsigned numChannels) : Writeable() {
	init(numChannels, CGestalt::maxBufferFrames() * 10);
}

ThreadedWriter::ThreadedWriter(unsigned numChannels, unsigned mNumBufferFrames) : Writeable() {
	init(numChannels, mNumBufferFrames);
}

ThreadedWriter::~ThreadedWriter() {
	if (mBuffer.mAreBuffersAllocated)
		mBuffer.freeBuffers();
	delete mSynch;
	delete mThread;
}
