///
/// Convolver.cpp -- The low-latency convolution classes.
///	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
/// 

#include "Convolver.h"

using namespace csl;

// Fancy constructor that reads the IR file and does its FFT

Convolver :: Convolver (char *filename) : Effect() {
	mNumBufs = 0;
	mFFTSize = CGestalt::blockSize() * 2;
	this->initialize(filename);
}

Convolver :: Convolver (unsigned len, char *filename) : Effect() {
	mNumBufs = 0;
	mFFTSize = len * 2;
	this->initialize(filename);
}

Convolver :: Convolver(Buffer & impulseResp) : Effect() {
	mNumBufs = 0;
	mFFTSize = CGestalt::blockSize() * 2;
	this->initialize(impulseResp);
}

Convolver :: Convolver(unsigned len, Buffer & impulseResp) : Effect() {
	mNumBufs = 0;
	mFFTSize = len * 2;
	this->initialize(impulseResp);
}

// Destructor frees everything

Convolver :: ~Convolver () {
	for (unsigned i = 0; i < mNumBufs; i++) {
		if (mFilterFFTs[i]) delete mFilterFFTs[i];
		if (mInputFFTs[i]) delete mInputFFTs[i];
	}
	if (mFilterFFTs) delete(mFilterFFTs);
	if (mInputFFTs) delete(mInputFFTs);
	if (mSampleBuffer) CSL_FFTW_free(mSampleBuffer);
	if (mSpectrumBuffer) CSL_FFTW_free(mSpectrumBuffer);
	if (mForwardPlan) CSL_FFTW_deplan(mForwardPlan);
	if (mInversePlan) CSL_FFTW_deplan(mInversePlan);
}

// init with a file name reads file, then calls other init

void Convolver :: initialize (const char *filename) throw (CException) {
	SNDFILE *filterFile;
	SF_INFO filterFileInfo;
	unsigned frames;
//	float *data;
									// open and load input IR file
	logMsg(kLogInfo, "Reading IR file %s", filename);
	filterFile = sf_open(filename, SFM_READ, &filterFileInfo);
	if (!filterFile) {
		sf_perror(NULL);
		throw IOError("error opening IR sound file");
	}
	if (filterFileInfo.channels != 1) {
		sf_perror(NULL);
		throw IOError( "IR sound file must be mono, but isn't");
	}
	frames = filterFileInfo.frames; 
	Buffer * tmpBuf = new Buffer(1, frames);			// set up tmp buffer
	tmpBuf->allocateBuffers();							// allocate sample buffer
	sf_read_float(filterFile, tmpBuf->monoBuffer(0), frames);	//read the whole file in
	sf_close(filterFile);

	this->initialize( * tmpBuf);						// now do the real init
	delete tmpBuf;
}

// this init method allocates all the buffers and stores the FFTs of the windows of the given IR data buffer

void Convolver :: initialize (Buffer & buffer) throw (CException) {
	mWindowCount = 0;									// incremental counter
	mMyBuffers = true;									// whether I allocated the filter buffers
	mMyInput = true;									// whether to use pullInput
	unsigned frames = buffer.mNumFrames;				// # of frames in IR
	unsigned fftlen = mFFTSize;
	unsigned newNumBufs = (frames / fftlen) + 1;		// calculate # windows needed for IR

	unsigned cbufsize = fftlen * sizeof(fftwf_complex);	// size of complex buffers
	float norm = 1.0/(float)(fftlen);					// normalization factor
//	norm /= 10 * (fftlen/512.0);						// fudge factor
	bool allocBuffers = false;							// allocate main FFT buffers
	unsigned k;
	
//	logMsg(kLogInfo, "Convolver allocating %u  buffers", newNumBufs);
	if (mNumBufs == 0) {								// first time through
		mSampleBuffer = (CSL_FFTW_sample *) CSL_FFTW_alloc(sizeof(sample) * fftlen);
				CHECK_PTR(mSampleBuffer);
		mSpectrumBuffer = (CSL_FFTW_cmplx * ) CSL_FFTW_alloc(cbufsize);
		new fftwf_complex[fftlen];
				CHECK_PTR(mSpectrumBuffer);
	//	memset(mSpectrumBuffer, 0, fftlen * sizeof(sample));		// ??
														// create forward and reverse FFT plans
		mForwardPlan = CSL_FFTW_r2c_plan(fftlen, mSampleBuffer, mSpectrumBuffer, FFTW_MEASURE);
		mInversePlan = CSL_FFTW_c2r_plan(fftlen, mSpectrumBuffer, mSampleBuffer, FFTW_MEASURE);
		allocBuffers = true;
	} 
	if ((mNumBufs != 0) && (newNumBufs > mNumBufs)) {		// if we're growing, free old storage
		for (unsigned i = 0; i < mNumBufs; i++) {
			delete mFilterFFTs[i];
			delete mInputFFTs[i];
		}
		delete(mFilterFFTs);
		delete(mInputFFTs);
		allocBuffers = true;
	}
	mNumBufs = newNumBufs;

	if (allocBuffers) {												// allocate arrays of pointers to complex numbers
		mFilterFFTs = (CSL_FFTW_cmplx ** ) CSL_FFTW_alloc(sizeof(void *) * newNumBufs);// for storage of IR segments and input
					// new fftwf_complex * [newNumBufs];	
				CHECK_PTR(mFilterFFTs);
		mInputFFTs = (CSL_FFTW_cmplx ** ) CSL_FFTW_alloc(sizeof(void *) * newNumBufs);	
				CHECK_PTR(mInputFFTs);
	}

	float * data = buffer.monoBuffer(0);							// input pointer
	for (unsigned i = 0; i < newNumBufs; i++) {						// loop to allocate/store the FFTs for each segment of the IR
//		printf("%d", i);
		if (allocBuffers) {											// set up the past input and IR filter spectrum buffers
			mInputFFTs[i] = (CSL_FFTW_cmplx * ) CSL_FFTW_alloc(cbufsize);
					CHECK_PTR(mInputFFTs[i]);
			mFilterFFTs[i] =  (CSL_FFTW_cmplx * ) CSL_FFTW_alloc(cbufsize); // new fftwf_complex[fftlen];
					CHECK_PTR(mFilterFFTs[i]);
		}
		bzero(mInputFFTs[i], cbufsize);								// zero out past input buffers
		for (unsigned j = 0; j < fftlen/2; j++) {					// fill bottom half of sample buffer from input
			k = j + (i * fftlen);
			if (k >= frames)	mSampleBuffer[j] = 0.0;				// copy into sample buffer
			else				mSampleBuffer[j] = data[k] * norm;
		}
		CSL_FFTW_exec(mForwardPlan);								// FFT a window of IR into mSpectrumBuffer
																	// copy the FFT frame to the mFilterFFTs
		memcpy(mFilterFFTs[i], mSpectrumBuffer, cbufsize);
	}
	logMsg(kLogInfo, "Convolver: numBufs %u; FFT size %u", mNumBufs, fftlen);
}


// To get the next buffer, we take a window of the input, FFT is, then do the multi-window
// complex multiply/sum in the freq. domain, then IFFT the summed convolution

void Convolver :: nextBuffer(Buffer & outputBuffer, unsigned channel) throw (CException) {
	unsigned numFrames = outputBuffer.mNumFrames;				// local copies of important items
	unsigned numBufs = mNumBufs;
	unsigned windowCount = mWindowCount;
	unsigned fftlen = mFFTSize;
	unsigned whichInp = windowCount % numBufs;					// pseudo ring buffer addressing
	unsigned cbufsize = fftlen * sizeof(fftwf_complex);

//	logMsg(kLogInfo, "N %u \F %u \tL %u", windowCount, numFrames, fftlen);

	if (numFrames != fftlen/2)										// Otherwise use a BlockResizer
		throw DomainError("Can't convolve with chosen buffer size");

	if (mMyInput) {
		Effect::pullInput(outputBuffer);							// get a buffer from the input stream
		sample * inp = mInputPtr;									// pullInput sets mInputPtr
		memset(mSampleBuffer, 0, fftlen * sizeof(sample));			// zero out the working sample buffer
		memcpy(mSampleBuffer, inp, numFrames * sizeof(sample));		// put input into the sample buffer
		CSL_FFTW_exec(mForwardPlan);								// FFT input from mSampleBuffer into mSpectrumBuffer
		memcpy(mInputFFTs[whichInp], mSpectrumBuffer, cbufsize);	// copy it into the mInputFFT[] buffer
	}
	bzero(mSpectrumBuffer, cbufsize);								// zero out the working FFT buffer before loop
																	// loop to  multiply/accumulate spectra for convolution
	for (unsigned i = 0; i < numBufs; i++) {						// loop over IR windows
		whichInp = (numBufs + windowCount - i) % numBufs;			// which input FFT to use
//		printf("  %d  %d  %d\t", windowCount, i, whichInp);
		complex_multiply_accumulate(							// sum complex vectors
					mFilterFFTs[i],								// mult current IR window
					mInputFFTs[whichInp],						// by past input in reverse order
					mSpectrumBuffer);							// sum into mSpectrumBuffer
	}
	CSL_FFTW_exec(mInversePlan);									// execute the IFFT for spectrum domain convolution
	memcpy(outputBuffer.mBuffers[0], mSampleBuffer, numFrames * sizeof(sample));	// copy to outp
	mWindowCount++;
}

///< set the IR spectrum array

void Convolver :: setFilters(fftwf_complex ** filterFFTs) {
	// ??
}

///< pass in the input buffer

void Convolver :: setInputf(fftwf_complex * inFFT) {
	memcpy(mInputFFTs[mWindowCount % mNumBufs], inFFT, (sizeof(fftwf_complex) * mFFTSize));
	mMyInput = false;
}

// fast complex ptr mult-accum utility

void Convolver :: complex_multiply_accumulate(CSL_FFTW_cmplx * left, CSL_FFTW_cmplx * right, 
											CSL_FFTW_cmplx * output) {
	CSL_FFTW_cmplx * loleft = left;						// local copies of args
	CSL_FFTW_cmplx * loright = right;
	CSL_FFTW_cmplx * looutput = output;
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
}
