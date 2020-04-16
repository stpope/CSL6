//
//  ThreadUtilities.cpp -- cross-platform Thread Utilities
//	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
//

#include "ThreadUtilities.h"
#include "CGestalt.h"

using namespace csl;

#ifdef USE_JTHREADS

// Thread is now concrete

CThread * CThread::MakeThread() { 
	return new CThread(); 
}

int CThread::createThread(VoidFcnPtr * func, void * args) {
	(*func)(args);				// call my function
	return 0;
}

//int CThread::createRealtimeThread(VOIDFCNPTR * func, void * args) {
//	mFunc = * func;
//	mArgs = args;
//	run();
//	return 0;
//}

void CThread::run() {
	logMsg("Start thread");
	(*mFunc)(mArgs);				// call my function
}

#else // PTHREADS

// This CThread uses pthreads

CThread::CThread() : mThread(NULL) { }

CThread::~CThread() { 
	pthread_cancel(mThread);
}

CThread * CThread::MakeThread() { 
	return new ThreadPthread(); 
}

void CThread::stopThread	(int timeOutMilliseconds) {
	pthread_cancel(mThread);
}

Synch *Synch::MakeSynch() { return new SynchPthread(); }

// ~~~~~ SyncPthread Implementation ~~~~~

// Constructor

SynchPthread::SynchPthread() {
	pthread_mutex_init(& mMutex, NULL);
	pthread_cond_init(& mCond, NULL);
}

SynchPthread::~SynchPthread() {
	pthread_mutex_destroy(&mMutex);
	pthread_cond_destroy(&mCond);
}

// The following functions are just wrappers around the pthreads respective commands.

int SynchPthread::lock() { return pthread_mutex_lock(&mMutex); }

int SynchPthread::unlock() { return pthread_mutex_unlock(&mMutex); }

int SynchPthread::condWait() { return pthread_cond_wait(&mCond, &mMutex); }

int SynchPthread::condSignal() { return pthread_cond_signal(&mCond); }

// ~~~~~ ThreadPthread Implementation ~~~~~

// Constructor and destructor for windows and other systems.

#ifdef CSL_WINDOWS

ThreadPthread::ThreadPthread() : CThread () /*, mThread(NULL)*/ { 
	pthread_attr_init(&mAttributes);
}

ThreadPthread::~ThreadPthread() {
	pthread_attr_destroy(&mAttributes);
//	if (mThread)
//		pthread_cancel(mThread);
}

#else // UNIX

ThreadPthread::ThreadPthread() : CThread() { 
	pthread_attr_init(&mAttributes);
}

ThreadPthread::~ThreadPthread() {
	pthread_attr_destroy(&mAttributes);
	if (mThread)
		pthread_cancel(mThread);
}

#endif

int ThreadPthread::createThread(VoidFcnPtr * func, void * args) {
	pthread_attr_setdetachstate(&mAttributes, PTHREAD_CREATE_DETACHED);
	pthread_attr_setscope(&mAttributes, PTHREAD_SCOPE_SYSTEM);
	return pthread_create(&mThread, &mAttributes, func, args);
}

#ifdef CSL_MACOSX_OLD_WAY

// Most of the code in these two functions is taken from Apple PlayAudioFile example

#include <mach/mach.h> 		// used for setting policy of thread

unsigned GetThreadBasePriority (pthread_t inThread) {
	thread_basic_info_data_t threadInfo;
	policy_info_data_t thePolicyInfo;
	unsigned int count;
    
	// get basic info
	count = THREAD_BASIC_INFO_COUNT;
	thread_info (pthread_mach_thread_np (inThread), THREAD_BASIC_INFO, (integer_t*)&threadInfo, &count);
	switch (threadInfo.policy) {
		case POLICY_TIMESHARE:
			count = POLICY_TIMESHARE_INFO_COUNT;
			thread_info(pthread_mach_thread_np (inThread), THREAD_SCHED_TIMESHARE_INFO, (integer_t*)&(thePolicyInfo.ts), &count);
			return thePolicyInfo.ts.base_priority;
			break;
		case POLICY_FIFO:
			count = POLICY_FIFO_INFO_COUNT;
			thread_info(pthread_mach_thread_np (inThread), THREAD_SCHED_FIFO_INFO, (integer_t*)&(thePolicyInfo.fifo), &count);
			if (thePolicyInfo.fifo.depressed) {
				return thePolicyInfo.fifo.depress_priority;
			} else {
				return thePolicyInfo.fifo.base_priority;
			}
			break;
		case POLICY_RR:
			count = POLICY_RR_INFO_COUNT;
			thread_info(pthread_mach_thread_np (inThread), THREAD_SCHED_RR_INFO, (integer_t*)&(thePolicyInfo.rr), &count);
			if (thePolicyInfo.rr.depressed)
				return thePolicyInfo.rr.depress_priority;
			else
				return thePolicyInfo.rr.base_priority;
			break;
	}
	return 0;
}

int ThreadPthread::createRealtimeThread(VoidFcnPtr * func, void* args) {
				// first create a thread
	int value = createThread(func, args);
	int result;
	int threadPriority = 62; // the number in Apple's example code

				// bump up the priority and make it fixed priority
				// code taken from Apple PlayAudioFile example
	thread_extended_policy_data_t theFixedPolicy;
	thread_precedence_policy_data_t thePrecedencePolicy;
	int relativePriority;
				// make thread fixed
	theFixedPolicy.timeshare = false;	// set to true for a non-fixed thread
	result = thread_policy_set (pthread_mach_thread_np(mThread), THREAD_EXTENDED_POLICY, (thread_policy_t)&theFixedPolicy, THREAD_EXTENDED_POLICY_COUNT);
				// set priority
				// precedency policy's "importance" value is relative to spawning thread's priority
	relativePriority = threadPriority - GetThreadBasePriority (pthread_self());
	thePrecedencePolicy.importance = relativePriority;
	result = thread_policy_set (pthread_mach_thread_np(mThread), THREAD_PRECEDENCE_POLICY, (thread_policy_t)&thePrecedencePolicy, THREAD_PRECEDENCE_POLICY_COUNT);
	return value;
}

#else

//int ThreadPthread::createRealtimeThread(VOIDFCNPTR * func, void* args) {
//	return createThread(func, args);
//}

#endif

#endif

