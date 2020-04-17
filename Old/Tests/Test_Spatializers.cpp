//
//	Test_Spatializers.cpp -- C main functions for the CSL4  sound spatialization (VBAP & Ambisonic) Demo.
//	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
//

#include "CSL_Includes.h"		// include all of CSL core
#include "SoundFile.h"
#include "SpatialSource.h"
#include "SpatialAudio.h"
#include "DistanceSimulator.h"
#include "VBAP.h"
#include "PAIO.h"			// PortAudio IO

#include "string.h"
#include <time.h>

// Edit your folder name in here

#define SNDFILENAME "../../../CSL_Data/hihat_mono.aiff"
#define ROOMFILENAME "../../../CSL_Data/CSL/Spatializers/5point1.dat"

using namespace csl;

int main (int argc, const char * argv[]) {

//				PAIO(unsigned s_rate, unsigned b_size, PaDeviceIndex in_device, PaDeviceIndex out_device, unsigned in_chans, unsigned out_chans);
	PAIO *gIO = new PAIO(CGestalt::frameRate(), 1024, -1, Pa_GetDefaultOutputDevice(), 0, 5);		// 5 channel output
	
	// Open some soundfile....
	SoundFile mySong(SNDFILENAME);
	mySong.openForRead();
	mySong.setIsLooping(true);

//	gIO->mChannelMap[0] = 8;
//	gIO->mChannelMap[1] = 8;
//	for(unsigned i = 2; i < 10; i++)
//		gIO->mChannelMap[i] = i - 2;
//		mChannelMap[channel] = speakerIndex;

	SpatSource s2(mySong, deg2rad(-0), 0., 100);
//	DistanceSimulator distancsSim(s2);

	// Either read a layout from a file, or...
	SpeakerLayout myLayout(ROOMFILENAME);

	// Setting a layout as defalut will allow any spatializer to use it automatically
	// as most systems have a single layout, then it is always convenient to set it as default and forget about the fact that a layout exists
	SpeakerLayout::setDefaultSpeakerLayout(&myLayout);
	
	// Create a spatializer and specify the type of panner to use...
	Spatializer v(kVBAP);
//	Spatializer v(kAmbisonic);

	v.addSource(s2);	
//	v.addSource(distancsSim);
				
	logMsg("CSL playing...");
	gIO->setRoot(v);						// make some sound
	gIO->open();
	gIO->start();							// start the driver callbacks

	s2.setPosition('s', 0.0, 0.0);

	float azimuth = 0.0;
	float elevation = 0.0;
	float distance = 20;
//	float size = 0.0; 
//	float freq = 100;
	for(unsigned i = 0; i < 5000; ++i) {
//		v.rotate(azimuth);
//		distancsSim.setPosition('s', azimuth, elevation);
		s2.setPosition('s', azimuth, elevation, distance);
		sleepSec(0.05);
//		verb.setRoomSize(size);
//		mySong.setFrequency(freq);
//		size += 0.05;
		azimuth += 10;
//		elevation  += 0.01;
//		distance += 0.1;

//		freq *= 1.1;
//		logMsg("Distance: %f", distance);
//		
	}

//	s2.setPosition('s', 30, 0, distance);
//	sleepSec(2.5);
//	s2.setPosition('s', 90, 0, distance);
//	sleepSec(2.5);
//	s2.setPosition('s', 150, 0, distance);
//	sleepSec(2.5);
//	s2.setPosition('s', -135, 45, distance);
//	sleepSec(2.5);
//	s2.setPosition('s', -90, 0, distance);
//	sleepSec(2.5);
//	s2.setPosition('s', -30, 0, distance);
//	sleepSec(2.5);
	
	logMsg("CSL done.");
	gIO->stop();							// stop the driver and close down
	gIO->close();	
	
	return 0;								// exit
}
