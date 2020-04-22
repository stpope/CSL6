///
///  ThreadedReader/ThreadedWriter -- frame streams that use separate threads
///	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
///
/// These class are simply a cacheing ugens that use a separate thread to get their input or
/// write their output.
///

#ifndef CSL_ThreadedFrameStream_H
#define CSL_ThreadedFrameStream_H

#include "CSL_Core.h"
#include "RingBuffer.h"
#include "ThreadUtilities.h"

namespace csl {


///
/// ThreadUtil class -- supports reader and writer
///
class ThreadUtil {
public:	
						// state
	unsigned 	mNumChannels;				///< number of channels I handle
	unsigned 	mNumBufferFrames;			///< number of frames I store
	bool 		mIsRunning;					///< am I running?
	int			mNumAvailableFrames;		///v how many frames are available?
	unsigned 	mAvailableFrameThreshold;	///v when do I call my input for more?
	RingBuffer 	mRingBuffer;					///< ring buffer used to store frames

								/// methods
	void setSize(unsigned numChannels, unsigned numBufferFrames);
	void setThreshold(unsigned threshold);
	void setChannels(unsigned numChannels);	
	virtual void init(unsigned numChannels, unsigned numBufferFrames) = 0;

	virtual void start();
	virtual void stop();	

protected:
	Synch * mSynch;
	Thread * mThread;
	Buffer mBuffer;					///< buffer used to cache or accumulate
};

class ThreadedReader : public Effect, public ThreadUtil {
public:	
						/// ctor / dtor
	ThreadedReader();
	ThreadedReader(UnitGenerator inp);
	ThreadedReader(unsigned numChannels);
	ThreadedReader(unsigned numChannels, unsigned numFrames);
	~ThreadedReader();
	
	void init(unsigned numChannels, unsigned numBufferFrames);
										/// nextBuffer uses cache and triggers writer thread
	void nextBuffer(Buffer & outputBuffer, unsigned chan) throw(CException);
	void start();

protected:
	static void * FeederFunction(void * arg);		///< shared init function
	void readIntoBuffer();
	
};

class ThreadedWriter : public Writeable, public ThreadUtil {
public:
				// ctor / dtor
	ThreadedWriter();
	ThreadedWriter(unsigned numChannels);
	ThreadedWriter(unsigned numChannels, unsigned numBufferFrames);
	virtual ~ThreadedWriter();

	Writeable * mOutput;
	
				// methods
	void init(unsigned numChannels, unsigned numBufferFrames);
	void setOutput(Writeable& output) { mOutput = &output; }
	void writeBuffer(Buffer & inputBuffer) throw(CException);
	void start();
	void stop();	

protected:	
	static void * FeederFunction(void* arg);		// thread function
	void writeToOutput();
};

}

#endif
