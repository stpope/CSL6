//
// Reverb.h -- The CSL port of the public domain Freeverb reverberator
// 	Freeverb was written by Jezar at Dreampoint, June 2000 -- http://www.dreampoint.co.uk
//	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
// 

#ifndef CSL_Reverb_H
#define CSL_Reverb_H

#include "CSL_Core.h"

#define undenormalise(samplev) if (((*(unsigned int*)&samplev)&0x7f800000) == 0) samplev = 0.0f

namespace csl {

class Comb;			// predeclaration of utility classes
class FAllpass;

///
/// CSL port of the public domain Freeverb reverberator
///

class Freeverb : public Effect, public Scalable {

public:
	Freeverb(UnitGenerator &input);
	~Freeverb();

	float roomSize();
	void setRoomSize(float size);	///< Setting the room size makes longer tails. The value has a range from 0 to 1.
	float dampening();	
	void setDampening(float damp);	///< Specified in percentage (from 0 to 100%).
	float wetLevel();
	void setWetLevel(float level);	///< Amount of wet (reverberation) in the mixed output.
	float dryLevel();
	void setDryLevel(float level);	///< Amount of the original "dry" signal in the output.
	float width();
	void setWidth(float width);		///< Currently not used, as this reverb became mono in/out.
	
	void nextBuffer(Buffer &outputBuffer, unsigned outBufNum) throw (CException);

protected:			// accessable parameters
	float mRoomSize;
	float mDampening;	
	float mWetLevel;
	float mDryLevel;
	float mWidth;
					// internal parameters	
	float mGain;

	std::vector <Comb *> mCombFilters;
	std::vector <FAllpass *> mAllpassFilters;

	SampleBufferVector mCombBuffers;
	SampleBufferVector mAllpassBuffers;	

	void constructReverbGraph();
	void updateParameters();
};

///
/// Comb filter class
///

class Comb {
public:
	Comb() : mFilterStore(0), mBufIdx(0) { };

	void	mute();
	float	damp() { return mDamp1; }
	float	feedback() { return mFeedback; }
	void	setDamp(float val);
	void	setFeedback(float val) { mFeedback = val; }
	void	setBuffer(float *buf, int size);
	
	inline  float process(float inp);	

private:
	float	mFeedback;
	float	mFilterStore;
	float	mDamp1;
	float	mDamp2;
	float	*mBufferPtr;
	int		mBufSize;
	int		mBufIdx;
};

///
/// All-pass filter class
///

class FAllpass {
public:
	FAllpass() : mBufIdx(0) { };

	void	mute();
	float	feedback() { return mFeedback; };
	void	setFeedback(float val) { mFeedback = val; };
	void	setBuffer(float *buf, int size);
	
	inline  float process(float inp);

private:
	float	mFeedback;
	float	*mBufferPtr;
	int		mBufSize;
	int		mBufIdx;
};

// Big to inline - but crucial for speed

inline float Comb::process(float input) {
	float output = mBufferPtr[mBufIdx];
	undenormalise(output);
	mFilterStore = (output * mDamp2) + (mFilterStore * mDamp1);
	undenormalise(mFilterStore);
	mBufferPtr[mBufIdx] = input + (mFilterStore * mFeedback);
	if (++mBufIdx >= mBufSize) 
		mBufIdx = 0;
	return output;
}

inline float FAllpass::process(float input) {
	float bufout = mBufferPtr[mBufIdx];
	undenormalise(bufout);
	float output = -input + bufout;
	mBufferPtr[mBufIdx] = input + (bufout * mFeedback);
	if (++mBufIdx >= mBufSize)
		mBufIdx = 0;
	return output;
}

///
// Stereoverb is a simple wrapper around 2 freeverbs with splitter/joiners 
// for handling stereo inputs - note the overrides of isActive() and numchannels()
///

class Stereoverb : public Effect {

public:
	Stereoverb(UnitGenerator & input);
	~Stereoverb();

	void setRoomSize(float size);
	void setDampening(float damp);
	void setWetLevel(float level);
	void setDryLevel(float level);
	void setWidth(float width);
	bool isActive();
	unsigned numChannels() { return 2; };		///< I'm stereo

	void nextBuffer(Buffer &outputBuffer) throw (CException);

protected:
	Freeverb * leftRev, * rightRev; // 2 mono reverberators
	Splitter * split;				// stereo-to-mono splitter
	Joiner * join;					// mono-to-stereo joiner
};

}

#endif
