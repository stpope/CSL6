///
/// Spectral.h -- UnitGenerator for going to/from the spectral domain
/// These classes use the CSL 5 FFT wrapper for FFTs.
///
/// See the copyright notice and acknowledgment of authors in the file COPYRIGHT
///

#ifndef CSL_Spectral_H
#define CSL_Spectral_H

#include "CSL_Core.h"
#include "Window.h"

//#define USE_FFTW              // you might want to hard-wire this...
#include "FFT_Wrapper.h"

namespace csl {

///
/// Forward FFT unit generator is an Effect because it handles an input
/// It puts spectral frames in the output buffer and then broadcasts a change message, so clients are expected to observe it.
///

class FFT : public Effect {

public:
										/// Default size to the buffer size and flags to measure
	FFT(UnitGenerator & in, int size = CGestalt::blockSize(), CSL_FFTType type = CSL_FFT_COMPLEX);
	~FFT();
										/// we override the general-case version because this needs a mono input
	void nextBuffer(Buffer & outputBuffer) throw (CException);	

	int fftSize() { return mFFTSize; }	/// no setter -- create a new FFT to change size

	bool mOverwriteOutput;				///< whether to replace the output with the input (or the spectrum) after signalling observers

protected:	
	int mFFTSize;						///< This should be unsigned, but is signed for compatability with FFTW
	FFT_Wrapper mWrapper;				///< actual FFT wrapper object
	Buffer mInBuf;						///< input buffer
	SampleBuffer mWindowBuffer;			///< Buffer to store window
    csl::HammingWindow * mWindow;       ///< the window itself
};

///
/// Inverse FFT is a generator
///

class IFFT : public UnitGenerator {

public:
										/// Default size to the buffer size and flags to measure
	IFFT(int size = CGestalt::blockSize(), CSL_FFTType type = CSL_FFT_COMPLEX);
	~IFFT();
										/// no setter -- create a new IFFT to change size
	int fftSize() { return mFFTSize; }
										/// getter methods
	void binValue(int binNumber, float * outRealPart, float * outComplexPart);
	void binValueMagPhase(int binNumber, float * outMag, float * outPhase);
		
										// set the values in the specified bin
	void setBin(int binNumber, float realPart, float imagPart);
	void setBins(float * real, float * imag);
	void setBins(SampleComplexVector cmplxSpectrum);
	void setBins(SampleBuffer cmplxSpectrum);
	void setBins(int lower, int upper, float* real, float* imag);
	void setBinMagPhase(int binNumber, float mag, float phase);
	void setBinsMagPhase(float* mags, float* phases);
	
	void nextBuffer(Buffer & outputBuffer) throw (CException);

protected:	
	int mFFTSize;						///< This should be unsigned, but is signed for compatability with FFTW
	FFT_Wrapper mWrapper;				///< actual FFT wrapper object
	Buffer mInBuf;						///< input buffer
	SampleComplexPtr mSpectrum;			///< spectral data I accumulate
};

}

#endif
