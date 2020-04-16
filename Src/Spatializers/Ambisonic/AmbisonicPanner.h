//
//	AmbisonicPanner.h -- Ambisonic effects and panners.
//	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
//	Higher Order Ambisonic classes written by Jorge Castellanos, Graham Wakefield, Florian Hollerweger, 2005
//

#ifndef CSL_AMBISONIC_PANNER_H
#define CSL_AMBISONIC_PANNER_H

#include "CSL_Core.h"
#include "SpatialPanner.h"
#include "Ambisonic.h"
#include "AmbisonicUtilities.h"
#include "SpeakerLayout.h"

namespace csl {

/// Ambisonic Panner
/// Wraps around the different ambisonic classes, providing a single interface for encoding, manipulating and decoding (i.e. panning) 
/// multiple UnitGenerators. 

class AmbisonicPanner : public SpatialPanner {
public:
	AmbisonicPanner(unsigned order = 1, SpeakerLayout *layout = SpeakerLayout::defaultSpeakerLayout()); // Default constructor
	AmbisonicPanner(unsigned hOrder, unsigned vOrder, SpeakerLayout *layout = SpeakerLayout::defaultSpeakerLayout()); // Default constructor
	~AmbisonicPanner();

	void addSource(SpatialSource &s);		///< Implement Panner's addSource, inserting an Encoder to each source.
	void removeSource(SpatialSource &s);	///< Remove a sound source

	void rotate(float amount); // AN ANGLE
	
											/// fill the buffer with the next buffer_length of values
	void nextBuffer(Buffer &outputBuffer /*, unsigned outBufNum */) throw (CException); 

	void dump() { }; ///< Print info about this instance

protected:
	AmbisonicMixer *mMixer;
	AmbisonicDecoder *mDecoder;
	AmbisonicRotator *mRotator;
	
//	void initialize();	
// 	virtual void speakerLayoutChanged();	///< called when the speaker layout changes, so panners update precalculated data	

};

} // end namespace

#endif