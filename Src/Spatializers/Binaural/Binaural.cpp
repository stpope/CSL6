///
///  Binaural.cpp -- HRTF-based binaural panner/spatializers
///
/// Classes
///		BinauralPanner: place sources in 3D using block-wise convolution with an HRTF.
///		BinauralSourceCache: used for caching previous state of spatial sources.
///
///	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
///	Created by Jorge Castellanos on 7/19/06.
///	Rewritten for FFT wrappers and pluggable sound file APIs in 8/09 by STP.
///	Inspired by and partially based on the VST HRTF Plug-in written by Ryan Avery.
///

#include "Binaural.h"

using namespace csl;
using namespace std;

//
// BinauralPanner constructor (blockSize typ. 512, FFT 1024, spect[513])
//

BinauralPanner::BinauralPanner(unsigned blockSize) 
			: SpatialPanner(),					// inherited constructor
												// set up FFT wrappers
			  mInFFT(blockSize * FFT_DOWNS, CSL_FFT_COMPLEX, CSL_FFT_FORWARD),		// forward
			  mOutFFT(blockSize * FFT_DOWNS, CSL_FFT_COMPLEX, CSL_FFT_INVERSE),		// inverse
			  mInBuf(1, blockSize * 2),			// dbl length
			  mTmpBuf(1, blockSize * 2 + 8),	// complex #s
			  mOutBuf(2, blockSize * 2),		// dbl length
			  mBlockInd(0) {					// starting index
	unsigned hrtfLength, winSize;				// set up panner variables
	mFramesPerBlock = blockSize;				// 512
	mNumBlocks = HRTFDatabase::Database()->numBlocks();		// 16
#ifdef IPHONE
	mNumBlocksToSum = mNumBlocks / SUM_DOWNS;				// sum part of the HRTF
#else
	mNumBlocksToSum = mNumBlocks;							// sum the whole HRTF
#endif
	hrtfLength = HRTFDatabase::Database()->hrtfLength();	// 512
	winSize = HRTFDatabase::Database()->windowSize();		// 1024
	mTmpBuf.mType = kSpectra;								// temp buffer is complex spectral data
	setCopyPolicy(kIgnore);									// so the CopyPolicy doesn't overide panning
	setNumChannels(2);										// be stereo

	SAFE_MALLOC(mIFFTOutL, sample, winSize);				// allocate real sample buffers
	SAFE_MALLOC(mIFFTOutR, sample, winSize);
	SAFE_MALLOC(mHOutL, SampleComplex, hrtfLength + 1);		// and complex spectral sums
	SAFE_MALLOC(mHOutR, SampleComplex, hrtfLength + 1);
	mInBuf.allocateBuffers();								// this holds the input samples;
															// the other buffers are ptrs only
}

// Destructor frees space

BinauralPanner::~BinauralPanner() {
	SAFE_FREE(mIFFTOutL);
	SAFE_FREE(mIFFTOutR);
	SAFE_FREE(mHOutL);
	SAFE_FREE(mHOutR);
}

// Returns a pointer to an alocated cache data for its own use.

void * BinauralPanner::cache() {
	return (void *)(new BinauralSourceCache(this)); 
}

//
// BinauralPanner::nextBuffer() does the heavy lifting of the block-wise convolution
//

void BinauralPanner::nextBuffer(Buffer & outputBuffer) throw (CException) {

	HRTFDatabase * hrtfDatabase = HRTFDatabase::Database();
	unsigned numFrames = outputBuffer.mNumFrames;
	unsigned tmpBufPtr, tmpHRTFPtr = 0;
	unsigned hrtfLength = hrtfDatabase->hrtfLength();
	unsigned hrtf2Use = hrtfLength / LEN_DOWNS;
	unsigned winSize = hrtfDatabase->windowSize();
#ifdef CSL_DEBUG
	logMsg("BinauralPanner::nextBuffer");
#endif
	if (outputBuffer.mNumChannels != 2) {
		logMsg(kLogError, "BinauralPanner needs a stereo output buffer");
		return;
	}
	if (numFrames != mFramesPerBlock) {
		logMsg(kLogError, 
			"BinauralPanner::nextBuffer # frames %d wrong for HRTF size %d (use a block resizer).", 
			numFrames, mFramesPerBlock);
		return;
	}
	outputBuffer.zeroBuffers();									// clear output

	for (unsigned i = 0; i < mSources.size(); i++) {			// i loops through sources
	
		SpatialSource * source = (SpatialSource *) mSources[i];	// Get the sound source

		if (source->isActive()) {								// if source is active
			mInBuf.mNumFrames = winSize;						// set in # frames
			mInBuf.zeroBuffers();								// clear buffer
			mInBuf.mNumFrames = numFrames;						// set in # frames
			bool samePos = ! source->positionChanged();			// check this before we grab the input

			source->nextBuffer(mInBuf);							// get input from the source
			
////////														// debugging: copy input to output
//			memcpy(outputBuffer.buffer(0), mInBuf.buffer(0), numFrames * sizeof(sample));
//			memcpy(outputBuffer.buffer(1), mInBuf.buffer(0), numFrames * sizeof(sample));
//			return;
////////														// Get the cached data	
			BinauralSourceCache * tcache = (BinauralSourceCache *) mCache[i]; 
																// set input complex fft pointer
			mTmpBuf.setBuffer(0, (SampleBuffer) tcache->mInSpect[mBlockInd]);
			mInBuf.mNumFrames = winSize;						// dbl length

			mInFFT.nextBuffer(mInBuf, mTmpBuf);					// Calculate input FFT into cache->mInSpect

			if (samePos) {						// if the position of the source is the same.
				if (mBlockInd > 0)				// assign the same HRTF you had the previous round
					tcache->mHRTF[mBlockInd] = tcache->mHRTF[mBlockInd - 1];
				else 
					tcache->mHRTF[mBlockInd] = tcache->mHRTF[mNumBlocks - 1];
					
			} else {							// if the position changed, then find the new HRTF
				unsigned hrtfIndex = hrtfDatabase->hrtfAt(*source->position());
				tcache->mHRTF[mBlockInd] = hrtfIndex;
//				CPoint hrtfPosition = hrtfDatabase->hrtfAt(localHRTF)->mPosition;
//				logMsg("\t\t\t\tHRTF: %5.1f @ %5.1f \t Source: %5.1f @ %5.1f", 
//					hrtfPosition.theta() * CSL_DEGS_PER_RAD, hrtfPosition.ele() * CSL_DEGS_PER_RAD, 
//					source->position()->theta() * CSL_DEGS_PER_RAD, source->position()->ele() * CSL_DEGS_PER_RAD);
			}
												// zero the complex spectrum buffers before summing loop
			memset(mHOutL, 0, hrtf2Use * sizeof(SampleComplex));
			memset(mHOutR, 0, hrtf2Use * sizeof(SampleComplex));
			
												// Do the convolution of HRTF with the input spectra
			tmpBufPtr = mBlockInd;				// set the block counter to the current block
			do {								// Go thru previous HRTFs doing complex multiply/accumulate
				HRTF * tempHRTF = hrtfDatabase->hrtfAt(tcache->mHRTF[tmpBufPtr]);
				for (unsigned j = 0; j < hrtf2Use; j++) {				// loop over buffers of past FFT out
					ComplexPtr in1j = tcache->mInSpect[tmpBufPtr][j];	// input spectrum
					ComplexPtr inLj = tempHRTF->mHrtfL[tmpHRTFPtr][j];	// L/R HRTF
					ComplexPtr inRj = tempHRTF->mHrtfR[tmpHRTFPtr][j];
					ComplexPtr outLj = mHOutL[j];						// L/R summation buffers
					ComplexPtr outRj = mHOutR[j];
					cmac(in1j, inLj, outLj);							// complex MAC macro
					cmac(in1j, inRj, outRj);
												// old-way: complex MAC loop with 2D arrays
//					cmac(cache->mInSpect[tmpBufPtr][j], tempHRTF->mHrtfL[tmpHRTFPtr][j], mHOutL[j]);
//					cmac(cache->mInSpect[tmpBufPtr][j], tempHRTF->mHrtfR[tmpHRTFPtr][j], mHOutR[j]);
				}
				if (++tmpBufPtr >= mNumBlocksToSum)
					tmpBufPtr = 0;
				if (++tmpHRTFPtr >= mNumBlocksToSum)
					tmpHRTFPtr = 0;
			} while (tmpBufPtr != mBlockInd);				// end of loop through prior inputs and HRTF spectra
			
															// debugging: overwrite spectral sum with input
//			memcpy(mHOutL, cache->mInSpect[mBlockInd], hrtfLength * sizeof(SampleComplex));
//			memcpy(mHOutR, cache->mInSpect[mBlockInd], hrtfLength * sizeof(SampleComplex));
															// Output IFFT stage
			mTmpBuf.mNumFrames = winSize;					// set up output buffer
			mTmpBuf.setBuffer(0, (SampleBuffer) mHOutL);	// IFFT input = summation buffer
			mOutBuf.setBuffer(0, mIFFTOutL);				// IFFT out = sample buffer

			mOutFFT.nextBuffer(mTmpBuf, mOutBuf);			// do left IFFT

			mTmpBuf.setBuffer(0, (SampleBuffer) mHOutR);
			mOutBuf.setBuffer(0, mIFFTOutR);

			mOutFFT.nextBuffer(mTmpBuf, mOutBuf);			// do right IFFT

			SampleBuffer outL = outputBuffer.buffer(0);	// Get pointers to output buffers (stereo)
			SampleBuffer outR = outputBuffer.buffer(1);
			SampleBuffer ifftL = mIFFTOutL;					// IFFT real out data ptrs
			SampleBuffer ifftR = mIFFTOutR;
			SampleBuffer cacheL = tcache->mPrevOutL;			// previous real output caches
			SampleBuffer cacheR = tcache->mPrevOutR;
#ifdef USE_FFTW
			sample scale = 1.0f / (float) numFrames;		// window size scale
#else
			sample scale = 1.0f ; //  / sqrtf(numFrames);
//			sample scale = 1.0f  / sqrtf(numFrames);
#endif
			for (unsigned j = 0; j < numFrames; j++) {			// Overlap and add loop
				*outL++ += ((*ifftL++ + *cacheL++) * scale);	// sum/scale IFFT & cache 
				*outR++ += ((*ifftR++ + *cacheR++) * scale);
			}
															// copy top-half of output to cache
			memcpy(tcache->mPrevOutL, mIFFTOutL + numFrames, numFrames * sizeof(sample));
			memcpy(tcache->mPrevOutR, mIFFTOutR + numFrames, numFrames * sizeof(sample));
		}							// end if source active
	}								// end source loop
	
	if (mBlockInd == 0)										// test/decrement panner's block counter
		mBlockInd = mNumBlocksToSum;						// for next call
	mBlockInd--;
}


// BinauralSourceCache constructor

BinauralSourceCache::BinauralSourceCache(BinauralPanner * parent) {
	mNumBlocks = parent->mNumBlocks;
	unsigned framesPerBlock = parent->mFramesPerBlock;
	unsigned hrtfLength = HRTFDatabase::Database()->hrtfLength() + 1;
//	unsigned windowSize = HRTFDatabase::Database()->windowSize();
	
								// allocate buffers
	SAFE_MALLOC(mPrevOutL, sample, framesPerBlock);
	SAFE_MALLOC(mPrevOutR, sample, framesPerBlock);
	SAFE_MALLOC(mHRTF, unsigned, mNumBlocks);

								// create/zero the complex arrays for past inp FFTs
	SAFE_MALLOC(mInSpect, SampleComplexVector, mNumBlocks);
	for (unsigned i = 0; i < mNumBlocks; i++) {
		SAFE_MALLOC(mInSpect[i], SampleComplex, hrtfLength); 
	}
}

BinauralSourceCache::~BinauralSourceCache() {
	for (unsigned i = 0; i < mNumBlocks; i++) {
		SAFE_FREE(mInSpect[i]);
	}
	SAFE_FREE(mInSpect);
	SAFE_FREE(mPrevOutL);
	SAFE_FREE(mPrevOutR);
	SAFE_FREE(mHRTF);
}
