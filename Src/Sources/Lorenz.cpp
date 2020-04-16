//
// Lorenz.cpp --  Implementation file for the Lorenz chaotic oscillator
//	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
// 

#include "Lorenz.h"
#include <iostream>
#include <stdio.h>

using namespace csl;

// Constructors
Lorenz::Lorenz(float tx, float ty, float tz) : UnitGenerator(), mX(tx), mY(ty), mZ(tz) { }

Lorenz::~Lorenz() { }

// Dump = pretty-print something about the receiver object
void Lorenz::dump() {
	logMsg("Lorenz Frame-Rate: %d, x: %g\n", mFrameRate, mX);
}

// The default next_buffer function just plays one variable of the Lorenz Strange Attractor between +1 and -1
void Lorenz::nextBuffer(Buffer & outputBuffer, unsigned outBufNum) throw (CException) {
	sample *bufferPtr = outputBuffer.buffer(outBufNum);
	unsigned numFrames = outputBuffer.mNumFrames;
	float xx , yy, zz;
	static float max;
	
#ifdef CSL_DEBUG
	logMsg("Lorenz oscillator's nextBuffer");
#endif		
	
	for(unsigned i = 0; i < numFrames; i++ ) {
		xx = mX + 0.01 * (-10. * mX + 10. * mY );
		yy = mY + 0.01 * (20. * mX - mY - mX * mZ );
		zz = mZ + 0.01 * (-8. * mZ / 3. + mX * mY );
		max = (mX > max) ? mX : max;
		*bufferPtr++ = mX / 17.f - 0.1f;
		mX = xx;
		mY = yy;
		mZ = zz;
	}
//	printf("max:\t%g\t\t",max);
}
