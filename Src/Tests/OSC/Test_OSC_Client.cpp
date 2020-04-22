//
//	Test_OSC_CLIENT.cpp -- main() function for stand-alone OSC client that sends messages
//		to the CSL OSC server in TEST_Control.cpp
//	The server's OSC "play note" cmd is:
//			/iX/pn, "ffff", dur ampl freq pos			for X = 1...16
//
//	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
//
//	 compile this like:	g++ -w -I../Includes TEST_OSC_Client.cpp ../Kernel/CGestalt.cpp -o osc_client -llo
//		(See the Makefile)
//

#include "CSL_Core.h"			// the core of CSL 5
#include "lo/lo.h"				// liblo for OSC

#define NUM_NOTES 100			// # notes to play

using namespace csl;

void noteLoop(lo_address & ad);					// fcn protptype

// Main fcn sends out NUM_NOTES OSC messages to localhost port CGestalt::outPort()

int main(int argc, char ** Argv) {
	char pNam[CSL_WORD_LEN];					// string for port number
	
	sprintf(pNam, "%d", CGestalt::outPort());
	lo_address ad = lo_address_new(NULL, pNam);	// set up a port to write to the server thread
	
	logMsg("Sending OSC note cmds");
	noteLoop(ad);								// call note loop fcn
	
	sleepSec(3);								// let the reverb dieout
	
	logMsg("Sending OSC quit");
	
	if (lo_send(ad, "/q", NULL) == -1) {
		logMsg(kLogError, "OSC error3 %d: %s\n", lo_address_errno(ad), lo_address_errstr(ad));
	}
}

// note player fcn - play pentatonic pitches

void noteLoop(lo_address & ad) {
	int steps[5] = { 0, 2, 5, 7, 9 };			// scale steps for pentatonic scale
	char pNam[CSL_WORD_LEN];					// string for port number
	for (unsigned i = 0; i < NUM_NOTES; i++) {	// note loop
		sprintf(pNam, "/i%d/pn", (i % 16) + 1);	// OSC cmd: /iX/pn, "ffff", dur ampl freq pos
		float dur = fRandM(0.15f, 1.5f);		// duration range
		float ampl = fRandM(0.1, 0.6);			// amplitude range
		int octave = iRandM(2, 9);				// rand octave
		int step = steps[iRandV(5)];			// rand scale step
		int key = (octave * 12) + step;			// pick a pentatonic note to play
		float freq = keyToFreq(key);			// frequency range
		float pos = fRand1();					// random stereo position
												// now send OSC
		if (lo_send(ad, pNam, "ffff", dur, ampl, freq, pos) == -1)
			logMsg(kLogError, "OSC error2 %d: %s\n", lo_address_errno(ad), lo_address_errstr(ad));
			
		sleepSec(fRandM(0.05, 0.25));			// sleep a bit
	}											// end note loop
}

// note player fcn - play stuttered drum samples

//void noteLoop() {
//	char pNam[CSL_WORD_LEN];					// string for port number
//	for (unsigned i = 0; i < NUM_NOTES; i++) {	// note loop
//		int numN = iRandM(3, 10);			// pick # notes to play
//		int pit1 = iRandM(30, 84);			// pick start pitch (in MIDI)
//		int pitX = iRandM(1, 4);			// pick pitch step
//		if (coin())							// pick step up or down
//			pitX = 0 - pitX;
//		float pos1 = fRandZ();				// pick starting pos (0 - 1)
//		if (pos1 == 0)
//			pos1 = 0.1;
//		pos1 = sqrtf(pos1);					// sqrt of starting pos (moves it away from 0)
//		if (coin())							// pick pos L or R
//			pos1 = 0.0f - pos1;
//		float posX = (pos1 > 0) 			// calc pan step
//					? ((0.0f - ((pos1 + 1.0f) / (float)numN))) 
//					: ((1.0f - pos1) / (float)numN);
//		float dela = fRandM(0.08, 0.25);	// calc delay
//		logMsg("n: %d  p: %d/%d  \tx: %5.2f/%5.2f  \td:%5.2f", numN, pit1, pitX, pos1, posX, dela);
//											// note loop
//		for (int i = 0; i < numN; i++) {	// loop to create string gliss
////			logMsg("\t\ts: %d  p: %d  x: %5.2f", cnt, pit1, pos1);
//											// set freq to MIDI pitch
//			((KarplusString *)strings[cnt])->setFrequency(keyToFreq(pit1));
//			pit1 += pitX;
//											// set pan
//			((Panner *)pans[cnt])->setPosition(pos1);
//			pos1 += posX;
//											// trigger
//			((KarplusString *)strings[cnt])->trigger();
//			if (sleepSec(dela))				// sleep
//				return;						// exit if sleep was interrupted
//			cnt++;							// pick next string
//			if (cnt == numStrings)			// should I check ifthe string is still active?
//				cnt = 0;					// reset string counter
//		}
//		sleepSec(fRandM(0.02, 0.2));		// sleep a bit between sets
//	}
//}

