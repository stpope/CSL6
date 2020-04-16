//
//  SimpleSines.h -- specification of the simple waveform generators used as CSL tutorial examples
//	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
//
// What's here:
// 	SimpleSine -- the simplest sine oscillator class, this version has no dynamic controls
// 	SineAsPhased -- same as the above, except that it inherits its freq/phase members from Phased
// 	SineAsScaled -- this version has dynamic scale/offset controls, like most CSL UGens

#ifndef CSL_SIMPLESINE_H
#define CSL_SIMPLESINE_H

#include "CSL_Core.h"			// include the main CSL header; this includes CSL_Types.h and CGestalt.h

namespace csl {				// All these classes live in their own C++ namespace

///
/// SimpleSine -- The simplest CSL sine oscillator class
///

class SimpleSine : public UnitGenerator {

public:
	SimpleSine();			///< Constructors
	SimpleSine(float frequency);
	SimpleSine(float frequency, float phase);
	~SimpleSine();			///< Destructor

							/// the monoNextBuffer method is where the DSP takes place
	void nextBuffer(Buffer & outputBuffer, unsigned outBufNum) throw (CException);

protected:
	float mFrequency;		///< my frequency value in Hz
	float mPhase;			///< my instantaneous phase in Radians
};

///
/// SineAsPhased -- A sine oscillator that uses the Phased mix-in class,
///	meaning that we inherit mFrequency and mPhase and macros for dynamic control
///

class SineAsPhased : public UnitGenerator, public Phased {

public:
	SineAsPhased();			///< Constructors
	SineAsPhased(float frequency);
	SineAsPhased(float frequency, float phase);
	~SineAsPhased();			///< Destructor
	
								/// the monoNextBuffer method is where the DSP takes place
	void nextBuffer(Buffer & outputBuffer, unsigned outBufNum) throw (CException);
	void dump();				///< pretty-print the receiver
};

///
/// SineAsScaled -- A sine oscillator that also has scale and offset as dynamic controls (from Scalable)
/// (Note the tripple inheritance)
///

class SineAsScaled : public UnitGenerator, public Phased, public Scalable {

public:
	SineAsScaled();			///< Constructors
	SineAsScaled(float frequency);
	SineAsScaled(float frequency, float phase);
	SineAsScaled(float frequency, float phase, float ampl, float offset);
	~SineAsScaled();			///< Destructor
	
								/// the monoNextBuffer method is where the DSP takes place
	void nextBuffer(Buffer & outputBuffer, unsigned outBufNum) throw (CException);
	void dump();				///< pretty-print the receiver
};

}

#endif
