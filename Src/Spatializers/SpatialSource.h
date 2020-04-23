//
//  SpeakerSource.h -- Spatial Sound Source
//	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
//	Created by Jorge Castellanos on 6/16/06. Hacked 8/09 by STP.
//
/// @todo Make a position object (possibly inherits from CPoint) that keeps both polar and cartesian values.

#ifndef SOUND_SOURCE_H
#define SOUND_SOURCE_H

#include "CSL_Core.h"
#include "CPoint.h"

namespace csl {

//class SpatialProperties {
//public:
//	SpatialProperties() : mPosition(0.f), mRoot(NULL), mPositionChanged(true) { };
//	~SpatialProperties() { };
//
//	float mPosition;
//	UnitGenerator *mRoot;
//	bool mPositionChanged;		//< true if this source's position has changed
//
//};

/// Temp Spatial Sound Source

class SpatialSource : public UnitGenerator, public virtual Scalable {
public:
	SpatialSource();						///< constructors
	SpatialSource(UnitGenerator &input, float azi = 0.0, float ele = 0.0, float dist = 1.0);
	virtual ~SpatialSource();
										/// Set the sound source position in cartesian coordinates.
	virtual void setPosition(CPoint pos); 
	virtual void setPosition(float x = 0., float y = 0., float z = 0.); 
	virtual void setPosition(double x = 0., double y = 0., double z = 0.); 

										/// Set the sound source position in spherical coordinates (degrees).
	virtual void setPosition(char s, float azi = 0, float ele = 0, float dist = 1.0) ;
	virtual void setPosition(char s, double azi = 0, double ele = 0, double dist = 1.0) ;
	CPoint & getPosition() { return (* mPosition); };

#ifdef DIRECTIONAL_SOURCES
										/// Specify what direction is the object facing
	void setOrientation(float horizontalAngle, float verticalAngle); 
										/// This is the actual sound radiation pattern of the sound object.
	void setDirectivity(float innerConeAngle, float outerConeAngle); 
										/// kType of radiation.
#endif

	void setAzimuth(float val)	{ mPosition->setAzimuth(val); };		///< Sets the horizontal angle.
	void setElevation(float val) { mPosition->setElevation(val); };		///< Sets the angle of elevation.
	void setDistance(float val)	{ mPosition->setMagnitude(val); };		///< Sets the distance from the center.
		
	float azimuth() { return mPosition->theta(); };						///< Returns the horizontal angle.
	float elevation() { return mPosition->ele(); };						///< Returns the angle of elevation.
	float distance() { return mPosition->r(); };						///< Returns the distance from the center.

	CPoint *position(unsigned channelNum = 0);

										/// Returns whether the sound source position changed since last block call.
	virtual bool positionChanged() { return mPositionChanged; };

	virtual void nextBuffer(Buffer & outputBuffer, unsigned outBufNum) throw (CException);
	virtual void nextBuffer(Buffer & outputBuffer) throw (CException);

	void dump();

protected:
	CPoint * mPosition;					///< source position
	bool mPositionChanged;				///< true if this source's position has changed recently

#ifdef DIRECTIONAL_SOURCES
	float mInnerCone;
	float mOuterCone;
	float mHorizontalOrientation;
	float mVerticalOrientation;
#endif
	
};

/*
	DESIGN THOUGHTS:
	Possibly Source could be called PositionableSource and be the base class of other Spatial Sources.
	For example, a subclass could include directivity and radiation pattern, wich is less common than position
	Also, different ways of sound radiation could be implemented by subclasses. 
	
	A second option is to specify sound radiation directly to the object that does the actual computation.
	This way the different processors could accept their very particular way of doing things.
	This approach would break with the concept of having the sound source KNOW its own characteristics.
	Cons: not only the processors of directivity, but also procesors of room acoustics would make use of 
	such info, so user would have to specify it twice (this could be a feature, as it would allow different
	types, one simpler for reflections, and a possibly complex one for the direct sound. 
	
	Maybe the answer is that sound radiation is not just a record class, but it responds with an actual value
	(or filter?) when asked for the value at x angle. That way the different types of pattern would know how to
	do their shit, but mantaining a common interface and return type, so it can be processed.
	
	Or.. should I just decide for one method and stick to it? That would defeat the purpose of an extensible
	framework made to be used for research, as well as for making content.
	
*/
}

#endif