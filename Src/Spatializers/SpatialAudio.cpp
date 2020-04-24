//
//  SpatialAudio.cpp -- Spatializer
//	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
//	Created by Jorge Castellanos on 7/23/06. Hacked 8/09 by STP.
//
/// @todo Observe the layout for changes, just in case it's deleted, to grab the default, avoiding dangling pointers.

#include "SpatialAudio.h"
#include "SimplePanner.h"

#ifdef USE_VBAP
	#include "VBAP.h"
#endif
#ifdef USE_AMBISONIC
	#include "AmbisonicPanner.h"
#endif
#ifdef USE_HRTF
	#include "Binaural.h"
#endif
#ifdef USE_WFS
	#include "WFS.h"
#endif

using namespace csl;

Spatializer::Spatializer(PannerType panMode, SpeakerLayout * speakerLayout)
			: UnitGenerator(), mPanner(NULL), mSpeakerLayout(speakerLayout) {
	setCopyPolicy(kIgnore);		// This is needed so the default kCopy doesn't overide
								// the multiple channel panning done here.
	setNumChannels(2);
	setPanningMode(panMode);
    if (mSpeakerLayout == NULL)
        mSpeakerLayout = SpeakerLayout::defaultSpeakerLayout();
}

Spatializer::~Spatializer() {
	delete mPanner;
}

void Spatializer::addSource(SpatialSource &soundSource) {
//	logMsg("Spatializer::addSource");	
	if (mType == kSimple) {
		mPanner->addSource(soundSource);			// Add the source to the panner.
	} else {										// create a new distance simulator passing it the source.
		DistanceSimulator *distanceProcessor = new DistanceSimulator(soundSource); 
		mPanner->addSource(*distanceProcessor);				// Add the source to the panner.
		mInputsHashMap[&soundSource] = distanceProcessor;	// Add the pointers to the map.
	}
}
	
// delete from the list, shifting if necessary

void Spatializer::removeSource(SpatialSource &soundSource) {
#ifdef CSL_DEBUG
	logMsg("Panner::removeSource");
#endif
	SpatialSource *tempSourcePtr = mInputsHashMap[&soundSource];
	if (tempSourcePtr) {	// If not null then remove it form the panner.
		mPanner->removeSource(*tempSourcePtr);
							// now remove it from the map.
		mInputsHashMap.erase(&soundSource);
	} else {
		mPanner->removeSource(soundSource);
	}
}

void Spatializer::update(void * arg) {
    // TO DO HERE?
}

void Spatializer::setPanningMode(PannerType panType) {
	SpatialPanner *tempPanner; // Placeholder for the panner until everything is setup.
	mType = panType;
	if (mType == kAutomatic) {
				// Call the panner expert system, so that it decides which panner to instantiate.
		mType = kVBAP;
	} else {
		switch(mType) {
		case kSimple:
			tempPanner = new SimplePanner();
			break;
#ifdef USE_HRTF
		case kBinaural:
			tempPanner = new BinauralPanner();
			break;
#endif
#ifdef USE_VBAP
		case kVBAP:
			tempPanner = new VBAP(kPeriphonic, mSpeakerLayout);
			break;
#endif
#ifdef USE_AMBISONIC
		case kAmbisonic:
			tempPanner = new AmbisonicPanner(1, mSpeakerLayout);
			break;
#endif
#ifdef USE_WFS
		case kWFS:
			tempPanner = new WFSPanner(mSpeakerLayout);
			break;
#endif
		default:
			logMsg(kLogWarning, "Unknown PannerMode Specified.");
			setPanningMode(kAutomatic); // I call myself setting the mode to Automatic.
			break;
		}
	}
						// create an iterator to go thru all the sources.
	map <SpatialSource *, DistanceSimulator *>::iterator idx; 
						// Add all the sources to the new panner
	for (idx = mInputsHashMap.begin(); idx != mInputsHashMap.end(); ++idx)
		tempPanner->addSource(*(idx->second));

						// NOTE: This is not thread safe. In multithreaded apps, this could make it crash!
						/// @todo make all spat framework thread safe.
						// get rid of the old panner.
	if (mPanner != NULL) 
		delete mPanner;
	mPanner = tempPanner; // Assign the new panner.
}

// the method Spatializer::nextBuffer calls the panner

void Spatializer::nextBuffer(Buffer &outputBuffer /*, unsigned outBufNum */) throw(CException) {
#ifdef CSL_DEBUG
	logMsg("Spatializer::nextBuffer");
#endif								// Ask the decoder to fill the buffer with the data to be processed
	outputBuffer.zeroBuffers();
	mPanner->nextBuffer(outputBuffer); 
}

PannerType SpeakerLayoutExpert::findPannerFromLayout(SpeakerLayout *layout) {
	unsigned numSpeakers = layout->numSpeakers();
	PannerType type = kVBAP;		// Just default to something that always works.
	switch(numSpeakers) {
		case 0:
		case 1:
			logMsg("Not enough speakers to be spatialized");// throw an exception	
			break;
		case 2:
			// if headphones, then Binaural.
			// otherwise for now use VBAP. At some point it could be used transaural.
			type = kVBAP;
			break;
		case 3:
		default:
			type = kVBAP;
			break;
	}
	return type;
}
