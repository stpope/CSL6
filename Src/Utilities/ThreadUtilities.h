//
//  ThreadUtilities.h -- cross-platform Thread Utilities
//
//	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
//

#ifndef CSL_ThreadUtilities_H
#define CSL_ThreadUtilities_H

#include "CSL_Types.h"

#ifdef USE_JTHREADS
	#include "JuceHeader.h"			// JUCE core
#else
	#include <pthread.h>
#endif

namespace csl {

#ifdef USE_JTHREADS

/// The JUCE CSL Thread class

class CThread : public juce::Thread {
public:
	CThread() : juce::Thread("CSL Thread") { };
	virtual ~CThread() { };
	static CThread * MakeThread();	///< factory method

	int (VoidFcnPtr * func, void * args);
//	int createRealtimeThread(VOIDFCNPTR * func, void * args);
	void run();

protected:
	VoidFcnPtr * mFunc;
	void * mArgs;
};

#else // USE_PTHREADS

/// The PThreads CSL Thread class

class CThread {
public:
	CThread();
	virtual ~CThread();
	static CThread * MakeThread();	///< factory method

	virtual int createThread(VoidFcnPtr * func, void * args) = 0;
//	virtual int createRealtimeThread(VoidFcnPtr * func, void * args) = 0;
	void stopThread	(int timeOutMilliseconds);	
	
	pthread_t mThread;
	pthread_attr_t mAttributes;

};

/// Sync is a cross-thread synchronization object

class Synch {
public:
	Synch() { }					///< Constructor
	virtual ~Synch() { }		///< Destructor
	static Synch* MakeSynch();	///< Factory method
						/// Utilities
	virtual int lock() = 0;
	virtual int unlock() = 0;
	virtual int condWait() = 0;
	virtual int condSignal() = 0;
};

#include <pthread.h>		// not on Windows...

/// PThread version of Sync

class SynchPthread : public Synch {
public:
	SynchPthread();
	~SynchPthread();

	pthread_mutex_t mMutex;
	pthread_cond_t mCond;

	int lock();
	int unlock();
	int condWait();
	int condSignal();
};

/// PThread version of Thread

class ThreadPthread : public CThread {
public:
	ThreadPthread();
	~ThreadPthread();

	int createThread(VoidFcnPtr * func, void* args);	
//	int createRealtimeThread(VoidFcnPtr * func, void* args);	
};

#endif

}

#endif
