//
//  BlockResizer.h -- Regularizes the amount of data called for
//	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
//
// 	This is useful for time-frequency transformations which only work with certain buffer sizes
//
//	To use, implement an overridden next_buffer method that:
//		- checks if it needs to buffer
//		- if so
//			-- buffers input
//			-- calls next_buffer if it has enough input buffered
//			-- retrieves output
//
//	E.g., 
//	status next_buffer(Buffer & inputBuffer, Buffer & outputBuffer) {
//		unsigned numFrames = outputBuffer.mNumFrames;
//		unsigned dimension = _fftSize;
//		
//		// if I don't need to do any buffering
//		while (numFrames == dimension && 
//		0 == _blockResizer._numAvailableInput && 
//		0 == _blockResizer._numAvailableFrames)
//			return FrameStream::next_buffer(inputBuffer, outputBuffer);
//	
//		_blockResizer.buffer_input(inputBuffer);
//		if (_blockResizer._numAvailableInput >= (int) dimension)
//		{
//			_blockResizer.read_input();
//			FrameStream::next_buffer(_blockResizer._inputBuffer, _blockResizer.mOutputBuffer);
//			_blockResizer.buffer_output();
//		}
//		
//		_blockResizer.read_output(outputBuffer);
//		return cslOk;
//	}

#ifndef CSL_BlockResizer_H
#define CSL_BlockResizer_H

#include "CSL_Core.h"
#include "RingBuffer.h"

namespace csl {

// Regularizes the amount of data called for
// This is useful for time-frequency transformations which
// only work with certain buffer sizes

class BlockResizer : public Effect {

public:
					// ctor / dtor
	BlockResizer();
	BlockResizer(unsigned quantum);
	BlockResizer(UnitGenerator &input, unsigned quantum);
	~BlockResizer();
					// state
	unsigned 	mFrameQuantum;		// how many frames need to be available
	
// IT SHOULD GET THE NUMBER OF CHANNELS FROM THE UGEN BEING FED.
	int			mNumAvailableInput;		// how much input do I have buffered?
	int			mNumAvailableFrames;	// how many frames are available?	
	
	Buffer		mInputBuffer;			// buffer used to pull input
	Buffer		mOutputBuffer;			// buffer used to pull output	
	
	RingBuffer	mInputRingBuffer;		// buffer used to store input
	RingBuffer 	mOutputRingBuffer;		// ring buffer used to store frames
	
					// methods
	void setBlockSize(unsigned quantum);
	void resizeInternalBuffers();
	
	void bufferInput(Buffer & inputBuffer);
	void readInput();			// fills an internal buffer
	void bufferOutput();		// reads from an internal buffer
	void readOutput(Buffer & outputBuffer);
	
																// THIS SHOULD BE A SUBCLASS SPECIAL FOR OVERLAPP ADD
	void readInputAdvancing(unsigned numFramesToAdvance);		// fills an internal buffer -- used in overlap-add
																// THIS SHOULD BE A SUBCLASS SPECIAL FOR OVERLAPP ADD
	void sumIntoOutputAdvancing(unsigned numFramesToAdvance);	// reads from an internal buffer -- used in overlap-add
																// ??? BLOCK RESIZER SHOULD ONLY RESIZE.
	void readOutputAndZero(Buffer& outputBuffer);
	
	void nextBuffer(Buffer & outputBuffer) throw(CException);
	
protected:
	void initBuffers();
	void freeBuffers();	
	void zeroBuffer(Buffer & outputBuffer);

};

}

#endif
