//
//	LPC.cpp -- implementation of the classes that perform LPC-based analysis/resynthesis.
//	These use Peter Kabal's libtsp routines (http://WWW.TSP.ECE.McGill.CA) for the heavy lifting.
//	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
//

#include "LPC.h"
#include "Window.h"
#include <libtsp.h>

#include <stdlib.h>
#include <math.h>

using namespace csl;

/// Forward FFT = analysis

// Constructor - Default size to the buffer size and flags to measure
// mBlockResizer(size) needed if different size than the default

LPCAnalyzer :: LPCAnalyzer(UnitGenerator & in, unsigned size, unsigned thopSize, unsigned order)
			: Effect(in), mWindowSize(size), mHopSize(thopSize), mLPCOrder(order),	// set up inst vars
			  mTempBuffer(1, mWindowSize), mWindowBuffer(1, mWindowSize),		// set up buffers
			  mLPCCoefficients(1, mLPCOrder),	 mFilterCoefficients(1, mLPCOrder),	///< the LPC PC coefficients
			  mPCCoefficients(1, mLPCOrder), mLPCResidual(1, CSL_mBlockSize),		// allocate LPC stores
			  mOverwriteOutput(FALSE), mUseWindow(FALSE),						// process flags
			  mRing(), mCoeffFile(NULL), mResidFile(NULL) {							// file I/O ptrs
									// Allocate buffer storage
	mTempBuffer.allocateBuffers();
	mLPCCoefficients.allocateBuffers();
	mLPCCoefficients.mType = kLPCCoeff;
	mPCCoefficients.allocateBuffers();
	mFilterCoefficients.allocateBuffers();
	mLPCResidual.allocateBuffers();
	mFilterInput.allocateBuffers();	
	mWindowBuffer.allocateBuffers();
									// Allocate, fill, and copy Hamming window
	HammingWindow * window = new HammingWindow(mWindowSize);
	memcpy(mWindowBuffer.mBuffers[0], window->window(), (mWindowSize * sizeof(sample)));
	free(window);
	mCopyPolicy = kCopy;		// so it plays back OK
	
	mLoPass= new Butter(mRing, BW_LOW_PASS, 1000.0);								// lpf/hpf filters
	mHiPass= new Butter(mRing, BW_HIGH_PASS, 3000.0);
	mFFT = new FFT(mRing, CSL_FFT_REAL, kFFTMeasure, (int) size); 						// FFT
}

// Destructor closes files

LPCAnalyzer :: ~LPCAnalyzer() {
	this->closeFiles();
}

// Open the files, write the headers

void LPCAnalyzer :: setFileNames(char * coeff, char * resid) {
	mCoeffFile = fopen(coeff, "wb");
	if (mCoeffFile == NULL) {
		perror("Cannot open coefficient file");
		return;
	}
	CSL_LPC_HEADER head;
	head.keyword = CSL_LPC_LKEY;
	head.order = mLPCOrder;
	head.windowSize = mWindowSize;
	head.hopSize = mHopSize;
	head.preEmphasis = mPreEmphasis;
	head.filesize = 0;
	head.duration = 0 ;
	fwrite( & head, sizeof(CSL_LPC_HEADER), 1, mCoeffFile);
						// open a mono 16-bit AIFF file for the residual
	mResidFile = new SoundFile(resid);
	mResidFile->openForWrite(kSoundFileFormatAIFF, 1, CGestalt::frameRate(),16);
}

// Close the output files

void LPCAnalyzer :: closeFiles() {
	if (mCoeffFile == NULL)
		return;
	fclose(mCoeffFile);
	mResidFile->close();
}

static int counter = 0;

// nextBuffer does the windowing and LPC analysis

void LPCAnalyzer :: nextBuffer(Buffer& outBuf, unsigned outBufNum) throw (CException) {
	sample * outPtr = outBuf.mBuffers[outBufNum];
	sample * winPtr = mWindowBuffer.mBuffers[0];	
	sample * reflCoeffPtr = mLPCCoefficients.mBuffers[0];
	sample * filtCoeffPtr = mFilterCoefficients.mBuffers[0];
	sample * pcCoeffPtr = mPCCoefficients.mBuffers[0];
	sample * filterInPtr = mFilterInput.mBuffers[0];
	sample * filterOutPtr = mLPCResidual.mBuffers[0];
	sample * tempPtr = mTempBuffer.mBuffers[0];

	float Fmemw = 0.0;				// temp memory location
	float Fmemd = 0.0;				// temp memory location

	unsigned numFrames = outBuf.mNumFrames;		// get buffer length
	if (numFrames < mHopSize)
		numFrames = mHopSize;
	Effect :: pullInput(numFrames);					// get the input samples via Effect
	sample * inPtr = mInputPtr;						// this is the input buffer pointer
	float * filtTempPtr = filterInPtr + mLPCOrder;			// the ptr to the new frame storage

//	if (mWindowSize > mHopSize) { 					// Copy the previous input data into the lower half buffer 
//		memcpy(tempPtr, tempPtr, numFrames * sizeof(sample));
//													// Copy the current input data into the upper half buffer 
//		memcpy(tempPtr + numFrames, inPtr, numFrames * sizeof(sample));
//	}
	memcpy(tempPtr, inPtr, (numFrames * sizeof(sample)));			// copy in to temp storage
	memcpy(filtTempPtr, inPtr, (numFrames * sizeof(sample)));		// make a copy for later
	if (mOverwriteOutput)
		memcpy(outPtr, inPtr, (numFrames * sizeof(sample)));		// copy input to out
	if (mUseWindow)												// apply window
		for (unsigned i = 0; i < numFrames; i++)	
			*tempPtr++ *= *winPtr++;
			//	preemphasize the data
			//	FIfPreem (double factor, float *Fmem, const float in[], float out[], int in_size)
			//		a = 0 for no pre-emph; a = 1 for full first difference, in may = out
	FIfPreem (mPreEmphasis, &Fmemw, tempPtr, tempPtr, numFrames);	// preemphasize the data
			//	perform autocorrelation
			//	SPautoc (const float in[], int in_size, float out[], int out_size)
			//		out[i] is autocorrelation with lag i
	SPautoc (tempPtr, numFrames, reflCoeffPtr, mLPCOrder+1);	
			// convert autocorrelation data to reflection coefficients
			//	double SPcorXpc (const float in[], float out[], int out_size)
	mPredErr = SPcorXpc (reflCoeffPtr, reflCoeffPtr, mLPCOrder);	
			//	Bandwidth widening of predictor coefficients
			//	SPpcBWexp (double factor, const float in[], float out[], int out_size)
	if (mBWExpansion != 1.0)
		SPpcBWexp (mBWExpansion, pcCoeffPtr, pcCoeffPtr, mLPCOrder);
			//	convert reflection coefficients to filter coefficients
			//	SPpcXec (const float in[], float out[], int out_size)
	SPpcXec (pcCoeffPtr, filtCoeffPtr, mLPCOrder);	
			//	preemphasize the filter input data
	FIfPreem (mPreEmphasis, &Fmemd, filterInPtr, filterInPtr, numFrames);
			//	Filter the data with an FIR filter to get the residual
			//	FIfConvol (const float in[], float out[], int out_size, const float coefficients[], int coeff_size)
	FIfConvol (filterInPtr, filterOutPtr, numFrames, filtCoeffPtr, mLPCOrder+1);
												// now gather statistics and do filters and pitch estimates
	Buffer * inBuf = Effect::inPort()->mBuffer;
	float rms = inBuf->rms(outBufNum);							// get RMS
	
	mRing.mBuffer.mBuffers[0] = mInputPtr;					// set the ring buffer read pointer
	mRing.seekTo(0);
	mLoPass->nextBuffer(mTempBuffer, 0);						// run the lpf/hpf filters
	float lpEnergy = mTempBuffer.rms(0);							// get LP energy
	mRing.seekTo(0);
	mHiPass->nextBuffer(mTempBuffer, 0);	
	float hpEnergy = mTempBuffer.rms(0);							// get HP energy

	mRing.seekTo(0);
	mFFT->nextBuffer(mTempBuffer);								// do the FFT
	float val, sum = 0.0, wSum = 0.0;								// get FFT spectral peaks and centroid
	float normFactor = 1.0 / sqrt( static_cast<double>( mWindowSize ) );
	float freq0 = static_cast<float>( CGestalt::frameRate()) / numFrames;
	for (unsigned i = 0; i < numFrames; i++) {						// loop to sum for spect. centroid
		val = *tempPtr++;
		val /= normFactor;
		sum += val;												// sum bins
		wSum += (val * (i + 1) * freq0);								// weighted sum for centroid
	}

	tempPtr = mTempBuffer.mBuffers[0];
	sample * spect = mFFT->mRealSpectrum;						// get the FFT spectrum
	memcpy(tempPtr, spect, (numFrames * sizeof(sample)));		// make a copy
	for (unsigned i = 1; i <= CSL_HPS_DEPTH; i++) {					// now loop through the get the harmonic product spectrum
		for (unsigned j = 0, k = 0; k < numFrames; j++, k += i)			// inner loop sums condensed spectra into tempPtr
			tempPtr[j] += spect[k];								// weighted sum for centroid
	}
	float hps = mTempBuffer.indexOfPeak(outBufNum, 3, 100) * freq0;		// get Harm. Prod. spectrum from FFT
	
	unsigned int zeroes = inBuf->zeroX(outBufNum);						// count the zero-crossings in the samples
	inBuf->autocorrelation(outBufNum, filterInPtr);							// write the autocorrelation into the given array
	unsigned int autocorr = mFilterInput.indexOfPeak(outBufNum, 100, 500);	// get the peak index of the autocorrelation
//	unsigned int autocorr2 = mFilterInput.indexOfMin(outBufNum, 50, 500);	// get the peak index of the autocorrelation
	float acpitch = (float) CGestalt::frameRate() / (float) autocorr;
	if (counter++ == 4) {
		printf("\n\n");
		for (unsigned i = 0; i < numFrames; i++)
	//		printf("%d,%7.5f\n", i, filterInPtr[i]);
			printf("%d\t%7.5f\t%7.5f\n", i, spect[i], tempPtr[i]);
		printf("\n\n");
	//	exit(0);
	}
	CSL_LPC_FRAME frame;										// now populate the frame header data structure
	frame.rmsamp = rms;			// RMS ampl
	frame.lofreq = lpEnergy;		// lpf energy < 200 Hz
	frame.hifreq = hpEnergy;		// hpf energy > 2 kHz
	frame.freqest1 = acpitch;		// autocorr freq estimate
	frame.freqest2 = hps;			// HPS freq estimate
	frame.zeroes = zeroes;		// zero-crossings
	frame.smoothfreq = 0;		// smoothed freq estimate
	frame.noisiness = 0;			// autocorr noise estimate
	frame.resid = mPredErr;		// LPC residual energy
	frame.formant1 = 0;			// LPC 1st formant
	frame.formant2 = 0;			// LPC 2nd formant
	frame.centroid = wSum / sum;	// spectral centroid
//	frame.mfccPeak1 = 0;			// 1st MFCC peak
//	frame.mfccPeak2 = 0;			// 2nd MFCC peak
	frame.MIDIKey = 0;			// MIDI key #
	frame.isTransition = 0;		// state flag (note-on, note-off, continuation, plosive, etc.

	fprintf(stderr, "\tR %7.5f   L %7.5f   H %7.5f   F1 %5.3f   F2 %5.3f   Z %d \tN %5.3f   C %5.3f\n", 
				frame.rmsamp, frame.hifreq, frame.lofreq, frame.freqest1, frame.freqest2,
				frame.zeroes, frame.resid, frame.centroid);
	if (mCoeffFile != NULL) {										// write the output files
		fwrite( & frame, sizeof(CSL_LPC_FRAME), 1, mCoeffFile);		// write frame header
		fwrite( pcCoeffPtr, sizeof(float), mLPCOrder, mCoeffFile);	// reflection coefficients
		mResidFile->writeBuffer(mLPCResidual);					// residual sound file
	}
	if ( ! mOverwriteOutput)
		memcpy(outPtr, filterOutPtr, (numFrames * sizeof(sample)));	// copy residual to out
	memcpy(filterInPtr, filterInPtr + numFrames, (mLPCOrder * sizeof(sample)));	// shift filter out buffer for next pass
	return;
}

//
// LPCFilter -- an effect that filters its input according to an LPC file
//

LPCFilter :: LPCFilter(UnitGenerator & in, char * lpcFile) : Effect(in) {
	mCopyPolicy = kCopy;
	mTempBuffer.setSize(1, CGestalt::blockSize() + CSL_LPC_MAXPOLES);
	mTempBuffer.allocateBuffers();
	
	mLPCFile = fopen(lpcFile, "rb");			// open and read the input file's header
	if (mLPCFile == NULL) {
		perror("Cannot open coefficient file");
		return;
	}
	CSL_LPC_HEADER head;
	fread( & head, sizeof(CSL_LPC_HEADER), 1, mLPCFile);
	if (head.keyword != CSL_LPC_LKEY) {
		logMsg(kLogError, "The file %s is not an LPC filter file (%x)", lpcFile, head.keyword);
		return;
	}
	mLPCOrder = head.order;			// store the analysis parameters
	mWindowSize = head.windowSize;
	mHopSize = head.hopSize;
	mDeEmphasis = head.preEmphasis;
}

LPCFilter :: LPCFilter(UnitGenerator & in, Buffer & lpcData, unsigned size, unsigned thopSize, unsigned order) 
			: Effect(in), mLPCFile(NULL), mLPCBuffer(lpcData) {
	mLPCOrder = order;
	mWindowSize = size;
	mHopSize = thopSize;
	mDeEmphasis = 0.0;
	mCopyPolicy = kCopy;
	mTempBuffer.setSize(1, CGestalt::blockSize() + CSL_LPC_MAXPOLES);
	mTempBuffer.allocateBuffers();
}

LPCFilter :: ~LPCFilter() {
	if (mLPCFile == NULL)
		fclose(mLPCFile);
}

// Here's the resynthesis
	
void LPCFilter :: nextBuffer(Buffer& outBuf, unsigned outBufNum) throw (CException) {
	unsigned numFrames = outBuf.mNumFrames;		// get buffer length
	if (numFrames > mHopSize)
		numFrames = mHopSize;
	Effect :: pullInput(numFrames);						// get the input samples via Effect
	sample * inPtr = mInputPtr;							// set up pointers
	sample * outPtr = outBuf.mBuffers[outBufNum];
	sample * tempPtr = mTempBuffer.mBuffers[0];
	float lpcCoeff[CSL_LPC_MAXPOLES];
	float filtCoeff[CSL_LPC_MAXPOLES];
	float Fmemd = 0.0;
	float voiced;
									// read the next window of filter coefficients
	fread(lpcCoeff, sizeof(float), mLPCOrder, mLPCFile);
									// read the error value
	fread(& voiced, sizeof(float), 1, mLPCFile);
			//	Convert predictor filter to error filter
			//	SPpcXec (const float in[], float out[], int out_size)
	SPpcXec (lpcCoeff, filtCoeff, mLPCOrder);								// Convert predictor filter to error filter
			//	Apply an all-pole IIR filter to the data
			//	FIfFiltAP (const float in[], float out[], int in_size, const float coefficients[], int num_coeffs)
			//	the first num_coeffs - 1 values of out[] are past outputs y[n]
	FIfFiltAP (inPtr, tempPtr, numFrames, filtCoeff, mLPCOrder+1);			// Filter the data
			//	FIfDeem (double factor, float *temp_val, const float in[], float out[], int out_size)
	FIfDeem (mDeEmphasis, &Fmemd, &tempPtr[mLPCOrder], outPtr, numFrames);		// Deemphasize the signal
	memcpy(outPtr, tempPtr, (numFrames * sizeof(sample)));
}
