//
//  SpatialPanner.cpp -- Specification of the abstract framework for panner/spatializers
//	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
//	Created by Jorge Castellanos on 3/29/06. Hacked 8/09 by STP.
//

#include "SpatialPanner.h"

using namespace csl;

SpatialPanner::SpatialPanner(SpeakerLayout *layout) 
			: UnitGenerator(CGestalt::frameRate(), 2), 
			  mSpeakerLayout(NULL) {	
//	mSources = (SpatialSource **) malloc(64 * sizeof(SpatialSource*));
	mTempBuffer.setSize(1, CGestalt::blockSize());		// mono buffer for operations
	mTempBuffer.allocateBuffers();
	setSpeakerLayout(layout);							// Grab the default layout.
}

SpatialPanner::~SpatialPanner() {
	mSpeakerLayout->detachObserver(this);				// Remove yourself from the list of observers.
	mTempBuffer.freeBuffers();
}

void SpatialPanner::setSpeakerLayout(SpeakerLayout *aLayout) {
				// TO DO: FIRST CHECK TO MAKE SURE THERE'S A LAYOUT, 
				// THEN DETACH AS OBSERVER OF PREVIOUS AND ATTACH TO NEWER ONE!
	if (aLayout == mSpeakerLayout) // If user is just setting the same layout again, do nothing.
		return;
		
	if (mSpeakerLayout != NULL) // If still alive, remove yourself as an observer.
		mSpeakerLayout->detachObserver(this); 	
			
	mSpeakerLayout = aLayout; 
	mSpeakerLayout->attachObserver(this); // Tell the speaker layout to inform you of any changes
	this->speakerLayoutChanged();
}

// add a source and create a cache object

void SpatialPanner::addSource(SpatialSource &soundSource) {
//	unsigned which = mSources.size();
//	for (unsigned i = 0; i < which; i++) {
//		if (mSources[i] == NULL) {
//			which = i;
//			break;
//		}
//	}
//	mSources[which] = &soundSource;
	mSources.push_back(&soundSource);
	mCache.push_back(this->cache());
}
	
// delete from the list, shifting if necessary

void SpatialPanner::removeSource(SpatialSource &soundSource) {
//	unsigned count = mSources.size();
	for (unsigned i = 0; i < mSources.size(); i++) {
		if (mSources[i] == &soundSource) {
//			mSources[i] = NULL;
			mSources.erase(mSources.begin() + i);
			mCache.erase(mCache.begin() + i);
			break;
		}
	}
#ifdef CSL_DEBUG
	logMsg("Panner::removeSource");
#endif
	return;
}

void *SpatialPanner::cache() {
	return (void *)new float; // Returns a pointer to an alocated cache data for its own use.
}

/** 
Called when the speaker layout changes, so panners update their data.
This method is implemented only by the Panner class, and depending on the message sent, it calls
the appropriate method implemented by subclasses. For example, when the speaker layout changes,
the method calls "speakerLayoutChanged()", which should be implemented by subclasses interested to
react to such change.
*/
void SpatialPanner::update(void * arg) {
//	cout << "Panner::Updated Speaker Layout!" << mNumTriplets << endl;	
								// Re-calculate all the stuff.
	if (mSpeakerLayout == NULL) 	// If it was destroyed get the default layout
		setSpeakerLayout(SpeakerLayout::defaultSpeakerLayout());
								// Update any info that depends on the layout
	this->speakerLayoutChanged();
}
