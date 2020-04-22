///
///  FDN.h -- CSL feedback delay network class
///	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
///

#ifndef INCLUDE_FDN_H
#define INCLUDE_FDN_H

#include "CSL_Core.h"		// my superclass
#include "Noise.h"

namespace csl {					// my namespace

///
/// FDN -- Feedback delay network
///

class FDN : public Effect {
public:
	FDN(UnitGenerator &op, unsigned int delayLineLengths[], unsigned int numDelayLines,
			sample inputGains[], sample outputGains[], sample feedbackMatrix[], sample feedbackGains[] ); 
	~FDN();

	Buffer *mDelayLine; 	///< the delay line (just a buffer, not a RingBuffer)


	void nextBuffer(Buffer &outputBuffer, unsigned outBufNum) throw (CException);

//	void dump();				///< print the receiver for debugging

protected:	
	void initDelayLines();		///< function to initialize the delay line
	unsigned mNumDelLines;		///< # of delay lines in FDN
	unsigned *mIndex;			///< current index in the delay lines
	unsigned *mDelLength;		///< allocated size of the delay lines
	
	sample *mInputGains;		///< Input gains to various delay lines
	sample *mOutputGains;		///< Output gains for various delay lines
	sample *mFeedbackMatrix;	///< Feedback matrix in FDN
	sample *mFeedbackGains;		///< Feedback gains for various delay lines

	sample *mFeedbackVector;	///< Feedback for various delay lines
	sample *mOutputVector;		///< Output for various delay lines
	
};		// end of class spec.

}		// end of namespace

#endif
