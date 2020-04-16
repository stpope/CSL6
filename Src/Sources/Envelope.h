///
///	Envelope.h -- The basic (concrete) CSL breakpoint envelope classes.
///	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
///
///
///	 These are UnitGenerators that represent arbitrarily long breakpoint functions and do linear
///	 interpolation between their breakpoints. There are several kinds of 
///	 constructors, e.g., using breakpoint objects or x/y value pairs, and one 
///	 can scale the values or times of an existing envelope.
///
///	 Classes:
///		LineSegment: A linearly interpolated segment with start and end values, and a duration (in seconds).
///		Envelope: a collection of LineSegments; may have an input and act like an effect, 
///			or have no input and act like a control UnitGenerator
///		Specific kinds of envelope (AR, ADSR, Triangle) and RandomEnvelope
///

#ifndef CSL_Envelope_H
#define CSL_Envelope_H

#include "CSL_Core.h"
#include "CPoint.h"

namespace csl {

///
/// LineSegment flags for line interpolation.
///

#ifdef CSL_ENUMS
typedef enum {			
	kLine,		///< linear interpolation between start and end values.
	kExpon,		///< linear interpolation between start and end values.
} LineMode;
#else
	#define kLine 1
	#define kExpon 2
	typedef int LineMode;
#endif

///
/// A linearly interpolated segment -- this has start and end values, and a duration (in seconds).
///

class LineSegment : public UnitGenerator {
public:
	LineSegment();			///< empty constructor
	LineSegment(float d, float s, float e, LineMode mode = kLine); ///< Declare dur in sec, start, stop values

							/// Accessors
	float start() { return mStart; }		///< Returns the initial value of the line segment.
	float end() { return mEnd; }			///< Returns the target value of the line segment.
	float duration() { return mDuration; }	///< Returns the total time it will take to get from start to end value.
	unsigned currentFrame() { return mCurrentFrame; };

	void setEnd(float tend) { mEnd = tend; }
	void setStart(float tstart) { mStart = tstart; mCurrentValue = mStart; }
	void setDuration(unsigned tduration) { mDuration = (float) tduration; }	///< Overloaded to accept either float or unsigned.
	void setDuration(float tduration) { mDuration = tduration; }
	void setMode(LineMode tmode) { mMode = tmode; }		///< Sets the interpolation kind (linear or exponential)
	
								/// next buffer interpolator
	void nextBuffer(Buffer &outputBuffer, unsigned outBufNum) throw (CException);
								/// handy version given Scalable port pointers
	virtual void nextBuffer(Buffer &outputBuffer, unsigned outBufNum, Port * scalePort, Port * offsetPort) throw (CException);

	void reset();				///< reset counters
	void trigger() { this->reset(); };		///< reset internal time to restart envelope
	void dump();				///< Prints to screen the start and end values and the duration of the line.

protected:
	float mStart;				///< Start value
	float mEnd;					///< Ending value
	float mDuration	;			///< Length of the line segment (IN SECONDS)
	LineMode mMode;				///< How am I to calculate the values from start to end values of the line.
	float mCurrentValue;		///< Internal book-keeping
	unsigned mCurrentFrame;		///< cache
};

/// a map between a time and a line segment

typedef map<float, LineSegment *> Breakpoints;

///
/// Envelope: a collection of LineSegments; may have an input (scale) and act like a processor, 
///	or have no input and act like a control UGen. I inherit Scalable setScale, setOffset for inputs
///

class Envelope : public UnitGenerator, public Scalable {
public:
	Envelope() : UnitGenerator(), Scalable(1, 0), mDuration(0), mSegments(0), mValues(0) { };
	Envelope(LineMode mode, float t, float x1, float y1, float x2 = 0, float y2 = 1.0, float x3 = 0, float y3 = 1.0,
				float x4 = 0, float y4 = 1.0, float x5 = 0, float y5 = 1.0, float x6 = 0, float y6 = 1.0);
	Envelope(LineMode mode, float t, unsigned int size, float x[], float y[]);
	Envelope(float t, float x1, float y1, float x2 = 0, float y2 = 1.0, float x3 = 0, float y3 = 1.0,
				float x4 = 0, float y4 = 1.0, float x5 = 0, float y5 = 1.0, float x6 = 0, float y6 = 1.0);
	Envelope(float t, unsigned int size, float x[], float y[]);

	virtual ~Envelope();
										/// This answers whether I'm active (ptr < end)
	virtual bool isActive();

	void addBreakpoint(float startTime, float value);
	
	void setMode(LineMode mode);
//	void setInterpolationAtSegment(LineMode mode, unsigned idx); ///< allows to specify interpolation other than linear for a segment.
	virtual void setDuration(float d);	///< set/scale durations
	virtual void scaleTimes(float s);	///< scale durations
	virtual void scaleValues(float s);	///< scale values so the max is s
	
	virtual void reset();				///< reset internal time to restart envelope
	virtual void trigger();				///< reset internal time to restart envelope
	virtual void dump();
										/// The main FrameStream work method
	virtual void nextBuffer(Buffer &outputBuffer, unsigned outBufNum) throw (CException);

protected:
	float mDuration;					///< Total duration, typically in seconds
	float mCurrentMark;					///< How far we have read	
	Breakpoints mSegmentMap;			///< list of envelope breakpoints
	LineSegment **mSegments;			///< array of line segments that for the envelope
	float *mValues;
	unsigned mSize;
										/// Internal helper method for computing the next buffer
	unsigned int privateNextBuffer(CPoint *breakpoint, LineSegment *segment, float *buffer, unsigned int numFrames);
	void createSegments();				///< Allocate memory for the segments.
	void calculateSegments();			///< Calculate the internal data
};

/// ADSR = 4-segment attack/decay/sustain/release envelope class.

/** 
	Most of this is inherited from Envelope.
	This design is an extended ADSR envelope with an initial constant segment and offset
	(default = 0@0; this can be used to make an attack delay as on ARP ADSRs), attack time and
	max. val (default = 1.0), decay time, sustain level, and release time and final value (default = 0.0).
	All times are assumed to be given in seconds.
	Note that there are many variations on the constructor.
	The 5 line segments used internally look something like the following

	|		/												.
	|      /  \												.
	|     /     \---------------							.
	|    /   		    	     \							.
	|---/						  \							.
	|			                   \-----					.
	--------------------------------------------------------------------

	Note that, internally, the breakpoints are kept with cumulative times, whereas the
	line segments only have start/stop Y values and durations (i.e., no absolute time reference).
*/

class ADSR : public Envelope {

public:	
	ADSR() : Envelope() { };
				/// Minimal version - ADSR
	ADSR(LineMode mode, float t, float a, float d, float s, float r);
				/// with initial delay - IADSR
	ADSR(LineMode mode, float t, float i, float a, float d, float s, float r);
				/// Minimal version - ADSR
	ADSR(float t, float a, float d, float s, float r);
				/// with initial delay - IADSR
	ADSR(float t, float i, float a, float d, float s, float r);
	~ADSR() { };
				/// Special accessors
	void setDuration(float d);		///< set duration = scale steady-state part of env
	void setDelay(float del);
	void setAttack(float attack);
	void setDecay(float decay);
	void setSustain(float sustain);
	void setRelease(float release);

	void release(void);		/// trigger the release segment
};

/// AR = 3-segment attack/release envelope class.

/**
	Most of this is inherited from Envelope.
	This design is an extended AR envelope with an initial constant segment and offset
	(default = 0@0; this can be used to make an attack delay as on ARP ARs), attack time and
	max. val (default = 1.0), and release time and final value (default = 0.0).
	All times are assumed to be given in seconds.
	Note that there are many variations on the constructor.
	The line segments used internally look something like the following

	| 
	|     /---------------			
	|    /				   \			
	|---/					\			
	|						 \-----		
	------------------------------------

	Note that, internally, the breakpoints are kept with cumulative times, whereas the
	line segments only have start/stop Y values and durations (i.e., no absolute time reference).
*/

class AR : public Envelope {

public:			/// Various Constructors
	AR() : Envelope() { };
				/// Minimal version - AR
	AR(LineMode mode, float t, float a, float r);
				/// with initial delay - IAR
	AR(LineMode mode, float t, float i, float a, float r);
				/// Minimal version - AR
	AR(float t, float a, float r);
				/// with initial delay - IAR
	AR(float t, float i, float a, float r);

	~AR() { };
				/// Special accessors
	void setDuration(float d);		///< set duration = scale steady-state part of env
	void setDelay(float del);
	void setAttack(float attack);
	void setRelease(float release);
	void setAll(float d, float a, float r);
				/// Operations
	void release(void);			///< trigger the release segment
};

///
/// Triangle envelope class -- equal attack/release times
///

class Triangle : public Envelope {

public:			/// Various Constructors
	Triangle() : Envelope() { };
				/// Simple constructor
	Triangle(LineMode mode, float duration, float amplitude);
				/// Versions with initial delay segments
	Triangle(LineMode mode, float duration,  float initialDelay, float amplitude);
				/// Simple constructor
	Triangle(float duration, float amplitude = 1.0f);
				/// Versions with initial delay segments
	Triangle(float duration,  float initialDelay, float amplitude);
				/// Minimal version - AR
	~Triangle() { };
};

///
/// RandEnvelope envelope class -- makes random control signals using a single line segment
///

class RandEnvelope : public Envelope {
public:										/// defaults are 1 Hz, +- 1.0 range
	RandEnvelope(float frequency = 1, float amplitude = 1, float offset = 0, float step = 0);
	~RandEnvelope() { };
						/// Accessors
	void setWalk(bool walk) { mWalk = walk; };
	void setAmplitude(float amplitude) { mAmplitude = amplitude; };
	void setFrequency(float frequency) { mFrequency = frequency; };
	void setStep(float step) { mStep = step; };
	void setOffset(float offset) { mOffset = offset; };

	virtual bool isActive() { return false; };
					/// These are no-ops in Random envelopes
	void reset() { };				///< reset internal time to restart envelope
	void trigger() { };				///< reset internal time to restart envelope
	void dump() { };				///< print the receiver
	void setDuration(float d) { };	///< set/scale durations
	void scaleTimes(float s) { };	///< scale durations

								/// The main UGen work method
	void nextBuffer(Buffer &outputBuffer, unsigned outBufNum) throw (CException);

protected:
	float mLastVal;				///< last line segment ending value (unscaled and unoffset)
	float mFrequency;			///< frequency for picking new values
	float mAmplitude;			///< scale (+-)
	float mStep;					///< max step between values (+-)
	float mOffset;				///< DC offset
	unsigned mCurrentIndex;		///< current index in line segment
	unsigned mSegmentLength;	///< line segment's length in frames
	bool mWalk;					///< whether to produce random values or a random walk
	LineSegment mSegment;		///< single line segment
	
	void initSegment();			///< set up the line segment
	void nextSegment();			///< choose the values for the next line segment

};

}

#endif
