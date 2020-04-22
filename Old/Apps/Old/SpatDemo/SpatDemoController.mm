/*
 *  Created by Jorge Castellanos, 2006.
 *  Copyright 2006 Jorge Castellanos. All rights reserved.
 *
 */


#import "SpatDemoController.h"

using namespace csl;

@implementation SpatDemoController


- (void)awakeFromNib
{

	// Open sound card IO - -1 = noInputDevice, default output, 0 channels for input, and 10 for output.
	PAIO *gIO = new PAIO(-1, Pa_GetDefaultOutputDeviceID(), 0, 10);
	
	// Specify the speaker setup
	SpeakerLayout *myLayout = new SpeakerLayout("/AlloSpeakers.dat");
	// As an alternative, instead of loading speaker positions from a file, they can be added programatically:
	// myLayout->addSpeaker(phi, theta);
	// or... use a preset layout like: StereoSpeakerLayout.
	
	// Open soundfile				- Path to the file
	SoundFile *mySong = new SoundFile("/CatFoodG.aif");
	mySong->openForRead();

	// make the sound "Positionable"
	source = new SpatSource(*mySong, 0, 0.);

	// Create a spatializer.
	Spatializer *panner = new Spatializer(kAutomatic, myLayout);
	panner->addSource(*source);	// Add the sound source to it
	
	// set the root of the graph
	gIO->setRoot(*panner);
	// init and start running audio.
	gIO->open();
	gIO->start();

}

- (IBAction)setAzimuth:(id)sender
{
	// called to set the position of the sound.
	source->setPosition('s', [sender floatValue], 45);
}



@end
