///
/// StringInstrument.cpp -- Karplus-Strong string instrument class.
///	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
/// 

/// Accessors
///		"du", set_duration_f -- ignored
///		"am", set_amplitude_f
///		"fr", set_frequency_f
///		"po", set_position_f
///
/// OSC note formats (4 arguments):
/// 	dur, ampl, c_fr, pos

#include "StringInstrument.h"
extern bool gVerbose;

using namespace csl;

// The constructor initializes the DSP graph's UGens

StringInstrument::StringInstrument(float ampl, float c_fr, float pos) :	// initializers for the UGens
		Instrument(),
        mString(c_fr),                          // init the KS string
		mPanner(mString, pos) {					// init the panner
	mString.setScale(ampl);
	this->init();
}

// setup instance

void StringInstrument::init() {
	mNumChannels = 2;
	mName = "K-S_String";						// set graph's name
	mGraph = & mPanner;							// store the root of the graph as the inst var _graph
	mUGens["String"] = & mString;				// add ugens that can be monitored to the map
	mEnvelopes.push_back(& mString);			// list envelopes for retrigger
												// set up accessor vector
//	mAccessors.push_back(new Accessor("du", set_duration_f, CSL_FLOAT_TYPE));
	mAccessors.push_back(new Accessor("am", set_amplitude_f, CSL_FLOAT_TYPE));
	mAccessors.push_back(new Accessor("fr", set_frequency_f, CSL_FLOAT_TYPE));
	mAccessors.push_back(new Accessor("po", set_position_f, CSL_FLOAT_TYPE));
}

// copy constructor

//StringInstrument::StringInstrument(AdditiveInstrument& in) :
//    Instrument(in),
//    mAEnv(in.mAEnv),
//       mSOS(in.mSOS),
//    mPanner(in.mPanner) { }

// The destructor frees the stuff we allocated

StringInstrument::~StringInstrument() { }

// Plug function for use by OSC setter methods

void StringInstrument::setParameter(unsigned selector, int argc, void **argv, const char *types) {
	if (argc == 1) {
		float d = * (float *) argv[0];
		if (types[0] == 'i') 
			d = (float) (* (int *) argv[0]);
		float s;
		if (types[0] == 'i') 
			d = (float) (* (int *) argv[0]);
		if (argc > 1) {
			s = * (float *) argv[1];
			if (types[1] == 'i') 
				s = (float) (* (int *) argv[1]);
		}
//		logMsg("Set %d SOS params to (%s)\n", argc, types);
		switch (selector) {					// switch on which parameter is being set
			case set_amplitude_f:
				mString.setScale(d);		break;
			case set_frequency_f:
				mString.setFrequency(d); 	break;
			case set_position_f:
				mPanner.setPosition(d); 	break;
			default:
				logMsg(kLogError, "Unknown selector in StringInstrument set_parameter selector: %d\n", selector);
		 }
	} else {			// multiple args
        logMsg(kLogError, "Unknown multi-arg (%d) setter in StringInstrument: %s\n", argc, types);
	}
}

/// OSC note formats (4 arguments, dur is ignored for now):
/// 	dur, ampl, c_fr, pos

void StringInstrument::playOSC(int argc, void **argv, const char *types) {
	float ** fargs = (float **) argv;
	unsigned nargs;

	if ((strcmp(types, "fff") == 0) && (argc == 3))
		nargs = 3;
	else {
		logMsg(kLogError, "Invalid type string in OSC message, expected \"fff\" got \"%s\"\n", types);
		return;
	}
	if (gVerbose)
			printf("\tString: a %5.3f   f %5.1f   p %5.3f\n", *fargs[0], *fargs[1], *fargs[2]);
//	mAEnv.setDuration(*fargs[0]);           // dur ignored
	mString.setScale(*fargs[0]);
	mString.setFrequency(*fargs[1]);
	mPanner.setPosition(*fargs[2]);
	this->play();
}

// Play a note specifying all parameters

void StringInstrument::playNote(float ampl, float c_fr, float pos) {
//	mAEnv.setDuration(dur);
	mString.setScale(ampl);
	mString.setFrequency(c_fr);
	mPanner.setPosition(pos);
	this->play();
}


void StringInstrument::playMIDI(float dur, int chan, int key, int vel) {
//	mAEnv.setDuration(dur);
	mString.setScale(sqrtf((float) vel / 128.0f));
	mString.setFrequency(keyToFreq(key));
	this->play();
}
