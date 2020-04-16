//
//  FileIO.cpp -- File IO using libsndfile
//	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
//

#include "FileIO.h"

using namespace csl;

FileIO::FileIO(char * path) : IO(), mIsPlaying(false), mIsThreadRunning(false), mFile(0) { 
	if ( ! path)
		mPath = CGestalt::sndFileName();
	else 
		mPath = path;
	mFile = new SoundFile(mPath);
}

FileIO::~FileIO() { 
	if (mFile)
		delete mFile;
	if (mThread)
		delete mThread;
}

// Open file output file writer

void FileIO::open() throw(CException) {
	int channels = CGestalt::numOutChannels();
	int blockSize = CGestalt::blockSize();
	int rate = CGestalt::frameRate();
	SoundFileFormat format;

#ifdef DO_TIMING
	mThisSec = mTimeVals = mTimeSum = 0;
#endif
	if (mInBuffer.mAreBuffersAllocated) 		// allocate I/O buffers
		mInBuffer.freeBuffers();
	mInBuffer.setSize(channels, blockSize);
	mInBuffer.allocateBuffers();	
	if (mOutBuffer.mAreBuffersAllocated) 
		mOutBuffer.freeBuffers();
	mOutBuffer.setSize(channels, blockSize);
	mOutBuffer.allocateBuffers();	
	const char * sPath = mPath.c_str();
	
	char * dot = (char *) strrchr(sPath, '.');	// get the file name extension
	format = kSoundFileFormatAIFF;				// default = AIFF files
	if (strcmp(dot, ".wav") == 0)
		format = kSoundFileFormatWAV;
	else if (strcmp(dot, ".snd") == 0)
		format = kSoundFileFormatSND;

	mFile->openForWrite(format, channels, rate);	// open output file
	if ( ! mFile->isValid()) {
		logMsg(kLogError, "Error opening sound file output");
		return;									//	THROW AN EXCEPTION
	}
	mThread = CThread::MakeThread();				// create background thread
	mDuration = (int)((float)blockSize / (float)rate * 1000000.0f);	// block rate in usec
	return;
}

void FileIO::start(float seconds) throw(CException) {
	mIsPlaying = true;
	mIsThreadRunning = false;
	if (seconds) {								// full-speed driver version
		unsigned numFrames = (unsigned) (seconds * CGestalt::frameRate());
		logMsg("Starting sound file output loop");
		while (mNumFramesPlayed < numFrames)	// note that this does not return until it's done,
			writeNextBuffer();					// so you can't start and then trigger envelopes!
	} else {   // Start file IO -- loop in the background
		logMsg("Starting sound file output thread");
		mThread->createThread(threadFunction, this);
	}
}

void FileIO::stop() throw(CException) {
	mIsPlaying = false;							// signal for writer thread to finish
	while(mIsThreadRunning)						// wait around for the feeder thread to die
		csl::sleepUsec(mDuration);
}

// close, free, and write output file

void FileIO::close() throw(CException) {
	if (mOutBuffer.mAreBuffersAllocated) 
		mOutBuffer.freeBuffers();
	if (mInBuffer.mAreBuffersAllocated) 
		mInBuffer.freeBuffers();
	mFile->close();
}

// compute and write a buffer

void FileIO::writeNextBuffer() {
	if (mGraph != 0)
		mGraph->nextBuffer(mOutBuffer);
	else
		mOutBuffer.zeroBuffers();
	mFile->writeBuffer(mOutBuffer);
	mNumFramesPlayed += mOutBuffer.mNumFrames;
}

// Background thread function -- loop to read from the DSP graph and write to the file

void * FileIO::threadFunction(void * ptr) {
	FileIO *me = (FileIO *)ptr;
//	printf("D: %d\n", me->mDuration);
	
#ifdef DO_TIMING
	int sleepTime;
	struct timeval *meThen = &me->mThen;
	struct timeval *meNow = & me->mNow;
#endif
	while (me->mIsPlaying) {			// loop until turned off
#ifdef DO_TIMING
		GET_TIME(meThen);
#endif
		me->writeNextBuffer();			// get and write the next buffer
#ifdef DO_TIMING
		GET_TIME(meNow);
		me->printTimeStatistics(meNow, meThen, & me->mThisSec, & me->mTimeSum, & me->mTimeVals);
		GET_TIME(meNow);
		sleepTime = me->mDuration - (((meNow->tv_sec - meThen->tv_sec) * 1000000) + (meNow->tv_usec - meThen->tv_usec));
	//	printf("S: %d\n", sleepTime);
		if (sleepTime > 0)
			csl::sleepUsec(sleepTime);
#else
		csl::sleepUsec(me->mDuration);
#endif
	}
				// do this to prevent a race condition
	me->mIsThreadRunning = false;
	logMsg("Stopping sound file output thread");
	return 0;
}
