///
///  BlockResizer.h -- Regularizes the amount of data called for, 
///		meaning that different parts of a graph can run at different callback block sizes.
/// 	This is useful for time-frequency transformations that only work with fixed buffer sizes.
///		A BlockResizer either caches large blocks between multiple smaller callbacks, or it calls a 
///		smaller-block-size graph several times per larger-sized downstream callbacks.
///		BlockResizer works for up-block and down-block situations.
///		This version also handles multi-channel mapping, so you can use a mono input and ask it for 
///		multi-channel output; it will copy.
///
///	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
///

#ifndef CSL_BlockResizer_H
#define CSL_BlockResizer_H

#include "CSL_Core.h"

namespace csl {

/// Regularizes the amount of data called for.
/// This is useful for time-frequency transformations that only work with certain buffer sizes.

class BlockResizer : public UnitGenerator {		// it could be an Effect

public:									/// ctor / dtor
	BlockResizer(UnitGenerator & input, unsigned blockSize);
	~BlockResizer();
										///  the work-horse method calls the up-hill graph as needed
	void nextBuffer(Buffer & outputBuffer) throw(CException);

protected:
	UnitGenerator * mInput;				///< my input unit generator (pointer)
	Buffer mInputBuffer;				///< buffer used to pull input
	unsigned mBufferSize;				///< fixed buffer size of the up-hill graph
	int mFramePointer;					///< where am I in reading the input?
};

}

#endif
