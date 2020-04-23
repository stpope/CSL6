//
//  SpeakerSource.cpp -- Spatial Sound Source
//	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
//	Created by Jorge Castellanos on 6/16/06. Hacked 8/09 by STP.
//

#include "SpatialSource.h"

#include <stdlib.h>
#include <iostream>

using std::cout;
using std::endl;

using namespace csl;

// SpatialSource constructors

SpatialSource::SpatialSource() : mPosition(NULL), mPositionChanged(true) { }

SpatialSource::SpatialSource(UnitGenerator &input, float azi, float ele, float dist)
		: mPosition(NULL), 
		  mPositionChanged(true) {
	if (input.numChannels() != 1)
		throw LogicError("Adding a non-mono ugen to a spatial source");
	mPosition = new CPoint(kPolar, (float)dist, (float)(azi * CSL_PI/180.0), (float)(ele * CSL_PI/180.0));
	this->addInput(CSL_INPUT, input);
#ifdef CSL_DEBUG
	logMsg("SpatialSource::input UG added");
#endif
}

SpatialSource::~SpatialSource() {
	if (mPosition)
		delete mPosition;
}

// Geometry

CPoint *SpatialSource::position(unsigned channelNum) {
	if (mPosition == NULL)
		mPosition = new CPoint(1.0, 0.0, 0.0);
	return mPosition;
}

void SpatialSource::setPosition(CPoint pos) {
	mPosition = & pos;
}

void SpatialSource::setPosition(float x, float y, float z) {
	if (mPosition) {
		mPosition->set(x, y, z ); 
		mPositionChanged = true;
	} else
		mPosition = new CPoint(x, y, z);
}

void SpatialSource::setPosition(double x, double y, double z) {
	if (mPosition) {
		mPosition->set(x, y, z ); 
		mPositionChanged = true;
	} else
		mPosition = new CPoint(x, y, z);
}

void SpatialSource::setPosition(char s, float azi, float ele, float dist) {
	if (mPosition) {
		mPosition->set(kPolar, (float)dist, (float)(azi * CSL_PI/180.0), (float)(ele * CSL_PI/180.0) ); 
		mPositionChanged = true;
	} else
		mPosition = new CPoint(kPolar, (float)dist, (float)(azi * CSL_PI/180.0), (float)(ele * CSL_PI/180.0));
}

void SpatialSource::setPosition(char s, double azi, double ele, double dist) {
	if (mPosition) {
		mPosition->set(kPolar, (float)dist, (float)(azi * CSL_PI/180.0), (float)(ele * CSL_PI/180.0) ); 
		mPositionChanged = true;
	} else
		mPosition = new CPoint(kPolar, (float)dist, (float)(azi * CSL_PI/180.0), (float)(ele * CSL_PI/180.0));
}

void SpatialSource::dump() {
//	cout << "x: " << mPosition->x << "\t" << "y: " << mPosition->y << "\t" << "z: " << mPosition->z << endl;
//	cout << "\t\tradius: " << distance() << "\tazi: " << azimuth() << "\tele: " << elevation() << endl;
	printf("\taz: %5.2f  el: %5.2f  dist: %4.2f\n",
			azimuth(), // * CSL_DEGS_PER_RAD, 
			elevation(), // * CSL_DEGS_PER_RAD, 
			distance());
}

// nextBuffer

void SpatialSource::nextBuffer(Buffer &outputBuffer, unsigned outBufNum) throw (CException) {
	Port *inPort = mInputs[CSL_INPUT];
	inPort->mUGen->nextBuffer(outputBuffer);			// get its UGen	
	mPositionChanged = false;
}

void SpatialSource::nextBuffer(Buffer &outputBuffer) throw (CException) {
#ifdef CSL_DEBUG
	logMsg("SpatialSource::nextBuffer");
#endif
	nextBuffer(outputBuffer, 0);
}
