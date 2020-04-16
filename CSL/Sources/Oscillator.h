//
//  Oscillator.h -- specification of the base oscillator class and a few simple waveform generators
//
//	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
//
// What's here:
// 	Oscillator -- Abstract oscillator class
// 	WavetableOscillator -- Oscillator with a stored wave table (default wave table is an 8192-sample  sine)
// 	CompOrCacheOscillator -- Abstract oscillator class for those who can compute of cache their wavetables
// 	Sine -- oscillator class (computes the sine fcn on the fly)
// 	FSine -- (uses a ringing filter for the sine calc)
// 	Sawtooth -- Sawtooth oscillator class (non-band-limited)
// 	Square -- Square oscillator class
// 	SumOfSines -- Sum-of-sines oscillator class

#ifndef _Oscillator_H
#define _Oscillator_H

#include "CSL_Core.h"
#include <stdarg.h>		// for varargs

#define DEFAULT_WAVETABLE_SIZE CSL_mMaxBufferFrames		// use large wave tables by default

namespace csl {

///
/// Oscillator -- Abstract oscillator class; inherits from UnitGenerator, Phased, and Scalable
/// and provides convenience constructors (freq, ampl, offset, phase)
///

class Oscillator : public UnitGenerator, public Phased, public Scalable {
public:								/// Constructor: parameters are optional.
	Oscillator(float frequency = 220.0, float ampl = 1.0, float offset = 0.0, float phase = 0.0);
	Oscillator(UnitGenerator & frequency, float ampl = 1.0, float offset = 0.0, float phase = 0.0);
	Oscillator(UnitGenerator & frequency, UnitGenerator & ampl, float offset = 0.0, float phase = 0.0);
	virtual ~Oscillator();				///< Destructor
	
	void dump();						///< print the receiver for debugging
};

///
/// Enumeration for interpolation policies
///

#ifdef CSL_ENUMS
typedef enum {
	kTruncate,
	kLinear,
	kCubic,
	kAllPass
} InterpolationPolicy;
#else
	#define kTruncate 1
	#define kLinear 2
	#define kCubic 3
	#define kAllPass 4
	typedef int  InterpolationPolicy;
#endif

///
/// WavetableOscillator -- Oscillator with a stored wave table that does table look-up.
/// The default wave table is an 8192-sample sine.
/// (perhaps accept a vector of freqs and a multichannel buffer?)
///

class WavetableOscillator : public Oscillator {
public:
	WavetableOscillator(Buffer & wave);
#ifndef SWIG_ONLY						// SWIG can't handle the initializers
	WavetableOscillator(float frequency = 1, float ampl = 1.0, float offset = 0.0, float phase = 0.0);
#else
	WavetableOscillator(Buffer & wave, float frequency = 220.0f);
	WavetableOscillator(Buffer & wave, float frequency = 220.0f, float phase = 0.0f);
	WavetableOscillator(float frequency = 1, float ampl = 1.0f, float offset = 0.0f, float phase = 0.0f);
#endif
	~WavetableOscillator();				///< Destructor
	void setWaveform(Buffer & wave, bool freeBufs = true);	///< plug in waveforms
										/// set the interpolation flag
	void setInterpolate(InterpolationPolicy whether) { mInterpolate = whether; };
										// get the next buffer of samples
	virtual void nextBuffer(Buffer & outputBuffer, unsigned outBufNum) throw (CException);

	InterpolationPolicy mInterpolate;	///< whether/how I should interpolate between samples
	Buffer mWavetable;					///< the stored wave form

protected:
	void fillSine();					///< fill the shared wavetable with 1 cycle of a sine wave
};

///
/// CompOrCacheOscillator -- Abstract oscillator class for those who can compute of cache their wavetables
///

class CompOrCacheOscillator : public WavetableOscillator, public Cacheable {
public:
	CompOrCacheOscillator(bool whether = false, float frequency = 220, float phase = 0.0);
	void createCache();

protected:
	virtual void nextBuffer(Buffer & outputBuffer, unsigned outBufNum) throw (CException);
	virtual void nextWaveInto(SampleBuffer dest, unsigned count, bool oneHz) = 0;
};

///
/// Sine -- oscillator class (this computes the sine fcn on the fly)
///

class Sine : public Oscillator {	
public:
	Sine(float frequency = 220, float ampl = 1.0, float offset = 0.0, float phase = 0.0);
	void nextBuffer(Buffer & outputBuffer, unsigned outBufNum) throw (CException);
};

/// 	FSine -- (uses a ringing filter for the sine calc)

class FSine : public Oscillator {	
public:
	FSine(float frequency = 220, float ampl = 1.0, float offset = 0.0, float phase = 0.0);
	void nextBuffer(Buffer & outputBuffer, unsigned outBufNum) throw (CException);
};

///
/// Sawtooth oscillator class (non-band-limited)
///

class Sawtooth : public Oscillator {
public:
	Sawtooth(float frequency = 220, float ampl = 1.0, float offset = 0.0, float phase = 0.0);
	void nextBuffer(Buffer & outputBuffer, unsigned outBufNum) throw (CException);
};

///
/// Square oscillator class (non-band-limited)
///

class Square : public Oscillator {
public:
	Square(float frequency = 220, float ampl = 1.0, float offset = 0.0, float phase = 0.0);
	void nextBuffer(Buffer & outputBuffer, unsigned outBufNum) throw (CException);
};

///
/// Impulse -- oscillator class (this create a single impulse delayed by 'delay' samples)
///

class Impulse : public Oscillator {
public:
	Impulse();
	Impulse(float delay);
	Impulse(float frequency, float ampl);
	Impulse(float frequency, float ampl, float offset);
	Impulse(float frequency, float ampl, float offset, float phase);
	
	void nextBuffer(Buffer & outputBuffer, unsigned outBufNum) throw (CException);

protected:
	int mCounter;
};

///
/// Struct for partial overtones
///

typedef struct {				// Harmonic partial data structure used here and by the SHARC classes
	float number;			// partial number (need not be an integer)
	float amplitude;			// partial amplitude (0.0 - 1.0)
	float phase;				// Partial phase in radians
} Partial;

///
/// Enum for SumOfSines description formats
///

#ifdef CSL_ENUMS
typedef enum {
	kFrequency,
	kFreqAmp,
	kFreqAmpPhase
} PartialDescriptionMode;
#else
	#define kFrequency 1
	#define kFreqAmp 2
	#define kFreqAmpPhase 3
	typedef int PartialDescriptionMode;
#endif

///
/// SumOfSines -- Sum-of-sines oscillator class
///
/// The constructor takes an int format (1, 2, or 3 elements per overtone), the number of partials, and a list of elements
///
/// This can cache a wavetable (if you use a strictly harmonic overtone series) or compute a buffer on the fly by summation (slow)
/// 
/// Examples:
///	Format kFrequency = list of values for overtone amplitudes with num = 1, 2, 3..., amps = values, phases = 0
///		SumOfSines vox(kFrequency, 5,  0.2, 0.2, 0.1, 0.1, 0.05)						-- first 5 overtones with weights 0.2, 0.2...
///
///	Format kFreqAmp = list of values for overtone freq/amplitudes with phases = 0
///		SumOfSines vox(kFreqAmp, 3,  1, 0.2,   2, 0.1,   3.01, 0.05)					-- overtones 1, 2, and 3.01 (i.e., non-harmonic)
///
///	Format kFreqAmpPhase = list of values for overtone num/amps/phases
///		SumOfSines vox(kFreqAmpPhase, 3  1.02, 0.2, 0,   2.1, 0.2, 0.1,   3.0, 0.1, 0.05)		-- verbose constructor
///

class SHARCSpectrum;

class SumOfSines : public CompOrCacheOscillator {
public:
	SumOfSines();							/// Constructors
	SumOfSines(float frequency);					///< empty constructor
	SumOfSines(unsigned numHarms, float noise);		///< 1/f spectrum + noise
	SumOfSines(float frequency, unsigned numHarms, float noise);
													/// given a var-args partials list
	SumOfSines(PartialDescriptionMode format, unsigned partialCount, ...);
	SumOfSines(SHARCSpectrum & spect);				/// given a SHARC spectrum
	
	void addPartial(Partial * pt);
	void addPartials(unsigned num_p, Partial ** pt);
	void addPartials(int argc, void ** argv);
	void addPartial(float nu, float amp);
	void addPartial(float nu, float amp, float phase);
	void clearPartials();

	void dump();						///< print the receiver for debugging
	
protected:
	std::vector<Partial *> mPartials;
	void nextWaveInto(SampleBuffer dest, unsigned count, bool oneHz);

private:
	Buffer outputBuffer;			// kludj so we can use the inherited macros
};

}

#endif
