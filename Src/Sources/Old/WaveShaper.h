//
//  WaveShaper.h -- CSL wave-shaping oscillator class that uses a look-up table
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
	WaveShaper(float frequency, unsigned size = 8192);
	
	void setTableLenght(unsigned length);	///< set the size in bytes of the table.
	unsigned tableLength() { return mTableSize; };
	
	Buffer mTransferFunction;		// the wave-shaping look-up table (it's just a sample buffer)

protected:
	status mono_next_buffer(Buffer & inputBuffer, Buffer & outputBuffer, unsigned inBufNum, unsigned outBufNum);
	void initWaveTable();			// function to initialize the default shaping table
	
private:
	unsigned mTableSize;		///< Table size in bytes.
	
};		// end of class spec.

} 		// end of namespace

#endif
