//
//  Variable.h -- the abstract external variable (plug or port) class
//	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
//

#ifndef CSL_Variable_H
#define CSL_Variable_H

#include "CSL_Core.h"

//#define USE_RANDOMS			// broken at present (can't link for libnewran.a)

#ifdef USE_RANDOMS
#include "newran/newran.h"		// random variables from Robert Davies
#endif

namespace csl {

///
/// CVariable is the the abstract external variable (plug or port) class.
/// This is the abstract class for static and dynamic variables; it's a sample stream. 
/// Clients of Variable (oscillators, dsp units, etc) first ask whether 
/// its values are fixed over the length of the buffer or not.
/// If the value is fixed, the client calls next_sample() to get the next sample
/// and use it the DSP loop. If the value is not fixed, the client calls 
/// next_buffer() to get the next buffer of values.
///

class CVariable {		// abstract class

protected:
	float mValue;				///< the value I represent
	
public:							/// Constructors
	CVariable() { };
	CVariable(float tvalue) : mValue(tvalue) { };
								/// Accessors
	float value() { return(mValue); };
	void setValue(float x) { mValue = x; };
	void setValue(int x) { mValue = (float) x; };
	void setValue(double x) { mValue = (float) x; };
};

///
/// StaticVariable -- he static external variable (plug) class.
/// This is a kind of variable that holds onto floating-point value that is
/// fixed each control rate period (e.g., changes at most once per control rate).
///

class StaticVariable : public CVariable, public UnitGenerator {

public:						/// Constructors
	StaticVariable(float x) : CVariable(x), UnitGenerator() { }; 
	StaticVariable(int x) : CVariable((float) x), UnitGenerator() { }; 
	StaticVariable(double x) : CVariable((float) x), UnitGenerator() { }; 
	
							/// this what being a static variable means!
	bool isFixed() { return true; };
							/// versions of nextBuffer
	void nextBuffer(Buffer & outputBuffer, unsigned outBufNum) throw (CException);
	
	float value() { return(CVariable::value()); };
	void setValue(float x) { CVariable::setValue(x); };
	void setValue(int x) { CVariable::setValue(x); };
	void setValue(double x) { CVariable::setValue(x); };
};

// An enumeration for the possible arithmetic operations

#ifdef CSL_ENUMS
typedef enum {
	kOpPlus,
	kOpTimes,
	kOpMinus,
	kOpDivided,
	kOpNegated
} VOperator;
#else
	#define kOpPlus 1
	#define kOpTimes 2
	#define kOpMinus 3
	#define kOpDivided 4
	#define kOpNegated 5
	typedef int VOperator;
#endif

///
/// The DynamicVariable class is a changing variable that can perform an 
/// operation (e.g., scaling) on another unit generator
///

class DynamicVariable : public CVariable, public Effect {

protected:
	VOperator mMode;		///< the operation I perform '+', '*', etc.
	
public:				/// Constructors
	DynamicVariable(UnitGenerator & vox, float val) : CVariable(val), Effect(vox), mMode(kOpTimes) { };
	DynamicVariable(UnitGenerator & vox, double val) : CVariable((float) val), Effect(vox), mMode(kOpTimes) { };
	DynamicVariable(float val, UnitGenerator & vox) : CVariable(val), Effect(vox), mMode(kOpTimes) { };
	DynamicVariable(float val, UnitGenerator & vox, VOperator m) :  CVariable(val), Effect(vox), mMode(m) { };
	DynamicVariable(UnitGenerator & vox, float val, VOperator m) :  CVariable(val), Effect(vox), mMode(m) { };

	DynamicVariable(int val, UnitGenerator & vox) : CVariable((float) val), Effect(vox), mMode(kOpTimes) { };
	DynamicVariable(UnitGenerator & vox, int val) : CVariable((float) val), Effect(vox), mMode(kOpTimes) { };
	DynamicVariable(int val, UnitGenerator & vox, VOperator m) :  CVariable((float) val), Effect(vox), mMode(m) { };
	DynamicVariable(UnitGenerator & vox, int val, VOperator m) :  CVariable((float) val), Effect(vox), mMode(m) { };

					/// my main operations
//	void nextBuffer(Buffer & outputBuffer) throw (CException);
	void nextBuffer(Buffer & outputBuffer, unsigned outBufNum) throw (CException);
	
	void setValue(float x) { CVariable::setValue(x); };
	void setValue(int x) { CVariable::setValue(x); };
	void setValue(double x) { CVariable::setValue(x); };
};

#ifdef USE_RANDOMS

#ifdef CSL_ENUMS

///
/// An enumeration for the possible Random Variable distributions
///

typedef enum {
	kUniform,
	kExponential,
	kCauchy,
	kNormal,
	kChiSq,
	kGamma,
	kPareto,
	kPoisson,
	kBinomial,
	kNegativeBinomial
} Distribution;

#endif

///
/// RandomVariable class
///

class RandomVariable : public CVariable, public UnitGenerator {
	
private:				// Newran stuff
	const static bool copySeedFromDisk = false;	///< set to true to get seed from disk file
	MotherOfAll urng;							///< declare uniform random number generator
						// Local stuff
	Random * mVar;				///< my random generator 
	Distribution mDist;			///< my probability distribution type
	float mMean;				///< my statistical mean 
	float mVariance;			///< my statistical variance 
	
public:					/// Constructors
	RandomVariable(Distribution d, float mean, float variance, float v1, float v2);
	~RandomVariable();
						/// my main operations
	void nextBuffer(Buffer & outputBuffer, unsigned outBufNum) throw (CException);
};

#endif // USE_RANDOMS

}

#endif
