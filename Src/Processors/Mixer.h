///
/// Mixer.h -- The multi-channel panner and mixer classes.
///	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
///

#ifndef CSL_Mixer_H
#define CSL_Mixer_H

#include "CSL_Core.h"
#include "CPoint.h"

namespace csl {

///
/// Mixer -- The n-input m-channel mixer class
///
/// This is like a binary operator, except that it has an array of inputs and cycles through them for each output buffer.
/// Clients can add and remove inputs at run-time.
///

class Mixer : public UnitGenerator, public Scalable {
public:
	Mixer();					///< Constructors
	Mixer(unsigned chans);
	Mixer(UnitGenerator & mScale);
	Mixer(unsigned chans, UnitGenerator & mScale);
	virtual ~Mixer();
					// Accessing
	UGenVector * getInputs(void) { return(&mSources); };	///< list of inputs, arbitrary # of channels
	unsigned getNumInputs(void);				///< number of active inputs
					/// add/remove inputs
	void addInput(UnitGenerator & inp);
	void addInput(UnitGenerator * inp);
	void addInput(UnitGenerator & inp, float ampl);
	void addInput(UnitGenerator * inp, float ampl);
	void removeInput(UnitGenerator & inp);
	void removeInput(UnitGenerator * inp);
	void deleteInputs();
					/// set the scale of an input
	void scaleInput(UnitGenerator & inp, float val);	

					/// fill the buffer with the next buffer_length of values
	void nextBuffer(Buffer &outputBuffer) throw (CException);
					/// print info about this instance
	void dump();
	unsigned activeSources();
	bool isActive() { return mSources.size() > 0; };	///< mixers with inputs are always active

protected:
	UGenVector mSources;						///< *vector* of inputs, arbitrary # of channels
	FloatVector mScaleValues;					///< scales of inputs
	Buffer mOpBuffer;							///< buffer used for operations, if needed
	void allocateOpBuffer(unsigned chans);		///< allocate the op buffer
	bool hasScales;								///< set to true if any of the scale values != 1.0
};

/// The CSL mono-to-stereo L/R panner class
///
/// This Effect simply takes a monophonic input stream and a dynamic panner value
/// and generates a stereo output stream where the panner can range from +- 1.0.
/// You normally create this with both the input and the pan signal, as in
///		Panner(UnitGenerator * in, UnitGenerator * pan)
/// One can also give it an amplitude scaler or envelope
///

class Panner : public Effect, public Scalable {
public:
						/// Constructors / destructor
	Panner();												///< empty constructor
	Panner(UnitGenerator &input);							///< given an input stream
	Panner(UnitGenerator &input, UnitGenerator &position);	///< given input and position stream
	Panner(UnitGenerator &input, float position);			///< given an input and an amplitude const
	Panner(UnitGenerator &input, UnitGenerator &position, UnitGenerator &amplitude);///< given an amplitude stream
	Panner(UnitGenerator &input, UnitGenerator &position, float amplitude);			///< given an amplitude value
	Panner(UnitGenerator &input, float position, float amplitude);					///< given an amplitude value and pan value
	~Panner();
						/// Operations
	void setPosition(UnitGenerator &pan);						///< set the position to a UGen
	void setPosition(float pan);								///< set the position to a float
	float position();											///< get the position to a float

	virtual unsigned numChannels() const { return 2; };			///< I'm stereo!

	virtual void nextBuffer(Buffer &outputBuffer) throw (CException);
};

///
/// N-channel (max 2 at present) input to M-channel output azimuth panner
///

#define MAX_OUTPUTS 16

class NtoMPanner : public Panner {

protected:
	unsigned mInCh, mOutCh;		// the # of input and output channels
	CPoint ** mSpeakers;			// the speaker array
	float mSpread;				// angle between the channels in stereo inputs
	Buffer mOpBuffer;			// a temporary operation Buffer
	void initSpeakers(void);

public:						// Constructors / destructor
							// Args are: i: input, p: pan, a: ampl, s: spread
	NtoMPanner() : Panner() { };
	NtoMPanner(UnitGenerator & i, float a, unsigned in_c, unsigned out_c);
	NtoMPanner(UnitGenerator & i, UnitGenerator & pX, UnitGenerator & pY, UnitGenerator & a, unsigned in_c, unsigned out_c);
	NtoMPanner(UnitGenerator & i, UnitGenerator & pX, UnitGenerator & pY, UnitGenerator & a, unsigned in_c, unsigned out_c, float spr);
	NtoMPanner(UnitGenerator & i, UnitGenerator & pX, UnitGenerator & pY, float a, unsigned in_c, unsigned out_c, float spr);
	~NtoMPanner();
							// Setup speaker arrays
	void init_stereo(float dist);
	void init_quad(float dist);
	void init_5point1(float dist);
	void init_6ch(float x, float y);
							// Operations -- these are only relevant if the positions are static variables
	void setX(float x);
	void setY(float y);
							// do it!
	virtual void nextBuffer(Buffer &outputBuffer) throw (CException);
};

///
/// Stereo width processor -- can mix stereo channels or subtract the sum from each to widen
///

class StereoWidth : public Effect {

public:						// Constructors / destructor
	StereoWidth ();
	~StereoWidth();
							// Operations
	void setWidth(float width) { mWidth = width; }
	void setPan(float pan) { mPan = pan; }
	void setGain(float gain) { mGain = gain; }

	void nextBuffer(Buffer & inputBuffer) throw (CException);

protected:
	float mWidth;			// stereo width range: -1->1. -1 = mix to mono, 0 = no change 1 = widen
	float mGain;			// amplitude scaler 0->10, 1 -- no scaling
	float mPan;				// pan position 0->1 0.5 -- no panning
};

}

#endif
