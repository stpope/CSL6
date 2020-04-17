///
///	Beep_demo.cpp -- a simple CSL "main" program -- play a 3-second FM beep
///	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
///

#include "Test_Support.cpp"		// include all of CSL core and the test support functions

/// Cmd-line options (see the handler in Test_Support.cpp):
///		-r srate -b blksiz -o noch -i nich -v verbosity -l logperiod -p outport -u
///
/// Added options
///		"-f val" = set base freq to val Hz
///		"-d val" = set duration to val seconds

#define FREQ 55
#define DUR 3

///
/// MAIN -- Plays a 3-second FM beep
///

int main (int argc, const char * argv[]) {
	unsigned value;						// variable used for arg parsing
	float frq = FREQ;						// float values for freq and dur
	float dur = DUR;

	READ_CSL_OPTS;								// parse the standard CSL options: 
	if (find_option(argc, argv, 'f', &value) > 0) {		// "-f val" = set base freq to val Hz
		frq = atof(argv[value]);
		printf("Setting frequency to: %g\n", frq);
	}
	if (find_option(argc, argv, 'd', &value) > 0) {		// "-d val" = set duration to val sec
		dur = atof(argv[value]);
		printf("Setting duration to: %g\n", dur);
	}
#ifdef USE_CAIO
	CAIO * theIO = new CAIO;				// create a CoreAudio IO object
#else
	PAIO * theIO = new PAIO;				// create a PortAudio IO object
#endif	
										// The DSP graph is taken from the FM test example
	Sine car, mod(frq);					// declare 2 oscillators: carrier and modulator
	ADSR a_env(dur, 0.1, 0.1, (dur - 0.6), 1);				// amplitude env = std ADSR
	Envelope i_env(dur, 0, 0, 0.1, 2, 0.2, 1, 2.2, 8, dur, 0);	// index env = time/value breakpoints
	a_env.setScale(0.2);					// make ampl envelope quieter
	i_env.setScale(frq * 3);				// multiply index envelope by mod freq * 3 (index depth)
	mod.setScale(i_env);					// scale the modulator by its envelope
	mod.setOffset(frq);					// add in the base freq
	car.setFrequency(mod);				// set the carrier's frequency
	car.setScale(a_env);					// scale the carrier's output by the amplitude envelope
	
	logMsg("CSL playing FM...");			// print a message and play
	theIO->setRoot(car);					// set the IO's root to be the FM carrier
	theIO->open();						// open the IO
	a_env.trigger();						// reset the envelopes to time 0
	i_env.trigger();	
	theIO->start();						// start the driver callbacks and it plays!

	sleepSec(dur + 0.25);					// sleep for dur plus a bit

	logMsg("CSL done.");
	theIO->stop();						// stop the driver and close down
	theIO->close();	

	return 0;							// exit
}

