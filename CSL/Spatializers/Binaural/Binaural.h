///
///  Binaural.h -- Specification of the HRTF-based binaural spatializer.
///		This is the CSL 5 version that uses the FFT wrapper framework
///
/// Classes
///		BinauralPanner: place sources in 3D using block-wise convolution with an HRTF;
///		best heard over headphones. Supports multiple moving sources and does cached
///		multi-block FFT/IFFT convolution.
///
///		BinauralSourceCache: used for caching previous state of spatial sources.
///
/// The companion file has the classes
///		HRTF: holds the data that corresponds to an HRTF for a single position.
///		HRTFDatabase: vector of HRTFs; implemented as a Singleton because it's large.
///
///	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
///	Created by Jorge Castellanos on 7/19/06.
///	Inspired by and partially based on the VST HRTF Plug-in written by Ryan Avery.
///	Rewritten for FFT wrappers and pluggable sound file APIs in 8/09 by STP.
///
/// @todo HRTFs should be equalized, currently they are being loaded and used raw.
/// @todo Interpolation of HRTFs. Currently the closest HRTF is being used.
///

#ifndef CSL_BINAURAL_H
#define CSL_BINAURAL_H

#include "BinauralDB.h"

#ifdef IPHONE
	#define SUM_DOWNS	1		// scale # blocks to sum   (16 / SUM_DOWNS)
	#define FFT_DOWNS	2		// scale IFFT len (FFT_DOWNS * blockSize)
	#define LEN_DOWNS	1		// scale sum len (513 / LEN_DOWNS)
#else
	#define SUM_DOWNS	1		// scale # blocks to sum   (16 / SUM_DOWNS)
	#define FFT_DOWNS	2		// scale IFFT len (FFT_DOWNS * blockSize)
	#define LEN_DOWNS	1		// scale sum len (513 / LEN_DOWNS)
#endif

namespace csl {

class BinauralSourceCache;					///< forward declaration

/// BinauralPanner: place sources in 3D using block-wise convolution with an HRTF.
/// The HRIRs (Head Related Impulse Responses) are loaded by the HRTFDatabase. 
/// The binaural panner uses these HRTFs and processes the incoming audio by
/// frequency domain convolution in blocks of 512 samples.

class BinauralPanner : public SpatialPanner {

public:
	BinauralPanner(unsigned blockSize = HRTF_BLOCK_SIZE);
	~BinauralPanner();
											/// work-horse method reads & transforms input, 
											/// sums previous data, and takes the IFFT for 
											/// each of multiple sources
	void nextBuffer(Buffer &outputBuffer) throw (CException);

	unsigned mNumBlocks;					///< # blocks per HRTF
	unsigned mNumBlocksToSum;				///< # blocks to include in sum per HRTF
	unsigned mFramesPerBlock;				///< # frames per block

protected:
	FFT_Wrapper mInFFT;						///< FFT Wrappers for in and out processing
	FFT_Wrapper mOutFFT;
	
	Buffer mInBuf, mTmpBuf, mOutBuf;		///< temp buffers for input and FFT wrappers
	
	SampleComplexVector mHOutL, mHOutR;		///< buffers for the mixed HRTF & input data pre-IFFT
	SampleBuffer mIFFTOutL, mIFFTOutR;		///< outputs of the IFFT

	unsigned mBlockInd;						///< block index
	void * cache();							///< Returns an instance of its cache data per sound source
};


/// BinauralSourceCache: used for caching previous state of spatial sources.
/// This has the history of input FFTs and the last output sample block for OLA

class BinauralSourceCache {
public:
	BinauralSourceCache(BinauralPanner *parent);
	~BinauralSourceCache();

	SampleComplexVector *mInSpect;			///< complex spectra of mNumBlocks past inputs
	SampleBuffer mPrevOutL, mPrevOutR;		///< previous sample window from the IFFT
	unsigned *mHRTF;						///< list of block IDs of the "split" hrtf
	unsigned mNumBlocks;					///< size of caches (16)
};

} // end namespace

#endif
