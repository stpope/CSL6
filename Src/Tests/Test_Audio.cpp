//
//	Test_Audio.cpp -- Streaming audio IO tests
//
//	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
//

#ifdef USE_JUCE
	#include "Test_Support.h"
	#include "JuceHeader.h"
#else
	#define USE_TEST_MAIN			// use the main() function in test_support.h
	#include "Test_Support.cpp"		// include all of CSL core and the test support functions
#endif

using namespace csl;

extern juce::AudioDeviceManager * gAudioDeviceManager;	// global JUCE audio device mgr

// dump io port-type & device names

void audio_dump() {
	for (unsigned i = 0; i < gAudioDeviceManager->getAvailableDeviceTypes().size(); i++) {
		logMsg("	AudioIODeviceType %s", 
			(const char *) gAudioDeviceManager->getAvailableDeviceTypes().getUnchecked(i)->getTypeName().toUTF8());
		juce::StringArray devs = gAudioDeviceManager->getAvailableDeviceTypes().getUnchecked(i)->getDeviceNames();
		for (unsigned j = 0; j < devs.size(); j++) {
			logMsg("		Device %s", (const char *) devs[j].toUTF8() );
		}
	}
	juce::AudioIODevice *  adm = gAudioDeviceManager->getCurrentAudioDevice();
	logMsg("Current AudioIODevice = %s : %s", (const char *) adm->getTypeName().toUTF8(), 
		(const char *) adm->getName().toUTF8());
}

// echo mic back using an InOut

void mic_test() {
//	CGestalt::setNumInChannels(2);
									// set up InOut UGen to copy input to effect
	InOut mio(theIO, 2, 2);			// stereo i/o
	logMsg("playing mic in unaltered...");
	runTest(mio, 10);				// 10 sec
	logMsg("done.\n");
}

// Apply a BPF to the input

void filt_test() {
	InOut mio(theIO, 2, 1);			// mono input
	Butter bpf(mio, BW_BAND_PASS, 1000.f, 100.f);
	logMsg("playing filter on mic in...");
	runTest(bpf, 10);				// 10 sec
	logMsg("done.\n");
}

// Add echo to the input

void echo_test() {
	InOut mio(theIO, 2, 2);			// stereo i/o
	Stereoverb mReverb(mio);		// stereo reverb
	mReverb.setRoomSize(0.988);		// long reverb time
	logMsg("playing echo on mic in...");
	runTest(mReverb, 10);			// 10 sec
	logMsg("done.\n");
}

// pan the input

void panner_test() {
	InOut mio(theIO, 2, 1);			// mono input
	Osc lfo(0.5, 1, 0, CSL_PI);		// position LFO
	Panner pan(mio, lfo);			// panner
	logMsg("playing panner on mic in...");
	runTest(pan, 10);				// 10 sec
	logMsg("done.\n");
}

//////////////////////////////////////////////////////////////////////////////

// make an Audio listener class -- implement update() and store in-coming audio

//class AudioListener : public Observer {
//public:
//								// update message prints to the log
//	void update(void * arg) {
//	}
//								// constructor
//	AudioListener(MIDIIn * in) : mIn(in) { };
//								// data member for the input
//	AudioListener * mIn;
//
//};
//
///// Create a MIDI in and attach a filtering listener to it
//
void listener_test() {
//	MIDIIn in;
//	in.open(DEFAULT_MIDI_IN);
//	MIDIListener lst(&in);			// create a listener
////	lst.mPeriod = 0.25;
//	lst.mKey = kNoteOn;				// filter noteOn events
//	in.attachObserver(&lst);		    // attach observer to input
//	logMsg("Start MIDI listener");
//	in.start();
//	
//	sleepSec(10);
//	
//	logMsg("done.");
//	in.stop();
//	in.close();
}

// test list for Juce GUI

testStruct audioTestList[] = {
	"Dump audio ports",		audio_dump,		"Dump list of audio ports to stdout",
    "THE REST OF THESE ARE W.I.P.", 0, "",
//    "Echo audio in",        mic_test,        "Play the microphone input back the output",
//    "Filter input",            filt_test,        "Apply a band-pass filter to the live input",
//    "Echo input",            echo_test,        "Add echo to the live input",
//    "Input panner",            panner_test,    "Stereo panner on the live input",
//    "Input listener",       listener_test,    "Demonstrate recording input listener",
	NULL,					NULL,			NULL
};
