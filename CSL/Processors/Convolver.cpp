///
/// Convolver.cpp -- The way-simplified convolution class.
//		Copyright(C) 2019 Stephen T. Pope. All rights reserved.
//		THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE.
//		The copyright notice above does not evidence any actual or intended
//		publication of such source code.
/// 

#include "Convolver.h"
#include "SoundFileJ.h"
#include "fft_N.h"

using namespace csl;

// Simple constructor

Convolver::Convolver(unsigned len) :
		Effect(),
		mFFTSize(len) {
	mFlSize = mFFTSize * sizeof(sample);
	mNumBufs = 0;
}

Convolver::Convolver(char * inFName, unsigned len) : Convolver(len) {
	setIRFile(inFName);
}

Convolver::Convolver(UnitGenerator & in, char * inFName, unsigned chan, unsigned len) :
		Effect(in),
		mFFTSize(len) {
	mFlSize = mFFTSize * sizeof(sample);
	mNumBufs = 0;
	setIRFile(inFName, chan);
}

// Destructor frees everything

Convolver::~Convolver () {
	free_buffers();
}

// free_buffers actualy frees everything

void Convolver::free_buffers() {
	if (mNumBufs) {
		for (unsigned i = 0; i < mNumBufs; i++) {
			SAFE_FREE(mFilterFFTRe[i]);
			SAFE_FREE(mFilterFFTIm[i]);
			SAFE_FREE(mInputFFTRe[i]);
			SAFE_FREE(mInputFFTIm[i]);
		}
		SAFE_FREE(mFilterFFTRe);
		SAFE_FREE(mFilterFFTIm);
		SAFE_FREE(mInputFFTRe);
		SAFE_FREE(mInputFFTIm);
		SAFE_FREE(mSpectrumBufferRe);
		SAFE_FREE(mSpectrumBufferIm);
		SAFE_FREE(mLastOutput);
		mNumBufs = 0;
	}
}
// if we're changing size, check buffers and allocate all new storage

void Convolver::checkBuffers(unsigned newNumBufs) {
	if (newNumBufs == mNumBufs)
		return;
	unsigned fftlen = mFFTSize;
	logMsg(kLogInfo, "Convolver allocating %u buffers of %d", newNumBufs, fftlen);
	if (mNumBufs)											// free if previously allocated
		free_buffers();
	SAFE_MALLOC(mSpectrumBufferRe, Sample, fftlen);			// allocate temp buffers
	SAFE_MALLOC(mSpectrumBufferIm, Sample, fftlen);
	SAFE_MALLOC(mLastOutput, Sample, fftlen / 2);
	
	SAFE_MALLOC(mFilterFFTRe, SampleBuffer, newNumBufs);	// allocate for storage of IR segments and input
	SAFE_MALLOC(mFilterFFTIm, SampleBuffer, newNumBufs);
	SAFE_MALLOC(mInputFFTRe,  SampleBuffer, newNumBufs);
	SAFE_MALLOC(mInputFFTIm,  SampleBuffer, newNumBufs);
	
	for (unsigned i = 0; i < newNumBufs; i++) {				// loop to allocate/store the FFTs for each segment of the IR
		SAFE_MALLOC(mFilterFFTRe[i], Sample, fftlen);
		SAFE_MALLOC(mFilterFFTIm[i], Sample, fftlen);
		SAFE_MALLOC(mInputFFTRe[i],  Sample, fftlen);
		SAFE_MALLOC(mInputFFTIm[i],  Sample, fftlen);
//		logMsg("Bufs: %x @ %x - %x @ %x", mFilterFFTRe[i], mFilterFFTIm[i], mInputFFTRe[i], mInputFFTIm[i]);
	}
//	logMsg("SBufs: %x @ %x\n", mSpectrumBufferRe, mSpectrumBufferIm);
	mNumBufs = newNumBufs;
	Fft_setup(fftlen);
}

// set the IR file name; runs the analysis ffts

void Convolver::setIRFile(char * inFName, unsigned chan) {
	JSoundFile * inFile = JSoundFile::openSndfile(string(inFName));
	unsigned frames = inFile->duration();				// # of frames in IR
	if ( ! frames) {
		logMsg(kLogFatal, "Convolver can't find IR file %s", inFName);
	}
	SampleBuffer data = 0;
	if (chan >= inFile->numChannels())
		data = inFile->buffer(0);						// input pointer to 0th channel
	else
		data = inFile->buffer(chan);					// input pointer to Nth channel
	unsigned fftlen = mFFTSize;
	unsigned newNumBufs = (frames / fftlen) + 1;		// # windows needed for IR
	mWindowCount = 0;									// incremental counter
	float norm = 1.0f / (float) fftlen;					// normalization factor
	checkBuffers(newNumBufs);

	for (unsigned i = 0; i < mNumBufs; i++) {			// now take fft of IR file
		SampleBuffer sPtr = mFilterFFTRe[i];
		unsigned offs = i * fftlen;
		for (unsigned j = 0; j < fftlen; j++) {			// fill bottom half of sample buffer from input
			unsigned  k = j + offs;
			if (k >= frames)	*sPtr++ = 0.0;			// copy into sample buffer
			else				*sPtr++ = data[k] * norm;
		}
		memset(mFilterFFTIm[i], 0, mFlSize);
														// in-place complex fft - overwrites the input vectors
		bool succ = Fft_transform(mFilterFFTRe[i], mFilterFFTIm[i], mFFTSize);
		if ( ! succ)
			logMsg(kLogError, "Error doing fft of IR");
	}
}

// Convolver::nextBuffer() does the heavy lifting of the block-wise convolution

void Convolver::nextBuffer(Buffer & outputBuffer) throw (CException) {
	unsigned numFrames = outputBuffer.mNumFrames;		// local copies of important items
	unsigned numBufs = mNumBufs;
	unsigned windowCount = mWindowCount;
	unsigned fftlen = mFFTSize;
	unsigned wInp = windowCount % numBufs;				// pseudo ring buffer addressing
	unsigned rbufsize = numFrames * sizeof(sample);		// I/O buffer size
	unsigned cbufsize = fftlen * sizeof(sample);		// FFT buffer size
	sample norm = 1.0f / (float) fftlen;

	if (numFrames != fftlen / 2) {
		logMsg(kLogError,
			   "Convolver::nextBuffer # frames %d wrong for FFT size %d (use a block resizer).",
			   numFrames, mFFTSize);
		return;
	}
	Effect::pullInput(numFrames);					// get some input
	SampleBuffer inp = mInputPtr;					// pullInput sets mInputPtr
	bzero(mInputFFTRe[wInp], cbufsize);				// zero out the working FFT buffer before loop
	bzero(mInputFFTIm[wInp], cbufsize);
	memcpy(mInputFFTRe[wInp], inp, rbufsize);		// copy input into the mInputFFTRe[] buffer

//	memcpy(outputBuffer.buffer(0), inp, cbufsize);	// testing, just copy i to o
//	return;
													// in-place complex fft - overwrites the input vectors
	bool succ = Fft_transform(mInputFFTRe[wInp], mInputFFTIm[wInp], fftlen);
	if ( ! succ)
		logMsg(kLogError, "Error doing fft of input");

	bzero(mSpectrumBufferRe, cbufsize);								// zero out the working FFT buffer before loop
	bzero(mSpectrumBufferIm, cbufsize);
	for (unsigned i = 0; i < numBufs; i++) {						// loop over IR windows
		wInp = (numBufs + windowCount - i) % numBufs;				// which input FFT to use
//		logMsg("CMAC: [%d : %d] %x @ %x * %x @ %x -> %x @ %x", i, wInp,
//			   			mFilterFFTRe[i], mFilterFFTIm[i],
//			   			mInputFFTRe[wInp], mInputFFTIm[wInp],
//			   			mSpectrumBufferRe, mSpectrumBufferIm);
		complex_multiply_accumulate(								// sum complex vectors
						mFilterFFTRe[i],	mFilterFFTIm[i],		// mult current IR window
						mInputFFTRe[wInp],	mInputFFTIm[wInp],		// by past input in reverse order
						mSpectrumBufferRe,	mSpectrumBufferIm);		// sum into mSpectrumBuffer
	}
//	bzero(mSpectrumBufferRe, cbufsize);				// testing 1 - synth a 517 Hz (44100 / 512 * 6) sine
//	bzero(mSpectrumBufferIm, cbufsize);
//	mSpectrumBufferRe[6] = 600.0;
//	wInp = windowCount % numBufs;					// testing 2 - just resynth the input
//	memcpy(mSpectrumBufferRe, mInputFFTRe[wInp], cbufsize);
//	memcpy(mSpectrumBufferIm, mInputFFTIm[wInp], cbufsize);
													// execute the IFFT for spectral domain convolution
	succ = Fft_inverseTransform(mSpectrumBufferRe, mSpectrumBufferIm, fftlen);
	if ( ! succ)
		logMsg(kLogError, "Error doing ifft");

	SampleBuffer outP = outputBuffer.buffer(0);		// copy real vector to output, cross-fading between windows
	SampleBuffer re = mSpectrumBufferRe;
	SampleBuffer im = mSpectrumBufferIm;
	SampleBuffer las = mLastOutput;
	float fadeI = 0.0f;								// fade in/out scale factors
	float fadeO = 1.0f;
	float step =  1.0 / numFrames;

	for (unsigned i = 0; i < numFrames; i++) {
		*outP++ = *re++ * norm;						// no cross-fade
//		float samp1 = *re++ * fadeI;				// of cross-fade last window with this one
//		float samp2 = *las++ * fadeO;
//		*outP++ = (samp1 + samp2) * norm;
//		fadeI += step;
//		fadeO -= step;
	}
	memcpy(mLastOutput, mSpectrumBufferRe + numFrames, rbufsize);	// copy the upper half of the result into the cache
	mWindowCount++;
}

// fast complex MAC using non-interleaved complex arrays

void Convolver::complex_multiply_accumulate(SampleBuffer leftRe,  SampleBuffer leftIm,
											SampleBuffer rightRe, SampleBuffer rightIm,
											SampleBuffer outRe,   SampleBuffer outIm) {
	SampleBuffer loleftR = leftRe;								// local copies of args
	SampleBuffer loleftI = leftIm;
	SampleBuffer lorightR = rightRe;
	SampleBuffer lorightI = rightIm;
	SampleBuffer looutR = outRe;
	SampleBuffer looutI = outIm;
//	logMsg("CMAC: %x @ %x * %x @ %x -> %x @ %x", leftRe, leftIm, rightRe, rightIm, outRe, outIm);
	unsigned frame = mFFTSize;
	while (frame--) {											// loop over frames
		sample lRe = *loleftR++;								// cache vector values
		sample lIm = *loleftI++;
		sample rRe = *lorightR++;
		sample rIm = *lorightI++;
		sample re = (lRe * rRe) - (lIm * rIm);					// complex multiply
		sample im = (lRe * rIm) + (lIm * rRe);
		*looutR++ += re;										// sum into work fft
		*looutI++ += im;
	}
}
