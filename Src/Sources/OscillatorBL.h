//
//  OscillatorBL.h -- Band-limited oscillators -- these can use on-demand sine summation, or store their wavetables 
//		(the latter choice might be dangerous with dynamic frequency control, of course)
//	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
//

#ifndef INCLUDE_OscillatorBL_H
#define INCLUDE_OscillatorBL_H

#include "Oscillator.h"

namespace csl {

///
/// Band-limited square, impulse, and triangle waveform oscillators
///

class SquareBL : public SumOfSines {
public:
	SquareBL();
	SquareBL(float frequency);
	SquareBL(float frequency, float phase);
	
//	void nextBuffer(Buffer & outputBuffer, unsigned outBufNum) throw (CException);
	void nextWaveInto(sample * dest, unsigned count, bool oneHz);

};

/*
class ImpulseBL : public SumOfSines {
protected:
	void nextWaveInto(sample * dest, unsigned count, bool oneHz);

public:
	ImpulseBL();
	ImpulseBL(float frequency);
	ImpulseBL(float frequency, float phase);
};

class TriangleBL : public SumOfSines {
protected:
	void nextWaveInto(sample * dest, unsigned count, bool oneHz);

public:
	TriangleBL();
	TriangleBL(float frequency);
	TriangleBL(float frequency, float phase);
};

class SawtoothBL : public SumOfSines {
protected:
	void nextWaveInto(sample * dest, unsigned count, bool oneHz);

public:
	SawtoothBL();
	SawtoothBL(float frequency);
	SawtoothBL(float frequency, float phase);
};

*/

}

#endif
