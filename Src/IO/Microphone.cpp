///
///  Microphone.h -- CSL class that copies the input buffer (from the sound driver) to its output
///	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
///

#include "Microphone.h"

using namespace csl;

void Microphone :: nextBuffer(Buffer &outputBuffer) throw(CException) {
	
	outputBuffer.copySamplesFrom(mIO.getInput());

}
