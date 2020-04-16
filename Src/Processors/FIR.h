///
/// FIR.h -- CSL filter specification and FIR filter classes
///	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
///
/// The FilterSpecification uses the Parks-McClellan/Remez iterative algorithm
/// It is provided under GPL by Jake Janovetz (janovetz@uiuc.edu)
///
/// The FIR implementation is based on a minimal version written for the MAT 240B course;
/// it does not use the CSL RingBuffer helper class.

#ifndef CSL_FIR_H
#define CSL_FIR_H

#include "CSL_Core.h"

namespace csl {


class FIR; 	///< forward declaration

///
/// FilterSpecification class for designing multi-band-pass FIR filter impulse responses
///

class FilterSpecification {
public:

	friend class FIR;  ///< Allow the FIR to access private members of this class.

					/// Constructors
	FilterSpecification(unsigned numTaps = 0, unsigned numBands = 0, double *freqs = NULL, double *resps = NULL, double *weights = NULL);
	~FilterSpecification();
	
					/// Accessors
	void setFrequencies(double *frequencies);
	void setResponses(double *responses);
	void setWeights(double *weights);
	void setNumTaps(unsigned numTaps);
	unsigned mNumTaps;		///< number of taps desired

	void planFilter();		///< method to plan the filter (execute the search/iterate algorithm)	
	double * mTapData;		///< the FIR tap weights (created by the planFilter method)

protected:
	unsigned mNumBands;		///< length of specification
	double * mFrequencies;	///< band edge frequencies (2 * mNumBands)
	double * mResponses;		///< band responses (mNumBands)
	double * mWeights;		///< band error weights (mNumBands)

};

/// Examples
///
///	-- Simple LPF (2 bands) at 0.2 Fs with 0.05-width transition bands
///	responses =	{    1       x        0 };
///	freqs = 	{ 0    0.2   0.25    0.5 };
///	weights = 	{   10               20 };
///	
///	-- basic BPF (3 bands) between 0.2 and 0.3 Fs with 0.05-width transition bands
///	responses =	{    0.5        x      1      x         0.8 };
///	freqs = 	{ 0    0.15   0.2   0.3   0.35    0.5 };
///	weights = 	{   20               5                20 };
///	
///	-- Fancier dual-BPF
///	responses =	{    0        x      1       x         0        x      1        x       0 };
///	freqs = 	{ 0   0.05   0.1   0.15   0.18   0.25   0.3   0.36   0.41   0.5 };
///	weights = 	{   10               1                  3                1                20 };
///

///
/// FIR Filter class
///

class FIR : public Effect {

public:						/// Various constructors
							/// Takes a UGen, and optionally the number of taps and the tap IR array.
	FIR (UnitGenerator & in, unsigned numTaps = 2,  float * tapDelay = NULL);
//	FIR (char * file_name);									///< read data from a file
	FIR (FilterSpecification & fs);							///< give it a filter specification object
	FIR (UnitGenerator & in, char * fileName);
	FIR (UnitGenerator & in, FilterSpecification & fs);
	~FIR ();
	
	void setTaps(unsigned numTaps,  float *tapDelays);
	void readTaps(char *fileName);
	void print_taps();
					/// The work method...
	void nextBuffer(Buffer &outputBuffer, unsigned outBufNum) throw (CException);

protected:
	FilterSpecification *mFilterSpec;
	unsigned mOffset;		///< offset "pointer" for loop counting
							/// Here are the sample buffers (dynamically allocated)
	sample *mDLine;			///< mNumTaps length delay line
	
	void resetDLine();		///< zero-out mDline and reallocate memory if necessary;
	
							/// Parks-McClellan/Remez FIR filter design algorithm
	void remez(double h[], int numtaps, int numband, double bands[], double des[], double weight[], int type);

};

}

#endif
