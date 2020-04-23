///
///	Beep_demo.cpp -- a simple CSL "main" program -- OSC server for use with an iPhone
///	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
///

#include "CSL_Includes.h"	// include all of CSL core
#include "PAIO.h"			/// PortAudio IO
#include "lo/lo.h"

using namespace csl;

// OSC globals

static int done = 0;						// running flag
static lo_server_thread sSrvThrd;		// the liblo thread guy

// CSL globals

PAIO * theIO;
Sine * car, * mod;
ADSR * a_env;
Envelope * i_env;

#define FREQ 55
#define DUR 1
// error logger

void osc_error(int num, const char *msg, const char *path) {
    logMsg(kLogError, "OSC server error %d in path %s: %s\n", num, path, msg);
}

// Shut down and quit

void quitCSL() {
	done = 1;
	logMsg("OSC server exiting.");
	lo_server_thread_stop(sSrvThrd);
}

int quit_handler(const char *path, const char *types, lo_arg **argv, int argc,  void *data, void *user_data) {
	quitCSL();
	return 0;
}

// catch any incoming messages and display them

int generic_handler(const char *path, const char *types, lo_arg **argv, int argc, void *data, void *user_data) {
    logMsg("OSC message: <%s>", path);
    for (unsigned i=0; i<argc; i++) {
	printf("\t\targ %d '%c' ", i, types[i]);
	lo_arg_pp((lo_type)types[i], argv[i]);
	printf("\n");
    }
    printf("\n");
    fflush(stdout);
    return 1;
}


void play_note() {
	logMsg("	play note");
	a_env->trigger();						// reset the envelopes to time 0
	i_env->trigger();	
}

void set_param(unsigned which, float value) {
	printf("	set %d to %5.3f\n", which, value);
//	logMsg("	set %d to %5.3f", which, value);

}

// event types
// slider press/release
//			OSC message: </event>
//				arg 0 'i' 2061
//				arg 1 'i' 0
//				arg 2 'i' 10
//				arg 3 'i' 2
//				arg 4 'i' 2
//				arg 5 'f' 0.388889
//			OSC message: </event>
//				arg 0 'i' 2061
//				arg 1 'i' 0
//				arg 2 'i' 10
//				arg 3 'i' 2
//				arg 4 'i' 0
//
// button press/release
//			OSC message: </event>
//				arg 0 'i' 2061
//				arg 1 'i' 0
//				arg 2 'i' 2
//				arg 3 'i' 1
//				arg 4 'i' 1
//				arg 5 'i' 0
//			OSC message: </event>
//				arg 0 'i' 2061
//				arg 1 'i' 0
//				arg 2 'i' 2
//				arg 3 'i' 1
//				arg 4 'i' 0


// General (simple) event handler processes button and slider press events

int event_handler(const char *path, const char *types, lo_arg **argv, int argc, void *data, void *user_data) {
	if (argc == 6) {			// button press
		logMsg("	types = %s", types);
		if (types[5] == 'i') {
//		if ((types[5] == 'i') && (argv[5]->i == 1))
			play_note();
			return 0;
		}
		if (types[5] == 'f') 
			set_param(argv[2]->i, argv[5]->f);
	}
	return 0;
}

// Set up the LibLo OSC thread

extern "C" void init_OSC(const char * thePort) {
	sSrvThrd = lo_server_thread_new(thePort, osc_error);
						// add method that will match the path /q with no args and exit
//    lo_server_thread_add_method(sSrvThrd, "/q", "", quit_handler, NULL);
    lo_server_thread_add_method(sSrvThrd, "/event", "iiiiii", event_handler, NULL);
//    lo_server_thread_add_method(sSrvThrd, "/event", "iiiiif", event_handler, NULL);
//    lo_server_thread_add_method(sSrvThrd, "/event", "iiiiii", event_handler, NULL);
//    lo_server_thread_add_method(sSrvThrd, "/event", "iiiii", event_handler, NULL);
						// add method that will match any path and args
//    lo_server_thread_add_method(sSrvThrd, NULL, NULL, generic_handler, NULL);
}

// Main OSC loop -- select - read - dispatch

extern "C" void OSC_loop() {
	lo_server_thread_start(sSrvThrd);					// start lo!
	done = 0;
	while (!done)
		sleepSec(1.0);
	return;
}

// CSL DSP graph

void make_instrument() {
	float frq = FREQ;						// float values for freq and dur
	float dur = DUR;

	theIO = new PAIO;						// create a PortAudio IO object
	car = new Sine;
	mod = new Sine(frq);					// carrier and modulator
	a_env = new ADSR(dur, 0.05, 0.1, (dur - 0.6), 1);					// amplitude env = std ADSR
	i_env = new Envelope(dur, 0, 0, 0.1, 2, 0.2, 1, 2.2, 8, dur, 0);	// index env = time/value breakpoints
	a_env->setScale(0.2);					// make ampl envelope quieter
	i_env->setScale(frq * 3);				// multiply index envelope by mod freq * 3 (index depth)
	mod->setScale( * i_env);				// scale the modulator by its envelope
	mod->setOffset(frq);					// add in the base freq
	car->setFrequency( * mod);				// set the carrier's frequency
	car->setScale( * a_env);				// scale the carrier's output by the amplitude envelope
	theIO->setRoot( * car);					// set the IO's root to be the FM carrier
}

///
/// MAIN -- set up OSC addresses and start server

///

int main (int argc, const char * argv[]) {
	unsigned value;							// variable used for arg parsing
	float frq = FREQ;						// float values for freq and dur
	float dur = DUR;

	make_instrument();
	init_OSC("54321");
	
	theIO->open();							// open the IO
	theIO->start();							// start the driver callbacks and it plays!
	logMsg("CSL playing FM...");			// print a message and play

	OSC_loop();

	logMsg("CSL done.");
	theIO->stop();							// stop the driver and close down
	theIO->close();	

	return 0;								// exit
}

