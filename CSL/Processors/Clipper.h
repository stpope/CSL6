//
//  Clipper.h -- hard clipping processor
//	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
//

#ifndef CSL_Clipper_H
#define CSL_Clipper_H

#include "CSL_Core.h"

namespace csl {

/// Whether to clip using the minimum, maximum or both values.

typedef enum {
	kMin,
	kMax,
	kBoth
} ClipperFlags;

///
/// Clipper class
///

class Clipper : public Effect {

public:
				/// Constructor takes the input UGen and optionally the flags, min and max. 
	Clipper(UnitGenerator & input, float min = -1, float max = 1, ClipperFlags flags = kBoth);
	~Clipper();	

	void dump();						///< print the receiver for debugging

	void nextBuffer(Buffer &outputBuffer, unsigned outBufNum) throw (CException);

private:
	ClipperFlags mFlags;
	float mMin, mMax;
};

}

#endif
