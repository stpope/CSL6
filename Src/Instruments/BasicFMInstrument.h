///
/// BasicFMInstrument.h -- Simple and fancy FM example instrument classes.
///	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
/// 
/// This instrument implements single-operator FM with ADSR envelopes for the
/// amplitude and modulation index envelopes; it provides 13 parameter accessors
/// for use by OSC.
///
/// Accessors
///		"du", set_duration_f
///		"am", set_amplitude_f
///		"in", set_index_f
///		"cf", set_c_freq_f
///		"mf", set_m_freq_f
///		"po", set_position_f
///		"aa", set_attack_f	-- amplitude envelope ADSR
///		"ad", set_decay_f
///		"as", set_sustain_f
///		"ar", set_release_f
///		"ia", set_iattack_f	-- mod. index envelope ADSR
///		"id", set_idecay_f
///		"is", set_isustain_f
///		"ir", set_irelease_f
///
/// OSC note formats (6 or 14 arguments):
/// 	dur, ampl, c_fr, m_fr, ind, pos
/// 	dur, ampl, c_fr, m_fr, ind, pos, att, dec, sus, rel, i_att, i_dec, i_sus, i_rel

#ifndef INCLUDE_Basic_FM_Instrument_H
#define INCLUDE_Basic_FM_Instrument_H

#include "Instrument.h"
#include "Mixer.h"

namespace csl  {

///
/// FMInstrument
///
class FMInstrument : public Instrument {
public:
	FMInstrument();				///< Constructor
	FMInstrument(FMInstrument&);///< copy constructor
	~FMInstrument();
								/// Plug functions
	virtual void setParameter(unsigned selector, int argc, void **argv, const char *types);
								/// Play functions
	void parseArgs(int argc, void **argv, const char *types);
	virtual void playOSC(int argc, void **argv, const char *types);	
	
	void playNote(float dur = 1, float ampl = 1, 
				float c_fr = 110, float m_fr = 110, float ind = 1, float pos = 0,
				float att = 0.1, float dec = 0.1, float sus = 0.5, float rel = 0.1, 
				float i_att = 0.1, float i_dec = 0.1, float i_sus = 0.5, float i_rel = 0.1);
	void playMIDI(float dur, int chan, int key, int vel);	

								///< These are the UGens of the DSP graph (i.e., the FM instrument)
	ADSR mAEnv, mIEnv;			///< amplitude & modulation index envelopes
   	Osc mCar, mMod;				///< 2 sine oscillators, carrier and modulator
	Panner mPanner;				///< stereo panner
};

///
/// FancyFMInstrument - FM with vibrato (with AR-envelope), attack chiff (filtered noise with AR-envelope), 
///		and random freq. drift and ampl. swell envelopes
///
class FancyFMInstrument : public FMInstrument {
public:
	FancyFMInstrument();			///< Constructor
	~FancyFMInstrument();
	
	void setParameter(unsigned selector, int argc, void **argv, const char *types);
	void playOSC(int argc, void **argv, const char *types);	
	void playMIDI(float dur, int chan, int key, int vel);	
									///< These are the additional UGens of the DSP graph (i.e., the FM instrument)
	Envelope mVibEnv;				///< vibrato envelope
	ADSR mChiffEnv;					///< attack-chiff envelope
   	Osc mVibrato;					///< sine oscillator for vibrato
	WhiteNoise mChiff;				///< chiff noise
	Butter mChFilter;				///< chiff filter
	Freeverb mReverb;
};

}

#endif
