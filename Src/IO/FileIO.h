//
//  FileIO.h -- IO using a sound file for writing data to sound files
//
//	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
//

#ifndef CSL_FileIO_H
#define CSL_FileIO_H

#include "CSL_Includes.h"
#include "ThreadUtilities.h"			// PThread utilities

namespace csl {

///
/// FileIO.h -- IO using a sound file for storing output sample data
///

class FileIO : public IO {
public:
										///< the path name determines the file type, e.g., xx.aiff, zz.snd, or yy.wav
	FileIO(char *path = NULL);
	~FileIO();

	bool mIsPlaying;					///< whether or not it's playing
	bool mIsThreadRunning;				///< is the background thread running?
	unsigned mDuration;					///< the file's buffer rate;
	void open() throw(CException);
										/// seconds is optional. If not passed, starts a background thread, 
										/// playing and writing, otherwise run for x seconds
										/// (so it doesn't return until done).
	void start(float seconds = 0) throw(CException);
	void stop() throw(CException);
	void close() throw(CException);
	void test() throw(CException) { };	///< test the IO's graph
										///< Get the current input from the sound card
	Buffer & getInput() throw(CException) { return mInBuffer; };
	Buffer & getInput(unsigned numFrames, unsigned numChannels) throw(CException) { return mInBuffer; };

protected:
	string mPath;						// the output file name
	SoundFile * mFile;					// the output sound file
	CThread * mThread;					// the background thread
	static void *threadFunction(void *); // the background thread function

	Buffer mInBuffer;					// buffer we use for input
	Buffer mOutBuffer;					// buffer we use for output
	void writeNextBuffer();
};

}

#endif
