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

#include "Test_Support.cpp"		// include all of CSL core and the test support functions

#include "Oscillator.h"			// Simplest sine oscillator
#include "BinaryOp.h"			// Simplest sine oscillator
 #include "RtpSender.h"

using namespace csl;

////// MAIN //////

#define SERVER
#ifdef SERVER

int main (int argc, const char * argv[]) {
	READ_CSL_OPTS;					// read the standard CSL options

	logMsg("Running CSL4 RTP Server test");
	
	Sine sineOsc (660.0, 0.2);			// create 2 sine oscillators
	Sine sineOsc2(440.0, 0.2);
	sineOsc.setOffset(sineOsc2);		// add 'em
	
	PAIO * theIO = new PAIO;			// PortAudio IO object

	RtpSender rtpSend;

	theIO->open();
	theIO->start();

	rtpSend.setInput(sineOsc);		// Plug in the rtpSender below the adder object
	rtpSend.addRtpDestination("127.0.0.1", 46000);
	
	theIO->setRoot(rtpSend);			// plug in the whole graph
	
	sleepSec(30);					// sleep a bit

	rtpSend.BYEDestroy(RTPTime(10,0),"Test server stopped normally",0);

	theIO->stop();
	theIO->close();
	
	logMsg("done!");
	return 0;
}

#else		// client

#endif
