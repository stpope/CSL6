///
/// StringInstrument.h -- Karplus-Strong string instrument class.
///
///	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
/// 
///
/// Accessors
///		"du", set_duration_f -- ignored
///		"am", set_amplitude_f
///		"fr", set_freq_f
///		"po", set_position_f
///
/// OSC note formats (4 arguments):
/// 	dur, ampl, c_fr, pos

#ifndef INCLUDE_STRING_Instrument_H
#define INCLUDE_STRING_Instrument_H

#include "Instrument.h"
#include "Mixer.h"

namespace csl  {

///
/// StringInstrument
///

class StringInstrument : public Instrument {
public:
	StringInstrument(float ampl = 0.25f, float c_fr = 220.0f, float pos = 0.0f);    ///< Constructor
    ~StringInstrument();         ///< Destructor
//	StringInstrument(StringInstrument&);	///< copy constructor
								/// Plug functions
	void setParameter(unsigned selector, int argc, void **argv, const char *types);
								/// Play functions
	void playOSC(int argc, void **argv, const char *types);	
	
	void playNote(float ampl = 0.25f, float c_fr = 220.0f, float pos = 0.0f);
	void playMIDI(float dur, int chan, int key, int vel);	

    KarplusString mString;      ///< the KS string
    Panner mPanner;				///< stereo panner
	
protected:
	void init();
};

}

#endif
