//
//	LPC.h -- specification of the classes that perform LPC-based analysis/resynthesis.
//	These use Peter Kabal's libtsp routines (http://WWW.TSP.ECE.McGill.CA) for the heavy lifting.
//	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
//

#ifndef CSL_LPC_H
#define CSL_LPC_H

#include "CSL_Includes.h"

namespace csl {

// File header and ferame srtuct for LPC data files
// This could use SDIF or add chunk types to AIFF files

// File header

typedef struct lpc_header {
	unsigned int keyword;		// magic #, chunk type
	unsigned int filesize;
	float duration;
	unsigned int numframes;
	unsigned int order;
	unsigned int windowSize;
	unsigned int hopSize;
	double preEmphasis;
} CSL_LPC_HEADER;

// Frame header - similar to an MIR database record; holds frame's derived ampl-domain and freq-domain data

typedef struct frame_header {
	unsigned int keyword;
	unsigned int framesize;
	unsigned int order;		// size of LPC refl. coeff. array
	unsigned int windowSize;	// frame/window size
	float rmsamp;			// RMS ampl
	float hifreq;				// hpf energy > 2 kHz
	float lofreq;				// lpf energy < 200 Hz
	float freqest1;			// autocorr freq estimate
	float freqest2;			// HPS freq estimate
	unsigned int zeroes;		// count of zero-crossings
	float smoothfreq;		// smoothed freq estimate
	float noisiness;			// autocorr noise estimate
	float resid;				// LPC residual energy
	float formant1;			// LPC 1st formant
	float formant2;			// LPC 2nd formant
	float centroid;			// spectral centroid
//	float mfccPeak1;			// 1st MFCC peak
//	float mfccPeak2;			// 2nd MFCC peak
	unsigned int MIDIKey;		// MIDI key #
	unsigned int isTransition;	// state flag (note-on, note-off, continuation, plosive, etc.
//	float[order] data;			// LPC data frame - reflection coefficients
} CSL_LPC_FRAME;

#define CSL_LPC_LKEY 0x55120942			// Magic number for files
#define CSL_LPC_FKEY 0x55120943			// Magic number for frames
#define CSL_LPC_MAXPOLES	150			// max pre-allocated frame size
#define CSL_HPS_DEPTH	10				// size of harmonic product spectrum reduction

///
/// LPCAnalyzer class takes LPC analysis and returns coefficients and the residual
/// It can write these to two files (if you set the file names), or signal its observers
/// (who would grab its internal state via the getter methods)
///

class LPCAnalyzer : public Effect {
public:
	LPCAnalyzer(UnitGenerator & in, unsigned size = CGestalt::blockSize(), unsigned hopSize = CGestalt::blockSize(), unsigned order = 50);
	~LPCAnalyzer();
							/// no setters
	unsigned windowSize() { return mWindowSize; }
	unsigned hopSize() { return mHopSize; }
	unsigned LPCOrder() { return mLPCOrder; }
	Buffer getCoefficients() { return mLPCCoefficients; }
	Buffer getResidual() { return mLPCResidual; }
	double getError() { return mPredErr; }

	void setFileNames(char * coeff, char * resid);	///< set the file names and open the files
	void setPreEmphasis(double factor) { mPreEmphasis = factor; }
	void setBWExpansion(double factor) { mBWExpansion = factor; }
	void closeFiles();						///< close the files
	void setOverwrite(bool reuse) { mOverwriteOutput = reuse; }
	void setUseWindow(bool use) { mUseWindow = use; }
										///< analyze a buffer, write it out if the files are set, then signal your observers
	void nextBuffer(Buffer& outputBuffer, unsigned outBufNum) throw (CException);
	
protected:	
	unsigned mWindowSize;				///< input window size
	unsigned mHopSize;					///< input hop size
	unsigned mLPCOrder;					///< LPC order
	double mPredErr;						///< LPC pred.error
	double mPreEmphasis;				///< LPC pre-emph factor
	double mBWExpansion;				///< LPC bandwidth expansion factor
	bool mOverwriteOutput;				///< whether to replace the output with the input (or the residual) after signalling observers
	bool mUseWindow;					///< whether to use a rectangular or Hamming window
							/// Protected buffers and analysis objects
	Buffer mTempBuffer;					///< the input buffer
	Buffer mWindowBuffer;				///< Buffer to store window
	Buffer mLPCCoefficients;				///< the LPC reflection coefficients
	Buffer mFilterCoefficients;			///< the LPC filter coefficients
	Buffer mFilterInput;					///< the LPC filter input store
	Buffer mPCCoefficients;				///< the LPC PC coefficients
	Buffer mLPCResidual;					///< the LPC residual
	
	RingBuffer mRing;					///< used for the analysis UGens
							/// Filters and FFT for analysis
	Butter * mLoPass;					///< low-pass filter
	Butter * mHiPass;						///< hi-pass filter
	FFT * mFFT;							///< FFT for pitch est.
							// Output files
	FILE * mCoeffFile;					///< file ptr for the coefficient file
	SoundFile * mResidFile;				///< sound file for the residual file
	
};

//	typical BW expansion factor in range 0.996 - 0.990
//		b           BW exp
//		1.000      0
//		0.996     10.2 Hz
//		0.995     12.8 Hz
//		0.994     15.3 Hz
//		0.990     25.6 Hz
//		0.980     51.4 Hz

///
/// LPCFilter is also an effect; it reads an input (residual) and filters its according to an LPC-derived FIR filter
///

class LPCFilter : public Effect, public Scalable {
public:
	LPCFilter(UnitGenerator & in, char * lpcFile);
	LPCFilter(UnitGenerator & in, Buffer & lpcData, unsigned size, unsigned hopSize, unsigned order);
	~LPCFilter();

	unsigned windowSize() { return mWindowSize; }
	unsigned hopSize() { return mHopSize; }
	unsigned LPCOrder() { return mLPCOrder; }
	double deEmphasis() { return mDeEmphasis; }
	
	Envelope * timeEnvelope() { return mTimeEnvelope; }
	void setTimeEnvelope(Envelope * env) { mTimeEnvelope = env; }
	
	void nextBuffer(Buffer& outputBuffer, unsigned outBufNum) throw (CException);	///< read some input and apply a filter to it

protected:	
	unsigned mWindowSize;			///< input window size
	unsigned mHopSize;				///< input hop size
	unsigned mLPCOrder;				///< LPC order
	double mDeEmphasis;
	
	Envelope * mTimeEnvelope;		///< playback time warp envelope (0-1)

	FILE * mLPCFile;					///< file ptr for the coefficient file
	Buffer mLPCBuffer;				///< (or) the coefficient buffer

	Buffer mTempBuffer;				///< the past output buffer

};

}

#endif
