//
//  ShoeBox.m
//  ShoeBox
//
//  Created by Jorge Castellanos & Will Wolcott on 4/29/05.
//  Copyright 2005 Jorge Castellanos & Will Wolcott. All rights reserved.
//


//	IMPORTANT NOTES: 
//	6) WRITE ALL INITIALIZERS NEEDED
//	7) WE MUST CHECK (IN THE INIT) THAT LISTENER AND SOURCE ARE ACTUALLY INSIDE THE ROOM.


#include "ShoeBox.h"

#include "CSL_Core.h"

const float kWoodCoefficient		= 0.85; // 1/4" Plywood
const float kMarbleCoefficient		= 0.99;
const float kConcreteCoefficient	= 0.98;
const float kBrockCoefficient		= 0.97;
const float kGlassCoefficient		= 0.96;

#define SPEED_OF_SOUND 0.341

using namespace csl;

//	This will be one of the many constructors to set up the object.

ShoeBox::ShoeBox() : mRoomSize(3, 4, 3), mSource(1, 2, 1), mListenerPosition(1.5, 2., 1.) {

	mDampX = kBrockCoefficient;
	mDampY = kBrockCoefficient;
	mDampZ = kBrockCoefficient;
	
	mDuration = 50;
	
	
}

ShoeBox::~ShoeBox() {
	

}

void ShoeBox::setWallMaterialsDamp(int dampX, int dampY, int dampZ) {

	switch(dampX) {
		case kMarble:
			mDampX = kMarbleCoefficient;
			break;
		case kConcrete:
			mDampX = kConcreteCoefficient;
			break;		
		case kBrick:
			mDampX = kBrockCoefficient;
			break;
		case kGlass:
			mDampX = kGlassCoefficient;
			break;
		case kWood:
			mDampX = kWoodCoefficient;
			break;
	}
	switch(dampY) {
		case kMarble:
			mDampY = kMarbleCoefficient;
			break;
		case kConcrete:
			mDampY = kConcreteCoefficient;
			break;		
		case kBrick:
			mDampY = kBrockCoefficient;
			break;
		case kGlass:
			mDampY = kGlassCoefficient;
			break;
		case kWood:
			mDampY = kWoodCoefficient;
			break;
	}
	switch(dampZ) {
		case kMarble:
			mDampZ = kMarbleCoefficient;
			break;
		case kConcrete:
			mDampZ = kConcreteCoefficient;
			break;		
		case kBrick:
			mDampZ = kBrockCoefficient;
			break;
		case kGlass:
			mDampZ = kGlassCoefficient;
			break;
		case kWood:
			mDampZ = kWoodCoefficient;
			break;
	}

}

void ShoeBox::setSourceLocation(float x, float y, float z) {
	
	mSource.x = x;
	mSource.y = y;
	mSource.z = z;

	logMsg("setSourceCalled: X = %g, Y = %g, Z = %g", mSource.x, mSource.y, mSource.z);
	
	checkPositionValues();
}

void ShoeBox::setListenerLocation(float x, float y, float z) {
	
	mListenerPosition.x = x;
	mListenerPosition.y = y;
	mListenerPosition.z = z;
	
	checkPositionValues();
}

void ShoeBox::setRoomSize(float x, float y, float z) {
	
	mRoomSize.x = x;
	mRoomSize.y = y;
	mRoomSize.z = z;

	checkPositionValues();
}

bool ShoeBox::checkPositionValues() {
	
	if(mRoomSize.x < mListenerPosition.x || mRoomSize.x < mSource.x)
		return 1;
	if(mRoomSize.y < mListenerPosition.y || mRoomSize.y < mSource.y)
		return 1;
	if(mRoomSize.z < mListenerPosition.z || mRoomSize.z < mSource.z)
		return 1;
	
	return 0;
}

void ShoeBox::shellSort() {
	int i, j, increment;
	float temp, pairTemp, anglesTemp;

	increment = 3;
	while (increment > 0)
	{
		for (i=0; i < mNumReflections; i++)
		{
		  j = i;
		  temp = mReflectionsArray[i];
		  pairTemp = mGainsArray[i];
		  anglesTemp = mAnglesArray[i];
		  while ((j >= increment) && (mReflectionsArray[j-increment] > temp)) {
			mReflectionsArray[j] = mReflectionsArray[j - increment];
			mGainsArray[j] = mGainsArray[j - increment];
			mAnglesArray[j] = mAnglesArray[j - increment];
			j = j - increment;
		  }
		  mReflectionsArray[j] = temp;
		  mGainsArray[j] = pairTemp;
		  mAnglesArray[j] = anglesTemp;
		}
		
		if (increment/2 != 0)
		  increment = increment/2;
		else if (increment == 1)
		  increment = 0;
		else
		  increment = 1;
	}

}


void ShoeBox::setDuration(int ms) { mDuration = ms; }


void ShoeBox::findNumReflectionsFromTime() {
	
	// VERY FAST AND UGLY HACK TO NOT OVER CALCULATE REFLECTIONS. WE'LL NEED TO ADD A TIMETOREF FUNCT.
	mDuration = (mRoomSize.x + mRoomSize.y + mRoomSize.z) * 3;
//	0.341 is the speed of sound in mm/s @ 20 Celsius
	float clippingRadius = SPEED_OF_SOUND * mDuration;	// clippingRadius is the maximum ray length to be traced.
	mMaxBouncesX = (int)ceil(clippingRadius / (mRoomSize.x));
	mMaxBouncesY = (int)ceil(clippingRadius / (mRoomSize.y));
	mMaxBouncesZ = (int)ceil(clippingRadius / (mRoomSize.z));

	mNumReflections = mMaxBouncesX * mMaxBouncesY * mMaxBouncesZ * 8;

	if(mReflectionsArray != NULL)
		free (mReflectionsArray);
	if(mGainsArray != NULL)
		free (mGainsArray);
	if(mAnglesArray != NULL)
		free (mAnglesArray);

	mReflectionsArray = (float *)calloc(mNumReflections, sizeof(float)); // Allocating twice, because we are also storing the gain. 
	mGainsArray = (float *)calloc(mNumReflections, sizeof(float));
	mAnglesArray = (float *)calloc(mNumReflections, sizeof(float));
	if(mReflectionsArray && mGainsArray && mAnglesArray)
		logMsg("Allocated memory for the reflections and gains. Total %d bytes", mNumReflections);

	logMsg("Reflection time = %d, bounces = %d, %d, %d", mDuration, mMaxBouncesX, mMaxBouncesY, mMaxBouncesZ);

}
	
void ShoeBox::calculateReflections() {

//	Called in here because memory needs to be allocated accordingly to the sizes and reflection time requested.
	findNumReflectionsFromTime();
		
	float imageX[mMaxBouncesX],imageY[mMaxBouncesY],imageZ[mMaxBouncesZ];
	float powOfXDelta, powOfMiusXDelta, powOfYDelta, powOfMiusYDelta, powOfZDelta, powOfMiusZDelta;
	float distanceFromSource, initalDistance, initialDelay, reflections;
	float reflectionAngleI, reflectionAngleII, reflectionAngleIII, reflectionAngleIV;
	int reflXpos,reflXneg,reflYpos,reflYneg,reflZpos,reflZneg;
	int i, j, k;
	
	initalDistance =  sqrt((mSource.x - mListenerPosition.x)*(mSource.x - mListenerPosition.x) + (mSource.y - mListenerPosition.y)*(mSource.y - mListenerPosition.y) + (mSource.z - mListenerPosition.z)*(mSource.z - mListenerPosition.z));
	initialDelay = initalDistance / SPEED_OF_SOUND;	

//	setting the first (real) room coordinates. It's done outside the loop to avoid a phantom double calculation.
	imageX[0] = mSource.x;
	imageY[0] = mSource.y;			
	imageZ[0] = mSource.z;

//	We are pre-calculating the COORDINATES for all the image (virtual) sources
//	as the real room is set above, we'll start with [1] (meaning room two.
	for(i = 1; i < mMaxBouncesX; i+= 2)
		imageX[i] = (mRoomSize.x * (i+ 1)) - mSource.x;	// Even rooms		

	for(i = 2; i < mMaxBouncesX; i+= 2)
		imageX[i] = (mRoomSize.x * i) + mSource.x; // Odd rooms
		
	for(i = 1; i < mMaxBouncesY; i+= 2)
		imageY[i] = (mRoomSize.y * (i+ 1)) - mSource.y;

	for(i = 2; i < mMaxBouncesY; i+= 2)
		imageY[i] = (mRoomSize.y * i) + mSource.y;			

	for(i = 1; i < mMaxBouncesZ; i+= 2)
		imageZ[i] = (mRoomSize.z * (i+ 1)) - mSource.z;

	for(i = 2; i < mMaxBouncesZ; i+= 2)
		imageZ[i] = (mRoomSize.z * i) + mSource.z;

		
//	Here is where we calculate the distance from the source to the listener.
//	we are doing four rooms at a time: even, odd, negativeEven, negativeOdd
//	this way we don't need to calculate the negative values, just reusing the already caluculated.
	for(i = 0; i < mMaxBouncesX; i++) {
		powOfXDelta = (imageX[i] - mListenerPosition.x)*(imageX[i] - mListenerPosition.x);
		reflXpos = (int)floor(imageX[i]/mRoomSize.x);
		powOfMiusXDelta = (imageX[i] + mListenerPosition.x)*(imageX[i] + mListenerPosition.x);
		reflXneg = abs((int)floor(-imageX[i]/mRoomSize.x));
		for(j = 0; j < mMaxBouncesY; j++) {
			powOfYDelta = (imageY[j] - mListenerPosition.y)*(imageY[j] - mListenerPosition.y);
			reflYpos = (int)floor(imageY[j]/mRoomSize.y);
			powOfMiusYDelta = (imageY[j] + mListenerPosition.y)*(imageY[j] + mListenerPosition.y);
			reflYneg = abs((int)floor(-imageY[j]/mRoomSize.y));
			reflectionAngleI = atan2((imageY[j] - mListenerPosition.y),(imageX[i] - mListenerPosition.x));
			reflectionAngleII = atan2((imageY[j] - mListenerPosition.y),(-imageX[i] - mListenerPosition.x));
			reflectionAngleIII = atan2((-imageY[j] - mListenerPosition.y),(-imageX[i] - mListenerPosition.x));
			reflectionAngleIV = atan2((-imageY[j] - mListenerPosition.y),(imageX[i] - mListenerPosition.x));
			for(k = 0; k < mMaxBouncesZ; k++) {
				powOfZDelta = (imageZ[k] - mListenerPosition.z)*(imageZ[k] - mListenerPosition.z);
				reflZpos = (int)floor(imageZ[k]/mRoomSize.z);
				powOfMiusZDelta = (imageZ[k] + mListenerPosition.z)*(imageZ[k] + mListenerPosition.z);
				reflZneg = abs((int)floor(-imageZ[k]/mRoomSize.z));
		
//				calculating x, y, z, positive
				reflections =  pow(mDampX,reflXpos) * pow(mDampY,reflYpos) * pow(mDampZ,reflZpos);
				distanceFromSource = sqrt(powOfXDelta + powOfYDelta + powOfZDelta);
				*mReflectionsArray++ = (distanceFromSource / SPEED_OF_SOUND) - initialDelay;
				*mGainsArray++ = reflections * initalDistance / distanceFromSource;
				*mAnglesArray++ = reflectionAngleI;
//				NSLog(@"Reflections %d", reflections );

//				calculating x negative, y, z positive
				reflections = pow(mDampX,reflXneg) * pow(mDampY,reflYpos) * pow(mDampZ,reflZpos);
				distanceFromSource = sqrt(powOfMiusXDelta + powOfYDelta + powOfZDelta);
				*mReflectionsArray++ = (distanceFromSource / SPEED_OF_SOUND) - initialDelay;
				*mGainsArray++ = reflections * initalDistance / distanceFromSource;
				*mAnglesArray++ = reflectionAngleII;
//				NSLog(@"Reflections %d", reflections );				

//				calculating x positive, y negative, z positive
				reflections = pow(mDampX,reflXpos) * pow(mDampY,reflYneg) * pow(mDampZ,reflZpos);
				distanceFromSource = sqrt(powOfXDelta + powOfMiusYDelta + powOfZDelta);
				*mReflectionsArray++ = (distanceFromSource / SPEED_OF_SOUND) - initialDelay;
				*mGainsArray++ = reflections * initalDistance / distanceFromSource;
				*mAnglesArray++ = reflectionAngleIV;
//				NSLog(@"Reflections %d", reflections );

//				calculating x, y negative, z positive
				reflections = pow(mDampX,reflXneg) * pow(mDampY,reflYneg) * pow(mDampZ,reflZpos);
				distanceFromSource = sqrt(powOfMiusXDelta + powOfMiusYDelta + powOfZDelta);
				*mReflectionsArray++ = (distanceFromSource / SPEED_OF_SOUND) - initialDelay;
				*mGainsArray++ = reflections * initalDistance / distanceFromSource;
				*mAnglesArray++ = reflectionAngleIII;				
//				NSLog(@"Reflections %d", reflections );

//				calculating x, y positive, z negative
				reflections = pow(mDampX,reflXpos) * pow(mDampY,reflYpos) * pow(mDampZ,reflZneg);
				distanceFromSource = sqrt(powOfXDelta + powOfYDelta + powOfMiusZDelta);
				*mReflectionsArray++ = (distanceFromSource / SPEED_OF_SOUND) - initialDelay;
				*mGainsArray++ = reflections * initalDistance / distanceFromSource;
				*mAnglesArray++ = reflectionAngleI;				
//				NSLog(@"Reflections %d", reflections );

//				calculating x negative, y positive, z negative
				reflections = pow(mDampX,reflXneg) * pow(mDampY,reflYpos) * pow(mDampZ,reflZneg);
				distanceFromSource = sqrt(powOfMiusXDelta + powOfYDelta + powOfMiusZDelta);
				*mReflectionsArray++ = (distanceFromSource / SPEED_OF_SOUND) - initialDelay;
				*mGainsArray++ = reflections * initalDistance / distanceFromSource;
				*mAnglesArray++ = reflectionAngleII;				
//				NSLog(@"Reflections %d", reflections );

//				calculating x positive, y, z negative
				reflections = pow(mDampX,reflXpos) * pow(mDampY,reflYneg) * pow(mDampZ,reflZneg);
				distanceFromSource = sqrt(powOfXDelta + powOfMiusYDelta + powOfMiusZDelta);
				*mReflectionsArray++ = (distanceFromSource / SPEED_OF_SOUND) - initialDelay;
				*mGainsArray++ = reflections * initalDistance / distanceFromSource;
				*mAnglesArray++ = reflectionAngleIV;				
//				NSLog(@"Reflections %d", reflections );

//				calculating x, y, z negative
				reflections = pow(mDampX,reflXneg) * pow(mDampY,reflYneg) * pow(mDampZ,reflZneg);
				distanceFromSource = sqrt(powOfMiusXDelta + powOfMiusYDelta + powOfMiusZDelta);
				*mReflectionsArray++ = (distanceFromSource / SPEED_OF_SOUND) - initialDelay;
				*mGainsArray++ = reflections * initalDistance / distanceFromSource;
				*mAnglesArray++ = reflectionAngleIII;				
//				NSLog(@"Reflections %d", reflections );
				
//				NSLog(@"%g, %g, Number of Reflections:%i", timeFromSource, gain, reflections);
				
			}
		}
	}

//	We need to reset the pointers to the original address.
	mReflectionsArray -= mNumReflections;
	mGainsArray -= mNumReflections;
	mAnglesArray -= mNumReflections;
	
	shellSort();

	for(i = 0; i < mNumReflections; i++)
		logMsg("Reflection #%d, after %g s & gain of %g angle is: %g", i, mReflectionsArray[i], mGainsArray[i], (mAnglesArray[i] * 180)/3.14159 );

}

void ShoeBox::getReflections(float *ref, float *gains, float *angles, int num) {
//	write if statement that returns a non zero value if there are less reflections than requested.
	memcpy(ref, mReflectionsArray, num * sizeof(float));
	memcpy(gains, mGainsArray, num * sizeof(float));
	memcpy(angles, mAnglesArray, num * sizeof(float));
}

