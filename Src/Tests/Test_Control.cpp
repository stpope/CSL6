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

extern juce::AudioDeviceManager * gAudioDeviceManager;	// global JUCE audio device mgr
    
// dump io names

void IO_test() {
	MIDIIO::dumpDevices();
}

// echo 10 events of input

//#define DEFAULT_MIDI_IN MidiInput::getDefaultDeviceIndex()
//#define DEFAULT_MIDI_IN MidiOutput::getDefaultDeviceIndex()

// Run the IO_test, then edit these

void input_test() {
	logMsg("MIDI test");
	logMsg("\tDefault MIDI in = %d", juce::MidiInput::getDefaultDeviceIndex());
	logMsg("\tDefault MIDI out = %d", juce::MidiOutput::getDefaultDeviceIndex());
	MIDIIn in;
	in.open(juce::MidiInput::getDefaultDeviceIndex());
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

// MIDI file player uses instrument library

#include <AdditiveInstrument.h>
#include <BasicFMInstrument.h>

void testMIDIFile() {
	InstrumentLibrary lib;		// create the instrument library map
	Mixer mix(2);				// Create the main stereo output mixer
								// Now add 16 instruments 
	logMsg("Setting up instrument library");
	for (unsigned i = 0; i < 16; i++) {
		FMInstrument * in = new FMInstrument;
		lib[i].push_back(in);	// map of inst vectors
		mix.addInput(in);		// add to mix
	}
//	for (unsigned i = 0; i < lib.size(); i++) {
//		logMsg("IL %d: %d", i, lib[i].size());
//	}
    MulOp mult(mix, 0.4);       // use a MulOp to scale down
	Stereoverb rev(mult);		// stereo reverb
	rev.setRoomSize(0.8);		// medium-length reverb
	
	logMsg("Load MIDI score");
	MIDIPlayer pla(CGestalt::dataFolder(), "pravol12c.mid", &lib);
	
	logMsg("Start MIDI player");
	theIO->setRoot(rev);		// plug the reverb in as the IO client
	theIO->open();				// open the IO
	theIO->start();				// start the io
	
	pla.start(-1);				// play all tracks real-time
	
	sleepSec(2);				// let the reverb die out
	
	theIO->clearRoot();			// shut down
	pla.stop();
	logMsg("done.");
	theIO->stop();
	theIO->close();
	mix.deleteInputs();
}

//////////////////////////////////////////////////////////////////////////////

#ifdef USE_LOSC					// liblo for OSC

#include "lo/lo.h"				// liblo header has to be in your path (/usr/local/include)
#include "OSC_support.h"

// Server only (run client in a separate shell)

void testOSCServer() {
	InstrumentVector lib;
	logMsg("Setting up instrument library");
	Mixer mix(2);				// Create the main stereo output mixer
								// Now add 16  instruments 
	for (unsigned i = 0; i < 16; i++) {
		AdditiveInstrument * in = new AdditiveInstrument;
		lib.push_back(in);
		mix.addInput(*in);
	}
	Stereoverb rev(mix);		// stereo reverb
	rev.setRoomSize(0.8);		// medium-length reverb
	theIO->setRoot(rev);		// plug the mixer in as the IO client
	theIO->open();				// open the IO
	theIO->start();				// start the IO

	char pNam[CSL_WORD_LEN];	// string for port number
	sprintf(pNam, "%d", CGestalt::outPort());
	
	initOSC(pNam);				// Set up OSC address space root
	
	setupOSCInstrLibrary(lib);	// add the instrument library OSC
	
	mainOSCLoop(NULL);			// Run the OSC loop function (it exits on the quit command)
	logMsg("OSC server waiting for input\n");

	sleepSec(2);				// let the reverb dieout
	theIO->clearRoot();			// shut down
	theIO->stop();
	theIO->close();
	mix.deleteInputs();
}

// set up an OSC client/server with an instrument library

void testOSCClientServer() {
	InstrumentVector lib;
	logMsg("Setting up instrument library");
	Mixer mix(2);				// Create the main stereo output mixer
								// Now add 16  instruments 
	for (unsigned i = 0; i < 16; i++) {
		AdditiveInstrument * in = new AdditiveInstrument;
		lib.push_back(in);
		mix.addInput(*in);
	}
	Stereoverb rev(mix);		// stereo reverb
	rev.setRoomSize(0.8);		// medium-length reverb
	theIO->setRoot(rev);		// plug the mixer in as the IO client
	theIO->open();				// open the IO
	theIO->start();				// start the IO

	char pNam[CSL_WORD_LEN];	// string for port number
	sprintf(pNam, "%d", CGestalt::outPort());
	
	initOSC(pNam);				// Set up OSC address space root
	setupOSCInstrLibrary(lib);	// add the instrument library OSC

								// make a thread for the OSC server
	CThread * osc = CThread::MakeThread();
								// Run the OSC loop function (it exits on the quit command)
	osc->createThread(mainOSCLoop, NULL);
	sleepSec(0.5);				/// wait for it to start
	
								// now we're the OSC client
								// set up a port to write to the server thread
	lo_address ad = lo_address_new(NULL, pNam);
	logMsg("Sending OSC note cmds");
//	if (lo_send(ad, "/i1/p", "") == -1) {		// send a test note command
//		logMsg(kLogError, "OSC error1 %d: %s\n", lo_address_errno(ad), lo_address_errstr(ad));
//		goto done;
//	}
//	sleepSec(0.5);
	for (unsigned i = 0; i < 64; i++) {			// note loop
		sprintf(pNam, "/i%d/pn", (i % 16)+1);	// OSC cmd: /iX/pn, "ffff", dur ampl freq pos
		float dur = fRandM(0.15f, 1.5f);
		float ampl = fRandM(0.2, 0.8);
		float freq = fRandM(60, 500);
		float pos = fRand1();
		if (lo_send(ad, pNam, "ffff", dur, ampl, freq, pos) == -1) {
			logMsg(kLogError, "OSC error2 %d: %s\n", lo_address_errno(ad), lo_address_errstr(ad));
		}
		sleepSec(fRandM(0.06, 0.3));			// sleep a bit
	}
done:
	sleepSec(2);				// let the reverb dieout
	logMsg("Sending OSC quit");
	if (lo_send(ad, "/q", NULL) == -1) {
		logMsg(kLogError, "OSC error3 %d: %s\n", lo_address_errno(ad), lo_address_errstr(ad));
	}
	theIO->clearRoot();			// shut down
	theIO->stop();
	theIO->close();
	mix.deleteInputs();
}

#endif // USE_LOSC

// test list for Juce GUI

testStruct ctrlTestList[] = {
    "Dump ports",           IO_test,        "Dump list of MIDI ports to stdout",
    "MIDI file player",     testMIDIFile,   "Play a MIDI file on an instrument library",
//    "THE REST OF THESE ARE W.I.P.", 0, "",
//    "Dump input",           input_test,     "Dump MIDI input from default device",
//    "MIDI notes",           play_test,      "Play MIDI notes (reads MIDI kbd)",
//    "MIDI output",          output_test,    "Test sending MIDI output",
//    "MIDI listener",        testListener,   "Start the MIDI listener object",
#ifdef USE_LOSC				                        	// liblo for OSC
	"OSC client/server",	testOSCClientServer,	"OSC client/server on a library",
	"OSC server",			testOSCServer,	        "Start OSC server on a library",
#endif
	NULL,					NULL,			NULL
};

#endif
