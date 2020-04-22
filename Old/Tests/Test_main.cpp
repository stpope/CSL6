//
//	Test_main.cpp -- C main functions for the basic CSL4 tests.
//
//	The full list of tests is in the Test_all.cpp file.
//	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
//
// This program simple reads the run_tests() function (at the bottom of this file)
//  and executes a list of basic CSL tests
//
// This is a subset of the tests in Test_all; we use this file for debugging or demos.
//

#define USE_TEST_MAIN			// use the main() function in test_support.h

#include "Test_Support.cpp"		// include all of CSL core and the test support functions

using namespace csl;

/////////////////////// Here are the actual unit tests ////////////////////

// Simplest sine wave

void test_sin() {
	Sine vox(220);
	logMsg("playing sin...");
	runTest(vox, 3.0);
	logMsg("sin done.");
}

// FM with envelopes

void testADSR_FM() {
	ADSR a_env(3, 0.01, 0.1, 0.5, 1);				// set up 2 standard ADSRs (3-sec duration)
	ADSR i_env(3, 1.0, 0, 1, 1.4);
	Sine car(110);							// init the 2 oscillators (fc = fm)
	Sine mod(110);
	i_env.setScale(110);						// scale the index envelope by the mod. freq
	mod.setScale(i_env);
	mod.setOffset(110);
	car.setFrequency(mod);
	car.setScale(a_env);
	i_env.trigger();	
	a_env.trigger();	
	logMsg("playing ADSR FM\n");
	car.dump();
	runTest(car);
	logMsg("done.\n");
}

//////// RUN_TESTS Function ////////

void runTests() {
//	test_sin();				// Simple sine demo tests
	testADSR_FM();			// Example of FM
//	testPan();				// panner
}
