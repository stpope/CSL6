//
// Test_Support.h -- some support functions for the CSL testing main()s
//	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
//

#include "CSL_Includes.h"		// include all of CSL core

// Instruments

#include <AdditiveInstrument.h>
#include <FMInstrument.h>
#include <StringInstrument.h>
#include <SndFileInstrument.h>

//#include "JackIO.h"			// JACK IO

#ifdef USE_JUCE
	#define USE_JUCEIO  		// uncomment this to use the JUCE IO
#else
	#define USE_PAIO			// uncomment this to use the PortAudio IO
//	#define USE_FILEIO			// uncomment this to write to AIFF files
#endif

#ifdef USE_JUCEIO
    #define IO_CLASS IO
#endif

#ifdef USE_CAIO
	#include "CAIO.h"			// CoreAudio IO
	#define IO_CLASS CAIO
#endif

#ifdef USE_PAIO
	#include "PAIO.h"			// PortAu dio IO
	#define IO_CLASS PAIO
#endif

#ifdef USE_FILEIO				// File tests
	#include "FileIO.h"			// SndFile IO
	#define IO_CLASS FileIO
#endif

#ifndef IO_CLASS
	234523pp4509762o8e76452		// flag error
	#define IO_CLASS IO			// this is the new default
#endif

using namespace csl;

#ifdef USE_TEST_MAIN			// use the generic main() function here

extern void runTests();			// this is the list of tests to run (see the test files)

////// MAIN //////

int main (int argc, const char * argv[]);

#else							// not USE_TEST_MAIN = Juce test fcn prototypes

extern IO_CLASS * theIO;

#endif

// Test the given DSP graph for the given amount of time

void runTest(UnitGenerator & vox, double dur);

// The default is to play for 3 seconds

void runTest(UnitGenerator & vox);

// dump-test -- good for debugging

void dumpTest(UnitGenerator & vox);

