//
// Noise.cpp
//	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
// 

#include "Noise.h"
#include <time.h>

using namespace csl;

///
/// Abstract Noise class
///

/// Constructors call UnitGenerator & Scalable constructors
/// Noise defaults to expansion copy policy (i.e. distinct noise per channel)
/// if no seed is specified, current clock time is used

Noise::Noise() : 
	UnitGenerator(), 
	Scalable(1.f, 0.f), 
	//mCopyPolicy(kExpand), 
	mSeed(time(NULL)), 
	mDivisor(1.0f / (float) 0x7fffffff) {
	setCopyPolicy(kExpand);
}

Noise::Noise(double ampl, double offset) : 
	UnitGenerator(), 
	Scalable((float)ampl, (float)offset), 
	//mCopyPolicy(kExpand), 
	mSeed(time(NULL)), 
	mDivisor(1.0f / (float) 0x7fffffff) {
	setCopyPolicy(kExpand);
}

Noise::Noise(int seed, double ampl, double offset) : 
	UnitGenerator(), 
	Scalable((float)ampl, (float)offset), 
	//mCopyPolicy(kExpand),
	mSeed(seed),
	mDivisor(1.0f / (float) 0x7fffffff) {
	setCopyPolicy(kExpand);
}

void Noise::dump() {
	logMsg("a Noise generator");
	Scalable::dump();
	UnitGenerator::dump();
}

///
/// White noise -- equal power per frequency
///

void WhiteNoise::nextBuffer(Buffer & outputBuffer, unsigned outBufNum) throw (CException) {
	sample* out = outputBuffer.buffer(outBufNum);	// get ptr to output channel
	unsigned numFrames = outputBuffer.mNumFrames;			// get buffer length

	DECLARE_SCALABLE_CONTROLS;				// declare the scale/offset buffers and values
	LOAD_SCALABLE_CONTROLS;
#ifdef CSL_DEBUG
	logMsg("WhiteNoise nextBuffer");
#endif			
	for (unsigned i = 0; i < numFrames; i++) {					// sample loop	
		// call generate_random_number and do the division myself
		*out++ = ((((float) generateRandomNumber()) * mDivisor) * scaleValue) + offsetValue;		
		UPDATE_SCALABLE_CONTROLS;							// update the dynamic scale/offset
	}
}

/// Most of this code was taken from an public domain C implementation written by Phil Burk.
/// The code and the webpage explaining it is at
/// 	http://www.firstpr.com.au/dsp/pink-noise/phil_burk_19990905mPatestmPink.c
/// and
/// 	http://www.firstpr.com.au/dsp/pink-noise/

PinkNoise::PinkNoise() : Noise() {
	setCopyPolicy(kExpand);
	initialize(32);
}

PinkNoise::PinkNoise(double ampl, double offset) : Noise(ampl, offset) {
	setCopyPolicy(kExpand);
	initialize(32);
}

PinkNoise::PinkNoise(int seed, double ampl, double offset) : Noise(seed, ampl, offset) {
	setCopyPolicy(kExpand);
	initialize(32);
}

/// Setup PinkNoise structure for N rows of generators.
void PinkNoise::initialize(int numRows) {
	int i;
	int pmax;
	mPinkIndex = 0;
	mPinkIndexMask = (1<<numRows) - 1;
			// Calculate maximum possible signed random value. Extra 1 for white noise always added.
	pmax = (numRows + 1) * (1<<(PINK_RANDOM_BITS-1));
	mPinkScalar = 1.0f / pmax;
			// Initialize rows.
	for (i=0; i<numRows; i++ ) mPinkRows[i] = 0;
		mPinkRunningSum = 0;
}

// Generate Pink noise values between -1.0 and +1.0
sample PinkNoise::nextPink() {
	int newRandom;
	int sum;
	sample output;
				// Increment and mask index.
	mPinkIndex = (mPinkIndex + 1) & mPinkIndexMask;
				// If index is zero, don't update any random values.
	if (mPinkIndex != 0 ) {
				// Determine how many trailing zeros in PinkIndex.
				// This algorithm will hang if n==0 so test first. 
		int numZeros = 0;
		int n = mPinkIndex;
		while((n & 1) == 0 ) {
			n = n >> 1;
			numZeros++;
		}
				// Replace the indexed ROWS random value.
				// Subtract and add back to RunningSum instead of adding all the random values together. Only one changes each time.
		mPinkRunningSum -= mPinkRows[numZeros];
		newRandom = ((int)generateRandomNumber()) >> PINK_RANDOM_SHIFT;
		mPinkRunningSum += newRandom;
		mPinkRows[numZeros] = newRandom;
	}
				// Add extra white noise value.
	newRandom = ((int)generateRandomNumber()) >> PINK_RANDOM_SHIFT;
	sum = mPinkRunningSum + newRandom;
				// Scale to range of -1.0 to 0.9999.
	output = mPinkScalar * sum;
//#define PINK_MEASURE
#ifdef PINK_MEASURE
				// Check Min/Max
	static float   pinkMax = 0, pinkMin = 0;
	if (output > pinkMax ) pinkMax = output;
	else if (output < pinkMin ) pinkMin = output;
//	printf("max: %g\t min: %g", pinkMax, pinkMin);
#endif
	return output;
}

void PinkNoise::nextBuffer(Buffer & outputBuffer, unsigned outBufNum) throw (CException) {
	SampleBuffer out = outputBuffer.buffer(outBufNum);
	unsigned numFrames = outputBuffer.mNumFrames;

	DECLARE_SCALABLE_CONTROLS;							// declare the scale/offset buffers and values as above
#ifdef CSL_DEBUG
	logMsg("SineAsScaled nextBuffer");
#endif		
	LOAD_SCALABLE_CONTROLS;	
	for (unsigned i = 0; i < numFrames; i++ ) {
		*out++ = (nextPink() * scaleValue) + offsetValue;		// get a sample
		UPDATE_SCALABLE_CONTROLS;
	}
}

