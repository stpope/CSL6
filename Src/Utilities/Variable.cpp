//
//  Variable.cpp -- the abstract external variable (plug) class
//	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
//

#include "Variable.h"
#include <iostream>				// To be able to use cout

using namespace csl;

// Fill the first channel of the buffer with the constant value

void StaticVariable:: nextBuffer(Buffer & outputBuffer, unsigned outBufNum) throw (CException) {
	SampleBuffer buffer = outputBuffer.buffer(outBufNum);
	unsigned numFrames = outputBuffer.mNumFrames;

	for (unsigned i = 0; i < numFrames; i++)
		*buffer++ = mValue;
}

// Dynamic values grab their input and scale it

void DynamicVariable:: nextBuffer(Buffer & outputBuffer, unsigned outBufNum) throw (CException) {
	Effect::pullInput(outputBuffer);
	unsigned numFrames = outputBuffer.mNumFrames;
	unsigned i;
	SampleBuffer outp = outputBuffer.buffer(outBufNum);
	switch(mMode) {
	case kOpTimes:
		for (i = 0; i < numFrames; i++)	*outp++ *= mValue;
		break;
	case kOpDivided:
		for (i = 0; i < numFrames; i++)	*outp++ /= mValue;
		break;
	case kOpPlus:
		for (i = 0; i < numFrames; i++)	*outp++ += mValue;
		break;
	case kOpMinus:
		for (i = 0; i < numFrames; i++)	*outp++ -= mValue;
		break;
	case kOpNegated:
		for (i = 0; i < numFrames; i++)	*outp++ += (1.0f - mValue);
		break;
	default:
		// ?????????????????
		// just return the voice's values...
		break;
	}
}

// I have to override this here

//void DynamicVariable::nextBuffer(Buffer & outputBuffer) throw (CException) {
//	Effect::pullInput(outputBuffer);
//	UnitGenerator::nextBuffer(outputBuffer);
//}

#ifdef USE_RANDOMS

//v1 and v2 are arguments passed along to the newran functions...see the newran documentation for more info

RandomVariable::RandomVariable(Distribution d, float mean, float variance, float v1, float v2) 
		: UnitGenerator(), mVar(0), mDist(d), mMean(mean), mVariance(variance) {
	try {						
		//Initial Setup of seeding, etc.
#ifdef CSL_WINDOWS
		Random::SetDirectory("C:\\_libraries\\newran\\");  // set directory for seed control
#else
		Random::SetDirectory("./");				// set directory for seed control
#endif
		Random::Set(urng);						// set urng as generator to be used
#ifndef MSVS6
    if (copySeedFromDisk)
			Random::CopySeedFromDisk(true);		// get seed information from disk
#endif
		//Figure out which type of random variable the user wants and make it
		switch(mDist) {
			case kUniform:
				mVar = new Uniform();
				break;
			case kExponential:
				mVar = new Exponential();
				break;
			case kCauchy:
				mVar = new Cauchy();
				break;
			case kNormal:
				mVar = new Normal();
				break;
			case kChiSq:
				mVar = new ChiSq((int)v1,v2);
				break;
			case kGamma:
				mVar = new Gamma(v1);
				break;
			case kPareto:
				mVar = new Pareto(v1);
				break;
			case kPoisson:
				mVar = new Poisson(v1);
				break;
			case kBinomial:
				mVar = new Binomial((int)v1,v2);
				break;
			case kNegativeBinomial:
				mVar = new NegativeBinomial(v1,v2);
				break;
			default:
				mVar = new Uniform();
				break;
		}
	} catch (std::exception& e) {					// Used to be Catch(CException) -- ?? STP
		cerr << "\nRandomVariable exception: " << e.what() << "\n";
	}
}

RandomVariable::~RandomVariable() {
#ifndef MSVS6
	try {
		if (copySeedFromDisk)
			Random::CopySeedToDisk();		// write out seed
	} catch (CException & e) {
		cerr << "\nRandomVariable exception: " << e.mMessage << "\n";
   }
#endif
}

// Random Variables generate a stream of random values

void RandomVariable:: nextBuffer(Buffer & outputBuffer, unsigned outBufNum) throw (CException) {
	unsigned numFrames = outputBuffer.mNumFrames;
	SampleBuffer outp = outputBuffer.buffer(outBufNum];
	
	for (unsigned i = 0; i < numFrames; i++)
		*outp++ = nextFrame(outputBuffer);
}

#endif
