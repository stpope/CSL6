//
//  CSL_Graph.mm -- CSL_iphone_test load/play methods
//
// Load the HRTF database from a resource named files.txt or HRTF_1028.dat.
// HRTF test player method sets up an HRTF DB and spatial source on a piano sample.
//		Loops at changing azimuth angles and distancesusing IRCAM head 1047 HRTF
//
// Based on Charlie's demo; ext. to HRTF Binaural Panner, 0909 by STP.
//

#import "CSL_Graph.h"						
#include "CSL_Includes.h"				// the CSL C++ stuff can be included in the mm file
#include "SpatialAudio.h"
#include "Binaural.h"
#include "iphoneIO.h"

@implementation CSL_Graph

using namespace csl;

#define PAN_MODE kBinaural				// HRTF vs Ambisonics flag
//#define PAN_MODE kAmbisonic

// Globals (exposed for access by other threads)

iPhoneIO * gIO = 0;						// the IO driver object

// Load the HRTF database from a resource named files.txt or HRTF_1028.dat

- (void) loadHRTF {	
}

// Create a global IO (gIO) and Spatializer (gPanner); start the IO

- (void) startCSL {							
	// The DSP graph is taken from the FM test example
	float frq = 250.0f;
	float dur = 2.0f;
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
	
	gIO = new iPhoneIO;					// create the global IO
	gIO->setRoot(car);					// pass the CSL graph to the IO
	gIO->open();						// open the IO
	gIO->start();						// start the driver callbacks and it plays!

	a_env.trigger();						// reset the envelopes to time 0
	i_env.trigger();	
	
	sleepSec(dur + 0.25);					// sleep for dur plus a bit
	
	logMsg("CSL done.");
	gIO->stop();						// stop the driver and close down
	gIO->close();
}

// stop the IO and clean up

- (void) stopCSL {							
	gIO->stop();						// stop/close/release the driver
	gIO->close();
	delete gIO;
	gIO = 0;
}

int whichSnd = 0;						// hokey global snd selector; 
										// should be an argument to the method or something
#define MAX_DISTANCE 10.0f				// max distance before "bounce-back"
#define MIN_DISTANCE 2.0f				// min distance before "bounce-back"
#define POS_THRESH 0.5f					// angular diustance (in radians) for rear/center processing

// start a new source running in a thread; repeat the snd at different positions

- (void) playHRTF {							
}

@end
