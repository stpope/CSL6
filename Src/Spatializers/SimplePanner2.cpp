//
//	SimplePanner.cpp -- Basic panner using a filter.
//	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
//

#include "SimplePanner.h"

namespace csl {

// Default constructor

SimplePanner::SimplePanner() :
		mDryMix(2),								// set up mixers
		mRevMix(2), 
		mFiltMix(2),  
		mFiltSplit(mFiltMix),					// splitter for filters
		mRLPF(mFiltSplit, BW_LOW_PASS, 500.0f),	// filters
		mLLPF(mFiltSplit, BW_LOW_PASS, 500.0f),  
		mFiltJoin(mLLPF, mRLPF),				// joiner for filters
		mReverb(mRevMix),						// stereoverb
		mOutMix(2) {							// grand out mix
	mOutMix.addInput(mReverb);					// add processing paths to out mixer
	mOutMix.addInput(mDryMix);
	mOutMix.addInput(mFiltJoin);
	
	mOutMix.scaleInput(mDryMix,  0.4f);			// scale out mix levels
	mOutMix.scaleInput(mReverb,  0.4f);
	mOutMix.scaleInput(mFiltMix, 0.4f);
	
	mReverb.setWetLevel(1.0f);					// set up reverb
	mReverb.setDryLevel(0.0f);
	mReverb.setRoomSize(0.988f);				// long reverb time
}

SimplePanner::~SimplePanner() {
	// no-op
}

void SimplePanner::addSource(SpatialSource &soundSource) {
	unsigned which = mSources.size();
	for (unsigned i = 0; i < which; i++) {
		if (mSources[i] == NULL) {
			which = i;
			break;
		}
	}
	mSources[which] = &soundSource;
	Panner * inPan = new Panner(soundSource);	// add a panner on the source
	mPanners[which] = inPan;					// store panner in vector
	FanOut * inFan = new FanOut(*inPan, 3);		// send the panner to 2 or 3 mixers
	mFanOuts[which] = inFan;					// store fan-out in vector
	mDryMix.addInput(inFan);					// add fan-out to mixers
	mFiltMix.addInput(inFan);
	mRevMix.addInput(inFan);
}

// delete from the list, shifting if necessary

void SimplePanner::removeSource(SpatialSource &soundSource) {
							// find the source's panner and fanout
	for (unsigned i = 0; i < mSources.size(); i++) {
		if (mSources[i] == &soundSource) {
			FanOut * inFan = (FanOut *) mFanOuts[i];
			Panner * inPan = (Panner *) mPanners[i];
			mDryMix.removeInput(*inFan);
			mRevMix.removeInput(*inFan);
			mFiltMix.removeInput(*inFan);
			mPanners[i] = NULL;
			mFanOuts[i] = NULL;
			mSources[i] = NULL;
//			delete inFan;
//			delete inPan;
			return;
		}
	}
}

// fill the buffer with the next buffer of values
	
void SimplePanner::nextBuffer(Buffer &outputBuffer /*, unsigned outBufNum */) throw (CException) {
	float num, az, rem;
	float half = 1.0f/2.0f;
//	mPanners[0]->nextBuffer(outputBuffer);
										// set all the panner values
	for (unsigned i = 0; i < mPanners.size(); i++) {
		if ( ! mSources[i]) continue;
		az = ((SpatialSource *) mSources[i])->azimuth() / CSL_TWOPI;		// range 0 - 1 = 0 - 2 pi
		while (az < 0.0f)	az += 1.0f;
		rem = az;
		if (rem > half)		rem = 1.0f - rem;
		rem *= 4.0f;					// range 0 - 2
		rem -= 1.0f;					// range -1 - +1
		((Panner *) mPanners[i])->setPosition(rem);	// set stereo position
										// set reverb feed from distance
		float distScale = 1.0f / sqrt(((SpatialSource *) mSources[i])->distance());
//		float distScale = 1.0f / mSources[i]->distance();	// this brings up the reverb closer
		mRevMix.scaleInput(*mFanOuts[i], 1.0f - distScale);
										// set filter/dry ratio from front/back angle
		if (az < half) {				// front half
			mFiltMix.scaleInput(*mFanOuts[i], 0.0f);
			mDryMix.scaleInput(*mFanOuts[i], distScale);
		} else {						// rear half
			float ratio = fabs(0.75f - az) * 4.0f;		// scale 0 - 1
			mFiltMix.scaleInput(*mFanOuts[i], (1.0f - ratio) * distScale);
			mDryMix.scaleInput(*mFanOuts[i], ratio * distScale);
		}
	}
	mOutMix.nextBuffer(outputBuffer);	// now call the out mixer to run the graph
}

} // namespace
