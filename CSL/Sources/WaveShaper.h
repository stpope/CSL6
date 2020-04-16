//
//  WaveShaper.h -- CSL wave-shaping oscillator class that uses a look-up table
//
//	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
//

#ifndef INCLUDE_WAVESHAPER_H
#define INCLUDE_WAVESHAPER_H

#include "Oscillator.h"		// my superclass

namespace csl {				// my namespace

///
/// WaveShaper -- Wave-shaping oscillator class
///

class WaveShaper : public Sine {
public:
	WaveShaper();
	WaveShaper(float frequency, unsigned sel = 0, unsigned size = CSL_mMaxBufferFrames);
	
	void setTableLength(unsigned length);	///< set the size in bytes of the table.
	unsigned tableLength() { return mTableSize; };
	
	Buffer mTransferFunction;		///< the wave-shaping look-up table (it's just a sample buffer)

	void nextBuffer(Buffer & outputBuffer, unsigned outBufNum) throw (CException);
	
protected:
	void initWaveTable(unsigned sel = 0);	///< function to initialize the default shaping table
	
private:
	unsigned mTableSize;			///< Table size in bytes.
	
};		// end of class spec.

} 		// end of namespace

#endif
