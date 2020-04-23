//
//  SpatialAudio.h -- Spatializer
//
//	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
//	Created by Jorge Castellanos on 7/23/06. Hacked 8/09 by STP.
//

#ifndef CSL_SPATIAL_AUDIO_H
#define CSL_SPATIAL_AUDIO_H

#include "SpatialPanner.h"
#include "DistanceSimulator.h"

namespace csl {

/// Panner types

typedef enum {
	kAutomatic = 0,
	kBinaural,		///< HRTF 2-channel panning
	kVBAP,			///< Vector Base Amplitude Panning 
	kAmbisonic,		///< Full 3D Ambisonics
	kSimple,		///< Simple panning/filtering spatializer
	kWFS			///< Wave Field Synthesis
} PannerType;

/// Container class that simplifies the spatial audio interface.
/// This layer hides most of the inner-working of audio spatialization, 
/// providing a simple interface for sound placement in a space.

class Spatializer : public UnitGenerator, public Observer {
public:
	Spatializer(PannerType panMode = kAutomatic, SpeakerLayout *speakerLayout = SpeakerLayout::defaultSpeakerLayout());
	~Spatializer();

	void addSource(SpatialSource &s);		///< Add a sound souce to the list of inputs to be processed.
	void removeSource(SpatialSource &s);	///< Remove a Sound Source. 

	unsigned numSources() { return mPanner->numSources(); }; 	///< number of active inputs.

	void setPanningMode(PannerType panType);

	virtual void update(void *arg);	///< called when the speaker layout changes, so panners update precalculated data.

	virtual void nextBuffer(Buffer &outputBuffer /*, unsigned outBufNum */) throw (CException); ///< fill the buffer with data :-)

private:
	SpatialPanner *mPanner;
									/// a map between a source passed/key and a the corresponding distance 
									/// simulator (used for removing sources)
	map <SpatialSource *, DistanceSimulator *> mInputsHashMap;
									/// If null, it will use the default layout by calling 
									/// SpeakerLayout::defaultSpeakerLayout();
	SpeakerLayout *mSpeakerLayout; 
	PannerType mType;
};


class Auralizer : public Spatializer {
public:
	Auralizer() { };
	~Auralizer() { };


private:

// SHOULD REVERB BE PART OF THE AURALIZER? OR THE SPATIALIZER?
// TO AN EXTENT, ADDING REVERB IS ALREADY A VERY COARSE ATTEMPT TO SIMULATE A SPACE, MAKING IT PART OF AURALIZER
// BUT A SPATIALIZER IS ALSO SUPPOSED TO PLACE A SOUND IN A SPACE, THUS THE NAME.
// ADDING ROOM SIZE TO A SPAIZALIZER TO CONTROL REVERB AMOUNT WOULD GET INTO THE INTERFAXE OF AURALIZER
// UNLESS NOT SPECIFIED? NAHH!

/*
For now use Freeverb as the reverberator. In the future, a base reverb class should exist, 
allowing to specify the type of reverb. Although this would call for a subclass of Auralizer, 
rather than using setter functions, where by default it gets constructed to use Freverb, but
A subclass could just place any other reverb.
*/

//	Freeverb *mGlobalReverb

/*
The reverb settings would be set depending on the room specified.
The "Room" Class should have a method for returning volume, as well as average absorption
(in order to set the tail properties, as well as calculating reflections.

The calculation of early reflections should also share a common base class, so that subclasses
can initialize the "geometer" to any desired one."
*/


};

///*
//I GUESS I'VE FOUND A SOLUTION TO THE SPATIALIZER "CONTAINER".
//FIRST, IT WONT BE A SPATIALIZER, BUT THE AURALIZER WILL COME A LEVEL DOWN, AND BE ITSELF THE SPATIALIZER.
//THE DIFFERENCE IS THAT BY DEFAULT IT WON'T DEAL WITH ROOM MODELS. (IT COULD POSSIBLY BE A SUBCLASS OF SPATIALIZER IF THE "ECHO" INFO 
//BECOMES TO COMPLICATED TO HAVE FOR EVERY SPATIALIZER.
//
//HERE IT GOES:
//WHEN ADDING A SOURCE TO THE SPATIALIZER, THIS WILL MAKE A DISTANCE_SIMULATOR AND PASS IT THE ADDED SOURCE. THEN, IT'LL CALL "ADD SOURCE" OF THE
//PANNER AND WILL PASS IT THE NEW CREATED DISTANCE SIMULATOR. 
//IN THE NEXT BUFFER OF THE SPATIALIZER, THIS WILL CALL NEXT BUFFER OF THE PANNER, WHICH IN TURN WILL CALL NEXT BUFFER OF EACH SOURCE, MEANING EACH DISTANCE SIMULATOR
//WHICH IN TURN IT'LL CAL THE NEXT BUFFER OF EACH PASSED SOURCE.
//
//
//
//
//*/

class SpeakerLayoutExpert {
public:
	SpeakerLayoutExpert() { };
	~SpeakerLayoutExpert() { };
	
	static PannerType findPannerFromLayout(SpeakerLayout *layout = SpeakerLayout::defaultSpeakerLayout());

}; 

} // end of nameespace

#endif
