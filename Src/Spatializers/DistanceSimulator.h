//
//  DistanceSimulator.h -- Specification of the DistanceSimulators
//	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
//	Created by Jorge Castellanos on 6/23/06. Hacked 8/09 by STP.
//

#ifndef DISTANCE_SIMULATOR_H
#define DISTANCE_SIMULATOR_H
#include "SpatialSource.h"

#include "CSL_Core.h"
#include "CPoint.h"

namespace csl {

class IntensityAttenuationCue;
class AirAbsorptionCue;

/// Only handles single sound sources because objects have different positions. 
/// Two objects can't ocuppy the same position, and usually distance cues go before the 
/// panning, so handling multiple sources by one Distance simulator, would producde a multi-channel
/// object, where the positions aren't dynamic anymore.

class DistanceSimulator : public SpatialSource {
public:
	DistanceSimulator(UnitGenerator &source); // A UGen without position information. In this case, the distance simulator is the spatial source.
	DistanceSimulator(SpatialSource &source);
	~DistanceSimulator();

	virtual void nextBuffer(Buffer & outputBuffer, unsigned outBufNum) throw (CException);
										/// Returns wether the sound source position changed since last block call.
	virtual bool positionChanged();

protected:
				// SoundSource ... it refers to its input UGen, but with the knowledge of its position within a space.
//	SpatialSource *mSource;		// list of spatial inputs
	IntensityAttenuationCue *mIntensityCue;
	AirAbsorptionCue *mAirAbsorptionCue;

}; 

/// Pure Abstract Base Class for all distance cues. 
/// A DistanceCue subclass is just an algorithm that modifies a signal based on the distance from the listener.
/// Distance Cues should hold their state, so when "process" is called they can modify the signal buffer given.
/// When "compute" is called is usually due to a change in distance, so if anything has to be re-calculated, it's done.

class DistanceCue {
public:
	DistanceCue() { };
	virtual ~DistanceCue() { };
	
	virtual void compute(float distance) = 0;
	virtual void process(Buffer &inputBuffer) = 0;

}; 

/// Sound attenuation due to increasing distance. This "Base" Class uses the inverse square law.
/// Subclass for other methods.

class IntensityAttenuationCue : public DistanceCue {
public:
	IntensityAttenuationCue() { };
	virtual ~IntensityAttenuationCue() { };

	virtual void compute(float distance);	
	virtual void process(Buffer &aBuffer);
	
protected:	
	float mGain;

};

/// Simulates the frequency dependent air absorption using a one pole/zero lowpass filter.

class AirAbsorptionCue : public DistanceCue {
public:
	AirAbsorptionCue();
	virtual ~AirAbsorptionCue();

	virtual void compute(float distance); ///< Calculate the filter coefficients.
	virtual void process(Buffer &aBuffer); ///< Process each sample in the buffer, applying the filter.
	
protected:	
	float mBCoeff;
	float mACoeff[2];
	float mPrevOutput;
	float mPrevInput;

};

//class AirAbsorptionCue : public DistanceCue {
//public:
//	AirAbsorptionCue();
//	virtual ~AirAbsorptionCue();
//
//	virtual void compute(float distance);	
//	virtual void process(Buffer &aBuffer);
//	
//protected:	
//	float mBCoeff[3];
//	float mACoeff[3];
//	float mPrevOutputs[3];
//	float mPrevInputs[3];
//
//};

}

#endif
