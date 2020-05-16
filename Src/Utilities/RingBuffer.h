//
// RingBuffer.h -- the ring buffer class specification
// RingBufferTap, RingBuffer and BufferStream
//
// See the copyright notice and acknowledgment of authors in the file COPYRIGHT
//
// This is a typical circular buffer with one writer and multiple readers (tap instances)
//

#ifndef CSL_RingBuffer_H
#define CSL_RingBuffer_H

#include "CSL_Core.h"

namespace csl {

///
/// RingBufferTap is a reader that loops over a buffer
///

class RingBufferTap: public UnitGenerator, public Scalable, public Seekable {

public:
	friend class RingBuffer;				///< Allow RingBuffer to access private members of RingBufferTap

											/// Create a tap on a ring buffer, optionally offset relative 
											/// to the current write position.
	RingBufferTap(RingBuffer *parent = 0, int offset = 0);	

	unsigned mLoopStartFrame;				///< Number of frames from the beginning to start a loop at
	unsigned mLoopEndFrame;					///< Number of frames away from buffer end.

	void setOffset(int offset);
	unsigned duration();
	unsigned seekTo(int position, SeekPosition whence) throw(CException);
	void setLoopStart(unsigned frame) { mLoopStartFrame = frame; }
	void setLoopEnd(unsigned frame) { mLoopEndFrame = frame; }
	void setBuffer(RingBuffer *parent) { mParentBuffer = parent; }
	
	void nextBuffer(Buffer &outputBuffer) throw(CException);			///< nextBuffer method
	void nextBuffer(Buffer &outputBuffer, unsigned outBufNum) throw(CException);
	void destructiveNextBuffer(Buffer&outputBuffer) throw(CException);	///< zeroing as it goes.
	void destructiveNextBuffer(Buffer &outputBuffer, unsigned outBufNum) throw(CException);
	
protected:
	unsigned mTempCurrentFrame;				// A hack necessary to track info.
	RingBuffer *mParentBuffer;				// A reference to the buffer from which i'm reading.
	
};

///
/// RingBuffer is the storage + a default reader
///
// NOTE: Doesn't delete the memory allocated for the buffer. It should! Jorge, June 2006.

class RingBuffer : public Effect, public Scalable, public Writeable {

public:
	friend class RingBufferTap;	///< Allow the RingBufferTap to access private members of this class.

	RingBuffer();				///< Constructors
	RingBuffer(unsigned int nmChannels, unsigned int nmFrames);
	RingBuffer(UnitGenerator & input, unsigned int nmChannels, unsigned int nmFrames);

	unsigned mCurrentWriteFrame; ///< state -- users can manipulate my internal tap and buffer
	Buffer mBuffer;
	RingBufferTap mTap;			///< internal tap so a RingBuffer can also be a a UnitGenerator

	unsigned seekTo(int position) throw(CException);
	
	/// These loop setters allow for variable buffer lengths by varying the points where the buffer
	/// writes. 
	void setLoopStart(unsigned frame) { mTap.setLoopStart(frame); };	///< Calls the setLoopStart of it's tap.
	void setLoopEnd(unsigned frame) { mTap.setLoopEnd(frame); };		///< Calls the setLoopEnd of it's tap.

	/// Various flavours of next buffer. Also the nextBuffer(Buffer &) has to be implemented to be able to
	/// hold state of the currentWriteFrame. Otherwise every time it's called by the super nextBuffer
	/// the currentWriteFrame would be incremented more and more, leading to erroneous results.
	void nextBuffer(Buffer &outputBuffer) throw(CException);			///< Read a buffer from the ring buffer
	void writeBuffer(Buffer &inputBuffer) throw(CException);			///< Write a buffer of data into the ring buffer
	void sumIntoBuffer(Buffer &inputBuffer) throw(CException);			///< Do an adding write of data into the ring buffer
	void destructiveNextBuffer(Buffer &outputBuffer) throw(CException);	///< Read a buffer zeroing as you go
	void writeBuffer(Buffer &inputBuffer, unsigned bufNum) throw(CException);
	void sumIntoBuffer(Buffer &inputBuffer, unsigned bufNum) throw(CException);
	
protected:
	unsigned mTempCurrentWriteFrame;	///< Used in next buffer to save state between calls in the same block.
};

/// Class to simplify writing into and playing back buffers. 
/// Think of this as a simple buffer that has a seek, read and write calls built-in.

class BufferStream : public UnitGenerator, public Seekable, public Writeable {
public:
	BufferStream() : UnitGenerator(), Seekable(), Writeable(), 
			mCurrentFrame(0), mTempCurrentFrame(0), mTempCurrentWriteFrame(0) { };
	BufferStream(Buffer &buffer) : UnitGenerator(), Seekable(), Writeable(), mBuffer(&buffer),
			mCurrentFrame(0), mTempCurrentFrame(0), mTempCurrentWriteFrame(0) { };
	~BufferStream() { };
	void dump() { };

	void setBuffer(Buffer &buffer) { mBuffer = &buffer; }

	void nextBuffer(Buffer &outputBuffer) throw(CException); ///< Read a buffer from the buffer stream
	void nextBuffer(Buffer &outputBuffer, unsigned outBufNum) throw(CException);

	void writeBuffer(Buffer &inputBuffer) throw(CException); ///< Write a buffer of data into the ring buffer
	void writeBuffer(Buffer &inputBuffer, unsigned bufNum) throw(CException);

	unsigned seekTo(int position, SeekPosition whence) throw(CException);
	unsigned duration() { return mBuffer->mNumFrames; };
	void trigger() { mCurrentFrame = 0; };
	
protected:
	Buffer *mBuffer;
	unsigned mCurrentFrame;
	unsigned mTempCurrentFrame; 		///< a little hack necessary to track info
	unsigned mTempCurrentWriteFrame;	///< a little hack necessary to track info
};

}

#endif
