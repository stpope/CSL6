///
/// BinaryOp.h -- The specification file for simple arithmetic operations on UnitGenerators.
/// See the copyright notice and acknowledgment of authors in the file COPYRIGHT
///
/// What's here:
///
/// BinaryOp -- an abstract class that specific binary operators (add, mul, etc.) inherit from)
/// AddOp -- Adds two UnitGenerators (or a UnitGenerator and a scalar) together
/// MulOp -- Multiplies two UnitGenerators (or a UnitGenerator and a scalar) together
///

#ifndef CSL_BINARYOP_H
#define CSL_BINARYOP_H

#include "CSL_Core.h"

namespace csl {

///
/// BinaryOp -- An Effect that takes two input sources (UnitGenerators or scalars) and
/// gives a single UnitGenerator as its output. BinaryOp is an abstract class who's nextBuffer
/// method is to be implemented by subclasses
///

class BinaryOp : public Effect {

public:								/// The constructor for dual-UnitGenerator operation
	BinaryOp(UnitGenerator & op1, UnitGenerator & op2);
									/// Constructuctor for operating a UnitGenerator and a fixed float
	BinaryOp(UnitGenerator & op1, float op2);
									/// The same constructor, but with UGen and float inputs
	BinaryOp(float op1, UnitGenerator & op2);
	virtual ~BinaryOp();				///< The destructor

	void dump();					///< Prints instance info
	void setOperand(float op);		///< Set the operand from a fixed float

protected:	
									/// Abstract function that will do the processing for each buffer
	virtual void nextBuffer(Buffer & outputBuffer, unsigned outBufNum) throw(CException) = 0;
									/// Is the input fixed or a dynamic UnitGenerator?
	inline bool isFixed();
	inline bool inputIsFixed();
									/// Is the operand fixed or a dynamic UnitGenerator?
	inline bool operandIsFixed();	
									/// Convert the dynamic operand to a fixed sample (if operand is fixed)
	inline sample operandFixedValue(Buffer & outputBuffer);		

};

inline bool BinaryOp::inputIsFixed() {
	return (Effect::inPort())->isFixed();
}

inline bool BinaryOp::operandIsFixed() {
	return mInputs[CSL_OPERAND]->isFixed();
}

/// Declare the operand port (accessing the mInputs map) and current value.

#define DECLARE_OPERAND_CONTROLS						\
	Port * opPort = mInputs[CSL_OPERAND];				\
	SampleBuffer inValue;								\
	sample opValue

/// Load the op-related values at the start of the callback; if the operand is a dynamic UGen input,
/// then pull its value, get the pointer to its buffer, and set the first value, otherwise store the constant value

#define LOAD_OPERAND_CONTROLS							\
	Controllable::pullInput(opPort, numFrames);			\
	Effect::pullInput(numFrames);						\
	inValue = mInputPtr;								\
	opValue = opPort->nextValue()

/// Update the op-related value in the loop

#define UPDATE_OPERAND_CONTROLS							\
	opValue = opPort->nextValue()	;					\
	inValue++

///
/// AddOp -- A BinaryOp that adds two UnitGenerators or fixed constants together.
///

class AddOp : public BinaryOp {

public:								/// The constructor for dual-UnitGenerator operation
	AddOp(UnitGenerator & op1, UnitGenerator & op2);
									/// Constructuctor for operating a UnitGenerator and a fixed float
	AddOp(UnitGenerator & op1, float op2);
									/// Returns the next individual frame (for fixed values)
	AddOp(float op1, UnitGenerator & op2);

//	float nextValue() throw (CException);

protected:							/// Function that implements the adding routine
	void nextBuffer(Buffer & outputBuffer, unsigned outBufNum) throw (CException) ;
};

///
/// MulOp -- A BinaryOp that multiplies two UnitGenerators or fixed constants together, sample-by-sample.
///

class MulOp : public BinaryOp {

public:								/// The constructor for dual-UnitGenerator operation
	MulOp(UnitGenerator & op1, UnitGenerator & op2);
									/// Constructuctor for operating a UnitGenerator and a fixed float
	MulOp(UnitGenerator & op1, float op2);

	MulOp(float op2, UnitGenerator & op1);
	void setScale();
									/// Returns the next individual frame (for fixed values)
//	float nextValue() throw (CException);

//protected:							/// Function that implements the multiplying routine
	void nextBuffer(Buffer & outputBuffer, unsigned outBufNum) throw (CException) ;

};

}

#endif // CSL_BINARYOP_H

