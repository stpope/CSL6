///
/// AdditiveInstrument.h -- Sum-of-sines synthesis instrument class.
///	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
/// 

/// Accessors
///		"du", set_duration_f
///		"am", set_amplitude_f
///		"fr", set_frequency_f
///		"po", set_position_f
///		"at", set_attack_f	-- amplitude envelope ADSR
///		"de", set_decay_f
///		"su", set_sustain_f
///		"re", set_release_f
///		"pt", set_partial_f
///		"ps", set_partials_f
///
/// OSC note formats (4 or 8 arguments):
/// 	dur, ampl, c_fr, pos
/// 	dur, ampl, c_fr, pos, att, dec, sus, rel

#include "AdditiveInstrument.h"

using namespace csl;

// The constructor initializes the DSP graph's UGens

AdditiveInstrument::AdditiveInstrument() :	// initializers for the UGens
		Instrument(),
		mAEnv(0.25, 0.03, 0.06, 0.5, 0.15),		// set up standard ADSR (1/4-sec duration)
		mSOS(16, 0.75),							// SumOfSines init, 16 partials @ 1/f
		mPanner(mSOS, 0.0) {					// init the panner
	this->init();
}

AdditiveInstrument::AdditiveInstrument(SumOfSines & sos) :	// initializers for the UGens
		Instrument(),
		mAEnv(0.25, 0.03, 0.06, 0.5, 0.15),		// set up standard ADSR (1/4-sec duration)
		mSOS(sos),								// SumOfSines init, 16 partials @ 1/f
		mPanner(mSOS, 0.0) {					// init the panner
	this->init();
}

AdditiveInstrument::AdditiveInstrument(unsigned numHarms, float noise) :	// initializers for the UGens
		Instrument(),
		mAEnv(0.25, 0.03, 0.06, 0.5, 0.15),		// set up standard ADSR (1/4-sec duration)
		mSOS(numHarms, noise),					// SumOfSines init, numHarms partials @ 1/f
		mPanner(mSOS, 0.0) {					// init the panner
	this->init();
}

AdditiveInstrument::AdditiveInstrument(SHARCSpectrum & spect) :
		Instrument(),
		mAEnv(0.25, 0.03, 0.06, 0.5, 0.15),		// set up standard ADSR (1/4-sec duration)
		mSOS(spect),								// SumOfSines init, 16 partials @ 1/f
		mPanner(mSOS, 0.0) {					// init the panner
	this->init();
}

// setup instance

void AdditiveInstrument::init() {
	mSOS.setScale(mAEnv);						// plug in the envelope
	mNumChannels = 2;
	mName = "SumOfSines";						// set graph's name
	mGraph = & mPanner;							// store the root of the graph as the inst var _graph
	mUGens["Osc"] = & mSOS;						// add ugens that can be monitored to the map
	mUGens["A env"] = & mAEnv;
	mUGens["Pos"] = & mPanner;
	mEnvelopes.push_back(& mAEnv);				// list envelopes for retrigger
												// set up accessor vector
	mAccessors.push_back(new Accessor("du", set_duration_f, CSL_FLOAT_TYPE));
	mAccessors.push_back(new Accessor("am", set_amplitude_f, CSL_FLOAT_TYPE));
	mAccessors.push_back(new Accessor("fr", set_frequency_f, CSL_FLOAT_TYPE));
	mAccessors.push_back(new Accessor("po", set_position_f, CSL_FLOAT_TYPE));
	mAccessors.push_back(new Accessor("at", set_attack_f, CSL_FLOAT_TYPE));
	mAccessors.push_back(new Accessor("de", set_decay_f, CSL_FLOAT_TYPE));
	mAccessors.push_back(new Accessor("su", set_sustain_f, CSL_FLOAT_TYPE));
	mAccessors.push_back(new Accessor("re", set_release_f, CSL_FLOAT_TYPE));
	mAccessors.push_back(new Accessor("pt", set_partial_f, CSL_FLOAT_TYPE));
	mAccessors.push_back(new Accessor("ps", set_partials_f, CSL_FLOAT_TYPE));
}

// copy constructor

AdditiveInstrument::AdditiveInstrument(AdditiveInstrument& in) :
	Instrument(in),
	mAEnv(in.mAEnv),
   	mSOS(in.mSOS),
	mPanner(in.mPanner) { }

// The destructor frees the stuff we allocated

AdditiveInstrument::~AdditiveInstrument() { }

// Plug function for use by OSC setter methods

void AdditiveInstrument::setParameter(unsigned selector, int argc, void **argv, const char *types) {
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
			case set_duration_f:
				mAEnv.setDuration(d);		break;
			case set_amplitude_f:
				mAEnv.setScale(d);			break;
			case set_frequency_f:
				mSOS.setFrequency(d); 		break;	
			case set_position_f:
				mPanner.setPosition(d); 	break;
			case set_attack_f:
				mAEnv.setAttack(d);			break;
			case set_decay_f:
				mAEnv.setDecay(d);			break;
			case set_sustain_f:
				mAEnv.setSustain(d);		break;
			case set_release_f:
				mAEnv.setRelease(d);		break;
			case set_partial_f:
				mSOS.addPartial(d, s);		break;
			default:
				logMsg(kLogError, "Unknown selector in AdditiveInstrument set_parameter selector: %d\n", selector);
		 }
	} else {			// multiple args
		if (selector == set_partials_f) {
			logMsg("Set %d SOS partials to (%s)\n", argc, types);
			mSOS.addPartials(argc, argv); 
		} else {
			logMsg(kLogError, "Unknown multi-arg (%d) setter in AdditiveInstrument: %s\n", argc, types);
		}
	}
}

/// OSC note formats (4 or 8 arguments):
/// 	dur, ampl, c_fr, pos
/// 	dur, ampl, c_fr, pos, att, dec, sus, rel

void AdditiveInstrument::playOSC(int argc, void **argv, const char *types) {
	float ** fargs = (float **) argv;
	unsigned nargs;

//	if (strcmp(types, "ffff") == 0)
	if (argc == 4)
		nargs = 4;
//	else if (strcmp(types, "ffffffff") == 0)
	else if (argc == 8)
		nargs = 8;
	else {
		logMsg(kLogError, "Invalid type string in OSC message, expected \"ff...ff\" got \"%s\"\n", types);
		return;
	}
	printf("\tAdd: d %5.2f   a %5.2f   f %7.1f   p %5.2f\n", 
			*fargs[0], *fargs[1], *fargs[2], *fargs[3]);
	mAEnv.setDuration(*fargs[0]);
	mAEnv.scaleValues(*fargs[1]);
	mSOS.setFrequency(*fargs[2]);
	mPanner.setPosition(*fargs[3]);
	if (nargs == 8) {
		printf("\t\ta %5.2f d %5.2f s %5.2f r %5.2f\t\ta %5.2f d %5.2f s %5.2f r %5.2f\n", 
			fargs[6], fargs[7], fargs[8], fargs[9], fargs[10], fargs[11], fargs[12], fargs[13]);
		mAEnv.setAttack(*fargs[4]);
		mAEnv.setDecay(*fargs[5]);
		mAEnv.setSustain(*fargs[6]);
		mAEnv.setRelease(*fargs[7]);
	}
	this->play();
}

// Play a note specifying all parameters

void AdditiveInstrument::playNote(float dur, float ampl, float c_fr, float pos, float att, float dec, float sus, float rel) {
	mAEnv.setDuration(dur);
	mAEnv.scaleValues(ampl);
	mSOS.setFrequency(c_fr);
	mPanner.setPosition(pos);
	mAEnv.setAttack(att);
	mAEnv.setDecay(dec);
	mAEnv.setSustain(sus);
	mAEnv.setRelease(rel);
	this->play();
}


void AdditiveInstrument::playMIDI(float dur, int chan, int key, int vel) {
	mAEnv.setDuration(dur);
	mAEnv.scaleValues(sqrtf((float) vel / 128.0f));
	mSOS.setFrequency(keyToFreq(key));
	this->play();
}
