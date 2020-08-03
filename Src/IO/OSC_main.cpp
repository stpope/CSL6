//
//  OSC_main.cpp -- CSL "main" function for use with OSC input triggering CSL instrument objects.
//	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
//
// This version takes an instrument library (Instrument ** library) and creates an OSC address space for 
// its instruments.  In this test, we create a library with 4 FM instruments and 4 sound file players.
//
// The address space is structured as follows:
//		For each instrument in the library (which is created in the main() function), we create  
//			a top-level OSC node called "in" where n is a number.
//		Each instrument node has children for all the instrument's accessors, and "p" to play 
//			a note and "pn" to play  a note passing parameters to the instrument
// There's a top-level "q" command to quit CSL.

/********** Example OSC address space:
*
	/			-- root
		/q:		quit command
	/i1/			instrument 1 (basic FM instrument)
		/i1/du:	set-duration command
		/i1/am:	set-amplitude command
		/i1/in:		...other setters
		/i1/cf:
		/i1/mf:
		/i1/aa:		-- these are the defined accesors in the instrument; see FM_Instrument.cpp
		/i1/ad:
		/i1/as:
		/i1/ar:
		/i1/ia:
		/i1/id:
		/i1/is:
		/i1/ir:
		/i1/p:		simple play-note command
		/i1/pn:	play-note-with-parameters command
	[ ... ]			...other instruments
	/i8/			i8 is a sample-player instrument
		/i8/am:	set-amplitude command
		/i8/ra:	set playback rate
		/i8/po:	set position
		/i8/fi: set-file-name   -- accessors defined in SndFile_Instrument.cpp
		/i8/p:  play
		/i8/pn:	play-with-args
*
*******************************/

// There are several main() functions with different instrument libraries and OSC address spaces

// only these tests have been updated to the CSL6 methods
#define CSL_OSC_SERVER2				// 64 instruments: KS strings, snd-files, vector SOS & 2 FM variations
//#define CSL_OSC_SERVER3			// 32 fm bells for booh

// NB: the rest of these still need to be ported
//#define CSL_OSC_FM_SndFile		// 4 voices of FM, 4 of SndFiles, and 1 bell
//#define CSL_OSC_SAMPLER			// 16 voices of file playback
//#define CSL_OSC_ADDER				// 16 voices of sum-of-sines synthesis
//#define CSL_OSC_ORCHESTRA			// 16 FM, 16 file, 16 SOS

#define IO_CLASS JUCEIO
#include "JUCEIO.h"					// JUCE Audio IO

#include "CSL_Includes.h"			// include all of CSL core
#include "lo/lo.h"					// liblo header has to be in your path (/usr/local/include)
#include "OSC_support.h"
#include "Instrument.h"
#include "FMInstrument.h"
#include "SndFileInstrument.h"
#include "AdditiveInstrument.h"
#include "StringInstrument.h"

#ifndef CSL_WINDOWS
	#include <SHARC.h>
#endif

using namespace csl;

IO_CLASS * theIO = 0;							// Here's the global IO instance
juce::AudioDeviceManager gAudioDeviceManager;	// global JUCE audio device mgr
bool gVerbose = false;

#ifdef WET_DRY_MIX
Mixer * gIMix;									// stereo instrument mixer (sent to reverb)
Mixer * gOMix;									// stereo output mixer (reverb + dry signal)
#endif

///////////////////////// MAIN with 16 voices of plucked string, 8 of snd-file sampler + 16 FM voices, 16 FM bells + 8 SOS x-fade voices

#ifdef CSL_OSC_SERVER2

int main(int argc, const char * argv[]) {
	for (int i = 1; i < argc; i++ ) 		// check the command line - so far only -v is handled
		if (argv[i] && (argv[i][0] == '-') && (strlen(argv[i]) > 1))
			if (argv[i][1] == 'v')			// verbose flag
				gVerbose = true;
			else
				printf("Unknown cmd-line option: \"%s\" ignored\n", argv[i]);

	printf("CSL lib server running...\n");
	gIMix = new Mixer(2);					// stereo instrument mixer (sent to reverb)
	gOMix = new Mixer(2);					// stereo output mixer (reverb + dry signal)
	InstrumentVector lib;					// instrument library
//	CGestalt::setVerbosity(3);
//	READ_CSL_OPTS;							// load the standard CSL options -- s_rate, blk_sizes, I/O ports, etc.
	printf("OSC server listening to port %s\n", CSL_mOSCPort);
	initOSC(CSL_mOSCPort);					// Set up OSC address space root

	printf("Setting up  library with 16 strings, 8 samplers, 16 FMs, 16 Bells, 8 V_SOS\n");

	for (unsigned i = 0; i < 16; i++) {		//---- 16 plucked strings
		StringInstrument * in = new StringInstrument(0.2f, 400.0f, 0.75f);
		lib.push_back(in);
		gIMix->addInput(*in);
	}
	char *names[] = { "moon.snd", "wet.snd", "round.snd", "shine.snd"};
	for (unsigned i = 16; i < 24; i++) {	//---- 8 sound files
		SndFileInstrument0 * in = new SndFileInstrument0(CGestalt::dataFolder(), names[i % 4]);
		lib.push_back(in);
		gIMix->addInput(*in);
	}
	for (unsigned i = 24; i < 40; i++) {	//---- 16 FM voices
		FMInstrument * in = new FMInstrument();
		lib.push_back(in);
		gIMix->addInput(*in);
	}
	for (unsigned i = 40; i < 56; i++) {	//---- 16 FM bells
		FMBell * in = new FMBell();
		lib.push_back(in);
		gIMix->addInput(*in, 1.0f);
#ifdef WET_DRY_MIX
		gOMix->addInput(*in, 0.0f);			// this also has wet/dry signal mixing
#endif
	}
#ifndef CSL_WINDOWS							// load spectra from the SHARC library
	SHARCLibrary::loadDefault();
	SHARCLibrary * sharcLib = SHARCLibrary::library();
	std::vector<SHARCSpectrum *> sharcSpectra;
	sharcSpectra.push_back(sharcLib->spectrum("oboe", 50));
	sharcSpectra.push_back(sharcLib->spectrum("tuba", 36));
	sharcSpectra.push_back(sharcLib->spectrum("viola_vibrato", 40));
	sharcSpectra.push_back(sharcLib->spectrum("bass_clarinet", 28));
	sharcSpectra.push_back(sharcLib->spectrum("violinensemb", 46));
	sharcSpectra.push_back(sharcLib->spectrum("Eb_clarinet", 48));
	sharcSpectra.push_back(sharcLib->spectrum("alto_trombone", 55));
	sharcSpectra.push_back(sharcLib->spectrum("French_horn", 32));
	sharcSpectra.push_back(sharcLib->spectrum("oboe", 50));

	for (unsigned i = 56; i < 64; i++) {	//---- 8 Vector SOS voices
		VAdditiveInstrument * in = new VAdditiveInstrument(sharcSpectra[i - 56], sharcSpectra[i - 55]);
		lib.push_back(in);
		gIMix->addInput(*in);
	}
#endif
	Stereoverb rev(*gIMix);					// stereo reverb
	rev.setRoomSize(0.98);					// medium-long reverb
	gOMix->addInput(rev);					// add reverb to output mixer

	setupOSCInstrLibrary(lib);				// add the instrument library OSC

											// reset the HW frame rate & block size to the CSL definition
//	juce::AudioDeviceManager::AudioDeviceSetup setup;
//	gAudioDeviceManager.getAudioDeviceSetup(setup);
//	setup.bufferSize = CGestalt::blockSize();
//	setup.sampleRate = CGestalt::frameRate();
//	gAudioDeviceManager.setAudioDeviceSetup(setup, true);

	theIO = new IO_CLASS(CGestalt::frameRate(),			// create the IO object
						 CGestalt::blockSize(),
						 -1, -1,			// use default I/O devices
						 0, 2);    			// stereo out by default

	theIO->setRoot(*gOMix);					// plug the mixer in as the IO client
	theIO->open();							// open the IO
	theIO->start();							// start the IO
	printf("Starting OSC loop\n");
	mainOSCLoop(NULL);						// Run the main loop function (returns on quit)
	theIO->stop();
	theIO->close();
}

#endif

// -----------------------------------------------------------------------------------------------

#ifdef CSL_OSC_SERVER3				// 32 FM bells

int main(int argc, const char * argv[]) {
	InstrumentVector lib;			// instrument library
	printf("CSL booh server running...\n");
	printf("OSC server listening to port %s\n", CSL_mOSCPort);
	initOSC(CSL_mOSCPort);			// Set up OSC address space root
	printf("Setting up bell library\n");
	Mixer mix(2);					// Create the main stereo output mixer
	
	for (unsigned i = 0; i < 32; i++) {	//---- 32 FM bells
		FMBell * in = new FMBell();
		lib.push_back(in);
		mix.addInput(*in);
	}
	Stereoverb rev(mix);			// stereo reverb
	rev.setRoomSize(0.96);			// medium-length reverb
	setupOSCInstrLibrary(lib);		// add the instrument library OSC
	theIO = new IO_CLASS(CGestalt::frameRate(),	// create the IO object
						 CGestalt::blockSize(),
						 -1, -1,	// use default I/O devices
						 0, 2);     // stereo out by default
	theIO->setRoot(mix);			// plug the mixer in as the IO client
//	theIO->setRoot(rev);			// plug the mixer in as the IO client
	theIO->open();					// open the IO
	theIO->start();					// start the IO
	printf("Starting OSC loop\n");
	mainOSCLoop(NULL);				// Run the main loop function (returns on quit)
	theIO->stop();
	theIO->close();
}

#endif


// The rest of these need to be ported to match the example above ----------------------------------

///////////////////////// MAIN with 4 voices of FM, 4 of SndFiles, and 1 bell

#ifdef CSL_OSC_FM_SndFile

int main(int argc, const char * argv[]) {
	READ_CSL_OPTS;									// load the standard CSL options: 
	logMsg("OSC server listening to port %s", CSL_mOSCPort);
					// Create instrument library -- 4 FM instruments and 4 sample file players
	logMsg("Setting up instrument library");
	Mixer mix(2);	// Create the main stereo output mixer

					// Now add 4 FM instruments and 4 sound file player instruments
	for (unsigned i = 0; i < 4; i++) {
		library.push_back(new FMInstrument);
		mix.addInput( * library[i]);
	}
	for (unsigned i = 4; i < 6; i++) {
		library.push_back(new SndFileInstrument(CGestalt::dataFolder(), "round.snd"));
		mix.addInput( * library[i]);
	}
	for (unsigned i = 6; i < 8; i++) {
		library.push_back(new SndFileInstrument(CGestalt::dataFolder(), "wet.snd"));
		mix.addInput( * library[i]);
	}
	FMInstrument * bell = new FMInstrument;	// add another FM insteument set up as a bell
	bell->mAEnv.setAttack(0.001);
	bell->mAEnv.setDecay(0.001);
	bell->mAEnv.setSustain(0.2);
	bell->mAEnv.setRelease(2.9);
	bell->mIEnv.setAttack(0.001);
	bell->mIEnv.setDecay(2);
	bell->mIEnv.setSustain(0.1);
	bell->mIEnv.setRelease(0.6);
	
	library.push_back(bell);
	mix.addInput(bell);

	initOSC(UDP_PORT);		// Set up OSC address space root
							// add the instrument library OSC
	setupOSCInstrLibrary(library);

	theIO->setRoot(mix);	// plug the mixer in as the IO client
	theIO->open();			// open the IO
						
	theIO->start();			// start the io
	mainOSCLoop();			// Run the main loop function (returns on quit)
	
	theIO->stop();
	theIO->close();
} 

#endif

///////////////////////// MAIN with 16 voices of file playback

#ifdef CSL_OSC_SAMPLER

int main(int argc, const char * argv[]) {
	READ_CSL_OPTS;									// load the standard CSL options: 
	logMsg("OSC server listening to port %d", UDP_PORT);
	logMsg("Setting up sampler library");
	std::vector<Instrument *> library;
	Mixer mix(2);				// Create the main stereo output mixer
	for (unsigned i = 0; i < 16; i++) {
		library.push_back(new SndFileInstrument(CGestalt::dataFolder(), "wet.snd"));
		mix.addInput( * library[i]);
	}
	initOSC(UDP_PORT);			// Set up OSC address space root
	setupOSCInstrLibrary(library);	// add the instrument library OSC
	theIO->setRoot(mix);		// plug the mixer in as the IO client
	theIO->open();				// open the IO
	theIO->start();				// start the io
	mainOSCLoop();				// Run the main loop function (returns on quit)
	theIO->stop();
	theIO->close();
} 

#endif

///////////////////////// MAIN with 16 voices of sum-of-sines synthesis

#ifdef CSL_OSC_ADDER

int main(int argc, const char * argv[]) {
	READ_CSL_OPTS;									// load the standard CSL options: 
	logMsg("OSC server listening to port %d", UDP_PORT);
	logMsg("Setting up sampler library");
	std::vector<Instrument *> library;
	Mixer mix(2);				// Create the main stereo output mixer
	for (unsigned i = 0; i < 16; i++) {
		library.push_back(new AdditiveInstrument());
		mix.addInput( * library[i]);
	}
	initOSC(UDP_PORT);			// Set up OSC address space root
	setupOSCInstrLibrary(library);	// add the instrument library OSC
	theIO->setRoot(mix);		// plug the mixer in as the IO client
	theIO->open();				// open the IO
	theIO->start();				// start the io
	mainOSCLoop();				// Run the main loop function (returns on quit)
	theIO->stop();
	theIO->close();
} 

#endif

///////////////////////// MAIN with 16 voices each of of fancy FM, samplers, and sum-of-sines synthesis

#ifdef CSL_OSC_ORCHESTRA

int main(int argc, const char * argv[]) {
	READ_CSL_OPTS;									// load the standard CSL options:
	logMsg("OSC server listening to port %d", UDP_PORT);
	logMsg("Setting up sampler library");
	//	std::vector<Instrument *> library;
	Mixer mix(2);										// Create the main stereo output mixer
	for (unsigned i = 0; i < 16; i++) {						// 16 FMs
		library.push_back(new FancyFMInstrument);
		mix.addInput( * library[i]);
	}
	for (unsigned i = 16; i < 32; i++) {					// 16 sound files
		library.push_back(new SndFileInstrument(CGestalt::dataFolder(), "wet.snd"));
		mix.addInput( * library[i]);
	}
	for (unsigned i = 32; i < 48; i++) {					// 16 SOS
		library.push_back(new AdditiveInstrument());
		mix.addInput( * library[i]);
	}
	initOSC(UDP_PORT);				// Set up OSC address space root
	setupOSCInstrLibrary(library);	// add the instrument library OSC
	theIO->setRoot(mix);			// plug the mixer in as the IO client
	theIO->open();					// open the IO
	theIO->start();					// start the IO
	mainOSCLoop();					// Run the main loop function (returns on quit)
	theIO->stop();
	theIO->close();
}

#endif
