//
// BinaryOp.cpp
//	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
// 

#include "BinaryOp.h"
#include "CGestalt.h"

using namespace csl;

BinaryOp :: BinaryOp(FrameStream & op1, FrameStream & op2) : Processor(op1), 
		_operand(& op2),
		_operandValue(0),
		_haveOperandBuffer(false) {
	allocate_op_buffer();
}

BinaryOp :: BinaryOp(FrameStream & op1, float op2) : Processor(op1),
		_operand(NULL),
		_operandValue(op2),
		_haveOperandBuffer(false) {
	allocate_op_buffer();
}

BinaryOp :: ~BinaryOp() {
	if(_haveOperandBuffer)
		_operandBuffer.free_mono_buffers();
}

// answer whether the receiver needs an internal operations buffer

bool BinaryOp :: need_internal_buffer() {
	unsigned bufferSize = CGestalt::block_size();
	return !(input_is_fixed_over(bufferSize) || operand_is_fixed_over(bufferSize));
}

void BinaryOp :: allocate_op_buffer() {
	if (!need_internal_buffer())
		return;
	if (_haveOperandBuffer)
		_operandBuffer.free_mono_buffers();
	_haveOperandBuffer = true;
	_operandBuffer.set_size(CGestalt::num_out_channels(), CGestalt::block_size());
	_operandBuffer.allocate_mono_buffers();
}

// given a buffer length, this function returns whether or not its values change in that range

bool BinaryOp :: is_fixed_over(unsigned bufferLength) {
	return input_is_fixed_over(bufferLength) && operand_is_fixed_over(bufferLength);
}

inline bool BinaryOp :: input_is_fixed_over(unsigned bufferLength) {
	return _input->is_fixed_over(bufferLength);
}

inline bool BinaryOp :: operand_is_fixed_over(unsigned bufferLength) {
	return (0 == _operand) ? true : _operand->is_fixed_over(bufferLength);
}

inline sample BinaryOp :: operand_fixed_value(Buffer & inputBuffer, Buffer & outputBuffer) {
	return (0 == _operand) ? _operandValue : _operand->next_sample(inputBuffer, outputBuffer);
}

status BinaryOp :: next_buffer(Buffer & inputBuffer, Buffer & outputBuffer) {
	if (!input_is_fixed_over(outputBuffer._numFrames)) {
		Processor::pull_input(inputBuffer, outputBuffer);
		if (!operand_is_fixed_over(outputBuffer._numFrames))		
			_operand->next_buffer(inputBuffer, _operandBuffer);
	} else {
		if (!operand_is_fixed_over(outputBuffer._numFrames))	
			_operand->next_buffer(inputBuffer, outputBuffer);
	}
	return FrameStream::next_buffer(inputBuffer, outputBuffer);
}

// print info about this instance

void BinaryOp :: dump() {
	fprintf(stderr, "BinaryOp left: ");
	_input->dump();
	fprintf(stderr, "right: ");
	_operand->dump();
	fprintf(stderr, "\n");
}

// AddOp

AddOp :: AddOp(FrameStream & op1, FrameStream & op2) : BinaryOp(op1, op2) { }

AddOp :: AddOp(FrameStream & op1, float op2) : BinaryOp(op1, op2) { }

status AddOp :: mono_next_buffer(Buffer & inputBuffer, Buffer & outputBuffer, unsigned inBufNum, unsigned outBufNum) {
	sample * fp = outputBuffer._monoBuffers[outBufNum];
	sample * op = _operandBuffer._monoBuffers[outBufNum];
	unsigned numFrames = outputBuffer._numFrames;
	sample opVal;

	if(is_fixed_over(numFrames)) {		// if both the left and right are fixed...
		opVal = operand_fixed_value(inputBuffer, outputBuffer);
		opVal += _input->next_sample(inputBuffer, outputBuffer);
		for (unsigned i = 0; i < numFrames; i++)
			*fp++ = opVal;
	} else if (input_is_fixed_over(numFrames)) {		// if just the left is fixed
		opVal = _input->next_sample(inputBuffer, outputBuffer);
		for (unsigned i = 0; i < numFrames; i++)
			*fp++ += opVal;	// I put the op's next buffer in the outputBuffer
	} else if (operand_is_fixed_over(numFrames)) {	// if just the right is fixed
		opVal = operand_fixed_value(inputBuffer, outputBuffer);
		for (unsigned i = 0; i < numFrames; i++)
			*fp++ += opVal; 	// I put the input's next buffer in the outputBuffer
	} else {				// if neither are fixed	
		for (unsigned i = 0; i < numFrames; i++)
			*fp++ += *op++;
	}
	return cslOk;
}

// return the next individual value

float AddOp :: next_sample(Buffer & inputBuffer, Buffer & outputBuffer) {
	return _input->next_sample(inputBuffer, outputBuffer) + _operand->next_sample(inputBuffer, outputBuffer);
}

// MulOp

MulOp :: MulOp(FrameStream & op1, FrameStream & op2) : BinaryOp(op1, op2) { }

MulOp :: MulOp(FrameStream & op1, float op2) : BinaryOp(op1, op2) { }

status MulOp :: mono_next_buffer(Buffer & inputBuffer, Buffer & outputBuffer, unsigned inBufNum, unsigned outBufNum) {
	sample * fp = outputBuffer._monoBuffers[outBufNum];
	sample * op = _operandBuffer._monoBuffers[outBufNum];
	unsigned numFrames = outputBuffer._numFrames;
	sample opVal;

	if(is_fixed_over(numFrames)) {		// if both the left and right are fixed...
		opVal = operand_fixed_value(inputBuffer, outputBuffer);
		opVal *= _input->next_sample(inputBuffer, outputBuffer);
		for (unsigned i = 0; i < numFrames; i++)
			*fp++ = opVal;
	} else if (input_is_fixed_over(numFrames)) {		// if just the left is fixed
		opVal = _input->next_sample(inputBuffer, outputBuffer);
		for (unsigned i = 0; i < numFrames; i++)
			*fp++ *= opVal;	// I put the op's next buffer in the outputBuffer
	} else if (operand_is_fixed_over(numFrames)) {	// if just the right is fixed
		opVal = operand_fixed_value(inputBuffer, outputBuffer);
		for (unsigned i = 0; i < numFrames; i++)
			*fp++ *= opVal; 	// I put the input's next buffer in the outputBuffer
	} else {				// if neither are fixed	
		for (unsigned i = 0; i < numFrames; i++)
			*fp++ *= *op++;
	}
	return cslOk;
}

// return the next individual value

float MulOp :: next_sample(Buffer & inputBuffer, Buffer & outputBuffer) {
	return _input->next_sample(inputBuffer, outputBuffer) * _operand->next_sample(inputBuffer, outputBuffer);
}
