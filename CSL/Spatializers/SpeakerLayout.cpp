//
//  SpeakerLayout.cpp -- Class for loading and parsing loudspeaker position information used 
//		by the Spatializer and Auralizer classes.
//	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
//	Created by Jorge Castellanos on 6/16/06. Ammend for WFS, will wolcott, 6/07. Hacked 8/09 by STP.
//

#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include <string.h>

#include "SpeakerLayout.h"

#define INV_SQRT2 (1/1.414213562)

using std::cout;
using std::endl;
using namespace csl;

// A static private member that holds the default layout.

SpeakerLayout *SpeakerLayout::sDefaultSpeakerLayout = 0;

/// Returns a reference to the default layout.
/// If no layout has been set, it creates a stereo speaker layout.

SpeakerLayout *SpeakerLayout::defaultSpeakerLayout() {
					// if being used for the first time, create a new layout and set it as default.
	if (sDefaultSpeakerLayout == 0)
		sDefaultSpeakerLayout = new StereoSpeakerLayout();
	return sDefaultSpeakerLayout;
}

/// The default speaker layout is used by Panners, when not specified otherwise.
/// If using multiple Panners, but only one speaker setup, is best to set the layout
/// as default, and then forget about that; Panners will know to use such layout.
/// @note As a suggestion, set the default layout before creating any Panners. Otherwise they'll have to rebuild their data.

void SpeakerLayout::setDefaultSpeakerLayout(SpeakerLayout *defaultLayout) {
						// prevent a null speaker layout.
	if (sDefaultSpeakerLayout == 0)
		sDefaultSpeakerLayout = SpeakerLayout::defaultSpeakerLayout();
	
						// Make a "Hard-copy" of the layout. Client can then free their memory. 
	*sDefaultSpeakerLayout = *defaultLayout; // copy the layout to the new one.
	
						// Notify clients that the layout has changed, so they update accordingly.
	sDefaultSpeakerLayout->changed((void *)sDefaultSpeakerLayout);
}


// ******** Constructors ***********//

/**
Reads loudspeaker layout from a text file.
Loudspeaker layouts provided as textfiles should folow the format below:
-> A / indicates the start of a comment, which lasts until the end of the line (??????????????????????).
-> The Keywords CARTESIAN, SPHERICAL-DEGREES, SPHERICAL-RADIANS are used to specify the format of the provided speaker positions.
   The class will do necessary conversions to the internal representation in spherical radians.
   If no keyword is specified, spherical radians are assumed(??????????????????????).
-> Non-comment or non-keyword lines should contain speaker position information as three tab-separated coordinates.
   Each line will be interpreted as a new speaker.
Examples of parsable speaker layout files are provided with the code (.dat files).

---> needs .wfs file description

*/
SpeakerLayout::SpeakerLayout(const char *filePath) : mDimensions(2) {
	readSpeakerFile(filePath);
}

SpeakerLayout::~SpeakerLayout() {
	for (unsigned i = 0; i < mSpeakers.size(); ++i)
		delete mSpeakers[i];
//	if (mSpeakerDistanceDeltas != NULL)
//		delete[] mSpeakerDistanceDeltas;
//	mSpeakerDistanceDeltas = 0;
}

// load in a config file

void SpeakerLayout::readSpeakerFile(const char *filePath){
	if (filePath != NULL) {
		char testChar;
		char lineBuffer[1024]; // a space for reading lines of text into - arbitrary maximum length of 1023 characters
		char *file = (char *) malloc(strlen(filePath));
		strcpy(file, filePath);
		const char *cartesian = "CARTESIAN";
		const char *degrees = "SPHERICAL-DEGREES";
		const char *normal = "WITH-NORMAL";
		bool isCartesian = false;
		bool isDegrees = false;
		bool withNormal = false;
		CPoint tempSpeaker;
		CPoint tempNormal;
		float tempGain;
		
		ifstream inputFile(file, ios::in); // Open the file
		if ( ! inputFile) {  // Making sure file opened.
			logMsg(kLogError, "SpeakerLayout unable to open file\n");
			exit(1);
		}
								// read a character from file until reached end
		while ((testChar = inputFile.peek()) != EOF) {
			if (testChar == '/') {	// skip any lines that begin with / character
				inputFile.getline(lineBuffer, 1023); 
				continue;
			} else if (testChar == ' ' || testChar == '\t' || testChar == '\n') {
				inputFile.ignore(1);	// ignore space, tab and newline characters
			} else if ( ! isdigit(testChar) && testChar != '-') {
				inputFile.get(lineBuffer, 32);				// read the text flags (CARTESIAN, SPHERICAL-DEGREES etc)
				if ( ! strcmp(lineBuffer, cartesian)) isCartesian = true;
				if ( ! strcmp(lineBuffer, degrees)) isDegrees = true;
				if ( ! strcmp(lineBuffer, normal)) withNormal = true;
			} else {
				if (withNormal){
					// read speaker position and a point to which the speaker is facing
					inputFile >> tempSpeaker.x >> tempSpeaker.y >> tempSpeaker.z >> tempNormal.x >> tempNormal.y >> tempNormal.z >> tempGain;
					this->addSpeaker(tempSpeaker.x, tempSpeaker.y, tempSpeaker.z, tempNormal.x, tempNormal.y, tempNormal.z, tempGain);
				}else{
					// numeric information - read each word as consecutive speaker coordinates
					inputFile >> tempSpeaker.x >> tempSpeaker.y >> tempSpeaker.z;
					this->addSpeaker(tempSpeaker.x, tempSpeaker.y, tempSpeaker.z);
				}
			}
		}
		
		//	if (isCartesian) {
		//		// conversion of cartesian to sperical (using radians)
		//		cartesianToSphericalRadians();
		//	} else if (isDegrees) {	
		//		double degreeInRadians = CSL_PI/180.0;
		//		// conversion from degrees to radians
		//		for (unsigned i = 0; i < mNumSpeakers; i++) {
		//			mSpeakers[i]->coord1 *= degreeInRadians;
		//			mSpeakers[i]->coord3 *= degreeInRadians;
		//			mSpeakers[i]->coord3 = fabs(mSpeakers[mNumSpeakers]->coord3);
		//		}
		//	}
		
		dump();
		free(file);
	}
}

/// Add a speaker to the layout. Parameters should be specified in degrees.

void SpeakerLayout::addSpeaker(float azimuth, float elevation, float radius) {
	float scaleAz = azimuth * CSL_PI/180.0;
	float scaledEl = elevation * CSL_PI/180.0;
	Speaker * newSpeaker = new Speaker(scaleAz, scaledEl, radius);
	mSpeakers.push_back(newSpeaker);
	if (elevation)				// If not only horizontally placed, then set as full periphonic.
		mDimensions = 3;
							// Notify clients that the layout has changed, so they update accordingly.
	this->changed((void *)this);
//	cout << "	Pos = " << scaleAz << " @ "<< scaledEl << " @ "<< radius << endl; 
//	newSpeaker->dump();
}

/// Add a speaker to the layout. Parameters should be specified in x,y,z
void SpeakerLayout::addSpeaker(float x, float y, float z, float xNorm, float yNorm, float zNorm, float gain){

	Speaker * newSpeaker = new Speaker(x,y,z,xNorm,yNorm,zNorm, gain);
	mSpeakers.push_back(newSpeaker);
	
							// Notify clients that the layout has changed, so they update accordingly.
	this->changed((void *)this);
	cout << "	Pos = " << x << " "<< y << " "<< z << " Norm = " << xNorm << " " << yNorm << " " << zNorm << endl; 
}

/// When speakers are not placed at the same distance from the "sweet spot" (the center of the listening space)
/// it's common to assume they are at a fixed distance by setting all of them as if they were, and then the output
/// data is delayed to compensate for this difference. See: ActiveSpeakerLayout.

void SpeakerLayout::normalizeSpeakerDistances(float radius) {
	unsigned nnumSpeakers = mSpeakers.size();
	if (mSpeakerDistanceDeltas != NULL) {
		delete[] mSpeakerDistanceDeltas;
		mSpeakerDistanceDeltas = 0;
	}
	mSpeakerDistanceDeltas = new float[nnumSpeakers];
	float theRadius = radius;
	if ( ! radius) { // If not specified a decired radius find the closest one.
		float tempRadius = 0;
		// Find the speaker that's placed the farthest from the center.
		for (unsigned i = 0; i < nnumSpeakers; i++) { 
			tempRadius = mSpeakers[i]->radius();
			if (tempRadius > theRadius)
				theRadius = tempRadius;
		}
	}
	// Go thru all the speakers, change their distance from the source to be the new radius and save the differenc to the real radius
	for (unsigned i = 0; i < nnumSpeakers; i++) { 
		mSpeakerDistanceDeltas[i] = mSpeakers[i]->radius() - theRadius;
		mSpeakers[i]->setRadius(theRadius);
	}
}

/// Prints the number of speakers in the layout and their position.

void SpeakerLayout::dump() {
	unsigned nnumSpeakers = mSpeakers.size();
	cout << "Number of Loudspeakers: " << nnumSpeakers << endl;
	for (unsigned i = 0; i < nnumSpeakers; i++) { 
		cout << "Speaker #" << i << ": "; 
		mSpeakers[i]->dump();
	}
}

// Overloaded "=" operator

SpeakerLayout &SpeakerLayout::operator=(const SpeakerLayout &layout) {
	if (this != &layout) {
		unsigned nnumSpeakers = mSpeakers.size();
		// Remove all speakers
		for (unsigned i = 0; i < nnumSpeakers; ++i)
			delete mSpeakers[i];
		mSpeakers.clear(); // Erase all items in the vector.
		nnumSpeakers = layout.numSpeakers(); // How many speakers the new layout has?
		Speaker *tempSpeaker;
		for (unsigned i = 0; i < nnumSpeakers; ++i) {
			tempSpeaker = new Speaker(0, 0);
			*tempSpeaker = *(layout.speakerAtIndex(i));
			mSpeakers.push_back(tempSpeaker);
		}
	}
	return *this;
}

Speaker::Speaker(float tazimuth, float televation, float tradius) {
	mPosition.set(kPolar, tradius, tazimuth, televation);
	mNormal.set(1.0, 0.0, 0.0);
	mGain = 1.0f;
}

void Speaker::setRadius(float dist) {
	if (radius() != dist) {
		mPosition.normalize(); // make it unit length
		mPosition *= dist; // multiply by "radius" so that length becomes that value.
	}
}

Speaker::Speaker(float x, float y, float z, float xNorm, float yNorm, float zNorm, float gain)
{
	mPosition.set(x, y, z); // set position

	float spNormX,spNormY,spNormZ;
	
	// was a point in which it pointed, convert to a vector emitting from the speaker
	spNormX = xNorm - x; spNormY = yNorm - y; spNormZ = zNorm - z;
	
	float mag = sqrt(spNormX * spNormX + spNormY * spNormY + spNormZ * spNormZ);

	// ahhh! what the hell? just default to origin...this could cause some weird problems.
	if (mag < 0.00001){
		spNormX = -x;
		spNormY = -y;
		spNormZ - -z;
		mag = sqrt(x * x + y * y + z * z);
		}

	// normalize!
	spNormX /= mag;
	spNormY /= mag;
	spNormZ /= mag;
	
	mNormal.set(spNormX, spNormY, spNormZ);
	mGain = gain;
}

void Speaker::dump() {
		cout << "Azimuth: " << azimuth() * 180/CSL_PI  
			<< "\tElevation: " << elevation() * 180/CSL_PI
			<< "\tDistance: " << radius() << endl;
}


//void ActiveSpeakerLayout::nextBuffer(Buffer & outputBuffer, unsigned outBufNum) throw (Exception) {
//
//	input->nextBuffer(outputBuffer);
//	
//	if
//		mCompensatesForDistance;
//	mRemapsChannels;
//
//
//
//}

//////////////////////////////////////
/*       STEREO SPEAKER SETUP       */
/////////////////////////////////////

StereoSpeakerLayout::StereoSpeakerLayout() : SpeakerLayout() {
		addSpeaker(-30);
		addSpeaker(30);
}

HeadphoneSpeakerLayout::HeadphoneSpeakerLayout() : SpeakerLayout() {
		addSpeaker(-90);
		addSpeaker(90);
}
