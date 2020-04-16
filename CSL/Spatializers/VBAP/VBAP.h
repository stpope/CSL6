///
/// VBAP.h -- Vector Base Amplitude Panning class (v2 - Nov 2006)
///	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
/// Doug McCoy, 2004.
///

#ifndef CSL_VBAP_H
#define CSL_VBAP_H

#include "CSL_Core.h"
#include "SpatialPanner.h"

//#define CSL_DEBUG

#define MAX_NUM_VBAP_SOURCES 8

#include <iostream>

///////////////// used for matrix.h //////////////////
#define _NO_EXCEPTION

#include "matrix.h"
//#include <cmatrix>

#ifndef _NO_NAMESPACE
	using namespace std;
	using namespace math;
//  using namespace techsoft;
	#define STD std
#else
	#define STD
#endif

#define TRYBEGIN()
#define CATCHERROR()

typedef matrix<double> CSLMatrix;

///////////////// end used for matrix.h //////////////////

namespace csl {

#define deg2rad(x)   (((double)x ) * CSL_TWOPI) / 360.0

/// VBAP types
typedef enum {
	kAuto = 0,
	kPantophonic = 2,		///< Only uses the horizontally placed speakers (zero elevation)
	kPeriphonic = 3			///< Full 3D VBAP
} VBAPMode;

class SpeakerSetLayout;  // Forward declaration. See below.


/// Vector Base Amplitude Panning.
/** 
A panning technique that uses amplitude as the method for sound placement (just as stereo panning does).
When multiple speakers are present, sets of all possible adjacent speaker pairs or in triples (depending if doing
3D or only 2D panning) are found, reducing the "Active speakers" to two or three. See Pulkki for more info.
*/
class VBAP : public SpatialPanner {
public:
	/// Initializer for a VBAP Panner. Optionally a speaker layout can be specified.
	/// Defaults to auto, decided according to layout. If not specified uses the default speaker layout.
	VBAP(VBAPMode mode = kAuto, SpeakerLayout *layout = SpeakerLayout::defaultSpeakerLayout()); 

	virtual ~VBAP();
	
	/// Just as any Effect in CSL, this method gets called at runtime by the audio driver. Here is where the actual processing happens.
	void nextBuffer(Buffer &outputBuffer) throw (CException);
	void nextBuffer(Buffer &outputBuffer, unsigned outBufNum) throw (CException);

	void dump() { }; ///< Prints useful information about this VBAP instance.

protected:
	VBAPMode mMode;		///< Represents the dimensionality of the panner, either 2 or 3. 2D meaning horizontal panning only.
	SpeakerSetLayout *mSpeakerSetLayout;  ///< A reference to the layout that contains the speaker pairs or triplets.
 
	virtual void *cache();		///< Returns an instance of it's cache data per sound source.
 	virtual void speakerLayoutChanged();	///< called when the speaker layout changes, so panners update precalculated data	

};

/// Simple Stereo Panner
/// @todo Add method for setting the pan value from 0 to 1.
class StereoPanner : public VBAP {
public:
	StereoPanner();
	virtual ~StereoPanner();

	void addInput(UnitGenerator &input, float panPosition);
	
};

/// Generic Panner for most surround formats.
/// It accepts any number of speakers around the horizontal plane, and a subwoofer (.1 or not)
class SurroundPanner : public VBAP {
public:
	/// The constructor defaults to a 5.1 setup. 
	SurroundPanner(unsigned numSpeakers = 5, bool useSubwoofer = true);
	virtual ~SurroundPanner();

};

// Used for caching data from previous callbacks. Each SpatialSource has its own set of data to be remembered.
// This is just for internal use of the VBAP object.
class VBAPSourceCache {
public:
	VBAPSourceCache() : tripletIndex(0) { };
	~VBAPSourceCache() { };
	
	float gains[3];
	unsigned tripletIndex;
};
 
/// Groups two or three loudspeakers, and their inverse. Used for VBAP computations.
class SpeakerSet { 
public:
	SpeakerSet(unsigned size = 3);
	~SpeakerSet();

	unsigned *nodes;		///< the index for each of the three speakers that represent the triplet
	CSLMatrix *invL;		///< pre-computed inverse matrix for this triplet
	void dump();		///< just print friendly info about the triplet
};

/*// **** SPEAKER TRIPLET LAYOUT  ***** 
TO AVOID BUILDING A TRIPLET LAYOUT PER VBAP INSTANCE, THE VBAP CLASS WILL HAVE A STATIC METHOD THAT 
RETURS A MAP OF THE SPEAKER LAYOUT (KEY) AND THE TRIPLET SPEAKER LAYOUT (VALUE). THIS WAY, IF ANOTHER
INSTANCE HAS ALREADY CREATED A TRIPLET LAYOUT (IN OTHER WORDS IF A NEW INSTANCE IS USING THE SAME LAYOUT
THAN THE ALREADY CREATED INSTANCE) THEN A POINTER TO THE TRIPLET LAYOUT WILL BE RETURNED.
AS AN ALTERNATIVE, INSTEAD OF A MAP, THE TRIPLET SPEAKER LAYOUT WILL HOLD A POINTER TO THE SPEAKER LAYOUT
AND WILL BE ABLE TO COMPARE TO A PASSED POINTER. IN THES CASE, A STATIC VECTOR OF TRIPLET LAYOUTS WOULD HOLD
THE ALREADY CREATED TRIPLET LAYOUTS AND WHEN CREATING A NEW VBAP INSTANCE, IT WOULD GO THRU EACH TRIPLET LAYOUT
(IF ANY) COMPARING SPEAKER LAYOUT ADDRESSES. IF A MATCH IS FOUND, THE TRIPLET LAYOUT WOULD BE SET TO BE THE OBJECT
HELD BY THE VBAP INSTANCE (EACH VBAP INSTANCE WILL KEEP ITS OWN POINTER TO ITS TRIPLET LAYOUT OBJECT. FOR THIS TO WORK
SOME SORT OF COUNT WILL BE NEEDED SO WHEN NO OBJECT IS KEEPING A TRIPLET, THEN IT SHOULD GET DESTROYED.
WHEN SETTING THE SPEAKER LAYOUT, THE OBJECT WILL HAVE TO SEARCH AGAIN FOR TRIPLET LAYOUTS ALREADY CREATED, AND RE COMPUTE 
ONE IF NONE EXISTS.
ADVANTAGES: 
	A) THE TRIPLET FINDING GOES TO THE LAYOUT AND AWAY FROM THE VBAP, SIMPLIFYING THE VABAP INTERFACE.
	B) SAVES MEMORY USAGE BY HAVING ONLY ONE INSTANCE OF TRIPLETS FOR EACH LAYOUT USED.
		IN MOST CASES, ONLY ONE LAYOUT IS USED, WHICH TRANSLATIES INTO ONLY ONE SET OF TRIPLETS IN MEMORY.
	C) SAVES COMPUTATION BY NOT RECALCULATING TRIPLETS FOR EACH VBAP INSTANCE. IT DOES IT ONLY ON ONE.
	
COST:
	A) COMPUTATIONALLY THE COST IS MINIMAL. AT INSTANTIATION TIME, THE OBJECT HAS TO SEARCH THRU A VECTOR. 
		THIS BECOMES NULL IF NO OBJECTS ARE USED AND INSIGNIFICANT WHEN ONE OR MORE OBJECTS ARE INSTANTIATED.
	B) REGARDING MEMORY, IT'LL HAVE TO LOAD STATICALLY A VECTOR AT APP LOAD TIME...

*/
class SpeakerSetLayout {
friend class VBAP; // Grant VBAP the access to your private members.
public:
	/// Constructors & destructor:
	SpeakerSetLayout(SpeakerLayout *aLayout, VBAPMode mode = kAuto); ///< default constructor. Creates an empty speaker layout.
	~SpeakerSetLayout();				///< destructor

	/// Returns the Speaker Layout used to find the triplets.
	SpeakerLayout *speakerLayout() { return mSpeakerLayout; }; // Ussed to compare layouts in case it's already calculated.

	void dump();

private:
	// THE TRIPLETS SHOULD BE SHARED AMONG ALL VBAP OBJECTS, UNLESS MULTIPLE LAYOUTS ARE USED SIMULTANEOUSLY.
	SpeakerSet **mTriplets;	// list of output triples
	SpeakerLayout *mSpeakerLayout;
	unsigned mNumTriplets;
	unsigned mMode;
	
	void findSpeakerTriplets() throw(CException);
	void findSpeakerPairs() throw(CException);
	void invertTripleMatrix(SpeakerSet *lst);
	void addTriple(SpeakerSet *lst);
	void removeTriple(SpeakerSet *lst);
	bool evaluateCrossing(CPoint &li, CPoint &lj, CPoint &ln, CPoint &lm);
};

} // end namespace

#endif

