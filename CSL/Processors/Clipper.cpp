//
//  Clipper.cpp -- implementation of the Clipper class
//	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
//

#include "Clipper.h"
#include <math.h>

using namespace csl;

// Generic Clipper implementation

// Constructor takes the input UGen and optionally the flags, min and max.
Clipper::Clipper(UnitGenerator & input, float min, float max, ClipperFlags flags)
				: Effect(input), mFlags(flags), mMin(min), mMax(max) { }

Clipper::~Clipper() { }

void Clipper::dump() {
	logMsg("a Clipper");
	UnitGenerator::dump();
}

// nextBuffer does the dirty work
void Clipper::nextBuffer(Buffer &outputBuffer, unsigned outBufNum) throw (CException) {

	float tempSample;
	sample * outp = outputBuffer.buffer(outBufNum);
	unsigned numFrames = outputBuffer.mNumFrames;

	pullInput(outputBuffer);
	sample * inPtr = mInputPtr;
	
#ifdef CSL_DEBUG
	logMsg("Clipper nextBuffer");
#endif		

		switch(mFlags) {
			case kMin:
				for (unsigned i = 0; i < numFrames; i++) {
					tempSample = inPtr[i];
					if (inPtr[i] < mMin )
						tempSample = mMin;
						
					*outp++ = tempSample;
				}
				break;
			case kMax:
				for (unsigned i = 0; i < numFrames; i++) {
					tempSample = inPtr[i];
					if (inPtr[i] > mMax )
						tempSample = mMax;
						
					*outp++ = tempSample;					
				}
				break;
			case kBoth:
				for (unsigned i = 0; i < numFrames; i++) {
					tempSample = inPtr[i];
					if (inPtr[i] < mMin )
						tempSample = mMin;
					if (inPtr[i] > mMax )
						tempSample = mMax;
						
					*outp++ = tempSample;
				}
				break;
		}

	return;

}
