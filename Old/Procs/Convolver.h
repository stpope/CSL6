///
/// Convolver.h -- The low-latency convolution classes.
///	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
/// 
/// This is the canonical stored IR FFT-based convolver.
/// On start-up it stores the FFT spectra of the windows of the impulse response.
/// At run-=time it stores the current frame's input FFT and sums up the former inputs 
/// with the IR windows in reverse order, creating the summed tail of the IR over past inputs.
///

#ifndef INCLUDE_CONVOLUTION_H
#define INCLUDE_CONVOLUTION_H

#include "CSL_Core.h"
#include "Spectral.h"			// fftw includes and types

/// Pointer checking

#define CHECK_PTR(ptr)								\
	if (!ptr)	 throw MemoryError("Can't allocate buffer")

#define initvec( name, size, type )					\
	if ((name = (type *) calloc(size, sizeof(type))) == NULL)  \
		throw MemoryError("Can't allocate buffer");

#define cmac(in1, in2, out)							\
	out[0] += in1[0] * in2[0] - in1[1] * in2[1];	\
	out[1] += in1[0] * in2[1] + in1[1] * in2[0];

#define cbinc(buf, size)	if (++buf > size-1)		buf = 0;
#define cbdec(buf, size)	if (--buf < 0)			buf = size - 1;

#define cbarb(buf, size, amt)						\
	buf += amt;										\
	if (buf >= size) buf -= size;					\
	else if (buf < 0) buf += size;

namespace csl {

///
/// The CSL convolver class is an Effect
///

class Convolver : public Effect {

public:										/// Constructors
	Convolver() : Effect() { };
	Convolver(char *IRfilename);			///< give an IR file name or buffer and/or FFT len
	Convolver(unsigned len, Buffer & impulseResp);
	Convolver(unsigned len, char * IRfilename);
	Convolver(Buffer & inbuf);
	~Convolver();
															/// init/setup functions
	void initialize(Buffer & buf) throw (CException);		///< this takes the fft of the IR buffer
	void setFilters(fftwf_complex ** filterFFTs);			///< set the IR spectrum array
	void setInputf(fftwf_complex * inFFT);					///< pass in the input buffer
	CSL_FFTW_sample * mSampleBuffer;						///< public I/O buffer ptr

										/// main nextBuffer call sums past FFT'ed IRs and inputs
	void nextBuffer(Buffer &outputBuffer, unsigned outBufNum) throw (CException);

protected:	
	CSL_FFTW_cmplx ** mFilterFFTs;							///< A ring buffer of IR fft buffers
	CSL_FFTW_cmplx ** mInputFFTs;							///< A list of past input spectra
	CSL_FFTW_cmplx * mSpectrumBuffer;						///< current summation buffer
	CSL_FFTW_plan mForwardPlan, mInversePlan;				///< FFT plans
	unsigned mFFTSize, mWindowCount, mNumBufs;				///< window/buffer sizes
	bool mMyBuffers;										///< whether I allocated the filter buffers
	bool mMyInput;											///< whether to use pullInput
	
	void initialize (const char *filename) throw (CException);	///< this reads in the file
	
															/// fast complex ptr mult-accum utility
	void complex_multiply_accumulate(fftwf_complex* left, fftwf_complex* right, fftwf_complex * output);

};

}

#endif
