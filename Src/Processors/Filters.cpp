//
//  Filters.cpp -- implementation of the base Filter class and its standard subclasses
//
//	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
//

#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "Filters.h"

using namespace csl;

// FrequencyAmount -- Constructors

FrequencyAmount::FrequencyAmount() {
#ifdef CSL_DEBUG
	logMsg("FrequencyAmount::add null inputs");
#endif		
}

FrequencyAmount::~FrequencyAmount() { /* no-op for now */ }

// FrequencyAmount -- Accessors

void FrequencyAmount::setFrequency(UnitGenerator & frequency) { 
	this->addInput(CSL_FILTER_FREQUENCY, frequency);
#ifdef CSL_DEBUG
	logMsg("FrequencyAmount::set scale input UG");
#endif		
}

void FrequencyAmount::setFrequency(float frequency) { 
	this->addInput(CSL_FILTER_FREQUENCY, frequency);
#ifdef CSL_DEBUG
	logMsg("FrequencyAmount::set scale input value");
#endif		
}

float FrequencyAmount::getFrequency() {
	return(getPort(CSL_FILTER_FREQUENCY)->nextValue());
}

void FrequencyAmount::setAmount(UnitGenerator & amount) { 
	this->addInput(CSL_FILTER_AMOUNT, amount);
#ifdef CSL_DEBUG
	logMsg("FrequencyAmount::set offset input UG");
#endif		
}

void FrequencyAmount::setAmount(float amount) { 
	this->addInput(CSL_FILTER_AMOUNT, amount);
#ifdef CSL_DEBUG
	logMsg("FrequencyAmount::set offset input value");
#endif		
}

/// Generic Filter class with scalable order and generic next_buffer method
/// that implememnts the canonical filter diference equation.
/// Subclasses must supply filter order and override the setupCoeffs() method.

/// Default constructor generates a zeroth order "do-nothing" filter
Filter::Filter () : Effect(), Scalable(1.f,0.f) {
	init(1,1);
};


Filter::Filter(unsigned num_b, unsigned num_a) : Effect(), Scalable(1.f,0.f) {
	init(num_a, num_b);
}

Filter::Filter(UnitGenerator & in, unsigned num_b, unsigned num_a) : Effect(in), Scalable(1.f,0.f) {
	init(num_a, num_b);
};

/// This constructor takes arrays of coefficients and constructs the filter accordingly.
Filter::Filter(UnitGenerator & in, SampleBuffer bCoeffs, SampleBuffer aCoeffs, unsigned num_b, unsigned num_a) 
		: Effect(in), Scalable(1.f,0.f) {
	init(num_a, num_b);
	setupCoeffs(bCoeffs, aCoeffs, num_b, num_a);
};

void Filter::init(unsigned a, unsigned b) {
	mBNum = b;
	mANum = a;
	mBCoeff[0] = 1.f;
	mACoeff[0] = 1.f;
	mPrevOutputs = new Buffer(1, mANum);
	mPrevInputs = new Buffer(1, mBNum);
	mPrevOutputs->allocateBuffers();
	mPrevInputs->allocateBuffers();
	mFrameRate = CGestalt::frameRate();
}

/// Filter destructor frees temp memory

Filter::~Filter (void) { 
	if (mPrevOutputs) delete mPrevOutputs;
	if (mPrevInputs) delete mPrevInputs;
};

// This version does in-place filtering

void Filter::nextBuffer(Buffer & outputBuffer, unsigned outBufNum) throw (CException) {
#ifdef CSL_DEBUG
	logMsg("Filter nextBuffer");
#endif	
	SampleBuffer out = outputBuffer.buffer(outBufNum);		// get ptr to output channel
	unsigned numFrames = outputBuffer.mNumFrames;				// get buffer length
	SampleBuffer prevOuts = mPrevOutputs->buffer(0);
	SampleBuffer prevIns = mPrevInputs->buffer(0);	
	SampleBuffer inputPtr;

	DECLARE_SCALABLE_CONTROLS;						// declare the scale/offset buffers and values
	DECLARE_FILTER_CONTROLS;						// declare the freq/bw buffers and values
	LOAD_SCALABLE_CONTROLS;
	LOAD_FILTER_CONTROLS;

	bool isDynamic = false;
	if ((freqPort && ( ! freqPort->isFixed())) || (bwPort && ( ! bwPort->isFixed())))
		isDynamic = true;

	if (! isInline) {
		Effect::pullInput(numFrames);				// get some input
		inputPtr = mInputPtr;						// get a pointer to the input samples
	} else
		inputPtr = out;

	SampleBuffer prevOPtr = prevOuts;
	SampleBuffer prevIPtr = prevIns;
		
	for (unsigned i = 0; i < numFrames; i++) {		// here's the canonical N-quad filter loop
		if (isDynamic)
			this->setupCoeffs();					// calculate new coefficients for next sample
		*prevOPtr = 0.f;
		*prevIPtr = scaleValue * *inputPtr++ + offsetValue;		// get next input sample, scale & offset
		for (unsigned j = mBNum - 1; j > 0; j--) {
			*prevOPtr += mBCoeff[j] * prevIns[j];				// prevIns or prevOuts?
			prevIns[j] = prevIns[j-1];
		}
		*prevOPtr += mBCoeff[0] * prevIns[0];
		for (unsigned j = mANum - 1; j > 0; j--) {
			*prevOPtr += -mACoeff[j] * prevOuts[j];
			prevOuts[j] = prevOuts[j-1];
		}
													// put current output in the buffer and increment
		*out++ = (*prevOPtr * scaleValue) + offsetValue;	

		UPDATE_SCALABLE_CONTROLS;					// update the dynamic scale/offset
	} 
}

/// this version is to be inherited by the subclasses. provides a way to directly supply the filter info

void Filter::setupCoeffs(SampleBuffer bCoeffs, SampleBuffer aCoeffs, unsigned num_b, unsigned num_a ) {
	for (unsigned i = 0; i < num_b; i++)
		mBCoeff[i] = bCoeffs[i];
	for (unsigned i = 0; i < num_a; i++)
		mACoeff[i] = aCoeffs[i];
}

void Filter::clear(void) {
	mPrevInputs->zeroBuffers();
	mPrevOutputs->zeroBuffers();
}

/// log information about myself
void Filter::dump() {
	logMsg("a Filter");
	Scalable::dump();
	UnitGenerator::dump();
	
	fprintf(stderr, "A coefficients ");
	for (unsigned i=0;i<mANum;i++) {
		fprintf(stderr, "%.2f, ",mACoeff[i]);
	}
	fprintf(stderr, "\n");
	fprintf(stderr, "B coefficients ");
	for (unsigned i=0;i<mBNum;i++) {
		fprintf(stderr, "%.2f, ",mBCoeff[i]);
	}
	fprintf(stderr, "\n");
}

#pragma mark Butter

/// Butterworth IIR (2nd order recursive) filter.
/// This operates upon a buffer of frames of amplitude samples by applying the following equation
/// y(n) = a0*x(n) + mACoeff[1] *x(n-1) + mACoeff[2] *x(n-2) - b1*y(n-1) - b2*y(n-2) where x is an amplitude sample.
/// It has constructors that can calculate the coefficients from a given cutoff frequency.

Butter::Butter () { }

Butter::Butter (FilterType type, float cutoff) : Filter(3,3) {
	mFilterType = type;
	setFrequency(cutoff);
	setAmount(cutoff / 10.0);
	setupCoeffs();
	clear(); 
}

Butter::Butter (UnitGenerator & in, FilterType type, float cutoff) : Filter(in,3,3) {
	mFilterType = type;
	setFrequency(cutoff);
	setAmount(cutoff / 10.0);
	setupCoeffs();
	clear(); 
}

Butter::Butter (UnitGenerator & in, FilterType type, UnitGenerator & cutoff) : Filter(in,3,3) {
	mFilterType = type;
	setFrequency(cutoff);
	setAmount(100);
	setupCoeffs();
	clear(); 
}

// constructor for dual filter parameters (i.e. band pass and reject)

Butter::Butter (FilterType type, float center, float bandwidth) : Filter(3,3) {
	mFilterType = type;
	setFrequency(center);
	setAmount(bandwidth);
	setupCoeffs();
	clear(); 
}

Butter::Butter (UnitGenerator & in, FilterType type, float center, float bandwidth)
		: Filter(in,3,3) {
	mFilterType = type;
	setFrequency(center);
	setAmount(bandwidth);
	setupCoeffs();
	clear(); 
}

Butter::Butter (UnitGenerator & in, FilterType type, UnitGenerator & center, UnitGenerator & bandwidth)
		: Filter(in,3,3) {
	mFilterType = type;
	setFrequency(center);
	setAmount(bandwidth);
	setupCoeffs();
	clear(); 
}

//	 Calculate the filter coefficients based on the frequency characteristics

void Butter::setupCoeffs () {
	float C, D; 					// handy intermediate variables
	float centreFreq = mInputs[CSL_FILTER_FREQUENCY]->nextValue();
	float bandwidth = mInputs[CSL_FILTER_AMOUNT]->nextValue();

	mACoeff[0] = 0.f;
	switch (mFilterType) {			// These are the Butterworth equations
		case BW_LOW_PASS :	
			C = 1 / (tanf (CSL_PI * (centreFreq/mFrameRate)) );
			mBCoeff[0]	= 1 / (1 + (CSL_SQRT_TWO * C) + (C * C) );
			mBCoeff[1]	= 2 * mBCoeff[0];
			mBCoeff[2]	= mBCoeff[0];
			mACoeff[1]	= 2 * mBCoeff[0] * (1 - (C * C));
			mACoeff[2]	= mBCoeff[0] * (1 - (CSL_SQRT_TWO * C) + (C * C) );
			break;
		case BW_HIGH_PASS :
			C = tanf (CSL_PI * centreFreq / mFrameRate);
			mBCoeff[0]	= 1 / (1 + (CSL_SQRT_TWO * C) + (C * C) );
			mBCoeff[1]	= -2 * mBCoeff[0];
			mBCoeff[2]	= mBCoeff[0];
			mACoeff[1]	= 2 * mBCoeff[0] * ((C * C) - 1);
			mACoeff[2]	= mBCoeff[0] * (1 - (CSL_SQRT_TWO * C) + (C * C) );
			break;
		case BW_BAND_PASS :
			C = 1 / (tanf (CSL_PI * bandwidth / mFrameRate) );
			D = 2 * cos (2 * CSL_PI * centreFreq / mFrameRate);
			mBCoeff[0]	= 1 / (1 + C);
			mBCoeff[1]	= 0;
			mBCoeff[2]	= -1 * mBCoeff[0];
			mACoeff[1]	= -1 * mBCoeff[0] * C * D;
			mACoeff[2]	= mBCoeff[0] * (C - 1);
			break;
		case BW_BAND_STOP :
			C = tanf (CSL_PI * bandwidth / mFrameRate);
			D = 2 * cos (2 * CSL_PI * centreFreq / mFrameRate);
			mBCoeff[0]	= 1 / (1 + C);
			mBCoeff[1]	= -1 * mBCoeff[0] * D;
			mBCoeff[2]	= mBCoeff[0];
			mACoeff[1]	= -1 * mBCoeff[0] * D;
			mACoeff[2]	= mBCoeff[0] * (1 - C);
			break;
	} // switch 
}

#pragma mark Biquad

Biquad::Biquad () { }

Biquad::Biquad (FilterType type, float freq, float q, float d) : Filter(3,3) {
	mFilterType = type;
//	setFrequency(freq);
//	setAmount(dB);
	Fc = freq / CGestalt::frameRateF();
	Q = q;
	dB = d;
	setupCoeffs();
	clear();
}

Biquad::Biquad (UnitGenerator & in, FilterType type, float freq, float q, float d)
		: Filter(in,3,3) {
	mFilterType = type;
//	setFrequency(freq);
//	setAmount(dB);
	Fc = freq / CGestalt::frameRateF();
	Q = q;
	dB = d;
	setupCoeffs();
	clear();
}

// Calculate the filter coefficients based on the frequency characteristics
// https://stackoverflow.com/questions/52547218/how-to-caculate-biquad-filter-coefficient

#ifdef OLD_BQ

void Biquad::setupCoeffs() {
	float C, D; 					// handy intermediate variables
	float centreFreq = mInputs[CSL_FILTER_FREQUENCY]->nextValue();
	float dB = mInputs[CSL_FILTER_AMOUNT]->nextValue();
	float FS = CGestalt::frameRateF();
	float w0 = 2.0f * (float) M_PI * (centreFreq / FS);
	float cosW = cosf(w0);
	float sinW = sinf(w0);
	float A = pow(10.0f, (dB / 40.0f));
	float alpha = sinW / (2.0f * Q);
	float beta = pow(A, 0.5f) / Q;

	// http://shepazu.github.io/Audio-EQ-Cookbook/audio-eq-cookbook.html
	
	switch (mFilterType) {			// These are the std. biquad equations
		case BW_LOW_PASS :
			mBCoeff[0]	= (1.0f - cosW) / 2.0f;
			mBCoeff[1]	= 1.0f - cosW;
			mBCoeff[2]	= (1.0f - cosW) / 2.0f;
			mACoeff[0] = 1.0f + alpha;
			mACoeff[1]	= -2.0f * cosW;
			mACoeff[2]	= 1.0f - alpha;
			break;
		case BW_HIGH_PASS :
			mBCoeff[0]	= (1.0f + cosW)/2.0f;
			mBCoeff[1]	= -(1.0f + cosW);
			mBCoeff[2]	= (1.0f + cosW)/2.0f;
			mACoeff[0] = (1.0f + alpha);
			mACoeff[1]	= -2.0f * cosW;
			mACoeff[2]	= 1.0f - alpha;
			break;
		case BW_BAND_PASS :
			mBCoeff[0]	= sinW / 2.0f;
			mBCoeff[1]	= 0;
			mBCoeff[2]	= -sinW / 2.0f;
			mACoeff[0] = 1.0f + alpha;
			mACoeff[1]	= -2.0f * cosW;
			mACoeff[2]	= 1.0f - alpha;
			break;
		case BW_BAND_STOP :
			mBCoeff[0]	= 1.0f;
			mBCoeff[1]	= -2.0f * cosW;
			mBCoeff[2]	= 1.0f;
			mACoeff[0] = 1.0f + alpha;
			mACoeff[1]	= -2.0f * cosW;
			mACoeff[2]	= 1.0f - alpha;
			break;
		case BW_LOW_SHELF :
			mBCoeff[0] = A* ((A + 1.0f) - ((A -1.0f) * cosW) + beta * sinW);
			mBCoeff[1] = 2.0f * A * ((A - 1.0f) - ((A + 1.0f) * cosW));
			mBCoeff[2] = A * ((A + 1.0f) - (A - 1.0f) * cosW - beta * sinW);
			mACoeff[0] = ((A + 1.0f) + (A - 1.0f) * cosW + beta * sinW);
			mACoeff[1] = -2.0f * ((A - 1.0f) + (A + 1.0f) * cosW);
			mACoeff[2] = ((A + 1.0f) + (A - 1.0f) * cosW - (beta * sinW));
			break;
		case BW_HIGH_SHELF :
			mBCoeff[0] = A * ((A + 1.0f) + (A - 1.0f) * cosW + beta * sinW);
			mBCoeff[1] = -2.0f * A * ((A - 1.0f) + (A + 1.0f) * cosW);
			mBCoeff[2] = A * ((A + 1.0f) + (A - 1.0f) * cosW - beta * sinW);
			mACoeff[0] = ((A + 1.0f) - (A - 1.0f) * cosW + beta * sinW);
			mACoeff[1] = 2.0f * ((A - 1.0f) - (A + 1.0f) * cosW);
			mACoeff[2] = ((A + 1.0f) - (A - 1.0f) * cosW - beta * sinW);
			break;
		case PEAKING :
			mBCoeff[0] = 1.0f + (alpha * A);
			mBCoeff[1] = -2.0f * cosW;
			mBCoeff[2] = 1.0f - (alpha * A);
			mACoeff[0] = 1.0f + (alpha / A);
			mACoeff[1] = -2.0f * cosW;
			mACoeff[2] = 1.0f - (alpha / A);
			break;
	} // switch
}

#else
// https://www.earlevel.com/main/2012/11/26/biquad-c-source-code/

void Biquad::setupCoeffs() {
	float norm = 0.0f;
	float V = powf(10.0f, fabs(dB) / 20.0f);
	float K = tanf((float) M_PI * Fc);
	a0 = a1 = a2 = b1 = b2 = z1 = z2 = 0.0f;
	
	switch (mFilterType) {			// These are the std. biquad equations
		case BW_LOW_PASS:
			norm = 1.0f / (1.0f + K / Q + K * K);
			a0 = K * K * norm;
			a1 = 2.0f * a0;
			a2 = a0;
			b1 = 2.0f * (K * K - 1.0f) * norm;
			b2 = (1.0f - K / Q + K * K) * norm;
			break;
		case BW_HIGH_PASS:
			norm = 1.0f / (1.0f + K / Q + K * K);
			a0 = 1.0f * norm;
			a1 = -2.0f * a0;
			a2 = a0;
			b1 = 2.0f * (K * K - 1.0f) * norm;
			b2 = (1.0f - K / Q + K * K) * norm;
			break;
		case BW_BAND_PASS:
			norm = 1.0f / (1.0f + K / Q + K * K);
			a0 = K / Q * norm;
			a1 = 0;
			a2 = -a0;
			b1 = 2.0f * (K * K - 1.0f) * norm;
			b2 = (1.0f - K / Q + K * K) * norm;
			break;
		case BW_BAND_STOP:
			norm = 1.0f / (1.0f + K / Q + K * K);
			a0 = (1.0f + K * K) * norm;
			a1 = 2.0f * (K * K - 1.0f) * norm;
			a2 = a0;
			b1 = a1;
			b2 = (1.0f - K / Q + K * K) * norm;
			break;
		case PEAKING:
			if (dB >= 0.0f) {  		  // boost
				norm = 1.0f / (1.0f + 1.0f / Q * K + K * K);
				a0 = (1.0f + V / Q * K + K * K) * norm;
				a1 = 2.0f * (K * K - 1.0f) * norm;
				a2 = (1.0f - V / Q * K + K * K) * norm;
				b1 = a1;
				b2 = (1.0f - 1.0f / Q * K + K * K) * norm;
			} else {  				  // cut
				norm = 1.0f / (1.0f + V / Q * K + K * K);
				a0 = (1.0f + 1.0f / Q * K + K * K) * norm;
				a1 = 2.0f * (K * K - 1) * norm;
				a2 = (1.0f - 1.0f / Q * K + K * K) * norm;
				b1 = a1;
				b2 = (1.0f - V / Q * K + K * K) * norm;
			}
			break;
		case BW_LOW_SHELF:
			if (dB >= 0.0f) {		  	  // boost
				norm = 1.0f / (1.0f + sqrtf(2.0f) * K + K * K);
				a0 = (1.0f + sqrtf(2.0f * V) * K + V * K * K) * norm;
				a1 = 2.0f * (V * K * K - 1.0f) * norm;
				a2 = (1.0f - sqrtf(2.0f * V) * K + V * K * K) * norm;
				b1 = 2.0f * (K * K - 1.0f) * norm;
				b2 = (1.0f - sqrtf(2.0f) * K + K * K) * norm;
			} else {  				  // cut
				norm = 1.0f / (1.0f + sqrtf(2.0f * V) * K + V * K * K);
				a0 = (1.0f + sqrtf(2.0f) * K + K * K) * norm;
				a1 = 2.0f * (K * K - 1.0f) * norm;
				a2 = (1.0f - sqrtf(2.0f) * K + K * K) * norm;
				b1 = 2.0f * (V * K * K - 1.0f) * norm;
				b2 = (1.0f - sqrtf(2.0f * V) * K + V * K * K) * norm;
			}
			break;
		case BW_HIGH_SHELF:
			if (dB >= 0.0f) { 		  	 // boost
				norm = 1.0f / (1.0f + sqrtf(2.0f) * K + K * K);
				a0 = (V + sqrtf(2.0f * V) * K + K * K) * norm;
				a1 = 2.0f * (K * K - V) * norm;
				a2 = (V - sqrtf(2.0f * V) * K + K * K) * norm;
				b1 = 2.0f * (K * K - 1.0f) * norm;
				b2 = (1.0f - sqrtf(2.0f) * K + K * K) * norm;
			} else {    				// cut
				norm = 1.0f / (V + sqrtf(2.0f * V) * K + K * K);
				a0 = (1.0f + sqrtf(2.0f) * K + K * K) * norm;
				a1 = 2.0f * (K * K - 1.0f) * norm;
				a2 = (1.0f - sqrtf(2.0f) * K + K * K) * norm;
				b1 = 2.0f * (K * K - V) * norm;
				b2 = (V - sqrtf(2.0f * V) * K + K * K) * norm;
			}
	} // switch
}

#endif

void Biquad::setQ(float q) {
	Q = q;
	setupCoeffs();
}

void Biquad::setFrq(float f) {
	mInputs[CSL_FILTER_FREQUENCY]->mValue = f;
	setupCoeffs();
}

void Biquad::incrFrq(float f) {
	mInputs[CSL_FILTER_FREQUENCY]->mValue = f;
	setupCoeffs();
}

void Biquad::setBoost(float f) {
	mInputs[CSL_FILTER_AMOUNT]->mValue = f;
	setupCoeffs();
}

void Biquad::incrBoost(float f) {
	mInputs[CSL_FILTER_AMOUNT]->mValue = f;
	setupCoeffs();
}

void Biquad::nextBuffer(Buffer & outputBuffer, unsigned outBufNum) throw (CException) {
	SampleBuffer out = outputBuffer.buffer(outBufNum);		// get ptr to output channel
	unsigned numFrames = outputBuffer.mNumFrames;			// get buffer length
	SampleBuffer inP;
	if (isInline) {
		inP = out;
	} else {
		Effect::pullInput(numFrames);				// get some input
		inP = mInputPtr;							// get a pointer to the input samples
	}
	for (unsigned i = 0; i < numFrames; i++) {		// here's a minimal bi-quad filter loop
		float inV = *inP++;							// no variable controls or scalable inputs
		float samp = inV * a0 + z1;					// uses inst vars rather than arrays for coefficients
		z1 = inV * a1 + z2 - b1 * samp;
		z2 = inV * a2 - b2 * samp;
		*out++ = samp;
	}
}

#pragma mark Formant

Formant::Formant (UnitGenerator & in, float cutoff, float radius) : Filter (in, 3,3) {
	mNormalize = true;
	setFrequency(cutoff);
	setAmount(radius);
	setupCoeffs();
	clear(); 
}

Formant::Formant (UnitGenerator & in, UnitGenerator & cutoff, float radius) : Filter (in, 3,3) {
	mNormalize = true;
	setFrequency(cutoff);
	setAmount(radius);
	setupCoeffs();
	clear(); 
}

void Formant::setNormalize(bool normalize) {
	mNormalize = normalize;
	setupCoeffs();
};

//	Calculate the filter coefficients based on the frequency characteristics
//	to be done every sample for dynamic controls

void Formant::setupCoeffs () {
	float centreFreq = mInputs[CSL_FILTER_FREQUENCY]->nextValue();
	float radius = mInputs[CSL_FILTER_AMOUNT]->nextValue();
	    
	mACoeff[0] = 1.0F;
	mACoeff[1] = cos(CSL_TWOPI * centreFreq * 1.f / mFrameRate ) * (-2.0F) * radius;
	mACoeff[2] = radius * radius;
	if (mNormalize ) {						// Use zeros at +- 1 and normalize the filter peak gain.
		mBCoeff[0] = 0.5 - 0.5 * mACoeff[2];
		mBCoeff[1] = 0.0;
		mBCoeff[2] = -mBCoeff[0];
	} else {
		mBCoeff[0] = 1.0F;
		mBCoeff[1] = 0.0F;
		mBCoeff[2] = -1.0F;
	} 
}

Notch::Notch (UnitGenerator & in, float cutoff, float radius) : Filter (in,3,3) {
	setFrequency(cutoff);
	setAmount(radius);
	clear(); 
}

Notch::Notch (UnitGenerator & in, UnitGenerator & cutoff, float radius) : Filter(in,3,3) {
	setFrequency(cutoff);
	setAmount(radius);
	clear();
}

//	Calculate the filter coefficients based on the frequency characteristics
void Notch::setupCoeffs () {
	float centreFreq = mInputs[CSL_FILTER_FREQUENCY]->nextValue();
	float radius = mInputs[CSL_FILTER_AMOUNT]->nextValue();
	
	//coeff's similar to formant but opposite
	mBCoeff[0] = 1.0F;
	mBCoeff[1] = cos(CSL_TWOPI * centreFreq * 1.0f / mFrameRate ) * (-2.0F) * radius;
	mBCoeff[2] = radius * radius;
	mACoeff[0] = 1.0F;
	mACoeff[1] = 0.0F;
	mACoeff[2] = 0.0F;
}

// the Frequency input of FrequencyAmount is used as the Allpass coefficient
Allpass::Allpass (UnitGenerator & in, float coeff) : Filter(in,2,2) {		// 1st order 1-pole 1-zero filter
	setFrequency(coeff);
	setAmount(1);
	clear();
}

Allpass::Allpass (UnitGenerator & in, UnitGenerator & coeff) : Filter(in,2,2) {
	setFrequency(coeff);
	setAmount(1);
	clear();
}


//	Calculate the filter coefficients based on supplied coeffs
void Allpass::setupCoeffs () {
	float coefficient = mInputs[CSL_FILTER_FREQUENCY]->nextValue();
	
	mACoeff[0] = mBCoeff[1] = 1.0;
	mBCoeff[0] = mACoeff[1] = coefficient; 
}

// Moog VCF Methods

Moog::Moog (UnitGenerator & in) : Filter(in) {
	setFrequency(500.0);
	setAmount(0.99);
	y1 = y2 = y3 = y4 = oldy1 = oldy2 = oldy3 = x = oldx = 0.f;
}

Moog::Moog (UnitGenerator & in, UnitGenerator & cutoff) : Filter(in) {
	setFrequency(cutoff);
	setAmount(0.99);
	y1 = y2 = y3 = y4 = oldy1 = oldy2 = oldy3 = x = oldx = 0.f;
}

Moog::Moog (UnitGenerator & in, UnitGenerator & cutoff, UnitGenerator & resonance) : Filter(in) {
	setFrequency(cutoff);
	setAmount(resonance);
	y1 = y2 = y3 = y4 = oldy1 = oldy2 = oldy3 = x = oldx = 0.f;
}

Moog::Moog (UnitGenerator & in, float cutoff) : Filter(in) {
	setFrequency(cutoff);
	setAmount(0.5);
	y1 = y2 = y3 = y4 = oldy1 = oldy2 = oldy3 = x = oldx = 0.f;
}

Moog::Moog (UnitGenerator & in, float cutoff, float resonance) : Filter(in) {
	setFrequency(cutoff);
	setAmount(resonance);
	y1 = y2 = y3 = y4 = oldy1 = oldy2 = oldy3 = x = oldx = 0.f;
}

// Filter the next buffer of the input -- adapted from moog filter at music.dsp source code archive

void Moog::nextBuffer(Buffer & outputBuffer, unsigned outBufNum) throw (CException) {
#ifdef CSL_DEBUG
	logMsg("Moog Filter nextBuffer");
#endif	
	sample* out = outputBuffer.buffer(outBufNum);	// get ptr to output channel
	unsigned numFrames = outputBuffer.mNumFrames;		// get buffer length
	SampleBuffer inputPtr = mInputs[CSL_INPUT]->mBuffer->buffer(outBufNum);
	DECLARE_SCALABLE_CONTROLS;							// declare the scale/offset buffers and values
	DECLARE_FILTER_CONTROLS;							// declare the freq/bw buffers and values
	LOAD_SCALABLE_CONTROLS;
	LOAD_FILTER_CONTROLS;
	this->setupCoeffs();
	
	for (unsigned i = 0; i < numFrames; i++) 	{
//		this->setupCoeffs();
				// --Inverted feed back for corner peaking 
		x = *inputPtr++ - r * y4; 
				// Four cascaded onepole filters (bilinear transform) 
		y1 = x * p + oldx * p - k * y1; 
		y2 = y1 * p + oldy1 * p - k * y2; 
		y3 = y2 * p + oldy2 * p - k * y3; 
		y4 = y3 * p + oldy3 * p - k * y4; 
				// Clipper band limited sigmoid 
		y4 = y4 - (y4*y4*y4) / 6; 
		oldx = x; 
		oldy1 = y1; 
		oldy2 = y2; 
		oldy3 = y3;
		*out++ = y4;
		UPDATE_SCALABLE_CONTROLS;							// update the dynamic scale/offset
	}
}

// Calculate the filter coefficients based on the frequency characteristics

void Moog::setupCoeffs () {
	float centreFreq = mInputs[CSL_FILTER_FREQUENCY]->nextValue();
	float resonance = mInputs[CSL_FILTER_AMOUNT]->nextValue();

	float f, scale;
	f = 2 * centreFreq / mFrameRate;				 //[0 - 1] 
	k = 3.6 * f - 1.6 * f * f - 1;		 //(Empirical tunning) 
	p =	(k + 1 ) * 0.5; 
	scale = exp((1 - p ) * 1.386249 ); 
	r = resonance * scale; 
}
