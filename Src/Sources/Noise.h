//
// Noise.h -- Noise Unit Generators
//	Comprising Noise superclass, and WhiteNoise, PinkNoise subclasses 
// 

#ifndef INCLUDE_Noise_H
#define INCLUDE_Noise_H

#include "CSL_Core.h"			// include the main CSL header; this includes CSL_Types.h and CGestalt.h

#define PINK_MAX_RANDOM_ROWS	(30)
#define PINK_RANDOM_BITS   		(24)
#define PINK_RANDOM_SHIFT  		((sizeof(long)*8)-PINK_RANDOM_BITS)

namespace csl {

///
/// Abstract Noise class - inherits from UnitGenerator & Scalable, and provides constructors and basic pseudo-raondom methods
///

class Noise : public UnitGenerator, public Scalable {

public:
	inline int generateRandomNumber();					///< returns the next pseudo-random number
	inline float generateNormalizedRandomNumber();		///< returns next pseudo-random normalised to +/- 1.0

	void setSeed(int seed) { mSeed = seed; }			///< set the seed integer for the pseudo-random number generators

	void dump();										///< Tell me more about what is happening

	Noise();											///< Constructors
	Noise(double ampl, double offset = 0.0);		
	Noise(int seed, double ampl = 1.0, double offset = 0.0);
	~Noise() { };										///< Destructor
	
protected:
	int mSeed;											///< seed integer for the pseudo-random number generators
	float mDivisor;										///< factor to scale ints to +/- 1.0
};

///
/// White noise -- equal power per frequency
///

class WhiteNoise : public Noise {

public:										/// Constructors
	WhiteNoise() : Noise() { };
	WhiteNoise(double ampl, double offset = 0.0) : Noise(ampl, offset) { }; 
	WhiteNoise(int seed, double ampl = 1.0, double offset = 0.0) : Noise(seed, ampl, offset) { };
	~WhiteNoise() { };										///< Destructor
	
/********* THIS FUNCTION WAS PROTECTED, BUT IT'S NEEDED TO BE PUBLIC BECAUSE 
			OTHER UGENS MIGHT CALL IT DURING A NEXT BUFFER CALL . . . *********/
														/// the noise generator DSP function
	void nextBuffer(Buffer& outputBuffer, unsigned outBufNum) throw (CException);
	
protected:

};

///
/// Pink noise -- equal power per octave
///

class PinkNoise : public Noise {

public:
	PinkNoise();										///< Constructors
	PinkNoise(double ampl, double offset = 0.f); 
	PinkNoise(int seed, double ampl = 1.f, double offset = 0.f);
	~PinkNoise() { };									///< Destructor
	
									/// the monoNextBuffer method is where the DSP takes place
	void nextBuffer(Buffer & outputBuffer, unsigned outBufNum) throw (CException);
	
	sample nextPink();					///< returns the next pink noise sample
	
protected:
	int mPinkRows[PINK_MAX_RANDOM_ROWS];///< Pink noise generator rows
	int mPinkRunningSum;				///< Used to optimize summing of generators.
	int mPinkIndex;						///< Incremented each sample. 
	int mPinkIndexMask;					///< Index wrapped by ANDing with this mask. 
	float mPinkScalar;					///< Used to scale within range of -1.0 to +1.0 

	void initialize(int numRows);		///< set up PinkNoise for N rows of generators

};

// inline functions have to be declared in the header file to avoid linker problems

inline int Noise::generateRandomNumber() {
	// Calculate pseudo-random 32 bit number based on linear congruential method
	mSeed = (mSeed * 196314165) + 907633515;
	return mSeed;

}	

inline float Noise::generateNormalizedRandomNumber() {
	// Calculate pseudo-random 32 bit number based on linear congruential method
	mSeed = (mSeed * 196314165) + 907633515;
	return (float) mSeed / (float) 0x7fffffff; // dividing by INT_MAX
}


} // namespace csl

#endif

