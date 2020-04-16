//
// WhiteNoiseInstrument.h -- Simple filtered noise instrument class.
//	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
// 

#ifndef INCLUDE_WhiteNoise_Instrument_H
#define INCLUDE_WhiteNoise_Instrument_H

#include "Instrument.h"
#include "CSL_All.h"

namespace csl  {

enum {
						// Global
	kNumParameters = 16,
						// Parameters Tags
	kThresh = 0,		// Spectral filtering threshhold
	kWindowSize,		// Size of the FFT filtering window
	kNumBins,			// Number of spectral bins
	kOut,				// Output gain
	kNumParams
};

class WhiteNoiseInstrument : public Instrument {

protected:				// These are the UGens of the DSP graph (i.e., the FM instrument)
	ADSR mAEnv;			// amplitude & filter frequency envelopes
   	WhiteNoise mSig;	// White Noise signal
	Butter mLpfilter;	// This filters the white noise
   	MulOp mAPul;		// the output and frequency value multiplier guys

public:
	WhiteNoiseInstrument();
	~WhiteNoiseInstrument();
	
// Plug functions

	unsigned num_accessors();
	void get_accessors(Accessor **);
	void set_parameter(unsigned selector, void * value);
	void play_osc_note(char * types, void * args, char * endOfArgs);		
	void playMIDI(float dur, int chan, int key, int vel);	

};

}

#endif
