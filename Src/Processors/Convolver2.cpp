///
/// Convolver2.cpp -- The simplified convolution class.
/// This is an FFT-based convolver that uses the CSL FFT_Wrapper.
///			stephen@heaveneverywhere.com - 1905
//
//		Copyright(C) 2019 Stephen T. Pope. All rights reserved.
//		THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE.
//		The copyright notice above does not evidence any actual or intended
//		publication of such source code.
///

#include "Convolver2.h"
#include "SoundFileJ.h"
#ifdef ARM
#include "arm_cmplx_mult_cmplx_f32.c"			//  ARM optimized math code
#endif

using namespace csl;

// General constructor

Convolver2::Convolver2(UnitGenerator & in, char * inFName, unsigned chan, unsigned len, bool norm) :
			Effect(in),
			mFFTSize(len),
			mNormOut(norm),
			mFFTAnalyzer(len, CSL_FFT_COMPLEX, CSL_FFT_FORWARD),
			mFFTSynthesizer(len, CSL_FFT_COMPLEX, CSL_FFT_INVERSE),
			mFFTBuffer(1, len) {
	mFlSize = mFFTSize * sizeof(sample);
	mNumBufs = 0;
	setIRFile(inFName, chan);
}

Convolver2::Convolver2(UnitGenerator & in, float * irData, unsigned nTaps, unsigned len, bool norm) :
			Effect(in),
			mFFTSize(len),
			mNormOut(norm),
			mFFTAnalyzer(len, CSL_FFT_COMPLEX, CSL_FFT_FORWARD),
			mFFTSynthesizer(len, CSL_FFT_COMPLEX, CSL_FFT_INVERSE),
			mFFTBuffer(1, len) {
	mFlSize = mFFTSize * sizeof(sample);
	mNumBufs = 0;
	setIRData(irData, nTaps);
}

// Destructor frees everything

Convolver2::~Convolver2 () {
	free_buffers();
}

// free_buffers actualy frees everything

void Convolver2::free_buffers() {
	if (mNumBufs) {
		for (unsigned i = 0; i < mNumBufs; i++) {
			SAFE_FREE(mFilterFFT[i]);
			SAFE_FREE(mInputFFT[i]);
		}
		SAFE_FREE(mFilterFFT);
		SAFE_FREE(mInputFFT);
		SAFE_FREE(mSpectrumBuffer);
		SAFE_FREE(mLastOutput);
		mNumBufs = 0;
	}
}

// if we're changing size, check buffers and allocate all new storage

void Convolver2::checkBuffers(unsigned newNumBufs) {
	if (newNumBufs == mNumBufs)
		return;
	unsigned fftlen = mFFTSize + 1;
//	logMsg(kLogInfo, "Convolver allocating %u buffers of %d for IR len %d -- ", newNumBufs, fftlen, mNumTaps);
	if (mNumBufs)												// free if previously allocated
		free_buffers();
	SAFE_MALLOC(mFilterFFT, SampleComplexVector, newNumBufs);	// allocate for storage of IR segments and input
	SAFE_MALLOC(mInputFFT,  SampleComplexVector, newNumBufs);
	SAFE_MALLOC(mSpectrumBuffer, SampleComplex, fftlen);		// allocate temp buffers
	SAFE_MALLOC(mLastOutput, sample, fftlen / 2);
	
	for (unsigned i = 0; i < newNumBufs; i++) {					// loop to allocate/store the FFTs for each segment of the IR
		SAFE_MALLOC(mInputFFT[i],  SampleComplex, fftlen);
		SAFE_MALLOC(mFilterFFT[i], SampleComplex, fftlen);
//		logMsg("Bufs: %x - %x", mInputFFT[i], mFilterFFT[i]);
	}
//	logMsg("SBuf: %x\n", mSpectrumBuffer);
	mNumBufs = newNumBufs;
}

// set the IR file name; run the analysis ffts

void Convolver2::setIRFile(char * inFName, unsigned chan) {
	JSoundFile * inFile = JSoundFile::openSndfile(string(inFName));
	unsigned frames = inFile->duration();				// # of frames in IR
	if ( ! frames) {
		logMsg(kLogFatal, "Convolver2 can't find IR file %s", inFName);
	}
	SampleBuffer data = 0;
	if (chan >= inFile->numChannels())
		data = inFile->buffer(0);						// input pointer to 0th channel
	else
		data = inFile->buffer(chan);					// input pointer to Nth channel
	setIRData(data, frames);
	delete inFile;
}

// set the IR data array; run the analysis ffts

void Convolver2::setIRData(float * data, unsigned nTaps) {
	mNumTaps = nTaps;
	unsigned fftlen = mFFTSize;
	unsigned fftl2 = mFFTSize / 2;
	unsigned newNumBufs = (nTaps / fftl2) + 1;			// # windows needed for IR
	mWindowCount = 0;									// incremental counter
//	float norm = 1.0f / (float) fftlen;					// normalization factor
	checkBuffers(newNumBufs);

	for (unsigned i = 0; i < mNumBufs; i++) {			// now take fft of IR file
		SampleBuffer sPtr = (SampleBuffer) mFilterFFT[i];
		bzero(sPtr, fftlen * sizeof(SampleComplex));	// zero out the working FFT buffer before loop
		unsigned offs = i * fftl2;
		for (unsigned j = 0; j < fftl2; j++) {			// fill bottom half of sample buffer from input
			unsigned  k = j + offs;
			if (k >= nTaps)		*sPtr++ = 0.0;			// copy into sample buffer
			else				*sPtr++ = data[k]; 		//  * norm;
		}
		mFFTBuffer.setBuffer(0, (SampleBuffer) mFilterFFT[i]);
														// in-place complex fft - overwrites the input vectors
		mFFTAnalyzer.nextBuffer(mFFTBuffer, mFFTBuffer);
	}
}

// Convolver2::nextBuffer() does the heavy lifting of the block-wise convolution

void Convolver2::nextBuffer(Buffer & outputBuffer) throw (CException) {
	unsigned numFrames = outputBuffer.mNumFrames;		// local copies of important items
	unsigned numBufs = mNumBufs;
	unsigned windowCount = mWindowCount;
	unsigned fftlen = mFFTSize;
	unsigned wInp = windowCount % numBufs;				// pseudo ring buffer addressing
	unsigned rbufsize = numFrames * sizeof(sample);		// I/O buffer size
	unsigned cbufsize = fftlen * sizeof(sample);		// FFT buffer size
	unsigned cxbufsize = fftlen * sizeof(SampleComplex);// complex  buffer size
	float norm = 1.0f / (float) fftlen;

	if (numFrames != fftlen / 2) {
		logMsg(kLogError,
			   "Convolver2::nextBuffer # frames %d wrong for FFT size %d (use a block resizer).",
			   numFrames, mFFTSize);
		return;
	}
	Effect::pullInput(numFrames);					// get some input
	SampleBuffer inp = mInputPtr;					// pullInput sets mInputPtr
	bzero(mInputFFT[wInp], cxbufsize);				// zero out the working FFT buffer before loop
	memcpy(mInputFFT[wInp], inp, rbufsize);			// copy input into the mInputFFT[] buffer

//	memcpy(outputBuffer.buffer(0), inp, cbufsize);	// testing 0 - just copy in to out
//	return;
													// plug the input into the FFT buffer
	mFFTBuffer.setBuffer(0, (SampleBuffer) mInputFFT[wInp]);
													// in-place complex fft - overwrites the input vectors
	mFFTAnalyzer.nextBuffer(mFFTBuffer, mFFTBuffer);

	bzero(mSpectrumBuffer, cxbufsize);					// zero out the summation FFT buffer before loop
	for (unsigned i = 0; i < numBufs; i++) {			// loop over IR windows
		wInp = (numBufs + windowCount - i) % numBufs;	// which input FFT to use
		complex_multiply_accumulate(				// sum complex vectors
						mFilterFFT[i],				// mult current IR window
						mInputFFT[wInp],			// by past input in reverse order
						mSpectrumBuffer);			// sum into mSpectrumBuffer
	}
//	bzero(mSpectrumBuffer, cxbufsize);				// testing 1 - synth a (44100 / 512 * X) Hz sine
//	mSpectrumBuffer[6][0] = 0.25f;
//	wInp = windowCount % numBufs;					// testing 2 - just resynth the input
//	memcpy(mSpectrumBuffer, mInputFFT[wInp], cxbufsize);
//	mNormOut = true;
													// plug in spectral sum buffer for IFFT
	mFFTBuffer.setBuffer(0, (SampleBuffer) mSpectrumBuffer);
													// execute the IFFT for spectral domain convolution
	mFFTSynthesizer.nextBuffer(mFFTBuffer, mFFTBuffer);

	SampleBuffer outP = outputBuffer.buffer(0);		// copy real vector to output
	SampleBuffer res = (SampleBuffer) mSpectrumBuffer;
	SampleBuffer prev = mLastOutput;
	
	if (mNormOut) {
		for (unsigned i = 0; i < numFrames; i++) {
			*outP++ = (*res++ + *prev++) * norm;	// sum and scale
		}
	} else
		for (unsigned i = 0; i < numFrames; i++)
			*outP++ = (*res++ + *prev++);			// no normalization
													// remember 2nd half of output for later
	memcpy(mLastOutput, ((SampleBuffer) mSpectrumBuffer) + numFrames, rbufsize);
	mWindowCount++;
}

//	ARM FFT:
//	void arm_cfft_f32	(	const arm_cfft_instance_f32 * 	S,
//					 float32_t * 	p1,
//					 uint8_t 	ifftFlag,
//					 uint8_t 	bitReverseFlag
//					 )
//
//

// complex_multiply_accumulate - fast complex MAC using interleaved complex arrays
// It's an ASM library call on the BBGW

void Convolver2 :: complex_multiply_accumulate(SampleComplexVector left, SampleComplexVector right,
											  SampleComplexVector output) {
#ifdef ARM												// use NEON code for this
	arm_cmplx_mult_cmplx_f32((const float32_t *) left, (const float32_t *) right,
							 (float32_t *) output, (uint32_t) mFFTSize);
#else
	SampleComplexVector loleft = left;					// local copies of args
	SampleComplexVector loright = right;
	SampleComplexVector looutput = output;
	float re, im;										// temps
	float leftRe, rightRe, leftIm, rightIm;
	unsigned frame = mFFTSize;
//	printf("MAC: %x - %x - %x (%u)\n", left, right, output, frame);
	while (frame--) {									// loop over frames
		leftRe = (*loleft)[0];							// cache vector values
		rightRe = (*loright)[0];
		leftIm = (*loleft)[1];
		rightIm = (*loright)[1];
		re = (leftRe * rightRe) - (leftIm * rightIm);	// complex multiply
		im = (leftRe * rightIm) + (leftIm * rightRe);
		(*looutput)[0] += re;							// sum into work fft
		(*looutput)[1] += im;
		loleft++;										// increment register pointers
		loright++;
		looutput++;
	}
#endif
}
