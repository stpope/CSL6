//

// BinaryOp.h -- Simple arithmetic operations on FrameStreams

//	See the copyright notice and acknowledgment of authors in the file COPYRIGHT

// 



#ifndef INCLUDE_BinaryOp_H

#define INCLUDE_BinaryOp_H



#include "CSL_Core.h" 



namespace csl {



// Abstract class for binary operations mixing framestreams and variables



class BinaryOp : public FrameStream, public Processor {



protected:

	FrameStream * _operand;			// my other input (I inherit one from Processor)

	float _operandValue;			// the operand in case it's just a float (and _operand is NULL)

	Buffer _operandBuffer; 			// buffer used for operations, if it needs to be allocated

	bool _haveOperandBuffer;			// whether I have a temp buffer

	bool need_internal_buffer();		// return true if I need a buffer to compute next_buffer

	void allocate_op_buffer();

	

	status mono_next_buffer(Buffer & inputBuffer, Buffer & outputBuffer, unsigned inBufNum, unsigned outBufNum) = 0;

							// utility functions for the implementation

	inline bool input_is_fixed_over(unsigned bufferLength);

	inline bool operand_is_fixed_over(unsigned bufferLength);	

	inline sample operand_fixed_value(Buffer & inputBuffer, Buffer & outputBuffer);		



public:

	BinaryOp(FrameStream & op1, FrameStream & op2);

	BinaryOp(FrameStream & op1, float op2);

	virtual ~BinaryOp();



	bool is_fixed_over(unsigned bufferLength);

	void dump();

	

	void set_operand(float x) { _operandValue = x; };

	void set_operand(int x) { _operandValue = (float) x; };

	void set_operand(double x) { _operandValue = (float) x; };

	

	virtual sample next_sample(Buffer & inputBuffer, Buffer & outputBuffer) = 0;

	status next_buffer(Buffer & inputBuffer, Buffer & outputBuffer);

};



// Additive operator



class AddOp : public BinaryOp {



protected:

	status mono_next_buffer(Buffer & inputBuffer, Buffer & outputBuffer, unsigned inBufNum, unsigned outBufNum);



public:

	AddOp(FrameStream & op1, FrameStream & op2);

	AddOp(FrameStream & op1, float op2);



	sample next_sample(Buffer & inputBuffer, Buffer & outputBuffer);

};



// Multiplicative operator



class MulOp : public BinaryOp {



protected:

	status mono_next_buffer(Buffer & inputBuffer, Buffer & outputBuffer, unsigned inBufNum, unsigned outBufNum);



public:

	MulOp(FrameStream & op1, FrameStream & op2);

	MulOp(FrameStream & op1, float op2);

	

	sample next_sample(Buffer & inputBuffer, Buffer & outputBuffer);

};



}



#endif



