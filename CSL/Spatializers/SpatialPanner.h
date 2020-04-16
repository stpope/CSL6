//
//  SpatialPanner.h -- Specification of the abstract framework for panner/spatializers
//	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
//	Created by Jorge Castellanos on 3/29/06. Hacked 8/09 by STP.
//

#ifndef SPATIAL_PANNER_H
#define SPATIAL_PANNER_H

#include "SpeakerLayout.h"
#include "SpatialSource.h"

#include "CSL_Core.h"
#include "CPoint.h"

namespace csl {

/// Base class for all panners.
/// Handles source management and holds a speaker layout.
/// @todo Update all Panners so that they rebuild their caches when changing the speaker layout.

class SpatialPanner : public UnitGenerator, public Observer {
public:
										/// Constructor - a SpeakerLayout can be specified
	SpatialPanner(SpeakerLayout *layout = SpeakerLayout::defaultSpeakerLayout());
	virtual ~SpatialPanner();

										/// Set the speaker layout to be used by this panner.
										/// The panner will request the default layout if not set.
	void setSpeakerLayout(SpeakerLayout *aLayout);

	unsigned numSources() { return mSources.size(); }; 	///< number of active inputs.
	
	virtual void addSource(SpatialSource &s);			///< Add a souce to the list of inputs to be
														///  processed and create a cache object

	virtual void removeSource(SpatialSource &s);		///< Remove a Sound Source. 

	virtual void update(void *arg);		///< Called when the speaker layout changes

	virtual void nextBuffer(Buffer & outputBuffer) throw (CException) = 0;
	virtual void nextBuffer(Buffer & outputBuffer, unsigned outBufNum) throw (CException) {
		throw LogicError("Asking for mono nextBuffer of a SpatialPanner");
	}

protected:
										/// SpatialSource... refers to its input UGen, 
										/// but with the knowledge of its position within a space.
	UGenVector mSources;				///< Vector of pointers to the inputs
	vector <void *> mCache;				///< Vector of pointers to the prior I/O data.

	SpeakerLayout *mSpeakerLayout;		///< If null, it will use the default layout by calling 
										///  SpeakerLayout::defaultSpeakerLayout();
	Buffer mTempBuffer;					///< Buffer used to temporarily hold input source data.

	virtual void *cache();						///< create the cache
	virtual void speakerLayoutChanged() { };	///< Called when the speaker layout changes.
}; 

}

#endif
