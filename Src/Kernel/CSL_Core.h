///
/// CSL_Core.h -- the specification file for the core classes of CSL version 6
///
///	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
///
/// What's here:
///
/// Core Classes
///
///		Buffer -- the multi-channel sample buffer class (passed around between generators and IO guys)
///
///		BufferCMap --  a sample buffer with channel map and count (used for many-channel processing)
///
///		Port -- used to represent signal and control inputs and outputs in named maps; 
///			holds a UnitGenerator and its buffer
///
///		UnitGenerator -- an object that can fill a buffer with samples, the central abstraction of CSL DSP
///
///	Mix-in classes (added to UnitGenerator)
///
///		Controllable -- superclass of the mix-ins that add control or signal inputs (held in maps)
///
///		Effect -- A (controllable) UnitGenerator subclass that process an input port (e.g., filters, panners). 
///			All effects inherit from me.
///
///		Scalable -- A (controllable) mix-in that adds scale (multiplicative) and offset (additive) 
///			inputs (used by most common UGens)
///
///		Phased -- a (controllable) mix-in for generators with frequency inputs and persistent phase accumulators
///			All of these mix-in classes add macros for handling their special named control ports, as in
///			DECLARE_PHASED_CONTROLS, LOAD_PHASED_CONTROLS, and UPDATE_PHASED_CONTROLS
///
///		Writeable -- a mix-in for generators that one can write into a buffer on
///
///		Seekable -- a mix-in for generators that one can position (seek) as a stream
///
///		Cacheable -- a mix-in for generators that can cache their past output values (of any size)
///
/// Channel/Buffer processing
///
///		FanOut -- 1-in n-out fan-out object (now built in to UnitGenerator)
///
///		Splitter -- splits a stream into multiple 1-channel outputs
///
///		Joiner -- joins multiple 1-channel inputs into a single stream
///
///		Interleaver -- general inderleaver/de-interleaver
///
/// I/O
///
///		IO -- the input/output stream/driver, its utility functions and virtual constructors
///
///		IODevice -- a holder for a sound interface with name, id, # IO channels, etc.
///

#ifndef CSL_CORE_H			// This is in case you try to include this twice
#define CSL_CORE_H

#include "CSL_Types.h"		// CSL type definitions and central macros, Observer classes
#include "CSL_Exceptions.h"	// CSL exception hierarchy
#include "CGestalt.h"		// System constants class

namespace csl {				// All this happens in the CSL namespace

///
/// Sample buffer contents type (optional)
/// One could argue that we should use subclasses for this, but they're not behaviorally different at present.
///

#ifdef CSL_ENUMS
typedef enum {
	kSamples,				///< Regular audio samples
	kSpectra,				///< FFT complex spectral frames
	kLPCCoeff,				///< LPC reflection coefficients
	kIRData,				///< FIR Impulse Response frames
	kWavelet,				///< Wavelet coefficients
	kGeometry,				///< Spatial geometry buffers
	kUnknown				///< Unknown or other data type
} BufferContentType;
#else
	#define kSamples 0
	#define kSpectra 1
	#define kLPCCoeff 2
	#define kIRData 3
	#define kWavelet 4
	#define kGeometry 5
	#define kUnknown 6
	typedef int BufferContentType;
#endif

//-------------------------------------------------------------------------------------------------//
///
/// Buffer -- the multi-channel sample buffer class (passed around between generators and IO guys).
///
/// Buffers have an opaque pointer () to their data () and know their # channels and frames.
/// They have Boolean aspects about their buffer allocation, and can allocate, free, zero, and check their data.
///
/// Note that this is a "record" class in that its members are all public and it has no accessor
/// functions or complicated methods. It does handle sample buffer allocation and has
/// Boolean members to determine what its pointer state is.
///
/// Note also that Buffers are *not* thread-safe; they hand out pointers (sample*)
/// that are assumed to be volatile.
///

class Buffer {
public:									/// Constructor: default is mono and default-size
	Buffer(unsigned numChannels = 1, unsigned numFrames = CSL_mBlockSize); 
	virtual ~Buffer();					///< Destructor de-allocated
	
									// public data members	
	unsigned mNumChannels;				///< num channels in buffer (num mono buffers)
	unsigned mNumFrames;				///< num frames used in each buffer
	unsigned mNumAlloc;					///< num frames in each buffer
	unsigned mMonoBufferByteSize;		///< size of each buffer in bytes
	unsigned mSequence;					///< sequential serial number
	Timestamp mTimestamp;				///< the buffer's most recent timestamp
	float duration();					///< answer the buffer's duration in seconds
	
	bool mAreBuffersAllocated;			///< are the buffers allocated?
	bool mDidIAllocateBuffers;			///< who allocated my data buffers?
	bool mIsPopulated;					///< does the buffer have data?
	bool mAreBuffersZero;				///< have the buffers been zeroed out?
	BufferContentType mType;			///< Data type flag
										/// set the internal size variables (no buffer allocation takes place)
	void setSize(unsigned numChannels, unsigned numFrames);
										/// this version doesn't even allocate the pointers
	void setSizeOnly(unsigned numChannels, unsigned numFrames);

	void checkBuffers() throw (MemoryError);	///< allocate if not already there
	void allocateBuffers() throw (MemoryError);	///< fcn to malloc storage buffers
	void freeBuffers();							///< fcn to free them
	bool canStore(unsigned numFrames);			///< answer whether the recevei can store numFrames more frames

	void zeroBuffers();							///< fill all data with 0
	void fillWith(sample value);				///< fill data with the given value
	void scaleBy(sample value);					///< scale the samples by the given value
												// import data from the given buffer
	void copyFrom(Buffer & src) throw (RunTimeError);			
	void copyHeaderFrom(Buffer & source) throw (RunTimeError);	///< copy the "header" fields of a buffer
	void copySamplesFrom(Buffer & src) throw (RunTimeError);	///< import data from the given buffer
	void copySamplesFromTo(Buffer & src, unsigned offset) throw (RunTimeError);	///< same with write offset
	void copyOnlySamplesFrom(Buffer & src) throw (RunTimeError);///< import data from the given buffer
	bool readFromFile(char * fname);							///< read a buffer from a snd file; answer success

	csl::Status convertRate(int fromRate, int toRate);			///< convert the sample rate using libSampleRate

											/// answer a samp ptr with offset
	virtual SampleBuffer samplePtrFor(unsigned channel, unsigned offset);
											/// answer a samp ptr tested for extent (offset + maxFrame)
	virtual SampleBuffer samplePtrFor(unsigned channel, unsigned offset, unsigned maxFrame);
	
											/// convenience accessors for sample buffers
	virtual SampleBuffer buffer(unsigned bufNum);
	virtual SampleBuffer * buffers() { return mBuffers; }
											/// Set the buffer pointer (rare; used in joiners)
	virtual void setBuffers(SampleBuffer * sPtr) { mBuffers = sPtr; };
	virtual void setBuffer(unsigned bufNum, SampleBuffer sPtr) { mBuffers[bufNum] = sPtr; };
	virtual void setBuffer(unsigned bufNum, unsigned offset, sample samp) { *((mBuffers[bufNum]) + offset) = samp; };

	float normalize(float maxVal);			///< normalize the buffer(s) to the given max; answer the prior max
	float normalize(float maxVal, float from, float to);	///< normalize the given region only
	
/// Buffer Sample Processing (optional).
/// One could also easily add Buffer operators, such as (Buffer + Buffer) or (Buffer * Buffer)

	float rms(unsigned chan, unsigned from, unsigned to);	///< get the root-mean-square of the samples
	float avg(unsigned chan, unsigned from, unsigned to);	///< get the average of the samples
	float max(unsigned chan, unsigned from, unsigned to);	///< get the max of the absolute val of the samples
	float min(unsigned chan, unsigned from, unsigned to);	///< get the min of the samples
	
#ifdef CSL_DSP_BUFFER
	unsigned int zeroX(unsigned chan);		///< count the zero-crossings in the samples
	unsigned int indexOfPeak(unsigned chan);							///< answer the index of the peak value
	unsigned int indexOfPeak(unsigned chan, unsigned low, unsigned hi);	///< answer the index of the peak value
	unsigned int indexOfMin(unsigned chan);								///< answer the index of the peak value
	unsigned int indexOfMin(unsigned chan, unsigned low, unsigned hi);	///< answer the index of the peak value
	void autocorrelation(unsigned chan, SampleBuffer result);			///< autocorrelation into the given array
#endif

protected:
	SampleBufferVector mBuffers;		///< the storage vector -- pointers to (SampleBuffer) buffers
};

///
/// BufferCMap is a Sample buffer with channel map and count.
///
/// The map is so that one can have (e.g.,) a buffer that stands for 3 channels within an 8-channel space
///

class BufferCMap : public Buffer {
public:
	BufferCMap();							///< Constructors: default is useless
	BufferCMap(unsigned numChannels, unsigned numFrames); ///< ask for a given number of "virtual" channels
	BufferCMap(unsigned numChannels, unsigned realNumChannels, unsigned numFrames);
	~BufferCMap();							///< Destructor

	unsigned mRealNumChannels;				///< the actual number of channels used
	std::vector<int> mChannelMap;			///< the map between virtual and real channels

											/// Pointer accessor uses channel map
	SampleBuffer buffer(unsigned bufNum) { return mBuffers[mChannelMap[bufNum]]; }
};

///
/// UnitGenerator buffer copy policy flags (for multi-channel expansion)
///

#ifdef CSL_ENUMS
typedef enum {
	kCopy,				///< compute 1 channel and copy
	kExpand,			///< call monoNextBuffer multiple times
	kIgnore				///< ignore extra buffer channels
} BufferCopyPolicy;
#else
	#define kCopy 0
	#define kExpand 1
	#define kIgnore 2
	typedef int BufferCopyPolicy;
#endif

class RingBuffer; 	///< forward declaration

//-------------------------------------------------------------------------------------------------//
///
/// UnitGenerator -- the core of CSL; all unit generators inherit from this class.
///
/// These have members for their sample rate and number of channels, and know their outputs.
/// The main operation is the nextBuffer() method, which is overridden in many of the subclasses.
///
/// If more than 1 output is used, these can handle fan-out automatically, either synchronous
/// (as in loops in a graph) or async (as in separate call-back threads).
/// The mOutputCache RingBuffer may hold some large number of past samples, and can use nextBuffer()
/// to do n-way fan-out either synchronously or with differing buffer sizes or callback rates.
///
/// UnitGenerator inherits from Model, meaning that it has to send this->changed((void *) dataBuffer) 
/// from within its nextBuffer method so that dependent objects (like signal views) can get notification
/// when it computes samples.  This mechanism could also be used for signal flow.
///

class UnitGenerator : public Model {
public:
										/// Constructors (UGens are mono by default)
										/// defaults to mono and maxBlockSize if not specified.
	UnitGenerator(unsigned rate = CGestalt::frameRate(), unsigned chans = 1);	
	virtual ~UnitGenerator();			///< Destructor

										// accessing methods
	unsigned frameRate() { return mFrameRate; };				///< get/set the receiver's frame rate
	void setFrameRate(unsigned rate) { mFrameRate = rate; }

	virtual unsigned numChannels() { return mNumChannels; };	///< get/set the receiver's number of outputs
	void setNumChannels(unsigned ch) { mNumChannels = ch; }

	BufferCopyPolicy copyPolicy() { return mCopyPolicy; };		///< get/set the receiver's buffer copy policy
	void setCopyPolicy(BufferCopyPolicy ch) { mCopyPolicy = ch; }

//	string name() { return mName; };							///< get/set the receiver's name string
//	void setName(char * ch) { mName = string(ch); }
//	void setName(string ch) { mName = ch; }

									/// get a buffer of Frames -- this is the core CSL "pull" function;
									/// the given buffer can be written into, and a changed() message is sent.
	virtual void nextBuffer(Buffer & outputBuffer) throw (CException);
	
									/// really compute the next buffer given an offset base channel;
									/// this is called by nextBuffer, possibly multiple times
	virtual void nextBuffer(Buffer & outputBuffer, unsigned outBufNum) throw (CException);

									/// query whether I'm fixed (StaticVariable overrides this)
	virtual bool isFixed() { return false; };
									/// query whether I'm currently active (Envelopes can go inactive)
	virtual bool isActive() { return true; };
									/// add to or return the UGen vector of outputs
	void addOutput(UnitGenerator * ugen);
	void removeOutput(UnitGenerator * ugen);
	UGenVector outputs() { return mOutputs; };
	virtual unsigned numOutputs() { return mNumOutputs; };
									/// check for fan-out and copy previous buffer; return true if fanning out
	bool checkFanOut(Buffer & outputBuffer) throw (CException);
	void handleFanOut(Buffer & outputBuffer) throw (CException);

									/// set/get the value (not allowed in the abstract, useful for static values)
	virtual void setValue(sample theValue) { throw LogicError("can't set value of a generator"); };
	virtual sample value() { throw LogicError("can't get value of a generator"); };

	virtual void dump();			///< pretty-print the receiver
	virtual void trigger() { };		///< trigger ignored here

protected:				// My data members
	unsigned mFrameRate;			///< the frame rate -- initialized to be the default by the constructor
	unsigned mNumChannels;			///< my "expected" number of output channels
	BufferCopyPolicy mCopyPolicy;	///< the policy I use if asked for more or fewer channels
	UGenVector mOutputs;			///< the vector of my output UGens
	unsigned mNumOutputs;			///< the number of outputs
	Buffer * mOutputCache;			///< my past output ring buffer (only used in case of fan-out)
	unsigned mSequence;				///< the highest-seen buffer seq number
//	string mName;					///< my name (used for editors)
									/// utility method to zero out an outputBuffer
	void zeroBuffer(Buffer & outputBuffer, unsigned outBufNum);
};

//-------------------------------------------------------------------------------------------------//
///
/// Port -- used to represent constant, control-rate or signal inputs and outputs in named maps;
/// holds a UnitGenerator and its buffer, OR a single floating-point value (in which case the
/// UGen pointer is set to NULL and mPtrIncrement = 0).
///
/// The nextValue() message is used to get the dynamic or static value.
///

class Port {
public:
	Port();								///< Constructors: default is a float = 0
	Port(UnitGenerator * ug);			///< Given a UGen, use it as the input
	Port(float value);					///< given a float, hold it as the static value
	virtual ~Port();					///< Destructor

										// public data members
	UnitGenerator * mUGen;				///< my unit generator (pointer or NULL)
	Buffer * mBuffer;					///< the buffer used to hold my output
	float mValue;						///< my value (in case I'm fixed [mUGen == NULL])
	float *mValuePtr;					///< my value's address (const or buffer pointer)
	unsigned mPtrIncrement;				///< the inter-sample ptr increment (0 for const, 1 for dynamic)
	unsigned mValueIndex;				///< my index (into the UGen's buffer)

	void checkBuffer() throw (LogicError);	///< check the port's buffer and allocate it if needed
	inline float nextValue();				///< answer the next value (dynamic or constant)
	inline void nextFrame(SampleBuffer where);	///< write the val to a buffer
	inline bool isReady();				///< answer whether I'm ready to be read
	void resetPtr();					///< reset the buffer pointer without re-pulling the input
	virtual bool isActive();			///< answer whether I'm active
	void dump();						///< pretty-print the receiver
	bool isFixed() { return (mPtrIncrement == 0); };			///< am I fixed or dynamic
	virtual void trigger() { if (mUGen) mUGen->trigger(); };	///< trigger passed on here

};

// Answer the next value (dynamic or constant) as a fast inline function

inline sample Port::nextValue() {
	mValuePtr += mPtrIncrement;			// increment the pointer (mPtrIncrement may be 0)
	return *mValuePtr;					// return the value
}

// Write the next n-dimensional sample frame

inline void Port::nextFrame(SampleBuffer where) {
	for (unsigned i = 0; i < mBuffer->mNumChannels; i++)
		*where++ = mBuffer->buffer(i)[mValueIndex];
	mValueIndex++;						// increment the counter (an unsigned, initially 0)
}

// Answer whether the give port is ready (i.e., has its UGen or scalar value set up and primed)

inline bool Port::isReady() {
	return (mValuePtr != 0);
}

//-------------------------------------------------------------------------------------------------//
///
/// Controllable -- superclass of the mix-ins that add control or signal inputs.
/// This holds onto a map of port objects that represent the inputs,
/// and manages the naming and processing flow for dynamic inputs.
///
/// A typical complex UGen will have several ports, e.g., for frequency, scale, and
/// offset in the case of an oscillator that supports AM and FM.
/// The pullInput() message is used to call the nextBuffer() method of a given port.
///

class Controllable {
public:
	Controllable() : mInputs() { };			///< Constructor takes no arguments
	virtual ~Controllable();				///< Destructor (remove the output links of the ports)

	Port * getPort(CSL_MAP_KEY name);
protected:
	PortMap mInputs;						///< the map of my inputs or controls (used by the mix-in classes)

											/// Plug in a unit generator to the named input slot
	void addInput(CSL_MAP_KEY name, UnitGenerator & ugen);
											/// Plug in a float to the named input slot
	void addInput(CSL_MAP_KEY name, float value);
											/// method to read the control values (in case they're dynamic).
											/// this sends nextBuffer() to the input.
	void pullInput(Port * thePort, unsigned numFrames) throw (CException);
	void pullInput(Port * thePort, Buffer & theBuffer) throw (CException);

	virtual void dump();					///< pretty-print the receiver's input/controls map
};

//-------------------------------------------------------------------------------------------------//
///
/// Scalable -- mix-in class with scale and offset control inputs (may be constants or generators).
///
/// This uses the mInput map keys CSL_SCALE and CSL_OFFSET.
///
/// Most actual unit generators inherit this as well as UnitGenerator.
///
/// We use Controllable as a virtual superclass so that we can mix it in twice (in classes that are also Phased)
///

class Scalable : public virtual Controllable {
public:
	Scalable();										///< Constructors
	Scalable(float scale);							///< use the given static scale
	Scalable(float scale, float offset);			///< use the given static scale & offset
	Scalable(UnitGenerator & scale, float offset);	///< use the given dynamic scale & static offset
	Scalable(UnitGenerator & scale, UnitGenerator & offset);	///< use the given dynamic scale & offset
	~Scalable();									///< Destructor

													// accessors
	void setScale(UnitGenerator & scale);			///< set the receiver's scale member to a UGen or a float
	void setScale(float scale);

	void setOffset(UnitGenerator & offset);			///< set the receiver's offset member to a UGen or a float
	void setOffset(float offset);
	virtual void trigger();							///< trigger passed on here
	void isScaled();								///< answer whether scale = 1 & offset = 0

};

/// Macros for all the Scalable UnitGenerators (note that these don't end with ";")
///
/// Note that these make some assumptions about variable names declared in the method;

/// Declare the pointer to scale/offset buffers (if used) and current scale/offset values

#define DECLARE_SCALABLE_CONTROLS							\
	Port * scalePort = mInputs[CSL_SCALE];					\
	Port * offsetPort = mInputs[CSL_OFFSET];				\
	float scaleValue, offsetValue

/// Load the scale/offset-related values at the start

#define LOAD_SCALABLE_CONTROLS								\
	Controllable::pullInput(scalePort, numFrames);			\
	scaleValue = scalePort->nextValue();					\
	Controllable::pullInput(offsetPort, numFrames);			\
	offsetValue = offsetPort->nextValue()

// Update the scale/offset-related values in the loop

#define UPDATE_SCALABLE_CONTROLS							\
	scaleValue = scalePort->nextValue();					\
	offsetValue = offsetPort->nextValue()

#define CHECK_UPDATE_SCALABLE_CONTROLS						\
	if (scalePort)											\
		scaleValue = scalePort->nextValue();				\
	if (offsetPort)											\
		offsetValue = offsetPort->nextValue()

#define IS_UNSCALED											\
	(scalePort->isFixed()) && (offsetPort->isFixed()) &&	\
		(scaleValue == 1.0) && (offsetValue == 0.0)


//-------------------------------------------------------------------------------------------------//
///
/// Effect -- mix-in for classes that have unit generators as inputs (like filters).
///
/// Note that this always uses a separate buffer for the input.
///

class Effect : public UnitGenerator, public virtual Controllable {
public:
	Effect();									///< Constructors
	Effect(UnitGenerator & input);				///< use the given input

	virtual bool isActive();					///< am I active?

	void setInput(UnitGenerator & inp);			///< set the receiver's input generator
//	UnitGenerator *input() { return mInputs[CSL_INPUT]->mUGen; }	// no getter for now
	bool isInline;								///< whether to use input or buffer as source
	void setInline() { isInline = true; }		///< set the Effect to be inline
	
protected:
	SampleBuffer mInputPtr;						///< A pointer to my input's data.
												/// method to read the input value
	void pullInput(Buffer & outputBuffer) throw (CException);
	void pullInput(unsigned numFrames) throw (CException);
	virtual void trigger();						///< trigger passed on here
												/// get the input port
	inline Port * inPort() { return mInputs[CSL_INPUT]; };
};

//-------------------------------------------------------------------------------------------------//
///
/// Phased -- a mix-in for objects with phase accumulators (local float) and frequency controls (an input port).
///
/// This puts an item named CSL_FREQUENCY in the Controllable parent mInputs map.
///
/// We use Controllable as a virtual superclass so that we can mix it in twice (in classes that are also Scalable)
///

class Phased : public virtual Controllable {
public:
	Phased();										///< Constructors; this one is rearely used
	Phased(float frequency, float phase = 0);		///< use the given dynamic frequency & phase
	Phased(UnitGenerator & freq, float phase = 0);	///< use the given dynamic or static frequency
	~Phased();										///< Destructor

													/// Setter accessors
	void setFrequency(UnitGenerator & frequency);	///< set frequency
	void setFrequency(float frequency);				///< set frequency
	void setPhase(float phase) { mPhase = phase; };	///< set phase

protected:
	sample mPhase;									///< the actual phase accumulator
};

/// Macros for all the Phased UnitGenerators  (note that these don't end with ";")
/// These make some assumptions about variable names declared in the method;
/// i.e., the number of frames to compute must be named "numFrames."
/// Use this: 	unsigned numFrames = outputBuffer.mNumFrames;

/// Declare the frequency port (accessing the mInputs map) and current value.

#define DECLARE_PHASED_CONTROLS								\
	Port * freqPort = mInputs[CSL_FREQUENCY];				\
	float freqValue

/// Load the freq-related values at the start of the callback; if the frequency is a dynamic UGen input,
/// then pull its value, get the pointer to its buffer, and set the first value, 
/// otherwise store the constant value

#define LOAD_PHASED_CONTROLS								\
	Controllable::pullInput(freqPort, numFrames);			\
	freqValue = freqPort->nextValue()

/// Update the freq-related value in the loop

#define UPDATE_PHASED_CONTROLS								\
	freqValue = freqPort->nextValue()

#define CHECK_UPDATE_PHASED_CONTROLS						\
	if (freqPort)											\
		freqValue = freqPort->nextValue()

//-------------------------------------------------------------------------------------------------//
///
/// Writeable -- a mix-in for buffers and streams that one can write to
///

class Writeable {
public:								/// write to the receiver
	virtual void writeBuffer(Buffer& inputBuffer) throw(CException);
	virtual ~Writeable() { /* ?? */ };

protected:							/// write to the receiver
	virtual void writeBuffer(Buffer & inputBuffer, unsigned bufNum) throw(CException);
};

///
/// Enumeration for seek flags
///

#ifdef CSL_ENUMS
typedef enum {
	kPositionStart,
	kPositionCurrent,
	kPositionEnd
} SeekPosition;
#else
	#define kPositionStart 0
	#define kPositionCurrent 1
	#define kPositionEnd 2
	typedef int SeekPosition;
#endif

//-------------------------------------------------------------------------------------------------//
///
/// Seekable -- a mix-in for positionable streams
///

class Seekable {
public:
	Seekable() : mCurrentFrame(0), mActualFrame(0.0)  { }	///< Constructor
	virtual ~Seekable() { /* ?? */ };

	unsigned mCurrentFrame;					///< where I currently am in the buffer
	double mActualFrame;					///< where I actually am in the buffer

												/// general-purpose seek on a stream
	virtual unsigned seekTo(int position, SeekPosition whence) throw(CException) = 0;
	virtual void reset() throw(CException);		///< reset-to-zero
	virtual unsigned duration() { return 0; };	///< number of frames in the Seekable
};

///
/// Cacheable -- a mix-in for caching streams
///

class Cacheable {
public:
	Cacheable() : mUseCache(false) { }	;	///< Constructors
	Cacheable(bool uC) : mUseCache(uC) { };

	bool mUseCache;							///< whether I'm to cache (vs. compute)
};

/////////////// Utility UnitGenerator Classes ///////////////////////

///
/// A fan-out generator for DSP graphs with loops.
///
/// This takes a single input, and provides mNumFanOuts outputs;
/// it only calls its input every mNumFanOuts frames.
///
/// This behavior is now standard on UnitGenerators, using their mOutputs UGenVector
/// (see UnitGenerators::nextBuffer).
///

class FanOut : public Effect {
public:
	FanOut(UnitGenerator & in, unsigned taps);	///< Constructors
	~FanOut() { };

	virtual void nextBuffer(Buffer & outputBuffer) throw(CException);
	virtual void nextBuffer(Buffer & outputBuffer,  unsigned outBufNum) throw(CException);

protected:
	Buffer mBuffer;			///< my temp buffer
	unsigned mNumFanOuts;	///< the number of outputs
	unsigned mCurrent;		///< the current output
};

///
/// Splitter class -- a de-multiplexer for multi-channel signals
///

class Splitter : public FanOut {
public:
	Splitter(UnitGenerator & in);				///< Constructor
	~Splitter() { };

	unsigned numChannels() { return 1; };		///< I'm mono
												/// nextBuffer processes splitter channels
	virtual void nextBuffer(Buffer & outputBuffer) throw(CException);
	virtual void nextBuffer(Buffer & outputBuffer,  unsigned outBufNum) throw(CException);
};

///
/// Joiner class -- a multiplexer for multi-channel signals
///

class Joiner : public Effect {
public:										///< loop through my vector of inputs
	Joiner() : Effect() { mNumChannels = 0; };	///< Constructors
	Joiner(UnitGenerator & in1, UnitGenerator & in2);
	~Joiner() { };
												/// nextBuffer processes joiner channels
	virtual void nextBuffer(Buffer & outputBuffer) throw(CException);
	virtual void nextBuffer(Buffer & outputBuffer,  unsigned outBufNum) throw(CException);
	void addInput(UnitGenerator & in);			///< add the argument to vector of inputs
	bool isActive();
	virtual void trigger();						///< trigger passed on here
//	unsigned numChannels() { return mNumChannels; };
//	inline Port * inPort() { return mInputs[CSL_INPUT]; };
//	virtual unsigned numOutputs() { return mNumOutputs; };

protected:
//	UGenVector mInputs;						///< my vector of inputs
};

///
/// Interleaver handles copying interleaved sample buffers (like sound files and inter-process sockets)
/// to/from non-interleaved CSL-style Buffer objects.
///

class Interleaver {

public:
					/// Interleave = copy from CSL-style Buffer object to an interleaved sample vector
	void interleave(Buffer & output, SampleBuffer samples, unsigned numFrames, 
					unsigned numChannels) throw (CException);
	void interleave(Buffer & output, short * samples, unsigned numFrames, 
					unsigned numChannels) throw (CException);

					/// Interleave = copy from CSL-style Buffer object to an interleaved sample vector
					/// Remap = re-assign channels from the source buffer to the target while interleaving
	void interleaveAndRemap(Buffer & output, SampleBuffer samples, unsigned numFrames, unsigned numChannels,
						unsigned *channelMap) throw (CException);

					/// De-interleave = copy from interleaved SampleBuffer to CSL Buffer object
	void deinterleave(Buffer & output, SampleBuffer samples, unsigned numFrames, 
					unsigned numChannels) throw (CException);
	void deinterleave(Buffer & output, short * samples, unsigned numFrames, 
					unsigned numChannels) throw (CException);
};

//-------------------------------------------------------------------------------------------------//
///// Support for the IO classes

#ifndef CSL_WINDOWS					// on "normal" platforms

#ifdef DO_TIMING					// Here are the macros and globals for the timing code
#include <sys/time.h>
#define GET_TIME(val) if (gettimeofday(val, 0) != 0) logMsg(kLogError, "Output: Error reading current time");
#define SUB_TIMES(t1, t2) (((t1->tv_sec - t2->tv_sec) * 1000000) + (t1->tv_usec - t2->tv_usec))
#endif

#else								// If on Windows

#ifdef DO_TIMING					// Here are the macros and globals for the timing code
#include <Winsock2.h>
#include <winsock.h>
#include <time.h>
int getSysTime(timeval *val, void * e);
#define GET_TIME(val) if (getSysTime(val, 0) != 0) logMsg(kLogError, "Output: Error reading current time");
#define SUB_TIMES(t1, t2) (((t1->tv_sec - t2->tv_sec) * 1000000) + (t1->tv_usec - t2->tv_usec))
#endif

#endif								// Windows

/// IO Status flag

#ifdef CSL_ENUMS
typedef enum {
	kIONew,
	kIOInit,
	kIOOpen,
	kIORunning,
	kIOClosed,
	kIOExit
} IO_Status;
#else
	#define kIONew 0
	#define kIOInit 1
	#define kIOOpen 2
	#define kIORunning 3
	#define kIOClosed 4
	#define kIOExit 5
	typedef int IO_Status;
#endif

//-------------------------------------------------------------------------------------------------//
///
/// IO -- the abstract I/O scheduling class; subclasses interface to specific I/O APIs.
///
///	An IO object has a graph (a ptr to a UGen), and it registers itself with some call-back API
///	(like PortAudio, CoreAudio, Jack, VST, JUCE), setting up a callback function that in turn
///	calls the nextBuffer() method of its graph root.
///
/// One creates an IO with the desired rate, block size (optional) I/O device keys, and the	number of
/// in and out channels; you then set its root to be your DSP graph and send it start/stop messages.
///
/// All this is public because it's used by static call-back functions.
///

class IO : public Model  {											/// superclass = Model
public:
	IO(unsigned s_rate = 44100, unsigned b_size = CSL_mBlockSize,
		int in_device = -1, int out_device = -1,
		unsigned in_chans = 2, unsigned out_chans = 2);				/// default is stereo input & output
	virtual ~IO() { };
							// Control methods
	virtual void open() throw(CException) { mStatus = kIOOpen; };	///< open/close start/stop methods
	virtual void close() throw(CException) { mStatus = kIOClosed; };
	virtual void start() throw(CException) { mStatus = kIORunning; };
	virtual void stop() throw(CException) { mStatus = kIOOpen; };
	virtual void test() throw(CException) { };	///< test the IO's graph
	virtual void capture_on(float dur);		///< begin output capture
	virtual void capture_off();				///< end output capture
	virtual Buffer * get_capture();			///< answer the capture buffer
	void setRoot(UnitGenerator & root);		///< set/clear my graph root generator
	void clearRoot();
											/// get a buffer from the CSL graph
	void pullInput(Buffer & outBuffer, SampleBuffer out = 0)  throw(CException);

	virtual Buffer & getInput() throw(CException);	///< Get the current input from the sound card
	virtual Buffer & getInput(unsigned numFrames, unsigned numChannels) throw(CException);
	unsigned getAndIncrementSequence();		///< increment and answer my seq #

							// Data members
	UnitGenerator * mGraph;					///< the root of my client DSP graph, often a mixer or panner
	Buffer mInputBuffer;					///< the most recent input buffer (if it's turned on)
	Buffer mOutputBuffer;					///< the output buffer I use (passed to nextBuffer calls)
	Buffer mCaptureBuffer;					///< the output buffer I use for capturing output (for testing)
	SampleBuffer mInputPointer;				///< the buffer for holding the sound card input (if open)
	unsigned * mChannelMap;					///< the output channel remapping array

	unsigned mNumFramesPlayed;  			///< counter of frames I've played
	unsigned mSequence;						///< sequence counter
	unsigned mLoggingPeriod;				///< logging period in seconds
	unsigned mNumInChannels;				///< # inputs
	unsigned mNumOutChannels;				///< # outputs
	unsigned mNumRealInChannels;			///< # physical inputs
	unsigned mNumRealOutChannels;			///< # physical outputs
	IO_Status mStatus;						///< status flag
	bool mInterleaved;						///< flag if IO is interleaved
	unsigned mOffset;						///< used for capture offset

#ifdef DO_TIMING							// This is for the performance timing code
	struct timeval mThen, mNow;				///< used for getting the real time
	long mTimeVals, mThisSec, mTimeSum;		///< for printing run-time statistics
	float mUsage;							///< cpu usage %
											/// print the CPU usage message
	void printTimeStatistics(struct timeval * tthen, struct timeval * tnow, long * tsecond, 
							long * ttimeSum, long * ttimeVals);
#endif

protected:									/// initialize overridden in subclasses
	virtual void initialize(unsigned sr, unsigned bs, int is, int os, unsigned ic, unsigned oc) { };
	float maxSampEver;

};

//-------------------------------------------------------------------------------------------------//
///
/// IO Device class -- a holder for a sound interface with name, id, # IO channels, etc.
///

class IODevice {
public:
	IODevice() { } ;						/// Constructor takes all variables, calls initialize()
	IODevice(char * name, unsigned index, unsigned maxIn, unsigned maxOut, bool isIn, bool isOut);
	IODevice(string name, unsigned index, unsigned maxIn, unsigned maxOut, bool isIn, bool isOut);
						/// public members
	char mName[CSL_NAME_LEN];		///< my device name
	unsigned mIndex;				///< index (API-specific)
	unsigned mMaxInputChannels;		///< # HW ins
	unsigned mMaxOutputChannels;	///<# HW outs
	float mFrameRate;				///< current SR
	vector<float> mFrameRates;		///< the vector of frame rates I support
	bool mIsDefaultIn;				///< am i the default in?
	bool mIsDefaultOut;				///< am i the default out?
	void dump();					///< pretty-print the receiver' device
};

}	// end of namespace

#endif // CSL_CORE_H
