///
///	Filters.h -- CSL filter classes.
///	
///	The base Filter class can perform the generic filter equation based upon a set of feedback 
/// and feedforward coefficients. Subclasses of Filter implement the setupCoeffs method to build 
/// these coefficients according to different algorithms.
///	The subclasses here mostly inherit from the FrequencyAmount controllable, which specifies a 
/// center frequency and an 'amount' which may variously be resonance, radius, bandwidth etc 
/// according to the algorithm.
///	
///	OK so for example Butter class has a CenterFrequency port and a Bandwidth port,
/// but pull_controls must be calling Controllables to do the pullInput()
///	
///	Or, a generic filter that can take a multichannel UGen for each of aCoeffs and bCoeffs (i.e. 
///	multichannel ports), and other filter classes that wrap this and have Frequency and Bandwidth ports etc.
///	This reduces the calls to setupCoeffs, because they'd actually be part of the nextBuffer instead
///	
///	The reason is that the Scalable type approach & macros can't be extended to filter otherwise.
///	ALSO, there are no similar macros for Effect; how should this work?
///	
///	OK, the way he had it working here is filtering in place, i.e. no internal buffer, 
/// but if I inherit from Effect, I do have	an internal buffer; this means the first thing to do is 
/// memcopy the input to the output, then pounce on that; or do the in-place stuff in the Effect port 
/// and finally copy to output, say with scale & offset performed there.
///	
///	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
///

#ifndef CSL_Filters_H
#define CSL_Filters_H

#define FILTER_MAX_COEFFICIENTS (16)			// seems reasonable?

#include "CSL_Core.h"

namespace csl {

#ifdef CSL_ENUMS

typedef enum {
	BW_LOW_PASS = 0,
	BW_HIGH_PASS,
	BW_BAND_PASS,
	BW_BAND_STOP,
	BW_LOW_SHELF,
	BW_HIGH_SHELF,
	PEAKING,
	ALL_PASS
} FilterType;
#else
	#define BW_LOW_PASS 0
	#define BW_HIGH_PASS 1
	#define BW_BAND_PASS 2
	#define BW_BAND_STOP 3
	#define BW_LOW_SHELF 4
	#define BW_HIGH_SHELF 5
	#define PEAKING 6
	#define ALL_PASS 7
	typedef int FilterType;
#endif

/// Declare the pointer to freq/bw buffers (if used) and current scale/offset values

#define DECLARE_FILTER_CONTROLS										\
	Port * freqPort = mInputs[CSL_FILTER_FREQUENCY];				\
	Port * bwPort = mInputs[CSL_FILTER_AMOUNT]

/// Load the freq/bw-related values at the start

#define LOAD_FILTER_CONTROLS										\
	if (freqPort) Controllable::pullInput(freqPort, numFrames);		\
	if (bwPort) Controllable::pullInput(bwPort, numFrames)

/// FrequencyAmount -- mix-in class with frequency and amount (BW) control inputs (may be constants or
/// generators). amount (probably 0..1) is a generalised placeholder for bandwidth, resonance or radius, 
/// according to filter type or could equally be used as a kind of x,y location in the frequency domain

class FrequencyAmount : public virtual Controllable {
public:		
	FrequencyAmount();								///< Constructors
/*	FrequencyAmount(float frequency, float amount = 1.f);		///< also specify bandwidth/radius/resonance etc.
	FrequencyAmount(UnitGenerator & frequency, float amount = 1.f);	
	FrequencyAmount(UnitGenerator & frequency, UnitGenerator & amount);
*/	~FrequencyAmount();								///< Destructor
	
													// accessors
	void setFrequency(UnitGenerator & frequency);	///< set the receiver's frequency to a UGen or a float
	void setFrequency(float frequency);
	float getFrequency();

	void setAmount(UnitGenerator & amount);			///< set the receiver's amount to a UGen or a float
	void setAmount(float amount);
};

///
/// Filter: the canonical-form n-pole/m-zero filter class
///

class Filter : public Effect, public Scalable, public FrequencyAmount {
	
public:
	Filter();
	Filter(unsigned num_b, unsigned num_a = 1);
	Filter(UnitGenerator & in, unsigned num_b = 1, unsigned num_a = 1);
	Filter(UnitGenerator & in, SampleBuffer bCoeffs, SampleBuffer aCoeffs, unsigned num_b, unsigned num_a);
	~Filter();

	void clear(void);								///< clears the input/output buffers
	virtual void setupCoeffs() { };					///< to be overloaded by subclasses
													/// supply the coefficients directly
	void setupCoeffs(SampleBuffer bCoeffs, SampleBuffer aCoeffs, unsigned num_b, unsigned num_a );
	
	virtual void nextBuffer(Buffer & outputBuffer, unsigned outBufNum) throw (CException);
	
	void dump();									///< log information about myself
	
protected:
	void init(unsigned a, unsigned b);				///< shared initialization function

	float mBCoeff[FILTER_MAX_COEFFICIENTS];			///< array of numerator coeffs
	float mACoeff[FILTER_MAX_COEFFICIENTS];			///< array of denominator coeffs
	unsigned mBNum;									///< number of coeffs in b
	unsigned mANum;									///< number of coeffs in a
	Buffer * mPrevInputs;							///< arrays of past input and output samples
	Buffer * mPrevOutputs;
	float mFrame;									///< to keep hold of sample rate for calculating coeffs
};

/// Butterworth IIR (2nd order recursive) filter.
///
/// This operates upon a buffer of frames of amplitude samples by applying the following equation
/// y(n) = a0*x(n) + a1*x(n-1) + a2*x(n-2) - b1*y(n-1) - b2*y(n-2) where x is an amplitude sample.
/// It has constructors that can calculate the coefficients from a given cutoff frequency.
///
/// Note that the bandwidth is ignored for HPF & LPF filters

class Butter : public Filter {
	
public:
									// constructors / destructor
	Butter ();
	Butter (FilterType type, float cutoff);
	Butter (FilterType type, float center, float bandwidth);
	Butter (UnitGenerator & in, FilterType type, float cutoff);
	Butter (UnitGenerator & in, FilterType type, UnitGenerator & cutoff);
	Butter (UnitGenerator & in, FilterType type, float center, float bandwidth);
	Butter (UnitGenerator & in, FilterType type, UnitGenerator & center, UnitGenerator & bandwidth);
									// Filtering
	void setupCoeffs();
	
protected:
	int mFilterType;				// flag as to what kind of filter I am
	
};

/// General-purpose Biquad IIR (2nd order recursive) filter.
/// This is simplified and optimized, but doesn't support dynamic or scalable controls.
/// It uses inst vars rather than arrays for the coefficients.
///	NB: peak gain (dB) is used only for the peak and shelf types)

class Biquad : public Filter {
public:								// constructors / destructor
	Biquad ();
	Biquad (FilterType type, float freq, float Q, float dB = 0.0f);
	Biquad (UnitGenerator & in, FilterType type, float freq, float Q, float dB = 0.0f);
									// this version is optimized
	void nextBuffer(Buffer & outputBuffer, unsigned outBufNum) throw (CException);
									// Filtering
	void setQ(float q);
	void setFrq(float f);
	void incrFrq(float f);
	void setBoost(float d);
	void incrBoost(float d);

protected:
	int mFilterType;				///< flag as to what kind of filter I am
	float Fc, dB, Q;				///< ctr frq (Hz), peak (dB), Q(uality) factor
	float a0, a1, a2, b1, b2;		///< coefficients
	float z1, z2;					///< past outputs
	void setupCoeffs();
};

/// Formant Filter with zeros at +-z and complex conjugate poles at +-omega.
/// setupCoeffs() looks at the member var called normalize;
/// if normalize is true, the filter zeros are placed at z = 1, z = -1, and the coefficients 
/// are then normalized to produce a constant unity peak gain. The resulting filter 
/// frequency response has a resonance at the given frequency. The closer the poles 
/// are to the unit-circle (radius close to one), the narrower the resulting resonance width.

class Formant : public Filter {
	
public:
				/// constructors & destructor
	Formant (UnitGenerator & in, float center_freq, float radius);
	Formant (UnitGenerator & in, UnitGenerator & center_freq, float radius);
	~Formant(void) { };
	
				/// Filtering methods
	void setupCoeffs();
	void setNormalize(bool normalize);

protected:
	bool				mNormalize;
};

/// Notch Filter with poles at +-z and complex conjugate zeros at +-omega.

class Notch : public Filter {
	
public:
				/// constructors & destructor
	Notch (UnitGenerator & in, float center_freq, float radius);
	Notch (UnitGenerator & in, UnitGenerator & center_freq, float radius);
	~Notch(void) { };
				// Filtering
	void setupCoeffs ();

};

/// Allpass Filter with a pole and a zero at equal frequency and straddling the unit circle.
/// Allows all freqs to pass through but messes with phases.
///
/// Note that the Amount parameter of FrequencyAmount is ignored.

class Allpass : public Filter {

public:
				// constructors / destructor
	Allpass (UnitGenerator & in, float coeff);
	Allpass (UnitGenerator & in, UnitGenerator & coeff);
	~Allpass(void) { };
				// Filtering
	void setupCoeffs();
	
protected:
	UnitGenerator *		mCoeffUGen;
	float				mCoeff;
	Buffer				mCoeffBuffer;
};

/// Moog-style resonant VCF class

class Moog : public Filter {

public:
				// constructors / destructor
	Moog (UnitGenerator & in);
	Moog (UnitGenerator & in, UnitGenerator & cutoff);
	Moog (UnitGenerator & in, UnitGenerator & cutoff, UnitGenerator & resonance);
	Moog (UnitGenerator & in, float cutoff);
	Moog (UnitGenerator & in, float cutoff, float resonance);
	~Moog (void) { };
				// Filtering
	void setupCoeffs ();
	
	void nextBuffer(Buffer & outputBuffer, unsigned outBufNum) throw (CException);	

protected:
	float k, p, r; 	// coefficients
	float x, oldx;
	float y1, y2, y3, y4, oldy1, oldy2, oldy3;
	bool debugging;
};

}

#endif
