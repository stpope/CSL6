//
//  DistanceSimulator.cpp -- Specification of the DistanceSimulators
//	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
//	Created by Jorge Castellanos on 6/21/06. Hacked 8/09 by STP.
//

#include "DistanceSimulator.h"

using namespace csl;

// DistanceSimulator

DistanceSimulator::DistanceSimulator(UnitGenerator &source) : SpatialSource(source) {
	this->addInput(CSL_INPUT, source);
	mPosition = new CPoint(1.0, 0.0, 0.0); // If my parent is a simple UGen then I own my Position.
	mIntensityCue = new IntensityAttenuationCue;
	mAirAbsorptionCue = new AirAbsorptionCue;
}

DistanceSimulator::DistanceSimulator(SpatialSource &source) : SpatialSource(source) {
	this->addInput(CSL_INPUT, source);
	mPosition = source.position(); // If my parent is a SpatialSource, then just point to it.
	mIntensityCue = new IntensityAttenuationCue;
	mAirAbsorptionCue = new AirAbsorptionCue;
}

DistanceSimulator::~DistanceSimulator() {
	delete mIntensityCue;
	delete mAirAbsorptionCue;
}

/// Returns whether the sound source position changed since last block call.

bool DistanceSimulator::positionChanged() {
	return ((SpatialSource *) mInputs[CSL_INPUT]->mUGen)->positionChanged();
}

// work-horse method

void DistanceSimulator::nextBuffer(Buffer & outputBuffer, unsigned outBufNum) throw (CException) {
	float distance = this->distance();		// I call distance of myself, which in turn points to the source distance.
											// coded this way, in order to allow UnitGenerators passed to a distance simulator.
	Port *inPort = mInputs[CSL_INPUT];
	UnitGenerator *inputSound = inPort->mUGen;
	
	inputSound->nextBuffer(outputBuffer);			// get its UGen	

//	UnitGenerator *soundSource = ((UGenPort *)mInputs[CSL_INPUT])->mUGen;
//	soundSource->nextBuffer(outputBuffer);
	
	mIntensityCue->compute(distance); 
	mIntensityCue->process(outputBuffer);
											// Air damping at short distances (< than 10, 15 meters) can be ignored
	if (distance > 10) { 
		mAirAbsorptionCue->compute(distance);
		mAirAbsorptionCue->process(outputBuffer);
	}
	mPositionChanged = ((SpatialSource *) inputSound)->positionChanged();
}

// IntensityAttenuationCue

void IntensityAttenuationCue::compute(float distance) {
	mGain = 1 / distance; //(distance * distance);
}
	
void IntensityAttenuationCue::process(Buffer &aBuffer) {
	SampleBuffer outputPtr = aBuffer.buffer(0);
	unsigned numFrames = aBuffer.mNumFrames;
	
	for (unsigned k = 0; k < numFrames; k++)	// k loops through sample buffers
		*outputPtr++ *= mGain;
}

// AirAbsorptionCue

AirAbsorptionCue::AirAbsorptionCue() {
	mPrevOutput = 0;
	mPrevInput = 0;
}

AirAbsorptionCue::~AirAbsorptionCue() {

}

void AirAbsorptionCue::compute(float distance) {
	unsigned frameRate = CGestalt::frameRate();
	float cutoffFreq = 22000 * 9/distance;
    float w = 2.0 * frameRate;
    float Norm;

    cutoffFreq *= 2.0F * CSL_PI;
    Norm = 1.0 / (cutoffFreq + w);
    mBCoeff = (w - cutoffFreq) * Norm;
    mACoeff[0] = mACoeff[1] = cutoffFreq * Norm;
}


void AirAbsorptionCue::process(Buffer &aBuffer) {
	SampleBuffer outputPtr = aBuffer.buffer(0);
	unsigned numFrames = aBuffer.mNumFrames;
	
	for (unsigned i = 0; i < numFrames; i++) {
		mPrevOutput = (*outputPtr) * mACoeff[0] + mPrevInput * mACoeff[1] + mPrevOutput * mBCoeff;
		mPrevInput = (*outputPtr);
		*outputPtr++ = mPrevOutput;
	}
}

//	mBCoeff[0] = 1.0 - exp((-CSL_PI) * 2 * cutoffFreq/frameRate);
//	mACoeff[0] = 1.0 - mBCoeff[0];
//

//		prevOuts[0] = mBCoeff[0] * (*outputPtr) - mACoeff[0] * prevOuts[0];			// put current output sample in the output buffer and increment
//		*outputPtr++ = prevOuts[0];


//AirAbsorptionCue::AirAbsorptionCue() {
//	
//}
//
//AirAbsorptionCue::~AirAbsorptionCue() {
//
//}
//
//void AirAbsorptionCue::compute(float distance) {
//	unsigned cutoffFreq = 10000;
//	unsigned frameRate = CGestalt::frameRate();
//	float C = 1 / (tan (CSL_PI * (cutoffFreq/frameRate)) );
//	
//	mBCoeff[0] = 1 / (1 + (CSL_SQRT_TWO * C) + (C * C) );
//	mBCoeff[1] = 2 * mBCoeff[0];
//	mBCoeff[2] = mBCoeff[0];
//	mACoeff[0] = 0.f;
//	mACoeff[1] = 2 * mBCoeff[0] * (1 - (C * C));
//	mACoeff[2] = mBCoeff[0] * (1 - (CSL_SQRT_TWO * C) + (C * C) );
//}
//
//
//void AirAbsorptionCue::process(Buffer &aBuffer) {
//	SampleBuffer outputPtr = aBuffer.buffer(0];
//	unsigned numFrames = aBuffer.mNumFrames;
//	float *prevOuts = mPrevOutputs;
//	float *prevIns = mPrevInputs;
//
//	for (unsigned i = 0; i < numFrames; i++) 	{
//
//		prevOuts[0] = 0.f;
//		prevIns[0] = *outputPtr;	// get next input sample
//		for (unsigned j = 2; j > 0; j--) {
//			prevOuts[0] += mBCoeff[j] * prevIns[j];
//			prevIns[j] = prevIns[j-1];
//		}
//		prevOuts[0] += mBCoeff[0] * prevIns[0];
//		for (unsigned k = 2; k > 0; k--) {
//			prevOuts[0] += -mACoeff[k] * prevOuts[k];
//			prevOuts[k] = prevOuts[k-1];
//		}
//		*outputPtr++ = prevOuts[0];			// put current output sample in the output buffer and increment
//	} 
//	
//}
