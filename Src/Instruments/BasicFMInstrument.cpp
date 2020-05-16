//
// BasicFMInstrument.cpp -- Simple FM example instrument class.
//	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
// 
// Implementation of the 2-envelope FM example as an instrument class.
// This example shows how to create an instrument class from a DSP graph and set up its accessors for use with OSC.

#include "BasicFMInstrument.h"

using namespace csl;

#define BASE_FREQ 70.0

// The constructor initializes the DSP graph's UGens

FMInstrument::FMInstrument() :		// initializers for the UGens
		Instrument(),						// inherited constructor
		mAEnv(4.0, 0.2, 0.05, 0.5, 0.5),	// set up 2 standard ADSRs
		mIEnv(4.0, 1.95, 0.1, 0.9, 1.95),
		mMod(BASE_FREQ),					// modulator osc
		mPanner(mCar, 0.0) {				// init the panner
	mIEnv.scaleValues(BASE_FREQ / 3);		// scale the index envelope by the mod. freq
//	mIEnv.setScale(0.5);
	mMod.setScale(mIEnv);
	mMod.setOffset(BASE_FREQ);
	mCar.setFrequency(mMod);
	mCar.setScale(mAEnv);
	mNumChannels = 2;

	mName = "Basic_FM";
	mGraph = & mPanner;						// store the root of the graph as the inst var _graph
	mUGens["Carrier"] = & mCar;				// add ugens that can be monitored to the map
	mUGens["Modulator"] = & mMod;
	mUGens["F env"] = & mIEnv;
	mUGens["A env"] = & mAEnv;
	mUGens["Panner"] = & mPanner;

	mEnvelopes.push_back(& mAEnv);			// list envelopes for retrigger
	mEnvelopes.push_back(& mIEnv);
											// set up accessor vector
	mAccessors.push_back(new Accessor("du", set_duration_f, CSL_FLOAT_TYPE));
	mAccessors.push_back(new Accessor("am", set_amplitude_f, CSL_FLOAT_TYPE));
	mAccessors.push_back(new Accessor("in", set_index_f, CSL_FLOAT_TYPE));
	mAccessors.push_back(new Accessor("cf", set_c_freq_f, CSL_FLOAT_TYPE));
	mAccessors.push_back(new Accessor("mf", set_m_freq_f, CSL_FLOAT_TYPE));
	mAccessors.push_back(new Accessor("po", set_position_f, CSL_FLOAT_TYPE));
	mAccessors.push_back(new Accessor("aa", set_attack_f, CSL_FLOAT_TYPE));
	mAccessors.push_back(new Accessor("ad", set_decay_f, CSL_FLOAT_TYPE));
	mAccessors.push_back(new Accessor("as", set_sustain_f, CSL_FLOAT_TYPE));
	mAccessors.push_back(new Accessor("ar", set_release_f, CSL_FLOAT_TYPE));
	mAccessors.push_back(new Accessor("ia", set_iattack_f, CSL_FLOAT_TYPE));
	mAccessors.push_back(new Accessor("id", set_idecay_f, CSL_FLOAT_TYPE));
	mAccessors.push_back(new Accessor("is", set_isustain_f, CSL_FLOAT_TYPE));
	mAccessors.push_back(new Accessor("ir", set_irelease_f, CSL_FLOAT_TYPE));
}

// copy constructor

FMInstrument::FMInstrument(FMInstrument& in) :
	Instrument(in),
	mAEnv(in.mAEnv),
	mIEnv(in.mIEnv),
   	mCar(in.mCar),
   	mMod(in.mMod),
	mPanner(in.mPanner) { }

// The destructor frees the stuff we allocated

FMInstrument::~FMInstrument() { }

// Plug function for use by OSC setter methods

void FMInstrument::setParameter(unsigned selector, int argc, void **argv, const char *types) {
	if (argc == 1) {
		float d = * (float *) argv[0];
		if (types[0] == 'i') 
			d = (float) (* (int *) argv[0]);
		switch (selector) {					// switch on which parameter is being set
			case set_duration_f:
				mAEnv.setDuration(d);
				mIEnv.setDuration(d);
				break;
			case set_amplitude_f:
				mAEnv.scaleValues(d); break;
			case set_index_f:
				mIEnv.scaleValues(d); break;
			case set_c_freq_f:
				mMod.setOffset(d); 
	//			printf("\tcf %g\n", * (float *)d);
				break;	
			case set_m_freq_f:
				mMod.setFrequency(d); break;
			case set_position_f:
				mPanner.setPosition(d); 
	//			printf("\tpo %g\n", * (float *)d);
				break;
			case set_attack_f:
				mAEnv.setAttack(d); break;
			case set_decay_f:
				mAEnv.setDecay(d); break;
			case set_sustain_f:
				mAEnv.setSustain(d); break;
			case set_release_f:
				mAEnv.setRelease(d); break;
			case set_iattack_f:
				mIEnv.setAttack(d); break;
			case set_idecay_f:
				mIEnv.setDecay(d); break;
			case set_isustain_f:
				mIEnv.setSustain(d); break;
			case set_irelease_f:
				mIEnv.setRelease(d); break;
			default:
				logMsg(kLogError, "Unknown selector in FMInstrument set_parameter selector: %d\n", selector);
		 }
	} else {			// multiple args
		logMsg(kLogError, "Unknown multi-arg (%d) setter in FMInstrument: %s\n", argc, types);
	}
}

// Formats (4, 6 or 14 arguments):
// 	dur, ampl, fr, pos
// 	dur, ampl, c_fr, m_fr, ind, pos
// 	dur, ampl, c_fr, m_fr, ind, pos, att, dec, sus, rel, i_att, i_dec, i_sus, i_rel

void FMInstrument::parseArgs(int argc, void **argv, const char *types) {
	float ** fargs = (float **) argv;
	unsigned nargs;
	if (strcmp(types, "ffff") == 0) {
		nargs = 4;
		printf("\tFM:  d %5.2f   a %5.2f   f %7.1f   p %5.2f\n", 
			*fargs[0], *fargs[1], *fargs[2], *fargs[3]);
	} else if (strcmp(types, "ffffff") == 0) {
		nargs = 6;
		printf("\tFM: d %g   a %g c %g m %g i %g   p %g\n", 
			*fargs[0], *fargs[1], *fargs[2], *fargs[3], *fargs[4], *fargs[5]);
	} else if (strcmp(types, "ffffffffffffff") == 0)
		nargs = 14;
	else {
		logMsg(kLogError, "Invalid type string in OSC message, expected \"ff...ff\" got \"%s\"\n", types);
		mAEnv.scaleValues(0.0f);
		return;
	}
	mAEnv.setDuration(*fargs[0]);
	mIEnv.setDuration(*fargs[0]);
	mAEnv.scaleValues(*fargs[1]);
	mMod.setOffset(*fargs[2]);
	mMod.setFrequency(*fargs[2]);

	if (nargs == 4) {
		mIEnv.setScale(*fargs[2]);
		mPanner.setPosition(*fargs[3]);
	}
	if (nargs == 6) {
		mIEnv.setScale(*fargs[4] * *fargs[2]);
		mPanner.setPosition(*fargs[5]);
	}
	if (nargs == 14) {
		printf("\t\ta %g d %g s %g r %g\t\ta %g d %g s %g r %g\n", 
				*fargs[6], *fargs[7], *fargs[8], *fargs[9], *fargs[10], *fargs[11], *fargs[12], *fargs[13]);
		mAEnv.setAttack(*fargs[6]);
		mAEnv.setDecay(*fargs[7]);
		mAEnv.setSustain(*fargs[8]);
		mAEnv.setRelease(*fargs[9]);
		mIEnv.setAttack(*fargs[10]);
		mIEnv.setDecay(*fargs[11]);
		mIEnv.setSustain(*fargs[12]);
		mIEnv.setRelease(*fargs[13]);
	}
}

// Play a note with a given arg list

void FMInstrument::playOSC(int argc, void **argv, const char *types) {
	this->parseArgs(argc, argv, types);
	this->play();
}

// Play a note specifying all parameters

void FMInstrument::playNote(float dur, float ampl, float c_fr, float m_fr, float ind, float pos, 
				float att, float dec, float sus, float rel, 
				float i_att, float i_dec, float i_sus, float i_rel) {
	mAEnv.setDuration(dur);
	mIEnv.setDuration(dur);
	mAEnv.scaleValues(ampl);
	mMod.setOffset(c_fr);
	mMod.setFrequency(m_fr);
	mIEnv.scaleValues(ind * m_fr);
	mPanner.setPosition(pos);
	mAEnv.setAttack(att);
	mAEnv.setDecay(dec);
	mAEnv.setSustain(sus);
	mAEnv.setRelease(rel);
	mIEnv.setAttack(i_att);
	mIEnv.setDecay(i_dec);
	mIEnv.setSustain(i_sus);
	mIEnv.setRelease(i_rel);
	this->play();
}

void FMInstrument::playMIDI(float dur, int chan, int key, int vel) {
	mAEnv.setDuration(dur);
	mIEnv.setDuration(dur);
	mAEnv.scaleValues(sqrtf((float) vel / 128.0f));
	mMod.setOffset(keyToFreq(key));
	mMod.setFrequency(keyToFreq(key));
//	mIEnv.scaleValues(keyToFreq(key));
	this->play();
}

///////////////////////////////////////////////////

// FancyFMInstrument

//	Envelope mVibEnv;				///< vibrato envelope
//	ADSR mChiffEnv;					///< attack-chiff envelope
//  Sine mVibrato;					///< sine oscillator for vibrato
//	WhiteNoise mChiff;				///< chiff noise
//	Butter mChFilter;				///< chiff filter
//	Freeverb mReverb;

FancyFMInstrument::FancyFMInstrument() :		// initializers for the UGens
			FMInstrument(),
			mVibEnv(kExpon, 3, 0.0, 0.1, 2, 1.0, 3, 0.1),	///< Triangle-like AR vibrato envelope
			mChiffEnv(3, 0.01, 0.01, 0.0, 2),				///< attack-chiff envelope
			mVibrato(6),									///< sine oscillator for vibrato
			mChFilter(mChiff, BW_BAND_PASS, 4000.f, 200.f),	///< chiff filter
			mReverb(mPanner) {					// reverb the panner
	mVibEnv.setScale(8);						// scale vibrato envelope
	mVibrato.setScale(mVibEnv);
	mVibrato.setOffset(BASE_FREQ);				// shift vibrato up

	mChiffEnv.setScale(0.8);					// scale chiff envelope
	mChFilter.setScale(mChiffEnv);				// apply chiff envelope
	mCar.setOffset(mChFilter);					// add in chiff

	mMod.setOffset(mVibrato);					// add in vibrato and base freq

	mName = "Fancy_FM";
	mUGens["Vibrato"] = & mVibrato;
	mUGens["Vib env"] = & mVibEnv;
	mUGens["Attack chiff filter"] = & mChFilter;
	mUGens["Attack chiff env"] = & mChiffEnv;
	mUGens["Attack noise"] = & mChiff;
	mUGens["Reverb"] = & mReverb;
	mEnvelopes.push_back(& mVibEnv);			// list envelopes for retrigger
	mEnvelopes.push_back(& mChiffEnv);
}

// The destructor frees the stuff we allocated

FancyFMInstrument::~FancyFMInstrument() { }

void FancyFMInstrument::setParameter(unsigned selector, int argc, void **argv, const char *types) {
	FMInstrument::setParameter(selector,  argc, argv, types);				// call superclass' version
	if (argc == 1) {
		float d = * (float *) argv[0];
		if (types[0] == 'i') 
			d = (float) (* (int *) argv[0]);
		switch (selector) {						// switch on which parameter is being set
			case set_duration_f:
				mChiffEnv.setDuration(d);
				mVibEnv.setDuration(d);
				break;
			case set_c_freq_f:
				mVibrato.setOffset(d);			// shift vibrato up
				mMod.setOffset(mVibrato);
				break;
			case set_vib_depth_f:
				mVibEnv.setScale(d); break;
			case set_chiff_amt_f:
				mChiffEnv.setScale(d); break;
			default:
				break;
		 }
	} else 			// multiple args
		logMsg(kLogError, "Unknown multi-arg (%d) setter in FMInstrument: %s\n", argc, types);
}

void FancyFMInstrument::playOSC(int argc, void **argv, const char *types) {
	float ** fargs = (float **) argv;
	this->parseArgs(argc, argv, types);
	mVibrato.setOffset(*fargs[2]);				// shift vibrato up
	mMod.setOffset(mVibrato);					// add in vibrato and base freq
	mMod.setOffset(*fargs[2]);
	mVibrato.setFrequency(4.0 + (4.0 * fRandZ()));
	mChiffEnv.setAttack(0.05 * fRandZ());
	mChiffEnv.setDecay(0.05 * fRandZ());
	this->play();
}


void FancyFMInstrument::playMIDI(float dur, int chan, int key, int vel) {
	mAEnv.setDuration(dur);
	mIEnv.setDuration(dur);
	mAEnv.scaleValues(sqrtf((float) vel / 128.0f));
	mVibrato.setOffset(keyToFreq(key));				// shift vibrato up
	mMod.setOffset(mVibrato);					// add in vibrato and base freq
	mMod.setOffset(keyToFreq(key));
	mVibrato.setFrequency(4.0 + (4.0 * fRandZ()));
	mChiffEnv.setAttack(0.05 * fRandZ());
	mChiffEnv.setDecay(0.05 * fRandZ());
	this->play();
}
