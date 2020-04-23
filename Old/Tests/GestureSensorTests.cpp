//
//  GestureSensorTests.cpp -- CSL functions for testing the OSC-based gesture sensors.
//	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
//
// This is included as an example of using the various gesture sensors to drive real-time CSL programs.
// The program runs several threads that share some global data structures.
// Input:
//		Map the ebeam data to the base pitch and pitch range of the 4 voices
//		Map the matrix data to the bell pitch/density/position
//

// CSL headers

#include "Test_Support.cpp"		// include all of CSL core and the test support functions
#include "OSC_cpp.h"
#include "GestureSensors.h"

// Prototypes for the OSC "glue" code

extern "C" void init_OSC_addr_space();
extern "C" void main_OSC_loop(int port);

using namespace csl;

extern IO * gIO;

#define MATRIX_SOCKET 57123

Mixer gMix(2);				// Main stereo output mixer

#define MAX_BELLS 4			// number of bell layers

// Global bell-related data that's set by the controllers and read by the reader threads

float gSBaseFreq[MAX_BELLS] = { 200.0f, 400.0f, 800.0f, 1600.0f };	// base frequencies
float gBaseFreq[MAX_BELLS] = { 200.0f, 400.0f, 800.0f, 1600.0f };	// base frequencies
float gFreqRange[MAX_BELLS] = { 100.0f, 200.0f, 400.0f, 800.0f };   // freq ranges
float gDensity[MAX_BELLS] = { 3.0f, 3.0f, 3.0f, 3.0f };				// inter-onset delay range
float gPosX[MAX_BELLS] = { -1.0f, -0.33f, 0.33f, 1.0f };			// x position
float gPosY[MAX_BELLS] = { -1.0f, -0.33f, 0.33f, 1.0f };			// y position

// Layer thread functions (see below)

void * play_bells(void * which);
void * main_input_loop(void * port);

// Data mapping functions

void map_ebeam(EBeam & ebeam);
void map_matrix(Matrix & mat);
void map_birds(Bird & bird);

///////////////////////// MAIN //////////////////////////

int main(int argc, const char **argv) {
	READ_CSL_OPTS;										// load the standard CSL options: 
	logMsg("OSC Matrix server running...");
	
	init_OSC_addr_space();					// set up the OSC address space before creating the sensors objects

//  Matrix mat("Matrix");					// Matrix controller
//  Bird bird("FOB");						// Flock of Birds controller
//  Glove glove("DG");						// Data glove controller
	EBeam ebeam("EB");						// EBeam controller

	Thread * aThread = ThreadPthread::MakeThread();		// create background threads
	Thread * bThread = ThreadPthread::MakeThread();		// 4 bell threads
	Thread * cThread = ThreadPthread::MakeThread();		
	Thread * dThread = ThreadPthread::MakeThread();	
	Thread * oscThread = ThreadPthread::MakeThread();	
	
	gIO = new CAIO;					// Create a Port- or Core-Audio output object
	Stereoverb verb(gMix);			// add some stereo reverb to the global mixer
	gIO->set_root(verb);				// plug the reverb in as the IO client
	gIO->open();
	gIO->start();					// start the io
	
	int which = 0;					// fork the looping threads
	aThread->create_thread(play_bells, & which);
	sleep_sec(0.1);				
	which = 1;
	bThread->create_thread(play_bells, & which);
	sleep_sec(0.1);				
	which = 2;
	cThread->create_thread(play_bells, & which);
	sleep_sec(0.1);				
	which = 3;
	dThread->create_thread(play_bells, & which);
									// fork the OSC reader thread
	int udp_port = 57123;			// port for the OSC read socket
	oscThread->create_thread(main_input_loop, & udp_port);
	
	while (true) {					// this thread now just maps input values
		map_ebeam(ebeam);
	//	map_matrix(mat);
		sleep_sec(0.5);				// set the control update rate here
	}
} 

// Data mapping functions

// Map the ebeam data to the base pitch and pitch range of the 4 voices

void map_ebeam(EBeam & ebeam) {
	float x = ebeam.get_x();
	float y = ebeam.get_y();
	if ((x == 0.0) || (y == 0.0))
		return;
	int xQuadrant = x > 1800 ? 0 : 1;			// get the current quadrant
	int yQuadrant = y > 850 ? 0 : 1;
	float xVal = x / 1800 - xQuadrant;			// and position within the quadrant
	float yVal = y / 850 - yQuadrant;			// these are now in the range 0-1
	int index = xQuadrant * 2 + yQuadrant;
	gBaseFreq[index] = (xVal + 1) * gSBaseFreq[index];
	gFreqRange[index] = (yVal + 1) * gBaseFreq[index];
	printf("EB: %5.1f @ %5.1f\n", x, y);
}

// Map the matrix data to the bell pitch/density/position

void map_matrix(Matrix & mat) {
	mat.compute_reduction();					// get the reduced matrix data
	for (unsigned i = 0; i < 4; i++) {
		gBaseFreq[i] = mat.redMatrix[0][i];	// write to the global data read by the bell threads
		gFreqRange[i] = mat.redMatrix[1][i];
		gDensity[i] = mat.redMatrix[2][i];
		gPosX[i] = mat.redMatrix[3][i];
	}
}

// Map the FOBirds to the bell positions

void map_birds(Bird & bird) {

}

// Test sound: repeating random stereo reverberated dual-envelope FM bells

void * play_bells(void * number) {
	int which = * ((int *) number);
	ADSR a_env(1, 0.002, 0.1, 0.1, 0.6);	// amplitude env = std ADSR
	a_env.scale_values(0.25);				// scale loudness
	ADSR i_env(1, 0.1, 0.1, 0.5, 0.5);		// index env
	Sine vox, mod(110);						// declare 2 oscillators
	DynamicVariable var(110, i_env);		// multiply index envelope by mod freq
	MulOp i_mul(mod, var);					// scale the modulator by its envelope
	AddOp adder(i_mul, 220.0);				// add in the modulation
	vox.set_frequency(adder);				// set the carrier's frequency
	vox.set_scale(a_env);					// scale the carrier's output by the amplitude envelope
	Panner pan(vox, gPosX[which]);			// create stereo panner
	gMix.add_input(pan);					// plug the panner into the mixer
	float frq;								// base frequency
	while(true) {							// endless loop playing bells
		frq = gBaseFreq[which] + (f_rand() * gFreqRange[which]);  // pick a new base freq
		mod.set_frequency(frq * (1.414f + (f_rand() * 0.1)));	// plug the frequency into the graph
		var.set_value(frq);					// index multiplier
		adder.set_operand(frq);				// carrier freq adder
		pan.set_position(gPosX[which]);
		a_env.trigger();					// trigger the envelopes
		i_env.trigger();	
		sleep_sec(1 + (f_rand() * gDensity[which]));	// sleep a few sec
	}										// ...never reached, endless loop
}

// Generic OSC reader loop that's forked as a separate thread by each sensor instance

void * main_input_loop(void * port) {
	int * udp_port = (int * ) port;		// UDP port to read on
	main_OSC_loop(*udp_port);			// Run the main loop (doesn't return)
	return(NULL);
}

