///
///	Beep_demo.cpp -- a simple CSL "main" program -- play a 3-second FM beep
///	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
///

#include "CSL_Includes.h"	// include all of CSL core
#include "PAIO.h"			/// PortAudio IO
#include "FileIO.h"			/// PortAudio IO

#define FREQ 660
#define DUR 0.1

using namespace csl;

///
/// MAIN -- Plays an accelerando on an FM beep
///

int main (int argc, const char * argv[]) {
	float frq = FREQ;					// float values for freq and dur
	float dur = DUR;

	PAIO * theIO = new PAIO;			// create a PortAudio IO object
//	FileIO * theIO = new FileIO();		// create a FileAudio IO object

										// The DSP graph is taken from the FM test example
	Sine car, mod(frq);					// declare 2 oscillators: carrier and modulator
	ADSR a_env(dur, 0.01, 0.02, 0.2, 0.03);		// amplitude env = std ADSR
	Envelope i_env(dur, 0, 0, 0.01, 2, 0.06, 1, dur, 0);	// index env = time/value breakpoints
	a_env.setScale(0.8f);				// make ampl envelope quieter
	i_env.setScale(frq * 1);			// multiply index envelope by mod freq * 3 (index depth)
	mod.setScale(i_env);				// scale the modulator by its envelope
	mod.setOffset(frq);					// add in the base freq
	car.setFrequency(mod);				// set the carrier's frequency
	car.setScale(a_env);				// scale the carrier's output by the amplitude envelope
	
	logMsg("CSL playing FM accel...");		// print a message and play
	theIO->setRoot(car);				// set the IO's root to be the FM carrier
	theIO->open();						// open the IO
	theIO->start();						// start the driver callbacks and it plays!

//	dur = (60.0 / 90.0) * 1000000.0;			// play mm 90
	dur = 1000000.0;			// play mm 90
	for (unsigned i = 0; i < 20; i++) {
		a_env.trigger();				// reset the envelopes to play a note
		i_env.trigger();	
		sleepUsec((int)dur);
	}
//	dur = (60.0 / 120.0) * 1000000.0;			// play mm 120
//	for (unsigned i = 0; i < 15; i++) {
//		a_env.trigger();
//		i_env.trigger();	
//		sleepUsec((int)dur);
//	}
//	dur = (60.0 / 150.0) * 1000000.0;			// play mm 150
//	for (unsigned i = 0; i < 18; i++) {
//		a_env.trigger();
//		i_env.trigger();	
//		sleepUsec((int)dur);
//	}
	logMsg("CSL done.");
	theIO->stop();						// stop the driver and close down
	theIO->close();	
	return 0;							// exit
}
