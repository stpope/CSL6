//
//	AmbisonicPanner.h -- Ambisonic effects and panners.
//	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
//	Higher Order Ambisonic classes written by Jorge Castellanos, Graham Wakefield, Florian Hollerweger, 2005
//

#include "AmbisonicPanner.h"

using namespace csl;

// Default constructor

AmbisonicPanner::AmbisonicPanner(unsigned order, SpeakerLayout *layout) : SpatialPanner(layout) {
					// This is needed so the default kCopy doesn't 
					// overide the multiple channel panning done here.
	setCopyPolicy(kIgnore); 
	mMixer = new AmbisonicMixer(order);
	mRotator = new AmbisonicRotator(*mMixer);
	mDecoder = new AmbisonicDecoder(*mRotator, layout);
}

AmbisonicPanner::AmbisonicPanner(unsigned hOrder, unsigned vOrder, SpeakerLayout *layout) 
			: SpatialPanner(layout) {
	setCopyPolicy(kIgnore); 	mMixer = new AmbisonicMixer(hOrder, vOrder);
	mRotator = new AmbisonicRotator(*mMixer);
	mDecoder = new AmbisonicDecoder(*mRotator, layout);
}

AmbisonicPanner::~AmbisonicPanner() {
	delete mDecoder;
	delete mRotator;
	delete mMixer;
}

//void AmbisonicPanner::initialize() {
//	
//}

void AmbisonicPanner::addSource(SpatialSource &soundSource) {
	mMixer->addInput(*(new AmbisonicEncoder(soundSource)));
}

// delete from the list, shifting if necessary

void AmbisonicPanner::removeSource(SpatialSource &soundSource) {
//	unsigned count = mSources.size();
//	SpatialSource *tempSource;
//	
//	for (unsigned i = 0; i < count; i++) {
//		tempSource = mSources[i]->input();
//		if (tempSource ==  &soundSource) {
//			mSources.erase(mSources.begin() + i);
//			break;
//		}
//	}		
}


void AmbisonicPanner::rotate(float amount) {
	mRotator->setRotate(amount);
}

//void AmbisonicPanner::speakerLayoutChanged() {
//
//}

void AmbisonicPanner::nextBuffer(Buffer &outputBuffer /*, unsigned outBufNum */) throw(CException) {

#ifdef CSL_DEBUG
	logMsg("AmbisonicPanner::nextBuffer");
#endif
				// Ask the decoder to fill the buffer with the data to be processed
	((UnitGenerator *)mDecoder)->nextBuffer(outputBuffer);
}


