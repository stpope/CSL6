//
// DLine.h -- an Interpolating Delay Line
//	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
//
// This is a typical circular buffer with one writer and multiple readers (tap instances)

#ifndef CSL_DelayLine_H
#define CSL_DelayLine_H

#include "CSL_Core.h"
#include "RingBuffer.h"

namespace csl {

///
/// The delay line takes its input and write a delayed output
///

class DelayLine : public Effect {
public:
	DelayLine(unsigned maxDelayInSamples);			///< Constructor: takes the maximum delay lenght in samples.

	float delayTime();								///< Returns the delay amount in miliseconds.
	unsigned delayLength();							///< Returns the delay amount in samples.
	
	float setDelayTime(float delayInMiliseconds);		///< Set the delay amount using miliseconds as unit of meassurment.
	unsigned setDelayLength(unsigned delayInSamples); ///< Set the delay amount using samples as unit.
//	void setInterpolationKind();
	
	void nextBuffer(Buffer &output) throw(CException);

protected:
	RingBuffer mRingBuffer;
	unsigned mMaxDelayInSamples;
	unsigned mTotalDelayInSamples;

};

}

#endif