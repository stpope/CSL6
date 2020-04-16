///
///  CSL_Types.h -- the top-most include file for CSL 5.0 (September, 2009).
///     Updated for CSL 6.0 April, 2020
///
///	Central typedefs and platform-specific macros, also includes the model & observer classes.
/// This is normally included via CSL_Core.h or CSL_Includes.h.
///
///	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
///
/// CSL Coding Conventions
///		Naming
///			Class, member, and method names are written in "camelCase" as in "UnitGenerator."
///			Class names are capitalized, but members and methods are written lower-camel-case.
///			Data members (instance variables) are written with initial "m" then camelCase as in "mOffset."
///			Enumeration constants are written with initial "k" followed by embedded caps as in "kDone."
///			Compiler flags and #defines are generally written all-upperr-case as in "USE_CASND."
///		Privacy
///			In general data members are protected and have accessor functions where appropriate. 
///			The one exception is Buffer which is considered a record class and has public members.
///
/// Compile-time options
/// (These are not set in this file so that different applications can share the source tree.)
///
///		SoundFile type:
///			USE_JSND - use the JUCE-based sound file class (requires only JUCE)
///			USE_LSND - use libSndFile-based sound file class (supports many file types)
///			USE_CASND - use the CoreAudio version (runs on iPhone)
///
///		FFT implementation
///			USE_FFTW - use FFTW 3 (faster but complicated to build)
///			USE_FFTREAL - use FFTReal (smaller and simpler)
///			USE_KISSFFT	- use KISS FFT (smaller, untested)
///
///		Thread implementation
///			USE_JTHREADS - use the JUCE thread classes (otherwise use pthreads)
///
///		MIDI API
///			USE_JMIDI - use JUCE's MIDI I/O
///
///		OSC API
///			USE_LOSC - use LibLo for OSC (assumes liblio 0.26 or greater is installed)
///			USR_KOSC - use OSCKit for OSC (not implemented yet)
///
///	CORE CSL Types
///		sample(float), Sample, SampleBuffer, SampleBufferVector, SampleComplex, SampleComplexVector
///		PortMap, UGenVector, UGenMap, IODeviceVector, Timestamp, VoidFcnPtr
///
///	Constants
///		CSL_PI, CSL_TWOPI, CSL_PIHALF, CSL_SQRT_TWO, CSL_SPEED_OF_SOUND, 
///		CSL_EXP_PER_DB, CSL_SAMPS_PER_METER, CSL_DEGS_PER_RAD
///
///		Many useful macros below
///

#ifndef CSL_Types_H							// All CSL header files can be included only once.
#define CSL_Types_H							// That's what these flags are for.

//// Platform macros -- check for these (e.g., CSL_WINDOWS rather than WIN32) in your code

#ifdef Linux								// Linux
	#define CSL_LINUX 
#endif

#ifdef WIN32								// M$_Windows
	#define CSL_WINDOWS
#endif

#if defined __APPLE__						// Mac OSX
	#define CSL_MACOSX
#endif

#if defined (ANDROID)						// Android
	#define CSL_ANDROID
#endif

#ifdef JUCE_IOS								// iOS
	#define CSL_IOS
#endif

//// Global definitions

#define CSL_ENUMS				// define this to use the various enumerations (which are problematic with SWIG)
// #define CSL_DEBUG			// define this for very verbose debugging of constructors and call-backs
// #define CSL_DSP_BUFFER		// define this for Buffer Sample Processing (feature extraction)
// #define USE_JUCE				// use JUCE for all IO (now set as a compile-time flag)

////
//// Major System Defaults (used in CGestalt to set static constants)
////

#define CSL_mFrameRate 44100				///< default sample rate (tested up to 96000)

#ifdef IPHONE
	#define CSL_mBlockSize 256				///< I/O block size (set to 64-2048 in typical usage)
	#define CSL_mMaxBufferFrames 256		///< max block size
	#define CSL_mSndFileFrames 2000000		///< default file cache size = 2 MB
	#define CSL_mMaxSndFileFrames 2000000	///< max file cache size = 2 MB
#else										///  normal hosts
	#define CSL_mBlockSize 512				///< I/O block size (set to 64-2048 in typical usage)
	#define CSL_mMaxBufferFrames 8192		///< max block size (set large for zooming scopes)
	#define CSL_mSndFileFrames 20480000		///< default file cache size = 20 MFrames (~ 2 min)
	#define CSL_mMaxSndFileFrames 64000000	///< max file cache size = 64 MB (set to a large value)
#endif

//#define READ_IO_PROPS						///< overwrite the system frame rate and block size from the 
											///  selected hardware interface at startup time

#define DEFAULT_WTABLE_SIZE CGestalt::maxBufferFrames()	///< size of wavetables, or use blockSize?

#define CSL_mVerbosity 3					///< very verbose logging
#define CSL_mLoggingPeriod 10				///< log CPU usage every N sec

#define CSL_LOG_PREFIX "-- "				///< prefix for log msgs (may be empty)
// #define CSL_LOG_PREFIX "\nCSL: "			///< verbose logging tags

#define CSL_mOutPort 57123					///< RFS output port
#define CSL_mOSCPort 54321					///< OSC input port

#define CSL_WORD_LEN 32						///< default short string length 
#define CSL_DEF_LEN 128						///< default medium string length 
#define CSL_NAME_LEN 256					///< default string length 
#define CSL_LINE_LEN 512					///< default line length
#define CSL_STR_LEN 1024					///< default long string length

#define SAMPS_TO_WRITE (44100 * 30)			///< record 30 seconds by default
#define OUT_SFILE_NAME "XX_csl.aiff"		///< csl output file name temlpate
//#define OUT_SFILE_NAME "CSLXX.aiff"		///< csl output file name temlpate

// Data folder and init file locations

#ifdef CSL_WINDOWS
	#define CSL_DATA_DIR "..\\..\\..\\CSL_Data\\"	///< folder where the CSL data can be found
	#define CSL_INIT_FILE "..\\..\\..\\csl.ini"		///< where to store the CSL init file
#else
	#define CSL_DATA_DIR "~/Code/CSL/Data/"		///< folder where the CSL data can be found
	#define CSL_INIT_FILE "~/.cslrc"				///< where to store the CSL init file
	#define DO_TIMING								///< Gather performance timing (not on Windows)
#endif

//// Which Soundfile class to use? (choose with a compiler option, e.g., -DUSE_LSND)

#ifdef USE_JSND
	#define SoundFile JSoundFile			// JUCE snd file class
#endif
#ifdef USE_LSND
	#define SoundFile LSoundFile			// libSndFile
#endif
#ifdef USE_CASND
	#define SoundFile CASoundFile			// CoreAudio
#endif

#define USE_SNDFILEBUFFER					// add snd file buffer class

//// Which MIDI IO port class to use? 
//// (define here or choose with a compiler option, e.g., -DUSE_PMIDI)

//#define USE_JMIDI						// this is now a compile-time option, like the FFT and sndfile libs
#define DEFAULT_MIDI_IN 0
#define DEFAULT_MIDI_OUT 0

//// Use an OSC API?

#ifndef CSL_WINDOWS						// works on Mac & Linux
	#define USE_LOSC						// liblo for OSC
#endif

//// Which default "Osc" class to use -- WavetableOscillator (table-lookup) or Sine (computed)?

#define Osc WavetableOscillator			// default "Osc" -- or use Sine?

// Which FFT wrapper class to use? (choose with a compiler option, -DUSE_FFTREAL)

//#define USE_FFTW						// use FFTW (faster but complicated to build, also MIT license)
//#define USE_FFTREAL						// use FFTReal (smaller and simpler, also public domain)

////////////////////////// Core CSL Typedefs //////////////////////////////////////////////

#include <vector>							///< we use the STL vector, map, and string classes
#include <string>	
#include <map>

namespace csl {							///< All of CSL takes place within the "csl" namespace

//// CSL base audio and geometrical data type declarations - PAY ATTENTION ///////

                                        // float array typedefs
typedef float * FloatArray;					///< float pointer type
typedef FloatArray * FloatArrayPtr;			///< float pointer pointer type

                                        // short-hand for the base sample type
typedef float sample;						///< (could be changed to int, or double)
typedef float Sample;						///< the same, written upper-case

typedef sample* SampleBuffer;				///< 1-channel buffer data type, vector of (sample)
typedef SampleBuffer* SampleBufferVector;	///< Multi-channel buffer data type, vector of (SampleBuffer)
typedef SampleBuffer* SampleBufferArray;		///< Multi-channel buffer data type

typedef sample SampleComplex[2];				///< array-of-2 complex # type (like FFTW)
#define cx_r(val) val[0]						///< complex # accesor macros
#define cx_i(val) val[1]
#define ComplexPtr SampleBuffer				///< shorthand
typedef SampleComplex * SampleComplexVector;	///< complex vector
typedef SampleComplex * SampleComplexPtr;		///< complex pointer
typedef SampleComplexPtr * SampleComplexMatrix;	///< complex 2D matrix
typedef SampleComplexPtr * SampleComplexMatrix;	///< complex 2D matrix

//typedef struct { sample re; sample im; } SampleComplex;	///< alternate Complex # struct
//#define cx_r(val) val.re					/// complex # accesor macros
//#define cx_i(val) val.im
//#define ComplexPtr SampleComplex				///< shorthand

typedef SampleComplex* SampleComplexVector;	///< complex vector
typedef SampleComplex* SampleComplexPtr;		///< complex pointer
 
class CPoint;								///< Forward declaration
typedef std::vector <CPoint *> PointVector;	///< A vector of points

typedef std::vector<unsigned> UnsignedVector;	///< A vector of unsigneds
typedef std::vector<float> FloatVector;		///< A vector of floats

typedef void * VoidFcnPtr(void * arg);		///< the generic void fcn pointer
typedef void VoidFcnPtrN(void);				///< the truly void fcn pointer

typedef unsigned uint;

//// I/O and control port map types ------------------------------------------

class Buffer;								///< Forward declaration to Buffer (in CSL_Core.h)
class UnitGenerator;							///< Forward declaration to UnitGenerator (in CSL_Core.h)
class Port;									///< Forward declaration to Port
class IODevice;								///< Forward declaration to IO Device model
class Instrument;							///< Forward declaration to Instrument
class Observer;								///< Forward declaration

typedef unsigned CSL_MAP_KEY;					///< the type I use for map keys (could also be a string)
                                            ///< PortMap: a map between a name/key and a port object
                                            ///< (used for control and audio inputs)
typedef std::map <CSL_MAP_KEY, Port *> PortMap;
												/// Buffer/UGenVector: unit generator pointers (used for outputs)
typedef std::vector <Buffer *> BufferVector;
typedef std::vector <UnitGenerator *> UGenVector;
												/// UGenMap: a named map of unit generators (used for GUIs)
typedef std::map <std::string, UnitGenerator *> UGenMap;
												// these are used in mixers
typedef std::map <unsigned, UnitGenerator *> UGenIMap;
typedef std::map <unsigned, float> FloatMap;
typedef std::vector <float> FloatVector;
												/// IOs hold onto device vectors
typedef std::vector <IODevice *> IODeviceVector;
												/// Players hold onto Instrument vectors/maps
typedef std::vector <Instrument *> InstrumentVector;
typedef std::map <int, InstrumentVector> InstrumentLibrary;
typedef std::map <std::string, Instrument *> InstrumentMap;

typedef std::map <std::string, std::string> StringMap;

/// Timestamp type: we assume that we can get the host's best guess at the IO word clock
///  (normally passed into the audio IO callback function). call timeNow() to get the time
/// We also support float-time in fTimeNow()

typedef unsigned long Timestamp;

/// struct used for the JUCE pop-up menu of tests (see the test files)

typedef struct {
	const char * name;		// test name (menu item)
	VoidFcnPtrN * fcn;		// test void fcn ptr
	const char * comment;	// test comment
} testStruct;

//// Hashmap keys for the default I/O ports (could be strings or int indeces)

#define CSL_SCALE 1
#define CSL_OFFSET 2
#define CSL_INPUT 3
#define CSL_OPERAND 4
#define CSL_OPERAND2 5
#define CSL_FREQUENCY 6
#define CSL_POSITION 7
#define CSL_POSITIONX 7
#define CSL_POSITIONY 8
#define CSL_POSITIONZ 9
#define	CSL_FILTER_FREQUENCY 10
#define	CSL_FILTER_AMOUNT 11
#define CSL_RATE 12

////
//// Min/max, Boolean, statistics macros
////

#ifndef csl_min							// csl_min(a, b)
#define csl_min(a, b)	(((a) < (b)) ? (a) : (b))
#endif

#ifndef csl_max							// csl_max(a, b)
#define csl_max(a, b)	(((a) > (b)) ? (a) : (b))
#endif

// inclusive between-ness

#define csl_between(a, b, c)	(((a) >= (b)) && ((a) <= (c)))

// max/min ratio macros

#define csl_max_r(a)	(((a) > 1.0f) ? (a) : (1.0f/a))
#define csl_min_r(a)	(((a) < 1.0f) ? (a) : (1.0f/a))

#define csl_abs(a)		(((a) >= 0) ? (a) : (-a))

// min/max ratio of 2 values

#define csl_max_d(a, b)	((a > b) ? (a / b) : (b / a))
#define csl_min_d(a, b)	((a < b) ? (a / b) : (b / a))

// ratio/remainder -- is v1 ~= N * v2?

#define csl_ratio_rem(val1, val2, remainder)		\
	remainder = csl_max_d(val1, val2);				\
	remainder -= (int) remainder;					\
	if (remainder > 0.5f) {						\
		remainder = csl_max_r(rem);				\
		remainder -= 1.0f;						\
	}

//// TRUE/FALSE

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

//// Numerical constants (for convenience, all floats)

#define CSL_PI					3.1415926535897933f
// #define CSL_PI 3.141592653589793238462643383279502884197169399375108209749445923ld
#define CSL_TWOPI				6.2831853071795865f
#define CSL_PIHALF				1.570796326795f
#define CSL_SQRT_TWO			1.414213562f
#define CSL_SPEED_OF_SOUND		330.0f					// m/sec
#define CSL_EXP_PER_DB			0.11512925464970228f
#define CSL_SAMPS_PER_METER		133.63636363636364f
#define CSL_DEGS_PER_RAD		57.295779513082321f

///
/// Typedefs for basic types that differ across platforms
///

#ifdef CSL_WINDOWS			// Microsoft is explicit
	#include "stdafx.h"
							// Hypotenuse = complex-to-real-magnitude
	#define hypotf(av, bv)	sqrtf((av * av) + (bv * bv))

#ifdef MSVS6				// ignore pragmas not understood by Microsoft Visual C++
	#pragma warning(once:4068 4244 4305 4355)
	#pragma warning(once:4290)
#endif

//#else
//							// GCC long long is 64 bits on both 32 and 64 bit operating systems (long is only 64 on 64)
//	typedef signed long long INT64;
//	typedef unsigned long long UINT64;

#endif // CSL_WINDOWS

} // end of namespace

#endif // _CSLTypes_H
