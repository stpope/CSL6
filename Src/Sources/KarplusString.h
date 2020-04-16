//
//  KarplusString.h -- the simplest possible "physical model" string class
//	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
//

/* TODO:
	Jorge . . . November 2005
	1) Add a dump() function.
	2) implement exceptions
	3) add DEBUG print code.
	4) make it scalable . . .
	5) allow for dynamic frequency . . . ? do we want that?
	6) add a trigger function . . . (do we need an envelope?)
*/


#ifndef CSL_KARPLUSSTRING_H
#define CSL_KARPLUSSTRING_H

#include "CSL_Core.h"		// my superclass
//#include "RingBuffer.h"
//#include "Filters.h"
#include "Noise.h"

namespace csl {					// my namespace

///
/// KarplusString -- string model class
///

class KarplusString : public UnitGenerator, public Scalable, public Phased {

public:
	KarplusString();
	KarplusString(float frequency);
	void setFrequency(float frequency);
	void trigger();					///< reset internal buffers to re-pluck the string.
	void dump();					///< print debugging info.
		
	void nextBuffer(Buffer &outputBuffer, unsigned outBufNum) throw (CException);
	virtual bool isActive() { return (mEnergy > 0); };

	Buffer mDelayLine; 				///< the delay line (just a buffer, not a RingBuffer)

protected:	
	void initDelayLine();			///< function to initialize the delay line
	unsigned mIndex;				///< current index in the delay line
	unsigned mDelayLength;			///< allocated size of the delay line
	unsigned mEnergy;				///< energy left in buffer
	float mFrequency;
	
};		// end of class

}		// end of namespace

#endif

