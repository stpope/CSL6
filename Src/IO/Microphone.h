///
///  Microphone.h -- CSL class that copies the input buffer (from the sound driver) to its output
///	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
///

#ifndef CSL_Microphone_H
#define CSL_Microphone_H

#include "CSL_Includes.h"

namespace csl {

///
/// Microphone -- copies the input buffer (from the sound driver) to its output
///

class Microphone : public UnitGenerator {
public:
	Microphone(IO & anIO) : mIO(anIO) {};
	~Microphone() {};

	void nextBuffer(Buffer &outputBuffer) throw (CException);		///< copy next buffer from cache

protected:
	IO & mIO;					// my IO object
};

}

#endif
