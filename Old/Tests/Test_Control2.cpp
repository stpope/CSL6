//
//	Test_Control.cpp -- MIDI and OSC control IO tests
//
//	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
//

#ifdef USE_JUCE
	#include "Test_Support.h"
#else
	#define USE_TEST_MAIN			// use the main() function in test_support.h
	#include "Test_Support.cpp"		// include all of CSL core and the test support functions
#endif

#ifdef USE_JMIDI					// new way with JUCE midi

#include "MIDIIOJ.h"

#include <iostream>

using namespace csl;

extern AudioDeviceManager * gAudioDeviceManager;	// global JUCE audio device mgr

// dump io names

void IO_test() {
	MIDIIO::dumpDevices();
}

// echo 10 events of input

//#define DEFAULT_MIDI_IN MidiInput::getDefaultDeviceIndex()
//#define DEFAULT_MIDI_IN MidiOutput::getDefaultDeviceIndex()

// Run the IO_test, then edit these

#define DEFAULT_MIDI_IN 3
#define DEFAULT_MIDI_OUT 3

void input_test() {
	MIDIIn in;
	in.open(DEFAULT_MIDI_IN);
	in.start();

	try {
		int cnt = 0;
		while (cnt++ < 10) {			// loop to read 10 events
			while ( ! in.poll())		// poll and wait
				sleepSec(0.1);
			in.dumpMessage();			// dump received msg
			in.clear();					// clear input
		}
	} catch (CException ex) {
		logMsg(kLogError, "CSL MIDI Error %s...", ex.what());
	}	
	in.stop();
	in.close();
}

// play simple notes

void play_test() {
	MIDIIn in;
	in.open(DEFAULT_MIDI_IN);
	in.start();

	SumOfSines osc(kFrequency, 6,   0.4, 0.3, 0.2, 0.08, 0.05, 0.02);	// create an osc + env patch
	ADSR env(1.0, 0.05, 0.1, 0.7, 0.6);
	osc.setScale(env);
	osc.setFrequency(1017.0f);
	Freeverb rev(osc);					// stereo reverb
	rev.setRoomSize(0.92);				// longer reverb

	theIO->setRoot(rev);
	logMsg("testing MIDIIn playback");

	while(1) {								// here's the poll/play loop
		if(in.poll()) {
			if(in.mMsg.isNoteOn()) {
				logMsg("MIDI note %d  vel. %d", in.mMsg.getNote(), in.mMsg.getVelocity());
				osc.setFrequency(in.mMsg.getFrequency());
				env.scaleValues(in.mMsg.getVelocityFloat() / 2.0f );
				env.trigger();
			}
			if(in.mMsg.isNoteOff()) {
				env.release();
			}
			in.clear();						// clear input
		} else {
			sleepSec(0.1);					// sleep 100 msec
		}
	}
	in.stop();
	in.close();
}

// generate MIDI output 

void output_test() {

}

//////////////////////////////////////////////////////////////////////////////

// make a MIDI listener class -- implement update() and filter on midi command

class MIDIListener : public Observer {
public:
								// update message prints to the log
	void update(void * arg) {
		CMIDIMessage * msg = (CMIDIMessage *) arg;
		logMsg("MIDI msg %d k %d  v %d", msg->message, msg->getNote(), msg->getVelocity());
		mIn->clear();
	}
								// constructor
	MIDIListener(MIDIIn * in) : mIn(in) { };
								// data member for the input
	MIDIIn * mIn;

};

/// Create a MIDI in and attach a filtering listener to it

void testListener() {
	MIDIIn in;
	in.open(DEFAULT_MIDI_IN);
	MIDIListener lst(&in);			// create a listener
//	lst.mPeriod = 0.25;
	lst.mKey = kNoteOn;				// filter noteOn events
	in.attachObserver(&lst);		// attach observer to input
	logMsg("Start MIDI listener");
	in.start();
	
	sleepSec(10);
	
	logMsg("done.");
	in.stop();
	in.close();
}

// MIDI file Player uses instrument library

#include <BasicFMInstrument.h>
#include <AdditiveInstrument.h>

void testMIDIFile() {
	InstrumentLibrary library;	// create the instrument library
	logMsg("Setting up instrument library");
	Mixer mix(2);				// Create the main stereo output mixer
								// Now add 16 instruments 
	for (unsigned i = 0; i < 16; i++) {
		AdditiveInstrument * in = new AdditiveInstrument;
		library[i].push_back(in);
		mix.addInput(in);
	}
	Stereoverb rev(mix);		// stereo reverb
	rev.setRoomSize(0.85);		// medium-length reverb
	
	MIDIPlayer pla(CGestalt::dataFolder(), "pravol12c.mid", &library);
	logMsg("Start MIDI player");
	theIO->setRoot(rev);		// plug the reverb in as the IO client
	theIO->open();				// open the IO
	theIO->start();				// start the io
	pla.start(-1);				// play all tracks
	
	sleepSec(2);				// let the reverb die out
	
	theIO->clearRoot();			// shut down
	pla.stop();
	logMsg("done.");
	theIO->stop();
	theIO->close();
}

//////////////////////////////////////////////////////////////////////////////

// set up an OSC server with an instrument library

#include "OSC_support.h"

void testOSCServer() {
	InstrumentVector lib;
	logMsg("Setting up instrument library");
	Mixer mix(2);				// Create the main stereo output mixer
								// Now add 16  instruments 
	for (unsigned i = 0; i < 16; i++) {
		lib.push_back(new BasicFMInstrument);
		mix.addInput( * lib[i]);
	}
	char pNam[CSL_WORD_LEN];
	sprintf(pNam, "%d", CGestalt::outPort());
	
	initOSC(pNam);				// Set up OSC address space root
	setupOSCInstrLibrary(lib);	// add the instrument library OSC

	theIO->setRoot(mix);		// plug the mixer in as the IO client
	theIO->open();				// open the IO
	theIO->start();				// start the io
	
	mainOSCLoop();				// Run the main loop function (returns on quit)
	
	theIO->stop();
	theIO->close();
}

// test list for Juce GUI

testStruct ctrlTestList[] = {
	"Dump ports",		IO_test,
	"Dump input",		input_test,
	"MIDI notes",		play_test,
	"MIDI output",		output_test,
	"MIDI listener",	testListener,
	"MIDI file player",	testMIDIFile,
	"OSC server",		testOSCServer,
	NULL,				NULL
};

#endif
