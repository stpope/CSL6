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
extern bool gVerbose;

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
	printf("\tAdd: d %5.2f   a %5.2f   f %7.1f   p %5.2f\n", *fargs[0], *fargs[1], *fargs[2], *fargs[3]);
	mAEnv.setDuration(*fargs[0]);
	mAEnv.scaleValues(*fargs[1]);
	mSOS.setFrequency(*fargs[2]);
	mPanner.setPosition(*fargs[3]);
	if (nargs == 8) {
		printf("\t\ta %5.2f d %5.2f s %5.2f r %5.2f\n", fargs[4], fargs[5], fargs[6], fargs[7]);
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

#pragma mark VAdditiveInstrument // ------------------------------------------------------

// VAdditiveInstrument

// Constructor

//VAdditiveInstrument::VAdditiveInstrument() :
//		Instrument(),
//		mAEnv(2.0f, 0.03f, 0.06f, 0.5f, 0.15f),	// set up standard ADSR (1/4-sec duration)
//		mXEnv1(2.0f, 1.0f, 0.0001f),			// set up x-fade lin-segs
//		mXEnv2(2.0f, 0.0001f, 1.0f),
//		mSOS1(),								// SumOfSines init
//		mSOS2(),
//		mMix(2),								// mixer & panner
//		mPanner(mMix, 0.0f) {					// init the panner
//	mSOS1.setScale(mXEnv1);
//	mSOS1.setScale(mXEnv2);
//	mMix.addInput(mSOS1);
//	mMix.addInput(mSOS2);
//	this->init();
//}

VAdditiveInstrument::VAdditiveInstrument(SHARCSpectrum * spect1, SHARCSpectrum * spect2) :
		Instrument(),
		mAEnv1(2.0f, 0.5f, 0.1f, 1.0f, 0.5f),	// set up standard ADSRs (2-sec duration)
		mAEnv2(2.0f, 0.5f, 0.1f, 1.0f, 0.5f),
		mXEnv1(2.0f, 1.0f, 0.0001f),			// set up x-fade lin-segs
		mXEnv2(2.0f, 0.0001f, 1.0f),
		mSOS1(*spect1),							// SumOfSines init - use the given spectra
		mSOS2(*spect2),
		mMix(2),								// mixer & panner
		mPanner(mMix, 0.0) {					// init the panner
	mSOS1.setFrequency(110.0f);
	mSOS2.setFrequency(110.0f);
	mAEnv1.setScale(mXEnv1);
	mAEnv2.setScale(mXEnv2);
	mSOS1.setScale(mAEnv1);
	mSOS2.setScale(mAEnv2);
	mMix.addInput(mSOS1);
	mMix.addInput(mSOS2);
	this->init();
}

//VAdditiveInstrument::VAdditiveInstrument(VAdditiveInstrument&) {
//
//}

VAdditiveInstrument::~VAdditiveInstrument() {
	
}

void VAdditiveInstrument::init() {
	mNumChannels = 2;
	mName = "SpectralCrossFade";				// set graph's name
	mGraph = & mPanner;							// store the root of the graph as the inst var _graph
	mUGens["Osc1"] = & mSOS1;					// add ugens that can be monitored to the map
	mUGens["Osc2"] = & mSOS2;
	mUGens["A env1"] = & mAEnv1;
	mUGens["A env2"] = & mAEnv2;
	mUGens["X1 env"] = & mXEnv1;
	mUGens["X2 env"] = & mXEnv2;
	mUGens["Pos"] = & mPanner;
	mUGens["Mix"] = & mMix;
	mEnvelopes.push_back(& mAEnv1);				// list envelopes for retrigger
	mEnvelopes.push_back(& mAEnv2);
	mEnvelopes.push_back(& mXEnv1);
	mEnvelopes.push_back(& mXEnv2);
												// set up accessor vector
	mAccessors.push_back(new Accessor("du", set_duration_f, CSL_FLOAT_TYPE));
	mAccessors.push_back(new Accessor("am", set_amplitude_f, CSL_FLOAT_TYPE));
	mAccessors.push_back(new Accessor("fr", set_frequency_f, CSL_FLOAT_TYPE));
	mAccessors.push_back(new Accessor("po", set_position_f, CSL_FLOAT_TYPE));
	mAccessors.push_back(new Accessor("at", set_attack_f, CSL_FLOAT_TYPE));
	mAccessors.push_back(new Accessor("de", set_decay_f, CSL_FLOAT_TYPE));
	mAccessors.push_back(new Accessor("su", set_sustain_f, CSL_FLOAT_TYPE));
	mAccessors.push_back(new Accessor("re", set_release_f, CSL_FLOAT_TYPE));
}

/// Plug functions

void VAdditiveInstrument::setParameter(unsigned selector, int argc, void **argv, const char *types) {
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
		logMsg("Set %d V_SOS params to (%s)\n", argc, types);
		switch (selector) {					// switch on which parameter is being set
			case set_duration_f:
				mAEnv1.setDuration(d);
				mAEnv2.setDuration(d);
				mXEnv1.setDuration(d);
				mXEnv2.setDuration(d);
				break;
			case set_amplitude_f:
				mAEnv1.setScale(d);
				mAEnv2.setScale(d);
				break;
			case set_frequency_f:
				mSOS1.setFrequency(d);
				mSOS2.setFrequency(d);
				break;
			case set_position_f:
				mPanner.setPosition(d); 	break;
			case set_attack_f:
				mAEnv1.setAttack(d);
				mAEnv2.setAttack(d);
				break;
			case set_decay_f:
				mAEnv1.setDecay(d);
				mAEnv2.setDecay(d);
				break;
			case set_sustain_f:
				mAEnv1.setSustain(d);
				mAEnv2.setSustain(d);
				break;
			case set_release_f:
				mAEnv1.setRelease(d);
				mAEnv2.setRelease(d);
				break;
			default:
				logMsg(kLogError, "Unknown selector in VAdditiveInstrument set_parameter selector: %d\n", selector);
		}
	} else {			// multiple args
		logMsg(kLogError, "Unknown multi-arg (%d) setter in AdditiveInstrument: %s\n", argc, types);
	}
}

/// Play functions
/// OSC note formats (4 or 8 arguments):
/// 	dur, ampl, c_fr, pos
/// 	dur, ampl, c_fr, pos, att, dec, sus, rel

void VAdditiveInstrument::playOSC(int argc, void **argv, const char *types) {
	float ** fargs = (float **) argv;
	unsigned nargs;
	
	if ((argc == 4) && (strcmp(types, "ffff") == 0))
		nargs = 4;
	else if ((argc == 8) && (strcmp(types, "ffffffff") == 0))
		nargs = 8;
	else {
		logMsg(kLogError, "Invalid type string in OSC message, expected \"ff...ff\" got \"%s\"\n", types);
		return;
	}
	if (gVerbose)
		printf("\tV_Add: d %5.2f   a %5.2f   f %7.1f   p %5.2f\n", *fargs[0], *fargs[1], *fargs[2], *fargs[3]);
	mAEnv1.setDuration(*fargs[0]);
	mAEnv2.setDuration(*fargs[0]);
	mXEnv1.setDuration(*fargs[0]);
	mXEnv2.setDuration(*fargs[0]);
	mAEnv1.scaleValues(*fargs[1]);
	mAEnv2.scaleValues(*fargs[1]);
	mSOS1.setFrequency(*fargs[2]);
	mSOS2.setFrequency(*fargs[2]);
	mPanner.setPosition(*fargs[3]);
	if (nargs == 8) {
//		printf("\t\ta %5.2f d %5.2f s %5.2f r %5.2f\n", fargs[4], fargs[5], fargs[6], fargs[7]);
		mAEnv1.setAttack(*fargs[4]);
		mAEnv1.setDecay(*fargs[5]);
		mAEnv1.setSustain(*fargs[6]);
		mAEnv1.setRelease(*fargs[7]);
		mAEnv2.setAttack(*fargs[4]);
		mAEnv2.setDecay(*fargs[5]);
		mAEnv2.setSustain(*fargs[6]);
		mAEnv2.setRelease(*fargs[7]);
	}
	this->play();
}

void VAdditiveInstrument::playNote(float dur, float ampl, float c_fr, float pos, float att, float dec, float sus, float rel) {
	mAEnv1.setDuration(dur);
	mAEnv2.setDuration(dur);
	mXEnv1.setDuration(dur);
	mXEnv2.setDuration(dur);
	mAEnv1.scaleValues(ampl);
	mAEnv2.scaleValues(ampl);
	mSOS1.setFrequency(c_fr);
	mSOS2.setFrequency(c_fr);
	mPanner.setPosition(pos);
	mAEnv1.setAttack(att);
	mAEnv1.setDecay(dec);
	mAEnv1.setSustain(sus);
	mAEnv1.setRelease(rel);
	mAEnv2.setAttack(att);
	mAEnv2.setDecay(dec);
	mAEnv2.setSustain(sus);
	mAEnv2.setRelease(rel);
	this->play();
}

void VAdditiveInstrument::playMIDI(float dur, int chan, int key, int vel) {
	mAEnv1.setDuration(dur);
	mAEnv2.setDuration(dur);
	mXEnv1.setDuration(dur);
	mXEnv2.setDuration(dur);
	mAEnv1.scaleValues(sqrtf((float) vel / 128.0f));
	mAEnv2.scaleValues(sqrtf((float) vel / 128.0f));
	mSOS1.setFrequency(keyToFreq(key));
	mSOS2.setFrequency(keyToFreq(key));
	this->play();
}
