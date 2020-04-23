//
//	SimplePanner.cpp -- Basic panner using a filter.
//	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
//

#include "SimplePanner.h"

namespace csl {

// Default constructor

SimplePanner::SimplePanner() :
		mDryMix(2),								// set up stereo mixers for the graph
		mFiltMix(2), 
		mRevMix(2), 
		 
		mFiltSplit(mFiltMix),					// splitter for filters
		mRLPF(mFiltSplit, BW_LOW_PASS, 800.0f),	// 2 lo-pass filters
		mLLPF(mFiltSplit, BW_LOW_PASS, 800.0f),  
		mFiltJoin(mLLPF, mRLPF),				// joiner for filters
		
		mReverb(mRevMix),						// stereoverb
		mOutMix(2)								// grand output mixer
{
	mOutMix.addInput(mDryMix);
	mOutMix.addInput(mFiltJoin);
	mOutMix.addInput(mReverb);					// add processing paths to out mixer

	mOutMix.scaleInput(mDryMix,  0.4f);			// scale out mix levels
	mOutMix.scaleInput(mFiltJoin, 0.4f);
	mOutMix.scaleInput(mReverb,  0.4f);

	mReverb.setWetLevel(1.0f);					// set up reverb
	mReverb.setDryLevel(0.0f);
//	mReverb.setRoomSize(0.99f);					// long reverb time
}

SimplePanner::~SimplePanner() {
	// no-op
}

// Add a new source, creating the in-panner as a fan-out

void SimplePanner::addSource(SpatialSource &soundSource) {
	mSources.push_back(&soundSource);			// remember the source
	Panner * inPan = new Panner(soundSource);	// add a panner on the source
	mPanners.push_back(inPan);					// store panner in vector

	mDryMix.addInput(inPan);					// add panner to mixers
	mFiltMix.addInput(inPan);
	mRevMix.addInput(inPan);
}

// delete from the list, shifting if necessary

void SimplePanner::removeSource(SpatialSource &soundSource) {
							// find the source's panner and fanout
	for (unsigned i = 0; i < mSources.size(); i++) {
		if (mSources[i] == &soundSource) {
			Panner * inPan = (Panner *) mPanners[i];
			mDryMix.removeInput(*inPan);
			mFiltMix.removeInput(*inPan);
			mRevMix.removeInput(*inPan);
			
			mSources.erase(mSources.begin() + i);
			mPanners.erase(mPanners.begin() + i);
			delete inPan;
			return;
		}
	}
}

// fill the buffer with the next buffer of values
	
void SimplePanner::nextBuffer(Buffer &outputBuffer) throw (CException) {
	float az, pos;
	float half = 1.0f/2.0f;
	if (mSources.size() == 0)
		return;
										// set all the panner values
	for (unsigned i = 0; i < mSources.size(); i++) {
		az = ((SpatialSource *) mSources[i])->azimuth() / CSL_TWOPI;	// range 0 - 1 = 0 - 2 pi
		while (az < 0.0f)	az += 1.0f;	// clip to range 0 - 1
		while (az > 1.0f)	az -= 1.0f;
		pos = az;
		if (pos > half)		pos = 1.0f - pos;
		pos *= 4.0f;					// range 0 - 2
		pos -= 1.0f;					// range -1 - +1
		((Panner *) mPanners[i])->setPosition(pos);	// set stereo position
		
										// set reverb feed from distance
		float distScale = 1.0f / sqrt(((SpatialSource *) mSources[i])->distance());
//		float distScale = 1.0f / ((SpatialSource *) mSources[i]->distance());	// this brings up the reverb closer
		mRevMix.scaleInput(*mPanners[i], 1.0f - distScale);
		
										// set filter/dry ratio from front/back angle
		if (az < half) {				// front half
			mFiltMix.scaleInput(*mPanners[i], 0.0f);
			mDryMix.scaleInput(*mPanners[i], distScale);
		} else {						// rear half
			float ratio = fabs(0.75f - az) * 4.0f;		// scale 0 - 1
			mFiltMix.scaleInput(*mPanners[i], (1.5f - ratio) * distScale);
			mDryMix.scaleInput(*mPanners[i], ratio * distScale);
		}
	}
	mOutMix.nextBuffer(outputBuffer);	// now call the out mixer to run the graph
}

} // namespace
