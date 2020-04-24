//
// Spectral.cpp -- UnitGenerator for going to/from the spectral domain using FFTW
//	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
//

#include "Spectral.h"
#include <stdlib.h>
#include <math.h>
#include <string.h>

using namespace csl;

/// Forward FFT = analysis

// Constructor - Default size to the buffer size and flags to measure
// mBlockResizer(size) needed if different size than the default

FFT::FFT(UnitGenerator & in, int size, CSL_FFTType type)
			: Effect(in), mFFTSize(size), 
			  mWrapper(size, type, CSL_FFT_FORWARD), mInBuf(1, size), mWindowBuffer(0) {  
//	mSampleBuffer = (SampleBuffer) fftwf_malloc(sizeof(SampleBuffer) * mFFTSize);
	mWindow = new HammingWindow(mFFTSize);  ///< Window to scale input
	mWindowBuffer = mWindow->window();	///< ptr to window buffer
	mOverwriteOutput = false;			// leave the spectrum in the buffer by default
	mInBuf.allocateBuffers();
}

FFT::~FFT() {
    free(mWindow);
    free(mWindowBuffer);
}

// nextBuffer does the FFT -- note that we override the higher-level version of this method

void FFT::nextBuffer(Buffer& outputBuffer) throw (CException) {
	unsigned numFrames = outputBuffer.mNumFrames;	// get buffer length

	pullInput(numFrames);							// get the input samples via Effect
													// Copy the input data into the buffer 
	memcpy(mInBuf.buffer(0), mInputPtr, numFrames * sizeof(sample));

	SampleBuffer bufPtr = mInBuf.buffer(0);			// apply signal window to buffer
	SampleBuffer winPtr = mWindowBuffer;	
	for (int i = 0; i < mFFTSize; i++)
		*bufPtr++ *= *winPtr++;

	mWrapper.nextBuffer(mInBuf, outputBuffer);		// execute the FFT

	outputBuffer.mType = kSpectra;					// set the type flag of the output buffer
	this->changed((void *) & outputBuffer);			// signal dependents (if any) of my change
	return;
}

//
//// InvFFT = synthesis
//

IFFT::IFFT(int size, CSL_FFTType type) 
			: UnitGenerator(), mFFTSize(size),
			  mWrapper(size, type, CSL_FFT_INVERSE) {
	SAFE_MALLOC(mSpectrum, SampleComplex, mFFTSize);
}

IFFT::~IFFT() { 
	SAFE_FREE(mSpectrum);
}

void IFFT::binValue(int binNumber, float * outRealPart, float * outImagPart) {
	*outRealPart = cx_r(mSpectrum[binNumber]);
	*outImagPart = cx_r(mSpectrum[binNumber]);	
}

void IFFT::binValueMagPhase(int binNumber, float * outMag, float * outPhase) {
	float myReal, myComplex;
	binValue(binNumber, &myReal, &myComplex);
	*outMag = hypot(myReal, myComplex);
	*outPhase = (0.0 == myReal) ? 0 : atan(myComplex/myReal);
}

void IFFT::setBin(int binNumber, float realPart, float imagPart) {
#ifdef CSL_DEBUG
	logMsg("\t\tset [%d] = %5.3f @ %5.3f", binNumber, realPart, imagPart);
#endif
	cx_r(mSpectrum[binNumber]) = realPart;
	cx_i(mSpectrum[binNumber]) = imagPart;
}

void IFFT::setBins(float * real, float * imag) {
	for (int i = 0; i < mFFTSize; i++) {
		cx_r(mSpectrum[i]) = real[i];
		cx_i(mSpectrum[i]) = imag[i];		
	}
}

void IFFT::setBins(SampleComplexVector cmplxSpectrum) {
	for (int i = 0; i < mFFTSize; i++) {
		cx_r(mSpectrum[i]) = cx_r(cmplxSpectrum[i]);
		cx_i(mSpectrum[i]) = cx_i(cmplxSpectrum[i]);		
	}
}

void IFFT::setBins(SampleBuffer cmplxSpectrum) {
	for (int i = 0; i < mFFTSize; i += 2) {
		cx_r(mSpectrum[i]) = *cmplxSpectrum++;
		cx_i(mSpectrum[i]) = *cmplxSpectrum++;		
	}
}

void IFFT::setBins(int lower, int upper, float * real, float * imag) {
	if (lower < 0 || lower >= mFFTSize ) { return; } // It should throw an "out of range" exception, right?
	if (upper < 0 || upper >= mFFTSize ) { return; }
	if (upper < lower ) { return; }

	for (int i = lower; i < upper; i++) {
		cx_r(mSpectrum[i]) = real[i];
		cx_i(mSpectrum[i]) = imag[i];		
	}
}

void IFFT::setBinMagPhase(int binNumber, float mag, float phase) {
	float myReal, myComplex;
	myReal = mag * cos(phase);
	myComplex = mag * sin(phase);
	setBin(binNumber, myReal, myComplex);
}

void IFFT::setBinsMagPhase(float* mags, float* phases) {
	for (int i = 0; i < mFFTSize; i++) {
		cx_r(mSpectrum[i]) = mags[i] * cos(phases[i]);
		cx_i(mSpectrum[i]) = mags[i] * sin(phases[i]);
	}
}

// Do the IFFT::nextBuffer by calling the wrapper

void IFFT::nextBuffer(Buffer & outputBuffer) throw (CException) {
	if (outputBuffer.mNumFrames != mWrapper.mSize) {
		logMsg(kLogError, 
			"IFFT::nextBuffer # frames %d wrong for FFT size %d (use a block resizer).", 
			outputBuffer.mNumFrames, mWrapper.mSize);
		return;
	}
	mInBuf.setSize(1, outputBuffer.mNumFrames);
	mInBuf.setBuffer(0, (SampleBuffer) mSpectrum);

	mWrapper.nextBuffer(mInBuf, outputBuffer);			// execute the IFFT via the wrapper
	return;
}
