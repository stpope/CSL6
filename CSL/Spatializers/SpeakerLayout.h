//
//  SpeakerLayout.h -- Class for loading and parsing loudspeaker position information used 
//		by the Spatializer and Auralizer classes.
//	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
//	Created by Jorge Castellanos on 6/16/06. Hacked 8/09 by STP.
//

#ifndef SPEAKERLAYOUT_H
#define SPEAKERLAYOUT_H

#include "CSL_Core.h"
#include "CPoint.h"

namespace csl {

class Speaker;		// Forward declaration. See below.

/**
A speaker layout holds a set of loudspeakers.
Each speaker has a position (in a space, where measurements are assumed to be from the center point of the space (also the center of the coordinate system).
To simplify usage, a default layout will be created when instantiating any object that uses the speaker layout.
If the user does not specify a layout then the default layout is used.
Any layout can be set to become default (by calling setDefaultSpeakerLayout()) so new objects that require a SpeakerLayout
make use of this prefered layout without the need of passing it explicitly per instance.
Using multiple layouts is still possible by passing the desired layout to the object that will use it.
In that case, this object will go with this layout instead of the default.

Conventions used in this code:
Coordinate system:
	- Left oriented
	- Internal angle representation: spherical radians
	- Azimuth = 0 on the x/z plane and increasing towards the positive y direction
	- Elevation = 0 on x/y plane and increasing towards the positive z direction

the layout is a passive object, meaning that it doesn't deal with processing any audio data. It only provides speaker positions.
An "ActiveSpeakerLayout" is active and can be used as a UnitGenerator that compensates for speaker positions, etc.

*/
class SpeakerLayout : public Model {
public:
	// Constructors & destructor:
	SpeakerLayout(const char *filePath = NULL);	///< Creates an empty speaker layout. Optionally reads loudspeaker layout from file.
	virtual ~SpeakerLayout();				///< destructor

									/// Reads the speaker listing file according to the specification 
	void readSpeakerFile(const char *filePath);

									/// Returns a pointer to the default layout. If no default exists, it creates one.
	static SpeakerLayout *defaultSpeakerLayout();
									/// Use it to set a layout as default. Clients (e.g. a Panner) can then make use of this layout.
	static void setDefaultSpeakerLayout(SpeakerLayout *defaultLayout);
	
									/// Add a speaker specifying its position in degrees from the center of the listening space.
	void addSpeaker(float azimuth, float elevation = 0.0, float radius = 1.0);
									/// Add a WFS speaker
	void addSpeaker(float x, float y, float z, float xNorm, float yNorm, float zNorm, float gain);

	unsigned numSpeakers() const { return mSpeakers.size(); };	///< Returns the number of loudspeakers in the layout

									/// Sets speaker distances to a fixed distance from the center of the listening space.
									/// @param radius is optional. If not set, it finds the best radius by analyzing the layout. 
	void normalizeSpeakerDistances(float radius = 0);
	
									/// Returns the speaker at the specified index.
	Speaker *speakerAtIndex(unsigned speakerIndex) const { return mSpeakers[speakerIndex]; };

	bool isPeriphonic() { return (mDimensions == 3); }; ///< If any of the speakers in the layout has an elevation other than 0, it returns true.
	void dump();

protected:
	vector<Speaker *> mSpeakers;			///< Vector of pointers to the loudspeakers


private:
	unsigned mDimensions;
//	void cartesianToSphericalRadians();	// convert speaker layout given in cartesian coordinates to spherical radians
	static SpeakerLayout *sDefaultSpeakerLayout;
	float *mSpeakerDistanceDeltas;		///< Holds the diference of the optimal speaker distance and the real one. Only used if distances are normalized.
	
public:
	SpeakerLayout &operator=(const SpeakerLayout &layout); ///< Overloaded "=" operator allows copying the layout.
};

/// Standard "Stereo Speaker Layout", where two speakers are positioned 30¼ left, 30¼ right and no elevation (0¼). 

class StereoSpeakerLayout : public SpeakerLayout {
public:
	StereoSpeakerLayout();
};

/// "Headphone  Layout", where two phones are positioned 90¼ left, 90¼ right and no elevation (0¼). 

class HeadphoneSpeakerLayout : public SpeakerLayout {
public:
	HeadphoneSpeakerLayout();
};


/// Represents a speaker as a position relative to the center of a space.
class Speaker {
public:
	/// Speaker constructor.
	/// The speaker class should only be used by the speaker layout. Clients should modify speakers using the Speaker layout
	/// and not deal with speakers directly. The speaker positions have to be specified in (spherical) radians.
	Speaker(float azimuth, float elevation, float radius = 1.0);
	
	// constructor with normal point. x,y,z are position, xNorm,yNorm,zNorm are a point to which the speaker is facing. converted to a normalized vector by Speaker class.
	Speaker(float x, float y, float z, float xNorm, float yNorm, float zNorm, float gain = 1.0);

	~Speaker() { };
	
	CPoint position()	{ return mPosition; }; // Return the position of the speaker.
	float azimuth()	{ return mPosition.theta(); };
	float elevation()	{ return mPosition.ele(); };
	float radius()		{ return mPosition.r(); };
	void setRadius(float radius); ///< Specify the distance from the center of the coordinate space to the speaker.
	
	CPoint normal()	{ return mNormal; }; // Return the normal of the speaker.
	float speakerGain()		{ return mGain;};
	
	void dump(); ///< Print speaker information.

protected:
	CPoint mPosition;
	CPoint mNormal; // normal vector for speaker
	float mGain;	// a speaker dependant gain, defaults to 1.
}; 

}

#endif
