///
/// Convolver2.h -- The simplified convolution class.
/// This is an FFT-based convolver that uses the FFT wrapper.
///			stephen@heaveneverywhere.com - 1905
//		Copyright(C) 2019 Stephen T. Pope. All rights reserved.
//		THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE.
//		The copyright notice above does not evidence any actual or intended
//		publication of such source code.
///

#ifndef INCLUDE_CONVOLUTION2_H
#define INCLUDE_CONVOLUTION2_H

#include "CSL_Core.h"
//#define USE_FFTW							// use FFTW (faster but complicated to build)
#include "FFT_Wrapper.h"

namespace csl {

///
/// The Convolver2 is a CSL Effect
///

class Convolver2 : public Effect {
public:										/// Constructors
	Convolver2(UnitGenerator & in, char * inFName, unsigned chan = 0, unsigned len = 512, bool norm = false);
	Convolver2(UnitGenerator & in, float * irData, unsigned nTaps, unsigned len = 512, bool norm = false);
	~Convolver2();
											///< set the IR file name; runs the analysis ffts
	void setIRFile(char * inFName, unsigned chan = 0);
	void setIRData(float * irData, unsigned nTaps);
											/// main nextBuffer call does the fft/ifft
	void nextBuffer(Buffer &outputBuffer) throw (CException);
protected:
	unsigned mFFTSize, mFlSize, mNumBufs, mNumTaps, mWindowCount, mChan;
	bool mNormOut;							// whether to normalize the output (scale by 1 / fft-len)

	SampleComplexMatrix mInputFFT;			///< A list of past input spectra
	SampleComplexMatrix mFilterFFT;			///< list of IR ffts
	SampleComplexVector mSpectrumBuffer;	///< temp summing complex vector
	SampleBuffer mLastOutput;				///< most-recent output (1/2 window)

	FFT_Wrapper mFFTAnalyzer;				///< FFT analysis/synthesis wrappers
	FFT_Wrapper mFFTSynthesizer;
	Buffer mFFTBuffer;						///< buffer used for FFTs, no storage
	
	void checkBuffers(unsigned newNumBufs);	///< alloc buffers
	void free_buffers();
											/// fast complex MAC using non-interleaved complex arrays
	void complex_multiply_accumulate(SampleComplexVector left, SampleComplexVector right,
									 SampleComplexVector output);
};

}

#endif
