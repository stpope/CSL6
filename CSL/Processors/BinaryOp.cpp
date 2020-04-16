///
/// BinaryOp.cpp -- The implementation file for simple arithmetic operations on UnitGenerators.
/// See the copyright notice and acknowledgment of authors in the file COPYRIGHT.
///

#include "BinaryOp.h"
#include "CGestalt.h"

using namespace csl;

BinaryOp::BinaryOp(UnitGenerator & op1, UnitGenerator & op2) : Effect(op1) {
	this->addInput(CSL_OPERAND, op2);
#ifdef CSL_DEBUG
	logMsg("BinaryOp::add input and operand UGs");
#endif		
}

BinaryOp::BinaryOp(UnitGenerator & op1, float op2) : Effect(op1) {
	this->addInput(CSL_OPERAND, op2);
#ifdef CSL_DEBUG
	logMsg("BinaryOp::add input UG and float operand");
#endif	
}

BinaryOp::BinaryOp(float op1, UnitGenerator & op2) : Effect(op2) {
	this->addInput(CSL_OPERAND, op1);			// Set the operand port's UGen
#ifdef CSL_DEBUG
	logMsg("BinaryOp::add operand UG and float input");
#endif	
}

BinaryOp::~BinaryOp() {
	// mInputs.erase(CSL_OPERAND);	// Is this right?
}

// This function returns whether or not its values are dynamic
// Again, these might not be necessary due to ports doing dynamic/static checks automatically

bool BinaryOp::isFixed() {
	return inputIsFixed() && operandIsFixed();
}

// Set the operand from a fixed float

void BinaryOp::setOperand(float op) {
	Port * opPort = mInputs[CSL_OPERAND];
	if (opPort->mUGen != 0)
		throw RunTimeError("Can't set value of UGen port");
	opPort->mValue = op;
}

// print info about this instance

void BinaryOp::dump() {
	fprintf(stderr, "BinaryOp left: ");
	Effect::inPort()->dump();
	fprintf(stderr, "right: ");
	mInputs[CSL_OPERAND]->dump();
	fprintf(stderr, "\n");
}

// AddOp


AddOp::AddOp(UnitGenerator & op1, UnitGenerator & op2) : BinaryOp(op1, op2) { }

AddOp::AddOp(UnitGenerator & op1, float op2) : BinaryOp(op1, op2) { }

AddOp::AddOp(float op1, UnitGenerator & op2) : BinaryOp(op1, op2) { }

void AddOp::nextBuffer(Buffer & outputBuffer, unsigned outBufNum) throw (CException) {
#ifdef CSL_DEBUG
	logMsg("AddOp nextBuffer");						// for verbose debugging (define CSL_DEBUG in CSL_Core.h)
#endif
	SampleBuffer outputBufferPtr = outputBuffer.buffer(outBufNum);
	unsigned numFrames = outputBuffer.mNumFrames;
	DECLARE_OPERAND_CONTROLS;						// Declare and load the operand
	LOAD_OPERAND_CONTROLS;
	for (unsigned i = 0; i < numFrames; i++) {
		*outputBufferPtr++ = *inValue + opValue;		// Do the actual add
		UPDATE_OPERAND_CONTROLS;					// Update both the operand
	}
}
// MulOp

MulOp::MulOp(UnitGenerator & op1, UnitGenerator & op2) : BinaryOp(op1, op2) { }

MulOp::MulOp(UnitGenerator & op1, float op2) : BinaryOp(op1, op2) { }

MulOp::MulOp(float op1, UnitGenerator & op2) : BinaryOp(op1, op2) { }

void MulOp::nextBuffer(Buffer & outputBuffer, unsigned outBufNum) throw (CException)  {
#ifdef CSL_DEBUG
	logMsg("MulOp nextBuffer");								// for verbose debugging (define CSL_DEBUG in CSL_Core.h)
#endif
	SampleBuffer outputBufferPtr = outputBuffer.buffer(outBufNum);
	unsigned numFrames = outputBuffer.mNumFrames;
	DECLARE_OPERAND_CONTROLS;							// Declare and load the operand
	LOAD_OPERAND_CONTROLS;

	for (unsigned i = 0; i < numFrames; i++) {
		*outputBufferPtr++ = * inValue * opValue;		// Do the actual multiply
		UPDATE_OPERAND_CONTROLS;						// Update both the operand
	}
}
