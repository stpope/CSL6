///
/// AdditiveInstrument.h -- Sum-of-sines synthesis instrument class.
///
///	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
/// 
///
/// Accessors
///		"du", set_duration_f
///		"am", set_amplitude_f
///		"fr", set_freq_f
///		"po", set_position_f
///		"aa", set_attack_f	-- amplitude envelope ADSR
///		"ad", set_decay_f
///		"as", set_sustain_f
///		"re", set_release_f
///		"pt", set_partial_f
///		"ps", set_partials_f
///
/// OSC note formats (4 or 8 arguments):
/// 	dur, ampl, c_fr, pos
/// 	dur, ampl, c_fr, pos, att, dec, sus, rel

#ifndef INCLUDE_ADDITIVE_Instrument_H
#define INCLUDE_ADDITIVE_Instrument_H

#include "Instrument.h"
#include "Mixer.h"

namespace csl  {

///
/// AdditiveInstrument
///

class AdditiveInstrument : public Instrument {
public:
	AdditiveInstrument();			///< Constructor
	AdditiveInstrument(SumOfSines & sos);
	AdditiveInstrument(unsigned numHarms, float noise);
	AdditiveInstrument(SHARCSpectrum & spect);

	AdditiveInstrument(AdditiveInstrument&);		///< copy constructor
	~AdditiveInstrument();
								/// Plug functions
	void setParameter(unsigned selector, int argc, void **argv, const char *types);
								/// Play functions
	void playOSC(int argc, void **argv, const char *types);	
	
	void playNote(float dur = 1, float ampl = 1, 
				float c_fr = 110, float pos = 0,
				float att = 0.05, float dec = 0.05, float sus = 0.5, float rel = 0.5);
	void playMIDI(float dur, int chan, int key, int vel);	

	ADSR mAEnv;					///< amplitude envelope
   	SumOfSines mSOS;			///< sum-of-sine oscillator
	Panner mPanner;				///< stereo panner
	
protected:
	void init();
};

///
/// VAdditiveInstrument = vector-additive - cross-fade between 2 SOS spectra
///		Envelope mXEnv(dur, pause, xfade, 0.0f) 
///			float paus = fRandV(0.5) * dur;
///			float fade = fRandV(dur - paus);
///		AR(float t, float i, float a, float r);
///

class VAdditiveInstrument : public Instrument {
public:
	VAdditiveInstrument();			///< Constructor
	VAdditiveInstrument(SumOfSines & sos1, SumOfSines & sos2);
	VAdditiveInstrument(VAdditiveInstrument&);		///< copy constructor
	~VAdditiveInstrument();
								/// Plug functions
	void setParameter(unsigned selector, int argc, void **argv, const char *types);
								/// Play functions
	void playOSC(int argc, void **argv, const char *types);	
	
	void playNote(float dur = 1, float ampl = 1, 
				float c_fr = 110, float pos = 0,
				float att = 0.05, float dec = 0.05, float sus = 0.5, float rel = 0.5);
	void playMIDI(float dur, int chan, int key, int vel);	

	ADSR mAEnv;					///< amplitude envelope
	AR mXEnv;					///< cross-fade envelope, AR with init delay
   	SumOfSines mSOS1, mSOS2;	///< 2 sum-of-sine oscillators
	Panner mPanner;				///< stereo panner
};

}

#endif
