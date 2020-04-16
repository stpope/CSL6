///
/// Convolver.h -- The way-simplified convolution class.
/// This is an FFT-based convolver that uses the Nayuki FFT directly (no wrapper).
///	It also stores the FFT data in non-interleaved vectors (e.g., mFilterFFTRe & mFilterFFTIm)
///		stephen@heaveneverywhere.com - 1905
//
//		Copyright(C) 2019 Stephen T. Pope. All rights reserved.
//		THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE.
//		The copyright notice above does not evidence any actual or intended
//		publication of such source code.
///

#ifndef INCLUDE_CONVOLUTION_H
#define INCLUDE_CONVOLUTION_H

#include "CSL_Core.h"

namespace csl {

///
/// The Convolver is a CSL Effect
///

class Convolver : public Effect {
public:										/// Constructors
	Convolver(unsigned len = 512);
	Convolver(char * inFName, unsigned len = 512);
//	Convolver(UnitGenerator & in, char * inFName, unsigned len = 512);
	Convolver(UnitGenerator & in, char * inFName, unsigned chan = 0, unsigned len = 512);
	~Convolver();
											///< set the IR file name; runs the analysis ffts
	void setIRFile(char * inFName, unsigned chan = 0);
											/// main nextBuffer call does the fft/ifft
	void nextBuffer(Buffer &outputBuffer) throw (CException);

protected:
	unsigned mFFTSize, mFlSize, mNumBufs, mWindowCount, mChan;

	SampleBufferArray mFilterFFTRe;			///< A ring buffer of IR fft buffers
	SampleBufferArray mFilterFFTIm;
	SampleBufferArray mInputFFTRe;			///< A list of past input spectra
	SampleBufferArray mInputFFTIm;
	SampleBuffer mSpectrumBufferRe;			///< current summation buffer
	SampleBuffer mSpectrumBufferIm;
	SampleBuffer mLastOutput;				///< most-recent output

	void checkBuffers(unsigned newNumBufs);	///< alloc buffers
	void free_buffers();
											/// fast complex MAC using non-interleaved complex arrays
	void complex_multiply_accumulate(SampleBuffer leftRe,  SampleBuffer leftIm,
									 SampleBuffer rightRe, SampleBuffer rightIm,
									 SampleBuffer outRe,   SampleBuffer outIm);
};

}

#endif
