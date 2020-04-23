//
//  ShoeBox.h
//  ShoeBox
//
//  Created by Jorge Castellanos & Will Wolcott on 4/29/05.
//  Copyright 2005 Jorge Castellanos & Will Wolcott. All rights reserved.
//


#ifndef SHOEBOX_H
#define SHOEBOX_H

#include "CPoint.h"

namespace csl {

enum {
	kMarble = 0,		
	kConcrete,
	kBrick,	
	kGlass,
	kWood
};

//	This class calculates the reflection time and amplitude of a sound 
//	assuming the space is rectangulsr, in other words, a shoe box.

class ShoeBox {
public:
	// Defaults to a listener positioned at the center of the room.
	ShoeBox();
	~ShoeBox();
	
	void setSourceLocation(float x, float y, float z);
	void setListenerLocation(float x, float y, float z);
	void setRoomSize(float x, float y, float z);
	CPoint roomSize() { return mRoomSize; };
	CPoint source() { return mSource; };
	CPoint listener() { return mListenerPosition; };
	void setWallMaterialsDamp(int dampX, int dampY, int dampZ);
	void setDuration(int ms);
	void getReflections(float *ref, float *gains, float *angles, int num);
	void calculateReflections();
	void findNumReflectionsFromTime();	
	
private:
	
	CPoint mRoomSize, mSource, mListenerPosition;
	float mDampX, mDampY,mDampZ;
	float *mReflectionsArray, *mGainsArray, *mAnglesArray;
	int mDuration, mNumReflections, mMaxBouncesX, mMaxBouncesY, mMaxBouncesZ;	// The miliseconds that will be calculated reflections.

	void shellSort();
	bool checkPositionValues();

};


}

#endif
