//
// Test_Support.cpp -- some support functions for the CSL testing main()s
//	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
//
// Note that this file is not normally put into project or makefiles,
// since it is #included in the test's main program explicitly (once).
//
// Global IO and argument-processing support; usage() and run_tests() functions
//
// Default CSL main() options:
//
//  -r srate -b blksiz -o noch -i nich -v verbosity -l logperiod -p outport -u
//
//		-r srate		set frame rate to srate Hz
//		-b blksiz		set buffer block size to blksiz
//		-o nch			set # of output channels to nch
//		-i nch			set # of input channels to nch
//		-v verbos		set verbosity level to verbos: 0/1/2
//		-l logpd		set logging period to logpd seconds
//		-p port			set the RemoteIO output port to port
//		-u				print usage message and exit
//

#include "Test_Support.h"				// include my stuff
#include <stdlib.h>

using namespace csl;

IO_CLASS * theIO = 0;					// Here's the global IO instance

#ifdef USE_TEST_MAIN					// use the generic main() function here (otherwise we use a GUI component)

extern void runTests();					// this is the list of tests to run (see the test files)

////// MAIN //////

int main (int argc, const char * argv[]) {
	printf("\n");
	logMsg("Running CSL6 tests\n");		// print a log message
	theIO = new IO_CLASS;				// create the IO object
	theIO->open();						// open & start it
	theIO->start();
	runTests();							// call the function with the list of unit tests
	theIO->stop();						// stop & close the IO
	theIO->close();	
	exit(0);
}

#endif // USE_TEST_MAIN

// Test the given DSP graph for the given amount of time

void runTest(UnitGenerator & vox, double dur) {
	theIO->setRoot(vox);				// plug into the IO to make some sound
	sleepSec(dur);						// sleep for the desired duration
	theIO->clearRoot();					// clear the IO's root to make silence
	sleepSec(0.1);						// wait for it to stop
}

// The default is to play for 3 seconds

void runTest(UnitGenerator & vox) {
	runTest(vox, (double) 3.0);
}

// dump-test -- good for debugging

void dumpTest(UnitGenerator & vox) {
	theIO->stop();						// stop it
	theIO->setRoot(vox);				// set the root of the graph
	vox.dump();							// dump the root
	theIO->test();						// and test the callback
	theIO->clearRoot();					// make some silence
	theIO->start();						// stop it
}

//
// Command-line option parsing
//
// find_option() -- locate an option in the argv[] list
//		find_option(argc, argv, 't' str)
//			returns -1 if -t not found;
//			returns 0 of "-t" opt found without arg, 
//			returns 1 and sets ptr "str" to first opt after "-t" if opt and arg found
//
//		Usage example:
//			if (find_option(argc, argv, 't' str) > 0) 
//				float value = atof(str)'
//

//int find_option(int argc, const char ** argv, char flag, unsigned * datum) {
//	for (int i = 1; i < argc; i++ ) 	{
//		if ((argv[i]) && (argv[i][0] == '-')) {		// linear search, slow, but simple
//			if (argv[i][1] == flag) {				// if flag found
//				if ((i + 1) == argc) {				// if it's the last cmd-line option
//					datum = NULL;
//					return 0;
//				 } else {							// else of there's a possible follow-on arg
//					*datum = i+1;
//					return 1;
//				}
//			}
//		}
//	}
//	return -1;			// if not found
//}
