///
///  CGestalt.h -- the CSL 6.0 CGestalt (system utility) class specification,
///		system constants, logging, and utility methods; SAFE_MALLOC/SAFE_FREE macros
///		The logging system has a number of print methods that use the printf var-args format 
///		and can be filtered on verbosity.
///
///		There's a variety of random-number functions.
///
///		The timer sleep methods are interruptable with a global flag.
///
/// The MVC Observer/Subject or dependency pattern classes are also here.
///
///	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
///

#ifndef CSL_Gestalt_H
#define CSL_Gestalt_H

#include "CSL_Types.h"		// The system default values are defined there
#include "CSL_Exceptions.h"	// The system default values are defined there

#include <stdarg.h>			// for va_arg, vaList, etc.
#include <string>

#ifdef USE_JUCE
	#include "JuceHeader.h"
//	#include "modules/juce_core/time/juce_Time.h"
//	#include <sys/time.h>
#else
	#include <sys/time.h>
#endif

namespace csl {

///
/// The CSL system defaults class
///

class CGestalt {						

public:												// Accessor (getter/setter) methods

	static unsigned frameRate();					///< default frame rate
	static float frameRateF();						///< default frame rate as a float
	static void setFrameRate(unsigned frameRate);
	static sample framePeriod();					///< default frame rate

	static unsigned numInChannels();				///< default number of input channels
	static void setNumInChannels(unsigned numChannels);

	static unsigned numOutChannels();				///< default number of output channels
	static void setNumOutChannels(unsigned numChannels);

	static unsigned blockSize();					///< the default block size
	static void setBlockSize(unsigned blockSize);

	static unsigned maxBufferFrames();				///< the max num frames that can be requested
	static void setMaxBufferFrames(unsigned numFrames);

	static unsigned maxSndFileFrames();				///< the max num frames that can be cached
	static void setMaxSndFileFrames(unsigned numFrames);

	static unsigned sndFileFrames();				///< the default num frames that are cached
	static void setSndFileFrames(unsigned numFrames);

	static unsigned verbosity();					///< the default logging verbosity 
	static void setVerbosity(unsigned verbosity);	// (0 = only fatal errors, 1 = warnings, 
													// 2 = most msgs, 3 = include info msgs)

	static unsigned loggingPeriod();				///< the default logging period
	static void setLoggingPeriod(unsigned loggingPeriod);

	static unsigned outPort();						///< the default RemoteIO output port
	static void setOutPort(unsigned outPort);

	static std::string dataFolder();				///< the default directory for test data
	static void setDataFolder(std::string dFolder);	///< typ. "~/Code/CSL/CSL_Data/"
	
	static bool stopNow();							///< flag to stop threads and timers
	static void setStopNow();
	static void clearStopNow();
	
	static std::string initFileText(char key);		///< read/write the init file (typ. ~/.cslrc)
	static void storeToInitFile(char key, std::string data);
	
	static std::string sndFileName();				///< pick a new sound file name to use based on OUT_SFILE_NAME

	static unsigned screenWidth();					///< current screen width
	static void setScreenWidth(unsigned numPixels);
	static unsigned screenHeight();					///< current screen height
	static void setScreenHeight(unsigned numPixels);

};

/////////////////////////////////////////////////////////////////////////
///
/// Useful Macros
///

/// SAFE_MALLOC = safe malloc using new operator
/// used like: SAFE_MALLOC(mSampleBuffer, sample, mWindowSize);

#define SAFE_MALLOC(ptr, type, len)						\
	ptr = new type[len];								\
	if ((char *) ptr == NULL)							\
        throw MemoryError("can't allocate buffer");     \
    bzero(ptr, sizeof(type) * len)

/// Matrix allocate
///	SAFE_MATMALLOC(mRealSpectrum, FloatArray, mNumFrames, float, mWinSize);
/// SAFE_MATMALLOC(mSpectBands, FloatArray, mNumFrames, float, NUM_BANDS)

#define SAFE_MATMALLOC(ptr, mtype, num, type, len)		\
	ptr = new mtype[num];								\
	if ((char *) ptr == NULL)							\
		throw MemoryError("can't allocate buffer");		\
	for (unsigned i = 0; i < num; i++) {				\
		ptr[i] = new type[len];							\
		if ((char *) ptr[i] == NULL)					\
			throw MemoryError("can't allocate buffer");	\
	}

/// Structure allocate
///	SAFE_STRUCTMALLOC(mRealSpectrum, FloatArray, mNumFrames, float);

#define SAFE_STRUCTMALLOC(ptr, mtype, num, type)		\
	ptr = new mtype[num];								\
	if ((char *) ptr == NULL)							\
		throw MemoryError("can't allocate buffer");		\
	for (unsigned i = 0; i < num; i++) {				\
		ptr[i] = new type;								\
		if ((char *) ptr[i] == NULL)					\
			throw MemoryError("can't allocate buffer");	\
	}

// Safe free and matrix version

#define SAFE_FREE(ptr)									\
if (ptr) {											    \
        delete[] ptr;                                   \
        ptr = 0; }

#define SAFE_MATFREE(ptr, num)							\
	for (unsigned i = 0; i < num; i++) {				\
		if (ptr[i])										\
			delete[] ptr[i];							\
	}													\
	delete[] ptr

/////////////////////////////////////////////////////////////////////////
///
/// Logging functions are standard C functions
///


///
/// These are the public logging messages
///

// #define LOG_W_PRINTF			// this doesn't work the way I want it to...
	
#ifdef LOG_W_PRINTF
	#define kLogInfo
	#define kLogWarning
	#define kLogError
	#define kLogFatal 
	#define logMsg(...) { printf(__VA_ARGS__); printf("\n"); }
	#define logMsg(...) { printf(__VA_ARGS__); printf("\n"); }
	#define logMsgNN(...) { printf(__VA_ARGS__); }
	#define logMsgNN(...) { printf(__VA_ARGS__); }
#else
	
#ifdef CSL_ENUMS
	typedef enum {					///< Enumeration for log message severity level
		kLogInfo,					///< you can filter on this.
		kLogWarning,
		kLogError,
		kLogFatal
	} LogLevel;
#else
#define kLogInfo 0
#define kLogWarning 1
#define kLogError 2
#define kLogFatal 3
	typedef int LogLevel;
#endif

void logMsg(const char * format, ...);		///< default is kLogInfo severity
void logMsg(LogLevel level, const char* format, ...);
    
void logMsgNN(const char * format, ...);	///< no-newline versions
void logMsgNN(LogLevel level, const char* format, ...);

#endif

void logLine();								///< Log the file & line #
void logURL();								///< log file/line as a URL

void vlogMsg(bool cz, LogLevel level, const char * format, va_list args);


/////////////////////////////////////////////////////////////////////////
///
/// Misc. global functions in the csl namespace
///

/// Sleep for micro-seconds, milli-seconds or seconds
/// These are interruptable and return true if interrupted; false for normal termination

bool sleepUsec(float dur);					///< sleep for dur usec, msec or sec
bool sleepMsec(float dur);
bool sleepSec(float dur);

Timestamp timeNow();						///< high-accuracy system or IO time in ticks
float fTimeNow();							///< system or IO time in seconds

/// Which kind of accurate timer to use?

// #define C_TIME		Time::getHighResolutionTicks()
#define C_TIME		juce::Time::getMillisecondCounter()

///
/// A variety of useful random-number functions
///

float fRandZ(void);							///<  0 - 1			(name: zero)
float fRand1(void);							///< -1 - 1			(one)
float fRandV(float val);					///< 0 - v			(val)

float fRandM(float minV, float maxV);		///< min - max		(min/max)
float fRandR(float base, float range);		///< b +- (r * b)   (range)
float fRandB(float base, float range);		///< b +- r			(base)

/// Integer rands

int iRandV(int val);						///< 0 - v			(val)
int iRandM(int minV, int maxV);				///< min - max		(min/max)
int iRandB(int base, int range);			///< b +- r			(base)

// Bool coin-toss rands

bool coin();								///< Answer true or false
bool coin(float bias);						///< Answer with a bias (1 --> always true)


/// MIDI Conversions

///  keyToFreq -- converts from MIDI key numbers (1 - 127) to frequency in Hz.

float keyToFreq(unsigned midiKey);

///	freqToKey -- converts from frequency in Hz to MIDI key #

unsigned freqToKey(float frequency);


/////////////////////////////////////////////////////////////////////////
///
/// MVC Observer/Subject or dependency pattern
///		Advanced version with filtered observer updates
///

class Observer;								///< Forward declaration
typedef std::vector <Observer *> ObserverVector;

///
/// The Model/Observable/Subject class; instances of its subclasses should send themselves,
///			this->changed(some_data);
///	on "relevant" state changes; the code they inherit (from Model) manages updating the list 
/// of observer/dependent objects in that they each receive
///			update(some_data);
/// and can access the model-passed data (the model might pass "this").
///
/// Note the addition of timers, so observers can request no more than a certain rate of updates.

class Model {
public:
	Model() : mHasObservers(false), mHasObserverMap(false), mUpdateTime(0), mPeriod(0) { };	///< constructor
	virtual ~Model() { /* no-op */ };		///< (possibly notify obersvers on my death)
	
	void attachObserver(Observer *);		///< register/remove observers
	void detachObserver(Observer *);
	
	void changed(void * argument);			///< this is what I send to myself to notify my observers;
											/// It's not overridden in general.
											/// It results in the observers receiving update() calls
											///< override evaluate to filter updates to the observer map
	virtual int evaluate(void * argument) { return 0; };
	
private:
	ObserverVector mObservers;				///< the private vector of observers
	std::map<int, ObserverVector> mObsMap;	///< the private map-by-key of observers
	bool mHasObservers;						///< whether there are any observers (for fast checking)
	bool mHasObserverMap;					///< whether there are any observers (for fast checking)
	float mUpdateTime;						///< when I last updated
	float mPeriod;							///< update rate in sec
};

///
/// CSL Observer/Dependent/Monitor class -- instances receive update() messages from "models" 
///	and handle some argument, which may be the model's "this" pointer, or any other data.
///
///	Subclasses must override the update() method to do their stuff -- grabbing a data buffer, 
/// operating on their model, or displaying something.
///
///		The mPeriod is the requested period for updates.
///
///		The mKey is an int value for the model's evaluate() function to filter updates
///		i.e., if the model implements evaluate(void*), and its return value is non-zero and 
///		matches the observer's key, then an update will be sent. An example of using this is
///		a MIDI IO object's channel filtering for MIDI listeners (observers).
///

class Observer {
public:
	Observer() { mPeriod = 0; mKey = 0; };	///< constructor
	virtual ~Observer() { };				///< virtual destructor (don't notify or delete 
											///  observers in this version)
	float mPeriod;							///< max update rate
	int mKey;								///< key selector (e.g., MIDI chan)
											/// I receive update() mesages from my model;
	virtual void update(void * arg) = 0;	///< this will be implemented in subclasses.
};

} // end of namespace

#endif
