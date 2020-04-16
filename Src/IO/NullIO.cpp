//
//  NullIO.cpp -- Vacuous IO -- i.e., use this for no sound output or for graphs that use dependency for I/O
//	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
//

#include "NullIO.h"

using namespace csl;

// Constructors

NullIO::NullIO() : IO() {
	mSynch = Synch::MakeSynch();
    mThread = juce::Thread::MakeThread();
}

NullIO::NullIO(unsigned s_rate, unsigned b_size, 
			int in_device, int out_device, 
			unsigned in_chans, unsigned out_chans)
		: IO(s_rate, b_size, in_device, out_device, in_chans, out_chans) {
}

NullIO::~NullIO() { }

// Loop function for thread

void * NullIO::FeederFunction(void * arg) {
	NullIO * me = (NullIO *) arg;
	me->pullInput(me->mOutputBuffer);
	while (me->mRunning) {
		Synch* synch = me->mSynch;	// get my sync object
		synch->lock();				// lock to let people know I want the resource
		synch->condWait();			// wait to be told I should fill the buffer
		me->pullInput(me->mOutputBuffer);
		synch->unlock();
	}
	return NULL;
}

void NullIO::start() throw(CException) { 	///< start my timer thread
	mRunning = true;						/// whether or not I'm running
	mThread->createRealtimeThread(FeederFunction, this);
}

void NullIO::stop() throw(CException) { 	///< stop the timer thread
	mSynch->condSignal();
	mRunning = false;
}

/////////////// StdIO

StdIO::StdIO() : NullIO() {
	mSynch = Synch::MakeSynch();
	mThread = Thread::MakeThread();
}

StdIO::~StdIO() { }

void * StdIO::FeederFunction(void * arg) {
	StdIO * me = (StdIO *) arg;
	while (me->mRunning) {
		Synch * synch = me->mSynch;	// get my sync object
		synch->lock();			// lock to let people know I want the resource
		synch->condWait();		// wait to be told I should fill the buffer
		me->pullInput(me->mOutputBuffer);
		synch->unlock();
	}
	return NULL;
}

void StdIO::start() throw(CException) { 			///< start my timer thread
}

void StdIO::stop() throw(CException) { 			///< stop the timer thread
}
