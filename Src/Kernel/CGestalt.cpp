///
///  CGestalt.cpp -- the gestalt class implementation.
///
///	The MVC Observer/Subject or dependency pattern classes are also here.
//
//	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
//

#include "CGestalt.h"
#include <stdio.h>
#include <stdlib.h>		// for malloc
#include <string.h>
#include <math.h>

using namespace csl;	// this is the namespace, dummy!
using namespace std;

#ifdef WIN32
#include <crtdbg.h>
#endif

//using namespace juce;

////// These are the system default values //////
///
/// The actual start-up values are defined in CSL_Types.h
///

static unsigned mNumInChannels = 2;								///< stereo inputs by default
static unsigned mNumOutChannels = 2;							///< stereo outputs

static unsigned mFrameRate = CSL_mFrameRate;					///< default sample rate (tested up to 96000)
static float mFrameRateF = (float) CSL_mFrameRate;				///< default sample rate (tested up to 96000)
static csl::sample mFramePeriod = 1.0f / mFrameRateF;				///< 1 / default sample rate
static unsigned mBlockSize = CSL_mBlockSize;					///< typical block size (can be as small as 128 in real usage)
static unsigned mMaxBufferFrames = CSL_mMaxBufferFrames;		///< max block size (set large for zooming scopes)
static unsigned mSndFileFrames = CSL_mSndFileFrames;			///< max block size (set large for zooming scopes)
static unsigned mMaxSndFileFrames = CSL_mMaxSndFileFrames;		///< max block size (set large for zooming scopes)

static unsigned mVerbosity = CSL_mVerbosity;					///< very verbose
static unsigned mLoggingPeriod = CSL_mLoggingPeriod;			///< log CPU every 15 sec
static unsigned mOutPort = CSL_mOutPort;						///< RFS output port
static string mDataFolder = CSL_DATA_DIR;						///< User's CSL data folder ()
static bool mStopNow = false;									///< flag to stop threads and timers

static unsigned sScreenWidth;									///< screen width is global (so you can reference it without including this file)
static unsigned sScreenHeight;									///< screen height is global

/// CGestalt Accessors for system constants

unsigned CGestalt::frameRate() { return mFrameRate; }
float CGestalt::frameRateF() { return mFrameRateF; }
csl::sample CGestalt::framePeriod() { return mFramePeriod; }
unsigned CGestalt::maxBufferFrames() { return mMaxBufferFrames; }
unsigned CGestalt::sndFileFrames() { return mSndFileFrames; }
unsigned CGestalt::maxSndFileFrames() { return mMaxSndFileFrames; }
unsigned CGestalt::blockSize() { return mBlockSize; }
unsigned CGestalt::numInChannels() { return mNumInChannels; }
unsigned CGestalt::numOutChannels() { return mNumOutChannels; }
unsigned CGestalt::verbosity() { return mVerbosity; }
unsigned CGestalt::loggingPeriod() { return mLoggingPeriod; }
unsigned CGestalt::outPort()	{ return mOutPort; }

bool CGestalt::stopNow()	{ return mStopNow; }

unsigned CGestalt::screenWidth() { return sScreenWidth; }							///< current screen width
unsigned CGestalt::screenHeight() { return sScreenHeight; }							///< current screen height

// handle dataFolder paths relative to "~"

string CGestalt::dataFolder()	{
	if ((mDataFolder.size() > 0) && (mDataFolder[0] == '~')) {	    // find '~' in folder name
		string ans(getenv("HOME"));								// UNIX code here
		if (mDataFolder.size() > 1)
			ans = ans + mDataFolder.substr(1);
		return ans;
	} else
		return mDataFolder;										// else answer mDataFolder
}

void CGestalt::setFrameRate(unsigned sampleRate) {
	mFrameRate = sampleRate;
	mFrameRateF = (float) sampleRate;
	mFramePeriod = 1.0f / mFrameRateF;
}

// General setters

void CGestalt::setMaxBufferFrames(unsigned numFrames) { mMaxBufferFrames = numFrames; }
void CGestalt::setMaxSndFileFrames(unsigned numFrames) { mMaxSndFileFrames = numFrames; }
void CGestalt::setSndFileFrames(unsigned numFrames) { mSndFileFrames = numFrames; }
void CGestalt::setBlockSize(unsigned blockSize) { mBlockSize = blockSize; }
void CGestalt::setNumInChannels(unsigned numChannels) { mNumInChannels = numChannels; }
void CGestalt::setNumOutChannels(unsigned numChannels) { mNumOutChannels = numChannels; }
void CGestalt::setVerbosity(unsigned verb) { mVerbosity = verb; }
void CGestalt::setLoggingPeriod(unsigned valInSecs) { mLoggingPeriod = valInSecs; }
void CGestalt::setOutPort(unsigned numChannels) { mOutPort = numChannels; }
void CGestalt::setDataFolder(std::string dataFolder) { mDataFolder = dataFolder; }

void CGestalt::setStopNow()  { mStopNow = true; }
void CGestalt::clearStopNow()  { mStopNow = false; }

void CGestalt::setScreenWidth(unsigned numPixels) { sScreenWidth = numPixels; }
void CGestalt::setScreenHeight(unsigned numPixels) { sScreenHeight = numPixels; }

// get init file name

string initFileName() {
	string nam(CSL_INIT_FILE);
	if (nam[0] == '~') {
		string ans(getenv("HOME"));								// UNIX code here
		if (nam.size() > 1)
			ans = ans + nam.substr(1);
		return ans;
	} else
		return (string(CSL_INIT_FILE));
}

///< read/write the init file (typ. ~/.cslrc)
/// The reader takes a char key, as in 
///		string initMsg(CGestalt::initFileText('T'));
/// which looks for a line starting with "T " in the file and returns the part after the "T "
/// The write function takes a key and a string to store. 
///

string CGestalt::initFileText(char key) {
	FILE * inp = fopen(initFileName().c_str(), "r");
	if ( ! inp)
		return string("");
	char init[CSL_LINE_LEN];
	while ( ! feof(inp)) {						// loop through input file
	    if(fgets(init, CSL_LINE_LEN, inp) != NULL) {		// Read a line
		if (init[0] == key) {					// if the key matches, return it
			char * str = &init[2];
			return string((const char *)str);
		}
	    }
	}
	fclose(inp);
	return string("");
}						

// Store a string at a key in the init file

void CGestalt::storeToInitFile(char key, string data) {
//	logMsg("Store init %c -> %s", key, data.c_str());
	char rcStr[CSL_STR_LEN];
	char * rcLin = rcStr;
	char * cr;
	size_t len;
	string iNam = initFileName();
	FILE * inp = fopen(iNam.c_str(), "r+");		// open file
	if (inp) {
		len = fread(rcStr, 1, CSL_STR_LEN, inp);// read whole file
		rewind(inp);							// seek back to start
	} else {									// if file not there
		inp = fopen(iNam.c_str(), "w");
		len = 0;
	}
	fprintf(inp, "%c %s\n", key, data.c_str());	// write key line
	while (rcLin < (rcStr + len)) {
		cr = strchr(rcLin, '\n');				// find CR
		if (cr)
			*cr = 0;							// set it to 0
		if (rcLin[0] != key)					// copy non-matching lines
			fprintf(inp, "%s", rcLin);
		if (cr)
			rcLin = cr + 1;
		else 
			rcLin += strlen(rcLin);
	}
	fclose(inp);
}

// Handle file recording and output

#define cheapPrintf(val)				\
	if (val < 10)						\
		*xpos = '0';					\
	else								\
		*xpos = '0' + (val / 10);		\
	*(xpos+1) = '0' + (val % 10)

// create a new temp file name

string CGestalt::sndFileName() {
	char fsnam[CSL_NAME_LEN];				// temp file name
	FILE * dFile = NULL;					// temp file ptr
											// create out file name
	sprintf(fsnam, "%s%s", CGestalt::dataFolder().c_str(), OUT_SFILE_NAME);
	char * xpos = strstr(fsnam, "XX");		// find XX in the template
	int cnt = 0;
	if (xpos) {								// if found, replace it with a number
		do {
			cheapPrintf(cnt);				// pick next file name
			dFile = fopen(fsnam, "r");		// try to read file
			if (dFile != NULL) {			// if it's there
				fclose(dFile);				// close it and increment counter
				cnt++;
			}
		} while (dFile);						// as long as you find files
	} else {
		logMsg("Write to file \"%s\" filename not recognized", fsnam);
	}
	return string(fsnam);
}

// Logging functions

#pragma mark Logging

///
/// Logging code: if verbosity
///

// macro to process messasges that start with \n

#if 1
#define SWALLOW_CR()
#else
#define SWALLOW_CR()					\
	if (format[0] == '\n') {			\
		fprintf(stderr, "\n");			\
		format[0] = ' ';				\
	}
#endif

// Stream-based logging for RoCoCo -- for handling var-args

#ifdef RoCoCo
extern MemoryOutputStream * gLogStream;
#endif

void csl::vlogMsg(bool cz, LogLevel level, const char * format, va_list args) {
	switch (level) {
		case kLogInfo:
			if (mVerbosity < 3)		return;
		case kLogWarning:
			if (mVerbosity < 2)		return;
		case kLogError:
			if (mVerbosity < 1)		return;
        case kLogFatal:             /* no-op */ ;
	}
	char message[CSL_LINE_LEN];
	vsprintf(message, format, args);
	unsigned len = (unsigned) strlen(message);
	if (cz)
        sprintf(message + len++, "\n");
#ifdef RoCoCo
	gLogStream->write(message, len);
#endif
	fprintf(stderr, message);
	fflush(stderr);
	if (level == kLogFatal)
		exit(1);
}

/*
void csl::vlogMsg(bool cz, LogLevel level, const char * format, va_list args) {
	switch(level) {
		case kLogInfo:
			if (mVerbosity < 3)		return;
			SWALLOW_CR()
			fprintf(stderr, CSL_LOG_PREFIX);
			break;
		case kLogWarning:
			if (mVerbosity < 2)		return;
			SWALLOW_CR()
			fprintf(stderr, "%s%s", CSL_LOG_PREFIX, "Warning: ");		
			break;
		case kLogError:
			if (mVerbosity < 1)		return;
			SWALLOW_CR()
			fprintf(stderr, "%s%s", CSL_LOG_PREFIX, "Error: ");		
			break;
		case kLogFatal:
			SWALLOW_CR()
			fprintf(stderr, "%s%s", CSL_LOG_PREFIX, "FATAL ERROR: ");
			break;
	}
	vfprintf(stderr, format, args);
	if (cz)
        fprintf(stderr, "\n");
	fflush(stderr);
	if (level == kLogFatal)
		exit(1);
}
*/

// detailed calls

void csl::logMsg(const char * format, ...) {
	va_list args;
	va_start(args, format);
	vlogMsg(true, kLogInfo, format, args);
	va_end(args);
}

void csl::logMsgNN(const char * format, ...) {
	va_list args;
	va_start(args, format);
	vlogMsg(false, kLogInfo, format, args);
	va_end(args);
}

void csl::logMsg(LogLevel level, const char * format, ...) {
	va_list args;
	va_start(args, format);
	vlogMsg(true, level, format, args);
	va_end(args);
}

void csl::logMsgNN(LogLevel level, const char * format, ...) {
	va_list args;
	va_start(args, format);
	vlogMsg(false, level, format, args);
	va_end(args);
}

///< Log the file & line #

void csl::logLine() {
	// ToDo
}

///< log file/line as a URL

void csl::logURL() {
	// ToDo
}

#ifndef WIN32
#include <unistd.h>		// for usleep
#endif

///
/// Global Sleep functions that work for windows and mac/unix.
/// Note the use of the global flag gStopNow, which interrupts timers.

#pragma mark Misc

#define TIMER_INTERVAL 0.25f						// loop time in sec to check stopNow flag in sleep timers

// sleep for microseconds

bool csl::sleepUsec(float dur_in_usec) {
	if (dur_in_usec <= 0.0) return false;
	int interval;
	int periods = (int) ((dur_in_usec / 1000000.0f) / TIMER_INTERVAL);
	if (periods < 1) {
		interval = (int) dur_in_usec;
		periods = 1;
	} else {
		interval = (int) (TIMER_INTERVAL * 1000000.0f);		// or dur_in_usec / periods;
	}
	int count = periods;
	while (count && ( ! CGestalt::stopNow())) {		// timers count short loops of 0.25 sec or so
		usleep(interval);							// micro-sleep
		count--;
	}												// check global flag in timer loop
	if (CGestalt::stopNow()) return true;
	if ((interval * periods) < dur_in_usec)			// sleep for remainder interval
		usleep((int)(dur_in_usec - (interval * periods)));
	if (CGestalt::stopNow()) {
//		CGestalt::clearStopNow();					// should a timer clear the flag?
		return true;
	}
	return false;									// false means AOK termination
}

// sleep for milliseconds

bool csl::sleepMsec(float dur_in_msec) {
	if (dur_in_msec <= 0.0) return false;
	return sleepUsec(dur_in_msec * 1000.0f);
}

// sleep for seconds

bool csl::sleepSec(float dur_in_sec) {
	if (dur_in_sec <= 0.0) return false;
	return sleepUsec(dur_in_sec * 1000000.0f);
}

// current system or IO time in ticks

Timestamp csl::timeNow() {
#ifdef USE_JUCE
    return juce::Time::getHighResolutionTicks();
#else
	 struct timeval tv;
	 gettimeofday(&tv, NULL);
	 return ((Timestamp) tv.tv_sec * 1000) + ((Timestamp) tv.tv_usec / 1000);
#endif
}

// current system or IO time in seconds

float csl::fTimeNow() {
#ifdef USE_JUCE
    return (float) juce::Time::highResolutionTicksToSeconds(juce::Time::getHighResolutionTicks());
#else
	 struct timeval tv;
	 gettimeofday(&tv, NULL);
	 return (float) tv.tv_sec + ((float) tv.tv_usec / 1000000.0f);
#endif
}

//#endif

//// Randoms

// Answer a random as a float in the range 0 - 1

float csl::fRandZ(void) {
  return((float) rand() / (float) RAND_MAX);
}

// Answer a random as a float in the range -1 - +1

float csl::fRand1(void) {
  return ((csl::fRandZ() * 2.0f) - 1.0f);
}

// Answer a random as a float in the range min - max

float csl::fRandM(float minV, float maxV) {
  return (minV + (fRandZ() * (maxV - minV)));
}

// Answer a random as a float in the range base +- (range * base)

float csl::fRandR(float base, float range) {
  return (base + (fRandZ() * range * base));
}

// Answer a random as a float in the range base +- range

float csl::fRandB(float base, float range) {
  return (base + (fRand1() * range));
}

// Answer a random as a float in the range 0 - val

float csl::fRandV(float val) {
  return (csl::fRandZ() * val);
}

// Integer float fcns

// Answer a random as an int in the range 0 - val

int csl::iRandV(int val) {
	return (int)fRandV((float) val); 
}

// Answer a random as an int in the range min - max

int csl::iRandM(int minV, int maxV) { 
	return (int)fRandM((float) minV, (float) maxV); 
}

// Answer a random as an int in the range base +- range

int csl::iRandB(int base, int range) { 
	return (int)fRandB((float) base, (float) range);
}

// Answer true or false

bool csl::coin() {
	return (csl::fRandZ() > 0.5f);
}

// Answer true or false with a bias (1 = always true)

bool csl::coin(float bias) {
	return (csl::fRandZ() < bias);
}

///  keyToFreq -- converts from MIDI key numbers (1 - 127) to frequency in Hz.
//		8.17579891564371 Hz is a very low C = MIDI note 0
	
float csl::keyToFreq(unsigned midiKey) {
		return (8.17579891564371f * powf(2.0f, (midiKey / 12.0f)));
}

///	freqToKey -- converts from frequency in Hz to MIDI key #
/// 8.17579891564371 Hz is MIDI key 0

unsigned csl::freqToKey(float frequency) {
	return (unsigned) (12.0f * log2f(frequency / 8.17579891564371f));
}

//
// MVC Observable pattern implementations
//

#pragma mark MVC

// attach/remove observers from models

void Model::attachObserver (Observer * o) {
	mObservers.push_back(o);
	mHasObservers = true;
//	mUpdateTime = fTimeNow();
	if (o->mPeriod != 0) {				// if the observer has an update period
		mPeriod = o->mPeriod;
	}
	if (o->mKey != 0) {					// if the observer has an update filter key
		mHasObserverMap = true;
		mObsMap[o->mKey].push_back(o);
	}
#ifdef CSL_DEBUG
	logMsg("Model::attachObserver %x", o);
#endif
}

void Model::detachObserver (Observer * o) {
	unsigned count = mObservers.size();
	int i;
	for (i = 0; i < count; i++)
		if (mObservers[i] == o)
			break;
	if (i < count)
		mObservers.erase(mObservers.begin() + i);
	if (mObservers.size() == 0)
		mHasObservers = false;
//	if (o->mKey != 0) {					// if the observer has an update filter key
//		mObsMap[o->mKey].delete(o);
//	}		
#ifdef CSL_DEBUG
	logMsg("Model::detachObserver");
#endif
}

// This is the (simple) body of the main notification method;
// send this->changed(* any_object) from within model messages
// that want to trigger the observers

void Model::changed(void * argument) {
	if ( ! mHasObservers)					// speed hack
		return;
	ObserverVector::iterator pos;		// iterate over my observers
#ifdef CSL_DEBUG
	logMsg("Model::update %d", mObservers.size());
#endif
	//float nowt = fTimeNow();
	//if (nowt > (mUpdateTime + mPeriod)) {
			//mUpdateTime = nowt;
		if (mHasObserverMap) {
			int key = this->evaluate(argument);
			ObserverVector obs = mObsMap[key];
			for (pos = obs.begin(); pos != obs.end(); ++pos)
				(* pos)->update(argument);
		} else {
			for (pos = mObservers.begin(); pos != mObservers.end(); ++pos)
				(* pos)->update(argument);
		}
	//}
}
