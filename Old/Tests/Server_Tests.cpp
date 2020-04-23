//
//	Server_Tests.cpp -- the CSL remote sample-streaming client/server test program
//	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
//
//  These functions use RemoteIO and RemoteStreams to implement client/server sample streaming 
//  over the network. Both the client and the server are in this file; one compiles them
//  differently using conditional compilation (#ifdef) flags defined below.
//
//  The "server" is a CSL graph that uses a RemoteIO as its IO, so it gets buffer requests
//  based on in-coming request packets from the "client" and sends out sample buffers in via TCP.
//  In the example below, the server plays random FM bells.
//
//  It also responds to the OSC comand "/Density" (with 1 float arg in the range 0-5 or so) 
//  to set the event density of the bells, so it's an example of the minimal OSC server.
//
//  The client is the CSL socket-based play program; it uses a RemoteStream to get samples 
//  from the server; its IO is a "real" one; it streams from the net to its DAC.
//
//  One can compile the fancy stereo reverb to be in either the client or the server (for
//  possible load-balancing).
//
//  To build/run this, comment out the #define SERVER below and built the target; move the 
//  resulting file "server" to "client." Then #define SERVER and rebuild.
//  You should be able to start the server, then the client, then send OSC messages to the server.
//

#include "Test_Support.cpp"		// include all of CSL core and the test support functions

#include "RemoteStream.h"
#include "RemoteIO.h"

using namespace csl;

// Compile-time options
//
//  SERVER			-- compile main() for RFS client vs. server
//  CSL_TESTS		-- compile to play to sound file for testing
//  RVB_ON_CLIENT   -- use reverberator on lint (ligheweight server)
//  OSC_SERVER		-- set up an OSC server thread for bell density

#define SERVER  			// swap this off to compile the client
#define OSC_SERVER
//#define CSL_TESTS
#define RVB_ON_CLIENT

// global density in secs-per-bell

float bellDensity = 2.0;
void run_tests();

///////////// MAIN ///////////////

// NB: This is the same for both client and server based on the #ifdef switches below

int main (int argc, const char * argv[]) {
	READ_CSL_OPTS;					// read the standard CSL options
	logMsg("Running CSL remote tests");
						// Which IO to play to?
#ifdef SERVER						// the server plays out TCP_RemoteIO
	gIO = new RemoteIO;				// Socket-based IO object
#else								// the client plays out the output
#ifdef CSL_MACOSX
	gIO = new CAIO;					// CoreAudio IO object
#else
	gIO = new PAIO;					// PortAudio IO object
//  gIO = new FileIO("test.aiff");   // File IO object
#endif
#endif
	gIO->open();			// GO!
	gIO->start();
	run_tests();					// call the test function (at the end of this file)
	gIO->stop();
	gIO->close();	
	logMsg("Done");
	return 0;
}						//// end of main ////

#ifdef OSC_SERVER		// OSC Interface

#include "OSC_cpp.h"
									// Prototypes for OSC "glue" code
extern "C" void init_OSC_addr_space();
extern "C" void add_OSC_Method(char * name, OSC_CALL_BACK function);
extern "C" void main_OSC_loop(int port);

// Bell density setter function; signature (arg types) determined by OSC API

void float_set_density(void * ignored, int arglen, const void * vargs, OSCTimeTag when, 
						NetworkReturnAddressPtr ra) {
	char * error;
	char * typeString = (char *) vargs;
											// get the OSC argument pointer
	void * otherArgs = (void *) OSCDataAfterAlignedString(typeString, typeString+arglen, &error);
	if (strcmp(typeString, ",f")) {			// if not the right type string, complain and return
		logMsg(kLogError, "Invalid type string in OSC, expected \",f\" got \"%s\"", typeString);
		return;
	}
	float val = ((float *) otherArgs)[0];	// get the float argument
	logMsg("OSC set density to %g", val);
	bellDensity = val;						// set the density global
}

// Main OSC read loop function for forking as a separate thread

void * osc_loop(void * ignored) {
	int udp_port = 54321;		// UDP port to read on
	main_OSC_loop(udp_port);		// Run the main loop (doesn't return)
	return(NULL);
}

// Set up OSC server for /Density,f,float_val messages

void setup_OSC_for_bells() {
								// create thread obj
	Thread * cThread = ThreadPthread::MakeThread(); 
								// Set up OSC address space root	
	init_OSC_addr_space();	
								// add the single OSC command to set the density
	add_OSC_Method("Density", float_set_density);
								// Run the main loop in a separate thread
	cThread->create_thread(osc_loop, NULL);
}

#endif		// OSC end


// Test sound: repeating random stereo reverberated dual-envelope FM bells

void random_bells() {
											// nasty! set a large block size for the server
	csl::CGestalt::set_block_size(csl::CGestalt::max_buffer_frames());
	ADSR a_env(1, 0.002, 0.1, 0.1, 0.6);	// amplitude env = std ADSR
	a_env.scale_values(0.25);				// scale loudness
	ADSR i_env(1, 0.1, 0.1, 0.5, 0.5);		// index env
	Sine vox, mod(110);						// declare 2 oscillators
	DynamicVariable var(110, i_env);		// multiply index envelope by mod freq
	MulOp i_mul(mod, var);					// scale the modulator by its envelope
	AddOp adder(i_mul, 220.0);				// add in the modulation
	vox.set_frequency(adder);				// set the carrier's frequency
	vox.set_scale(a_env);					// scale the carrier's output by the amplitude envelope
	Sine pos(0.15);							// LFO panning
	Panner pan(vox, pos);					// create stereo panner
#ifdef RVB_ON_CLIENT						// do reverb on client (i.e., not here)
	io->set_root(pan);						// plug the panner into the output
#else
	Stereoverb verb(pan);					// reverberate the panner
	io->set_root(verb);						// plug the reverb into the output
#endif
	float frq;								// base frequency
	while(true) {							// endless loop playing bells
		frq = 500.0 + (f_rand() * 2000.0);  // pick a new base freq
		mod.set_frequency(frq * 1.414f);	// plug the frequency into the graph
		var.set_value(frq);					// index multiplier
		adder.set_operand(frq);				// carrier freq adder
		a_env.trigger();					// trigger the envelopes
		i_env.trigger();	
		sleep_sec(1 + (f_rand() * bellDensity));	// sleep a few sec
	}										// ...never reached, endless loop
}

//////////////// RemoteStream tests //////////////////////

// Test the RemoteStream <--> RemoteIO sample buffer streaming
// The constructor signature is:
//		RemoteStream(char * client_name, unsigned client_port, unsigned my_port, 
//						unsigned ch, unsigned buffersize);
//
// To run this, first start the RemoteIO server (another CSL program set up to use a RemoteIO 
// object as its output) on the machine named below ("localhost" to run both on one machine). 
// The player simply polls the remote stream and plays the output to its IO (a real one).

void remote_player() {
	RemoteStream rfs("localhost", CSL_DEFAULT_REQUEST_PORT, 2, csl::CGestalt::max_buffer_frames());
	logMsg("playing remote stream client...");
#ifdef RVB_ON_CLIENT			// do reverb on client (i.e., here)
	Stereoverb verb(rfs);		// reverberate the RFS here
	gIO->set_root(verb);			// plug the reverb into the output
#else
	gIO->set_root(rfs);			// else plug in the RFS to the IO
#endif
	sleep_sec(120);				// wait 2 minutes
	gIO->clear_root();			// turn it off
	logMsg("remote stream client done.");
}

// mixer reads streams from 2 servers

void remote_mixer() {
	RemoteStream rfs1("slow.mat.ucsb.edu", CSL_DEFAULT_REQUEST_PORT, 2, 
					csl::CGestalt::max_buffer_frames());
	RemoteStream rfs2("square.mat.ucsb.edu", CSL_DEFAULT_REQUEST_PORT, 2, 
					csl::CGestalt::max_buffer_frames());
	Mixer mix(2);
	mix.add_input(rfs1);
	mix.add_input(rfs2);
	logMsg("playing remote stream client...");
	gIO->set_root(mix);			// start the RFS
	sleep_sec(120);				// wait some
	gIO->clear_root();			// turn it off
	logMsg("remote stream client done.");
}

/////////////////////////// This is the list of tests to run ////////////////////////////

void run_tests() {

#ifndef CSL_TESTS

#ifdef SERVER				// if you're in the server
	logMsg("playing bells over remote IO...");
#ifdef OSC_SERVER			// OSC server for density messages
	setup_OSC_for_bells();
#endif
	random_bells();			// server synthezises bells to a RemoteIO
#else						// else we're in the client
	remote_player();		// client plays from RemoteStream
//	remote_mixer();
#endif

#else			// CSL_TESTS defined -- run bellsstand-alone
	logMsg("playing bells over remote IO...");
#ifdef OSC_SERVER			// OSC server
	setup_OSC_for_bells();
#endif
	random_bells();		// testing -- just play bells
	logMsg("Done!");
#endif

}

