//
//  NullIO.h -- Raw driver IO object and StandardIO
//	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
//

#ifndef CSL_NULLIO_H
#define CSL_NULLIO_H

#include "CSL_Core.h"

#include "ThreadUtilities.h"	// Thread util classes (used only by NullIO)

//#include "ThreadedFrameStream.h"

namespace csl {

///
/// NullIO is an IO that uses a thread and a timer to call its graph's nextBuffer(); it doesn't do
/// anything with the resulting buffer, but assumes that other objects (observers of the graph)
/// will handle some actual IO. 
///

class NullIO : public IO, public ThreadPthread {
public:				
	NullIO();						///< Constructor
	NullIO(unsigned s_rate, unsigned b_size, 
		int in_device = 0, int out_device = 0, 
		unsigned in_chans = 0, unsigned out_chans = 2);
	virtual ~NullIO();
	
	virtual void start() throw(CException);			///< start my timer thread
	virtual void stop() throw(CException);			///< stop the timer thread
	virtual Buffer & getInput() throw(CException) { return mInputBuffer; };
	virtual Buffer & getInput(unsigned numFrames, unsigned numChannels) throw(CException) { return mInputBuffer; };

protected:
	bool mRunning;								/// whether or not I'm running
	juce::Thread * mThread;								///< my timer thread
	Synch * mSynch;								///< the sync I wait on
	static void * FeederFunction(void * arg);		///< shared init function
//	Buffer mEmptyBuffer;
};


///
/// StdIO reads/write the UNIX Standard IO pipes 
///

class StdIO : public NullIO {
public:				
	StdIO();						///< Constructor
	StdIO(unsigned s_rate, unsigned b_size, 
		int in_device = 0, int out_device = 0, 
		unsigned in_chans = 0, unsigned out_chans = 2);
	virtual ~StdIO();
	
	void start() throw(CException);			///< start my timer thread
	void stop() throw(CException);			///< stop the timer thread
protected:
	static void * FeederFunction(void * arg);		///< shared init function

};

}	// end of namespace

#endif CSL_NULLIO_H

