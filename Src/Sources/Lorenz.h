///
/// Lorenz.h --  Header file for the Lorenz chaotic oscillator
///	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
/// 

#ifndef CSL_Lorenz_H
#define CSL_Lorenz_H

#include "CSL_Core.h"

namespace csl {

///
/// Lorenz chaotic oscillator = this plays one variable of the Lorenz Strange Attractor between +1 and -1
///

class Lorenz : public UnitGenerator {

public:
				/// Constructor
	Lorenz(float x = 0.02, float y = 20., float z = 20.);
	~Lorenz();

				/// Accessors
	float x() const { return mX; }
	float y() const { return mY; }
	float z() const { return mZ; }

	void setX(float tx) { mX = tx; }
	void setY(float ty) { mY = ty; }
	void setZ(float tz) { mZ = tz; }

	void dump();
	void nextBuffer(Buffer& outputBuffer, unsigned outBufNum) throw (CException);

protected:
	float mX, mY, mZ;	
};

}

#endif
