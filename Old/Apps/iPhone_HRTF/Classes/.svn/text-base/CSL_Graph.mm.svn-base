//
//  CSL_Graph.mm -- CSL_iphone_test load/play methods
//
// Load the HRTF database from a resource named files.txt or HRTF_1028.dat.
// HRTF test player method sets up an HRTF DB and spatial source on a piano sample.
//		Loops at changing azimuth angles and distancesusing IRCAM head 1047 HRTF
//
// Based on Charlie's demo; ext. to HRTF Binaural Panner, 0909 by STP.
// Extended to new SimplePanner 1003 by STP.
//

#import "CSL_Graph.h"						
#include "CSL_Includes.h"				// the CSL C++ stuff can be included in the mm file
#include "SpatialAudio.h"
#include "Binaural.h"
#include "iphoneIO.h"

@implementation CSL_Graph

using namespace csl;

// Simple vs HRTF vs Ambisonics flag

#define PAN_MODE kSimple				
//#define PAN_MODE kBinaural
//#define PAN_MODE kAmbisonic

// Globals (exposed for access by other threads)

iPhoneIO * gIO = 0;						// the IO driver object
Spatializer * gPanner = 0;				// the multi-source spatial panner

// Load the HRTF database from a resource named files.txt or HRTF_1028.dat

- (void) loadHRTF {	
	logMsg("Start CSL App");
										// get the bundle, the HRTF DB resource name, and its string
	NSBundle * bundle = [NSBundle mainBundle];
	NSString * filename = [bundle pathForResource: @"HRTF_1047" ofType: @"dat"];
	const char * theName = [filename cStringUsingEncoding: NSASCIIStringEncoding];
	logMsg("Loading HRTF data from \"%s\"", theName);
										// reload the HRTF DB
	HRTFDatabase::Reload((char *) theName);
										// log the DB name & size
	HRTFDatabase::Database()->dump();
}

// Create a global IO (gIO) and Spatializer (gPanner); start the IO

- (void) startCSL {							
	gPanner = new Spatializer(PAN_MODE);	// Create a spatializer
	gIO = new iPhoneIO;					// create the global IO
	gIO->setRoot(*gPanner);				// pass the CSL graph to the IO
	gIO->open();						// open the IO
	gIO->start();						// start the driver callbacks and it plays!
}

// stop the IO and clean up

- (void) stopCSL {							
	gIO->stop();						// stop/close/release the driver
	gIO->close();
	delete gPanner;
	delete gIO;
	gPanner = 0;
	gIO = 0;
}

int whichSnd = 0;						// hokey global snd selector; 
										// should be an argument to the method or something
#define MAX_DISTANCE 10.0f				// max distance before "bounce-back"
#define MIN_DISTANCE 2.0f				// min distance before "bounce-back"
#define POS_THRESH 0.5f					// angular diustance (in radians) for rear/center processing

// start a new source running in a thread; repeat the snd at different positions

- (void) playHRTF {							
	float gAzim, gElev, gDist;				// position is local to this thread
	float pPause, aSpeed, eSpeed, dSpeed;	// rate & angular velocities
	gAzim = 0.0f; // fRandV(CSL_PI);		// starting az/el
	gElev = 0.0f;
	gDist = fRandM(2.1, 3.9);			// distance is random per-thread
	pPause = fRandM(0.75, 1.25);		// delay time random
	aSpeed = 15; // fRandM(10, 30);		// rotation speed random
//	if (coin())							// rotate in random direction
//		aSpeed = 0 - aSpeed;
	eSpeed = 0.0f;						// stay at el = 0
	dSpeed = 0.0f; // fRandM(1.0, 3.5);	// dist speed random
	bool direction = true;				// direction of distance movement
										// open the bundle and find the sound file
	NSBundle * bundle = [NSBundle mainBundle];
	NSString * filename;
	switch (whichSnd) {					// pick which snd file to play
	case 0:
		filename = [bundle pathForResource: @"Piano_A5_mf_mono" ofType: @"caf"];
		break;
	case 1:
		filename = [bundle pathForResource: @"triangle_mono" ofType: @"caf"];
		break;
	case 2:
		filename = [bundle pathForResource: @"splash_mono" ofType: @"caf"];
		break;
	}
	whichSnd++;
										// get its name
	const char * sampName = [filename cStringUsingEncoding: NSASCIIStringEncoding];
										// open and read the sound file
	CASoundFile * sndfile = new CASoundFile(sampName);
	sndfile->openForRead();
	sndfile->dump();

	FanOut * fan = new FanOut(*sndfile, 3);		// we send the file to 3 outputs
										// make the sound file "positionable"
	SpatialSource * source = new SpatialSource(*sndfile);
	gPanner->addSource(*source);
	logMsg("Playing spatialized samples");

	while (true) {						// endless loop to play transpositions
										// set src position for HRTF
		source->setPosition('s', gAzim, gElev, gDist);
		source->dump();

		sndfile->trigger();				// start the file playing
		sleepSec(pPause);				// sleep a bit

		gAzim += aSpeed;				// update position
		gElev += eSpeed;
		if (direction)					// update distance
			gDist += dSpeed;
		else
			gDist -= dSpeed;
		if (fabs(gDist) > MAX_DISTANCE)	// bounce back
			direction = ! direction;
		if (fabs(gDist) < MIN_DISTANCE)
			direction = ! direction;
	}

	delete source;
	delete sndfile;
}

@end
