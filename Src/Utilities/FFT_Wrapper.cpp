//
//  FFT_Wrapper.cpp -- wrapper class for FFTs that hides implementation details
//		This file includes the 3 standard concrete subclasses: 
//			FFTW (assumes fftw3f), 
//			RealFFT (RealFFT code included in CSL), and 
//			KISS FFT (included, untested).
//
//	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
//

#include "FFT_Wrapper.h"
#include "math.h"
#include <string.h>		// for memcpy

using namespace csl;

//-------------------------------------------------------------------------------------------------//

#pragma mark FFTW

#ifdef USE_FFTW			// the FFTW-based version

// FFTW_Wrapper = FFTW-based concrete implementation

FFTW_Wrapper::FFTW_Wrapper(unsigned size, CSL_FFTType type, CSL_FFTDir forward) 
		: Abst_FFT_W(size, type, forward) {
											// allocate buffers
	mSampBuf = (SampleBuffer) fftwf_malloc(size * sizeof(sample));
	mSpectBuf = (fftwf_complex *) fftwf_malloc(mCSize * sizeof(fftwf_complex));

	if (mDirection == CSL_FFT_FORWARD) {	// create FFT plans
								// (int n, float *in, fftw_complex *out, unsigned flags);
		mPlan = fftwf_plan_dft_r2c_1d(size, mSampBuf, mSpectBuf, FFTWF_FLAGS);
	} else {								// inverse FFT
								// int n, fftw_complex *in, float *out, unsigned flags);
		mPlan = fftwf_plan_dft_c2r_1d(size, mSpectBuf, mSampBuf, FFTWF_FLAGS);
	}
//	logMsg("FFTW %d", size);	
}

FFTW_Wrapper::~FFTW_Wrapper() {
	fftwf_destroy_plan(mPlan);
	fftwf_free(mSampBuf);	
	fftwf_free(mSpectBuf);	
}

/// run the transform

void FFTW_Wrapper::nextBuffer(Buffer & in, Buffer & out) throw (CException) {	

	if (mDirection == CSL_FFT_FORWARD) {			// mDirection == CSL_FFT_FORWARD
													// copy input into sample buffer
		memcpy(mSampBuf, in.buffer(0), mSize * sizeof(sample));
//		printf("\t%x  -  %x\n", in.buffer(0), out.buffer(0));

		fftwf_execute(mPlan);						//// GO ////

		SampleBuffer ioPtr = out.buffer(0);			// out buffer
		fftwf_complex * spPtr = mSpectBuf;			// spectrum ptr

		if (mType == CSL_FFT_REAL) {				// real: write magnitude to mono output
			for (unsigned j = 0; j < mCSize; j++, spPtr++)
				*ioPtr++ = hypotf((*spPtr)[0], (*spPtr)[0]);

		} else if (mType == CSL_FFT_COMPLEX) {		// copy complex ptr to output
			memcpy(out.buffer(0), mSpectBuf + 1, (mSize * sizeof(sample)));
//			memcpy(out.buffer(0), mSpectBuf, (mCSize * sizeof(fftwf_complex)));

		} else if (mType == CSL_FFT_MAGPHASE) {		// write mag/phase to buffer[0]/[1]
			SampleBuffer inOutPh = out.buffer(1);
			for (unsigned j = 0; j < in.mNumFrames; j++) {
		//		fprintf(stderr, "re:%f cx:%f ", (*spPtr)[0], (*spPtr)[1]);
				*ioPtr++ = hypotf((*spPtr)[0], (*spPtr)[1]);		// write magnitude
				spPtr++;
				if ((*spPtr)[0] == 0.0f) {
					if ((*spPtr)[1] >= 0.0f) 
						*inOutPh++ = CSL_PIHALF;
					else
						*inOutPh++ = CSL_PI + CSL_PIHALF;
				} else {
					*inOutPh++ = atan((*spPtr)[1] / (*spPtr)[0]);	// write phase
				}
			}
		}
	} else {										// mDirection == CSL_FFT_INVERSE
													// copy data into spectrum
		memcpy(mSpectBuf, in.buffer(0), (mCSize * sizeof(fftwf_complex)));

		fftwf_execute(mPlan);						// GO
													// copy real output
		memcpy(out.buffer(0), mSampBuf, (mSize * sizeof(sample)));
	}
}

#endif // FFTW

//-------------------------------------------------------------------------------------------------//

#pragma mark FFTREAL

#ifdef USE_FFTREAL		// the FFTReal-based version

// FFTR_Wrapper = FFTReal-based concrete implementation

FFTR_Wrapper::FFTR_Wrapper(unsigned size, CSL_FFTType type, CSL_FFTDir forward) 
		: Abst_FFT_W(size, type, forward), mFFT(size) {
	SAFE_MALLOC(mTempBuf, sample, size + 1);
//	logMsg("FFTReal %d", size);	
}

FFTR_Wrapper::~FFTR_Wrapper() {
	SAFE_FREE(mTempBuf);
}

// execute = run the transform

void FFTR_Wrapper::nextBuffer(Buffer & in, Buffer & out) throw (CException) {
	if (mDirection == CSL_FFT_FORWARD) {			// mDirection == CSL_FFT_FORWARD
		SampleBuffer ioPtr = in.buffer(0);		// set input data ptr 
//		printf("\t%x  -  %x\n", in.buffer(0), out.buffer(0));

		mFFT.do_fft(mTempBuf, ioPtr);				// perform FFT

//		do_fft (flt_t f [], const flt_t x []) -- this is the fcn signature
//			- x: pointer on the source array (time).
//			- f: pointer on the destination array (frequencies). 
//				 f [0...length(x)/2] = real values, 
//				 f [length(x)/2+1...length(x)-1] = imaginary values of coeff 1...length(x)/2-1. 

		if (mType == CSL_FFT_COMPLEX) {				// raw: copy complex points to output
			SampleComplexPtr cxPtr = (SampleComplexPtr) out.buffer(0);
			SampleBuffer rPtr = mTempBuf;
			SampleBuffer iPtr = mTempBuf + (mSize / 2) ; // + 1;
			float normFactor = 1.0 / sqrt((double) mSize);
			for (unsigned j = 0; j < mSize / 2; j++) {
				SampleBuffer cplx = cxPtr[j];
				cx_r(cplx) = *rPtr++ * normFactor;
				cx_i(cplx) = *iPtr++ * normFactor;
			}
		} else if (mType == CSL_FFT_REAL) {			// real: write magnitude to mono output
			ioPtr = out.buffer(0);					// output pointer
			SampleBuffer rPtr = mTempBuf;
			SampleBuffer iPtr = mTempBuf + (mSize / 2) ; // + 1;
			for (unsigned j = 0; j < mSize / 2; j++)
				*ioPtr++ = hypotf(*rPtr++, *iPtr++);
		} else if (mType == CSL_FFT_MAGPHASE) {		// complex: write mag/phase to buffer[0]/[1]
			ioPtr = out.buffer(0);					// output pointer
			SampleBuffer rPtr = mTempBuf;
			SampleBuffer iPtr = mTempBuf + (mSize / 2);
			SampleBuffer inOutPh = out.buffer(1);
			for (unsigned j = 0; j < mSize / 2; j++) {
				*ioPtr++ = hypotf(*rPtr, *iPtr);
				if (*rPtr == 0.0f) {
					if (*iPtr >= 0.0f) 
						*inOutPh++ = CSL_PIHALF;
					else
						*inOutPh++ = CSL_PI + CSL_PIHALF;
				} else {
					*inOutPh++ = atan(*iPtr / *rPtr);
				}
				rPtr++;
				iPtr++;
			} // end of loop
		}
	} else {										// mDirection == CSL_FFT_INVERSE
		if (mType == CSL_FFT_COMPLEX) {				// CSL_FFT_COMPLEX format
													// copy complex spectrum to un-packed IFFT format
			SampleComplexPtr ioPtr = (SampleComplexPtr) in.buffer(0);
			SampleBuffer rPtr = mTempBuf;			// copy data to FFTReal format
			SampleBuffer iPtr = mTempBuf + (mSize / 2);

			for (unsigned j = 0; j < mSize / 2; j++) {	// loop to unpack complex array
				SampleBuffer cplx = ioPtr[j];
				*rPtr++ = cx_r(cplx);
				*iPtr++ = cx_i(cplx);
			}
		} else if (mType == CSL_FFT_REAL) {			// real: copy real spectrum to un-packed IFFT format
			SampleBuffer ioPtr = in.buffer(0);
			SampleBuffer rPtr = mTempBuf;			// copy data to FFTReal format
			SampleBuffer iPtr = mTempBuf + (mSize / 2);

			for (unsigned j = 0; j < mSize / 2; j++) {	// loop to unpack real array
				*rPtr++ = *ioPtr++;
				*iPtr++ = 0.0f;
			}
		}
		SampleBuffer oPtr = out.buffer(0);			// output pointer

		mFFT.do_ifft(mTempBuf, oPtr);

//		 do_ifft (const flt_t f [], flt_t x [])
//			- f: pointer on the source array (frequencies).
//				 f [0...length(x)/2] = real values,
//				 f [length(x)/2+1...length(x)] = imaginary values of coeff 1...length(x)-1.
//			- x: pointer on the destination array (time).
	}
}

#endif

//-------------------------------------------------------------------------------------------------//

#pragma mark KISSFFT

#ifdef USE_KISSFFT

// KISSFFT_Wrapper = KISS FFT-based concrete implementation

KISSFFT_Wrapper::KISSFFT_Wrapper(unsigned size, CSL_FFTType type, CSL_FFTDir forward) 
		: Abst_FFT_W(size, type, forward) {
	int dir = (forward == CSL_FFT_FORWARD) ? 0 : 1;
	mFFT = kiss_fft_alloc(size, dir, NULL, NULL);
	SAFE_MALLOC(inBuf, SampleComplex, size);
	SAFE_MALLOC(outBuf, SampleComplex, size);
}

KISSFFT_Wrapper::~KISSFFT_Wrapper() {
	free(mFFT);
	kiss_fft_cleanup();
}

// run the transform between in and out

void KISSFFT_Wrapper::nextBuffer(Buffer & in, Buffer & out) throw (CException) {

	if (mDirection == CSL_FFT_FORWARD) {			// mDirection == CSL_FFT_FORWARD
		SampleBuffer ioPtr = in.buffer(0);		// input data ptr 
		SampleComplexPtr cxPtr = inBuf;
		for (int j = 0; j < mSize; j++) {			// loop to pack complex array
			*cxPtr[0] = *ioPtr++;
			cxPtr++;
		}

//		 kiss_fft(kiss_fft_cfg cfg, const kiss_fft_cpx *fin, kiss_fft_cpx *fout)
//		 Perform an FFT on a complex input buffer.
//		 for a forward FFT,
//				fin should be  f[0] , f[1] , ... ,f[nfft-1]
//				fout will be   F[0] , F[1] , ... ,F[nfft-1]
//		 Note that each element is complex and can be accessed like f[k].r and f[k].i

		kiss_fft(mFFT, (const kiss_fft_cpx *) inBuf, (kiss_fft_cpx *) outBuf);

		ioPtr = out.buffer(0);					// output pointer
		if (mType == CSL_FFT_REAL) {				// real: write magnitude to mono output
			SampleBuffer rPtr = mTempBuf;
			SampleBuffer iPtr = mTempBuf + (mSize / 2);
			*ioPtr++ = *rPtr++;
			*ioPtr++;
			for (int j = 1; j < mSize; j++)
				*ioPtr++ = hypotf(*rPtr++, *iPtr++);
		} else if (mType == CSL_FFT_COMPLEX) {		// raw: copy complex points to output
			memcpy(ioPtr, outBuf, mSize * sizeof(SampleComplex));
		}
	} else {										// mDirection == CSL_FFT_INVERSE
		
		kiss_fft(mFFT, (const kiss_fft_cpx *) in.buffer(0), (kiss_fft_cpx *) outBuf);

		SampleComplexPtr cxPtr = outBuf;
		SampleBuffer ioPtr = out.buffer(0);
		for (int j = 0; j < mSize; j++)				// loop to unpack complex array
			*ioPtr++ = cxPtr[j][0];
	}
}

#endif
