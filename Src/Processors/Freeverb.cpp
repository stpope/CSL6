//
// Reverb.h -- The CSL port of the public domain Freeverb reverberator
// 	Freeverb was written by Jezar at Dreampoint, June 2000 -- http://www.dreampoint.co.uk
//	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
// 

#include "Freeverb.h"

//#undef GCC_AUTO_VECTORIZATION

using namespace csl; 

// These values assume 44.1KHz sample rate
// they will probably be OK for 48KHz sample rate
// but would need scaling for 96KHz (or other) sample rates.
// The values were obtained by listening tests.

const int	kNumCombs		= 8;
const int	kNumAllpasses	= 4;
const float	kMuted			= 0.0f;
const float	kFixedGain		= 0.015f;
//const float kFixedGain   = 1.0f;
const float kScaleWet		= 3.0f;
const float kScaleDry		= 2.0f;
const float kScaleDamp		= 0.4f;
const float kScaleRoom		= 0.28f;
const float kOffsetRoom		= 0.7f;
const float kInitialRoom	= 0.95;
const float kInitialDamp	= 0.5f;
const float kInitialWet		= 0.5;
const float kInitialDry		= 0.5;
const float kInitialWidth	= 1.0f;
const float kInitialMode	= 0.0f;
const float kFreezeMode		= 1.0f;
const int	kStereoSpread	= 23;

const int kCombBufferSizes[] = { 1116, 1188, 1277, 1356, 1422, 1491, 1557, 1617 };
const int kAllpassBufferSizes[] = { 556, 441, 341, 225 };

// ~~~~~~ Comb implementation ~~~~~~~~~~ //

void Comb::setBuffer(float *buf, int size) {
	mBufferPtr = buf;
	mBufSize = size;
}

void Comb::mute() {
	for (int i = 0; i < mBufSize; ++i)
		mBufferPtr[i] = 0;
}

void Comb::setDamp(float val) {
	mDamp1 = val; 
	mDamp2 = 1 - val;
}

// ~~~~~~ Allpass implementation ~~~~~~~~~~ //

void FAllpass::setBuffer(float *buf, int size) {
	mBufferPtr = buf; 
	mBufSize = size;
}

void FAllpass::mute() {
	for (int i = 0; i < mBufSize; i++)
		mBufferPtr[i] = 0;
}

// ~~~~~~~ Freeverb implementation ~~~~~~ //

Freeverb::Freeverb(UnitGenerator &input) : Effect(input) { 
	constructReverbGraph(); 
}

Freeverb::~Freeverb() {
	for (unsigned i = 0; i < mCombFilters.size(); i++) 
		delete mCombBuffers[i];

	for (unsigned i = 0; i < mAllpassFilters.size(); i++) 
		delete mAllpassBuffers[i];

	std::vector<Comb*>::iterator iComb = mCombFilters.begin();
	for (; iComb != mCombFilters.end(); ++iComb) 
		delete *iComb;

	std::vector<FAllpass*>::iterator iAllpass = mAllpassFilters.begin();
	for (; iAllpass != mAllpassFilters.end(); ++iAllpass) 
		delete *iAllpass;		
}

void Freeverb::constructReverbGraph() {
	int i, j;
	int bufferSize;
	sample * bufferPtr;
	Comb *combFilter;
	FAllpass *allpassFilter;
	mCombBuffers = new SampleBuffer[kNumCombs];
	mAllpassBuffers = new SampleBuffer[kNumAllpasses];
	
				// create buffers and combs
	for (i = 0; i < kNumCombs; i++) {
		bufferSize = kCombBufferSizes[i];
		bufferPtr = new sample[bufferSize];
		for (j = 0; j < bufferSize; j++)
			bufferPtr[j] = 0.0f;
		mCombBuffers[i] = bufferPtr;
		combFilter = new Comb();
		combFilter->setBuffer(bufferPtr, bufferSize);
		mCombFilters.push_back(combFilter);
	}
				// create buffers and allpasses	
	for (i = 0; i < kNumAllpasses; i++) {
		bufferSize = kAllpassBufferSizes[i];
		bufferPtr = new sample[bufferSize];
		for (j = 0; j < bufferSize; j++)
			bufferPtr[j] = 0.0f;
		mAllpassBuffers[i]= bufferPtr;
		allpassFilter = new FAllpass();
		allpassFilter->setBuffer(bufferPtr, bufferSize);
		allpassFilter->setFeedback(0.5f);
		mAllpassFilters.push_back(allpassFilter);
	}
				// Set default values
	setWetLevel(kInitialWet);	
	setDryLevel(kInitialDry);	
	setRoomSize(kInitialRoom);
	setDampening(kInitialDamp);
	setWidth(kInitialWidth);
	mGain = kFixedGain;	
	updateParameters();
}

void Freeverb::updateParameters() {
	for (std::vector<Comb*>::iterator iComb = mCombFilters.begin();
			iComb != mCombFilters.end(); ++iComb) {
		(*iComb)->setFeedback(mRoomSize);
		(*iComb)->setDamp(mDampening);
	}
}

float Freeverb::roomSize() { return mRoomSize; }

void Freeverb::setRoomSize(float size) {
	mRoomSize = size * kScaleRoom + kOffsetRoom;
	updateParameters();
}

float Freeverb::dampening() { return (mDampening * 100.0f) / kScaleDamp; }

/// The "damp" parameter can be thought as setting the material of the room walls.

void Freeverb::setDampening(float damp) {
	mDampening = (damp * 0.01f) * kScaleDamp;
	updateParameters();
}

float Freeverb::wetLevel() { return mWetLevel; }

void Freeverb::setWetLevel(float level) {
	mWetLevel = level;
	updateParameters();
}

float Freeverb::dryLevel() { return mDryLevel; }

/// Amount of the original "dry" signal in the output.

void Freeverb::setDryLevel(float level) {	mDryLevel = level; }

float Freeverb::width() {	return (mWidth*100.0f); }

void Freeverb::setWidth(float twidth) {
	mWidth = (twidth*0.01f);
	updateParameters();
}

// Do the work

void Freeverb::nextBuffer(Buffer &outputBuffer, unsigned outBufNum) throw (CException) {
	unsigned numFrames = outputBuffer.mNumFrames;
	sample * fp = outputBuffer.buffer(outBufNum);
	this->pullInput(outputBuffer);
	sample * inputBuf = mInputPtr;
	float out, input;
	unsigned i;
#ifdef CSL_DEBUG
	logMsg("Freeverb nextBuffer");
#endif		
	for (i = 0; i < numFrames; i++) {
		out = 0;
		input = inputBuf[i] * mGain;
					// Accumulate comb filters in parallel
		std::vector<Comb*>::iterator iComb = mCombFilters.begin();
		for (; iComb != mCombFilters.end(); ++iComb) 
			out += (*iComb)->process(input);
					// Feed through allpasses in series
		std::vector<FAllpass*>::iterator iAllpass = mAllpassFilters.begin();
		for (; iAllpass != mAllpassFilters.end(); ++iAllpass) 
			out = (*iAllpass)->process(out);
		*fp++ = out * mWetLevel + inputBuf[i] * mDryLevel; // Calculate output REPLACING anything already there
	}
}

//// Stereoverb ////////////////////////////////

// Constructor sets up splitter/joiner network

Stereoverb::Stereoverb(UnitGenerator &input) {
	split = new Splitter(input);

	leftRev = new Freeverb(* split);			// 2 mono reverbs
	leftRev->setWetLevel(kInitialWet);
	leftRev->setDryLevel(kInitialWet);
	leftRev->setRoomSize(kInitialRoom);		

	rightRev = new Freeverb(* split);
	rightRev->setWetLevel(kInitialWet);
	rightRev->setDryLevel(kInitialWet);
	rightRev->setRoomSize(kInitialRoom);	

	join = new Joiner (* leftRev, * rightRev);	// mono-to-stereo joiner
}

Stereoverb::~Stereoverb() {
	delete split;
	delete leftRev;
	delete rightRev;
	delete join;
}

bool Stereoverb::isActive() {
	return(split->isActive());
}

void Stereoverb::setRoomSize(float size) {
	leftRev->setRoomSize(size);
	rightRev->setRoomSize(size);
}

void Stereoverb::setDampening(float damp) {
	leftRev->setDampening(damp);
	rightRev->setDampening(damp);
}

void Stereoverb::setWetLevel(float level) {
	leftRev->setWetLevel(level);
	rightRev->setWetLevel(level);
}

void Stereoverb::setDryLevel(float level) {
	leftRev->setDryLevel(level);
	rightRev->setDryLevel(level);
}

void Stereoverb::setWidth(float width) {
	leftRev->setWidth(width);
	rightRev->setWidth(width);
}

// next_buffer work-horse method

void Stereoverb::nextBuffer(Buffer &outputBuffer) throw (CException) {
	join->nextBuffer(outputBuffer);
}
