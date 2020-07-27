//
//  Envelope.cpp -- Implementation of LineSegment and Envelope classes
//	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
//

#include "Envelope.h"
#include <stdlib.h>
#include <iostream>
#include <math.h>

//#define CSL_DEBUG

using namespace csl;

/// LineSegment class

/// Constructors

LineSegment::LineSegment() : 
			UnitGenerator(), mMode(kLine), mCurrentValue(0), mCurrentFrame(0) { }

LineSegment::LineSegment(float d, float s, float e, LineMode mode) : 
			UnitGenerator(), 
			mStart(s), mEnd(e), mDuration(d), mMode(mode), mCurrentValue(s), mCurrentFrame(0) {
	if (mode == kExpon) {
		if (mStart == 0) {
			mStart = 0.00001f; // Arbitrary small value.
//			logMsg("Start value can't be '0' for exponential segments. Corrected to be 0.00001.");
		}
		if (mEnd == 0) {
			mEnd = 0.00001f;
//			logMsg("End value can't be '0' for exponential segments. Corrected to be 0.00001.");
		}	
	}
	mCurrentValue = mEnd;
	mCurrentFrame = mEnd;
}

/// Reset method

void LineSegment::reset(){
	mCurrentValue = mStart;
	mCurrentFrame = 0;
}

void LineSegment::dump() {
	logMsg("\tLineSegment: %g to %g in %g (%s)", mStart, mEnd, mDuration, 
			(mMode == kLine ? "linear" : "exponential"));
}

void LineSegment::nextBuffer(Buffer &outputBuffer, unsigned outBufNum) throw (CException) {
	nextBuffer(outputBuffer, outBufNum, 0, 0);
}

void LineSegment::nextBuffer(Buffer &outputBuffer, unsigned outBufNum, Port * scalePort, Port * offsetPort) 
					throw (CException) {
	unsigned i;
	float rate = (float) mFrameRate;
	unsigned numFrames =  outputBuffer.mNumFrames;
	SampleBuffer outPtr = outputBuffer.buffer(outBufNum);
	float scaleValue = 1, offsetValue = 0;			// used for dynamic input;

	/// calculate the increment for the linear interpolation, the number of frames to calculate
	/// and the number of frames that are constant (at the end of the interpolations)
	unsigned numFramesToCalc = (int)(mDuration * rate) - mCurrentFrame;
	unsigned constantFrames;

	/// This test to see if we're at the end works for now, but we should use a bool to check if we are done
	/// in case mCurrentFrame loops
	if (numFramesToCalc <= 0) {
		numFramesToCalc = 0;
		constantFrames = numFrames;
		mCurrentValue = mEnd;
	} else {
		if (numFramesToCalc > numFrames)
			numFramesToCalc = numFrames;
		constantFrames = numFrames - numFramesToCalc;
	}
	float currentValue = mCurrentValue;
	float increment;
#ifdef CSL_DEBUG
//	logMsg("LineSegment nextBuffer");
#endif
	switch(mMode) {
		case kLine:
			increment = (mEnd - mStart) / (mDuration * rate);
			for (i = 0; i < numFramesToCalc; i++) {			// Generate the interpolated frames
				CHECK_UPDATE_SCALABLE_CONTROLS;				// update the dynamic scale/offset
				*outPtr++ = (currentValue * scaleValue) + offsetValue;
				currentValue += increment;
			}
			break;
		case kExpon:
			increment = pow(mEnd / mStart, 1/(mDuration * rate));
			for (i = 0; i < numFramesToCalc; i++) {			// Generate the interpolated frames
				CHECK_UPDATE_SCALABLE_CONTROLS;				// update the dynamic scale/offset
				*outPtr++ = (currentValue * scaleValue) + offsetValue;
				currentValue *= increment;
			}
			break;
		default:
			break;
	}
	for (i = 0; i < constantFrames; i++) {				///< Generate the constant frames	
		CHECK_UPDATE_SCALABLE_CONTROLS;		// update the dynamic scale/offset
		*outPtr++ = (mEnd * scaleValue) + offsetValue;
	}
	if (mCurrentValue != mEnd) {
		mCurrentFrame += numFrames;			///< if we're not already at the end increment my internal counters
		mCurrentValue = currentValue;
	}
//	fprintf(stderr, " _%5.3f_ ", (currentValue * scaleValue) + offsetValue);
}

/// Envelope class

/// Lots of useful constructors

Envelope::Envelope(LineMode mode, float t, float x1, float y1, float x2, float y2, float x3, float y3,
						float x4, float y4, float x5, float y5, float x6, float y6) 
				: UnitGenerator(), Scalable(1, 0), mDuration(t) {
	mSegmentMap[x1] = new LineSegment(0, 0, y1, mode);
	if (x2 != 0) mSegmentMap[x2] = new LineSegment(0, 0, y2, mode);	
	if (x3 != 0) mSegmentMap[x3] = new LineSegment(0, 0, y3, mode);	
	if (x4 != 0) mSegmentMap[x4] = new LineSegment(0, 0, y4, mode);	
	if (x5 != 0) mSegmentMap[x5] = new LineSegment(0, 0, y5, mode);	
	if (x6 != 0) mSegmentMap[x6] = new LineSegment(0, 0, y6, mode);	
	this->createSegments();
}

Envelope::Envelope(LineMode mode, float t, unsigned int size, float x[], float y[]) 
		: UnitGenerator(), Scalable(1, 0), mDuration(t) {
	for (unsigned int i = 0; i < size; i++)
		mSegmentMap[x[i]] = new LineSegment(0, 0, y[i], mode);
	this->createSegments();
}

Envelope::Envelope(float t, float x1, float y1, float x2, float y2, float x3, float y3,
						float x4, float y4, float x5, float y5, float x6, float y6) 
				: UnitGenerator(), Scalable(1.0f, 0.0f), mDuration(t) {
	mSegmentMap[x1] = new LineSegment(0, 0, y1);
	if (x2 != 0) mSegmentMap[x2] = new LineSegment(0, 0, y2);	
	if (x3 != 0) mSegmentMap[x3] = new LineSegment(0, 0, y3);	
	if (x4 != 0) mSegmentMap[x4] = new LineSegment(0, 0, y4);	
	if (x5 != 0) mSegmentMap[x5] = new LineSegment(0, 0, y5);	
	if (x6 != 0) mSegmentMap[x6] = new LineSegment(0, 0, y6);	
	this->createSegments();
}

Envelope::Envelope(float t, unsigned int size, float x[], float y[]) 
		: UnitGenerator(), Scalable(1, 0), mDuration(t) {
	for (unsigned int i = 0; i < size; i++)
		mSegmentMap[x[i]] = new LineSegment(0, 0, y[i]);
	this->createSegments();
}

Envelope::~Envelope() {
	for (unsigned i = 0; i < mSegmentMap.size(); ++i)
		delete mSegments[i];
	if (mValues)
		delete[] mValues;
	if (mSegments)
		free(mSegments);
}

void Envelope::createSegments() {
	mValues = new float[mSegmentMap.size()];
	mSegments = (LineSegment ** ) malloc(mSegmentMap.size() * (sizeof(char *)));
	calculateSegments();
}

void Envelope::calculateSegments() {
	unsigned i = 0;
	mSize = mSegmentMap.size();
	Breakpoints::iterator idx;
						// The very first segment is not really used. 
						// The number of segments is always 1 less than the number of points.
	for (idx = mSegmentMap.begin(); idx != mSegmentMap.end(); ++idx) {
		mValues[i] = idx->first;
		mSegments[i] = idx->second;
		i++;
	}
	mSegments[0]->setStart(mSegments[0]->end());
	mSegments[0]->setDuration(float(mValues[0]));
	for (i = 1; i < mSize; ++i) {
		mSegments[i]->setStart(mSegments[i - 1]->end());
		mSegments[i]->setDuration(float(mValues[i] - mValues[i - 1]));
	}
	mCurrentMark = mDuration;			// set to end
}

/////////////////////// Operations ///////////////////////////////////
 	
void Envelope::addBreakpoint(float startTime, float tvalue) {
	mSegmentMap[startTime] =  new LineSegment(0, 0, tvalue);
	LineSegment **tempLinsegPtr = mSegments;
	float *tempPtr = mValues;

	this->createSegments();
	if (startTime > mDuration)
		mDuration = startTime;
	if (tempPtr)
		delete[] tempPtr;
	if (tempLinsegPtr)
		free(tempLinsegPtr);
}

void Envelope::setMode(LineMode mode) {
	for (unsigned i = 0; i < mSize; i++)
		mSegments[i]->setMode(mode);
}

bool Envelope::isActive() {
//	float diff = mCurrentMark - mDuration;
//	if ((diff > 0.0) && (diff < 0.1))
//		fprintf(stderr, "   %5.4f = %s\n", diff, (mCurrentMark < mDuration) ? "true" : "false");
	return (mCurrentMark < mDuration);
}

/// Scaling operations

void Envelope::setDuration(float d) {
	//if ((mDuration == 0.0f) || (! isnormal(mDuration))) {
	if (mDuration == 0.0f) {
		logMsg(kLogError, "Impossible env dur");
		throw RunTimeError("Impossible env dur");
	}
	this->scaleTimes(d / mDuration);
}

void Envelope::scaleTimes(float s) {
	if (s != 1) {
		for (unsigned i = 0; i < mSize; ++i) {
			mSegmentMap.erase(mValues[i]);			// Erase old values.
			mValues[i] *= s;						// update the stored values to the new scaled ones.
		}
		for (unsigned i = 0; i < mSize; ++i) {
			mSegmentMap[mValues[i]] = mSegments[i]; // Add the new values to the map
			mSegments[i]->reset();					// Reset all segments.
		}		
	}
	mDuration *= s;
	calculateSegments();
}

// scale values so that the env's max is s

void Envelope::scaleValues(float s) {
	float maxV = 0.0f;
	for (unsigned i = 0; i < mSize; ++i) {
		LineSegment * seg = mSegments[i];
		if (seg->start() > maxV) maxV = seg->start();
		if (seg->end() > maxV) maxV = seg->end();
	}
	float scale = s / maxV;
	for (unsigned i = 0; i < mSize; ++i) {
		LineSegment * seg = mSegments[i];
		seg->setStart(seg->start() * scale);
		seg->setEnd(seg->end() * scale);
	}
}

/// Pretty-printer

void Envelope::dump() {
	logMsg("Envelope: dur = %5.3f  curr = %5.3f", mDuration, mCurrentMark);
	fprintf(stderr, "\tat 0 seconds  \t\t");
	mSegments[0]->dump();
	for (unsigned i = 1; i < mSize; i++) {
		fprintf(stderr, "\tat %5.3f seconds  \t\t", mValues[i-1]);
		mSegments[i]->dump();
	}
//	Scalable::dump();
}

/// Reset time to 0.0 to restart envelope

void Envelope::reset() {
	mCurrentMark = 0.0f;
	for (unsigned i = 0; i < mSize; ++i)
		mSegments[i]->reset();
}

// trigger means reset

void Envelope::trigger() {
	reset();
}

//
// Next buffer calculation -- Here's where the work gets done
//

void Envelope::nextBuffer(Buffer &outputBuffer, unsigned outBufNum) throw (CException) {
	SampleBuffer outPtr = outputBuffer.buffer(outBufNum);
	unsigned numFrames = outputBuffer.mNumFrames;
	unsigned  i, tempFrames;
	float rate = mFrameRate;
	float endMark, x;
	DECLARE_SCALABLE_CONTROLS;			// declare the scale/offset buffers and values
#ifdef CSL_DEBUG
//	logMsg("Envelope nextBuffer");
#endif
	LOAD_SCALABLE_CONTROLS;				// load the scaleC and offsetC from the constant or dynamic value
										// if the envelope is finished, just write the final value into the buffer
	if (mCurrentMark >= mDuration) {
		x =  mSegments[mSize - 1]->end(); //  mPoints[mSize - 1]->y;
		for (i = 0; i < numFrames; i++) {
			*outPtr++ = (x * scaleValue) + offsetValue;
			UPDATE_SCALABLE_CONTROLS;	// update the dynamic scale/offset
		}
		return;
	}									// find out what segment we're in and let the segment fill the buffer
//	if (mCurrentMark < mValues[0]) {
//	
//	}
	for (i = 0; i < mSize; ++i) {
		x = mValues[i];
		if (mCurrentMark < x) {		
			endMark = mCurrentMark + ((float)numFrames / rate);
			if (endMark < x) {				// if the current next_buffer request all falls into one line segment
				mSegments[i]->nextBuffer(outputBuffer, outBufNum, scalePort, offsetPort);
				mCurrentMark +=  ((float) (numFrames + 1) / rate);
			//	fprintf(stderr, "E: %5.3f", *outPtr);
				return;
			} else {						// if the current next_buffer request spans line segments, go
											// to the end of this line segment and call nextBuffer again
#ifdef CSL_DEBUG
				logMsg("\t\tEnvelope crossing: current mark: %g point: %g @ %g ls: %g to %g in %g", 
					mCurrentMark, mValues[i], mSegments[i]->end(), 
					mSegments[i]->start(), mSegments[i]->end(), mSegments[i]->duration());
#endif		
				tempFrames = (unsigned) ((mValues[i] - mCurrentMark) * rate);
				outputBuffer.mNumFrames = tempFrames;
										// get the first segment
				mSegments[i]->nextBuffer(outputBuffer, outBufNum, scalePort, offsetPort);

										// update pointers
				mCurrentMark += ((float) (numFrames + 1) / rate);
				outputBuffer.setBuffer(outBufNum, outputBuffer.buffer(outBufNum) + tempFrames);
				outputBuffer.mNumFrames = numFrames - tempFrames;
										// call myself recursively
				this->nextBuffer(outputBuffer, outBufNum);
										// clean up
				mCurrentMark += ((float) (numFrames + 1) / rate);
				outputBuffer.setBuffer(outBufNum, outPtr);
				outputBuffer.mNumFrames = numFrames;
				if (i == (mSize - 1)) {
#ifdef CSL_DEBUG
					logMsg("Envelope finished");
#endif		
					mCurrentMark = mDuration;
				}
				return;
			}
		}		
	}
					// OK to play past end
//	logMsg(kLogError, "Envelope nextBuffer: it shouldn't get here %.0f", rate);
//	this->dump();
//	return;			// shouldn't ever get here (THROW AN EXCEPTION IF CONTROL GETS HERE!)
}

/// ~~~~~~~ Atack / Decay / Sustain / Release Envelope ~~~~~~~~~~~

/// Minimal version - ADSR . . . call the Super constructor, passing the appropriate points.

csl::ADSR::ADSR(LineMode mode, float t, float a, float d, float s, float r)
		: Envelope(mode, t, 0.0, 0.0, a, 1.0, a + d, s, t - r, s, t, 0.0) { };

/// with initial delay - IADSR

csl::ADSR::ADSR(LineMode mode, float t, float i, float a, float d, float s, float r)
		: Envelope(mode, t, i, 0.0, i + a, 1.0, i + a + d, s, t - r, s, t, 0.0) { };

/// Minimal version - ADSR . . . call the Super constructor, passing the appropriate points.

csl::ADSR::ADSR(float t, float a, float d, float s, float r)
		: Envelope(t, 0.0, 0.0, a, 1.0, a + d, s, t - r, s, t, 0.0) { };

/// with initial delay - IADSR

csl::ADSR::ADSR(float t, float i, float a, float d, float s, float r)
		: Envelope(t, i, 0.0, i + a, 1.0, i + a + d, s, t - r, s, t, 0.0) { };

/// Operations --

void csl::ADSR::setDuration(float d) {
//	if (d <= mValues[3]) 
		Envelope::setDuration(d);
//	else {
//	this->dump();
//		float oldD = mDuration - mValues[3];
//		mDuration = d;	
//		mSegmentMap[mDuration - oldD] = mSegmentMap[mValues[3]]; 
//		mSegmentMap.erase(mValues[3]);
//		this->calculateSegments();
//	this->dump();
//	}
}

void csl::ADSR::setDelay(float del) {
	mSegmentMap[del] = mSegmentMap[mValues[0]]; // Add a new "initial delay key" and then remove the old one.
	mSegmentMap[del + mSegments[1]->duration()] = mSegmentMap[mValues[1]]; // update the value of the next 
	if (mValues[0] != del) {		// If value is different, then erase the old value.
		mSegmentMap.erase(mValues[0]);
		mSegmentMap.erase(mValues[1]);
	}
	this->calculateSegments(); 
}

void csl::ADSR::setAttack(float attack) {
	mSegmentMap[mValues[0] + attack] = mSegmentMap[mValues[1]]; // Add a new "attack key" and then remove the old one.
	mSegmentMap[mValues[0] + attack + mSegments[2]->duration()] = mSegmentMap[mValues[2]]; // update the value of the next 
	if (mValues[1] != mValues[0] + attack) {		// If value is different, then erase the old value.
		mSegmentMap.erase(mValues[1]);
		mSegmentMap.erase(mValues[2]);
	}
	this->calculateSegments(); 
}

void csl::ADSR::setDecay(float decay) {
	mSegmentMap[mValues[1] + decay] = mSegmentMap[mValues[2]]; 
	if (mValues[2] != mValues[1] + decay) 
		mSegmentMap.erase(mValues[2]);
	this->calculateSegments(); 
}

void csl::ADSR::setSustain(float sustain) {
	mSegments[2]->setEnd(sustain);
	mSegments[3]->setEnd(sustain);
	this->calculateSegments(); 
}

void csl::ADSR::setRelease(float trelease) {
	mSegmentMap[mDuration - trelease] = mSegmentMap[mValues[3]]; 
	if (mValues[3] != mDuration - trelease) 
		mSegmentMap.erase(mValues[3]);
	this->calculateSegments(); 
}

// Trigger the release segment

void csl::ADSR::release(void) {
	mCurrentMark = mValues[3];
}

/// ~~~~~~~ Atack / Release Envelope ~~~~~~~~~~~

/// Minimal version - AR

AR::AR(LineMode mode, float t, float a, float r)
		: Envelope(mode, t, 0.0, 0.0, a, 1.0, t - r, 1.0, t, 0.0) { };

/// with initial delay - I-AR

AR::AR(LineMode mode, float t, float i, float a, float r) 
		: Envelope(mode, t, i, 0.0, i + a, 1.0, t - r, 1.0, t, 0.0) { };

/// Minimal version - AR

AR::AR(float t, float a, float r)  
		: Envelope(t, 0.0, 0.0, a, 1.0, t - r, 1.0, t, 0.0) { };

/// with initial delay - I-AR

AR::AR(float t, float i, float a, float r) 
		: Envelope(t, i, 0.0, i + a, 1.0, t - r, 1.0, t, 0.0) { };

/// Operations

void AR::setDuration(float d) {
//	if (d <= mValues[2]) 
		Envelope::setDuration(d);
//	else {
//		float oldD = mDuration - mValues[2];
//		mDuration = d;	
//		mSegmentMap[mDuration - oldD] = mSegmentMap[mValues[2]]; 
//		mSegmentMap.erase(mValues[2]);
//		this->calculateSegments();
//	}
}

void AR::setDelay(float del) {
	mSegmentMap[del] = mSegmentMap[mValues[0]]; 
	if (mValues[0] != del)  // If value is different, then erase the old value.
		mSegmentMap.erase(mValues[0]);
	this->calculateSegments(); 
}

void AR::setAttack(float attack) {
	mSegmentMap[mValues[0] + attack] = mSegmentMap[mValues[1]]; 
	if (mValues[1] != mValues[0] + attack)  // If value is different, then erase the old value.
		mSegmentMap.erase(mValues[1]);
	this->calculateSegments(); 
}

void AR::setRelease(float trelease) {
	mSegmentMap[mDuration - trelease] = mSegmentMap[mValues[2]];
	if (mValues[2] != mDuration - trelease) 
		mSegmentMap.erase(mValues[2]);
	this->calculateSegments(); 
}

void AR::setAll(float d, float a, float r) {
	mDuration = d;
	mSegmentMap[mValues[0] + a] = mSegmentMap[mValues[1]]; 
	mSegmentMap[mDuration - r] = mSegmentMap[mValues[2]];
	mSegmentMap[mDuration] = mSegmentMap[mValues[3]];
	if (mValues[2] != a) mSegmentMap.erase(mValues[1]);
	if (mValues[3] != r) mSegmentMap.erase(mValues[2]);
	if (mValues[4] != d) mSegmentMap.erase(mValues[3]);	
	
	this->calculateSegments(); 
}

/// trigger the release segment

void AR::release(void) {
	mCurrentMark = mValues[2];
}


/// ~~~~~ Triangle class ~~~~~

// simple constructor

Triangle::Triangle(LineMode mode, float t, float a) 
		: Envelope(mode, t, 0.0, 0.0, t/2, a, t, 0.0) { }

// versions with initial delay segments

Triangle::Triangle(LineMode mode, float t, float i, float a) 
		: Envelope(mode, t, i, 0.0, (t-i)/2, a, t, 0.0) { }

// simple constructor

Triangle::Triangle(float t, float a) 
		: Envelope(t, 0.0, 0.0, t/2, a, t, 0.0) { }

// versions with initial delay segments

Triangle::Triangle(float t, float i, float a) 
		: Envelope(t, i, 0.0, (t-i)/2, a, t, 0.0) { }


/// ~~~~~~~~ RandEnvelope envelope class ~~~~~~~

// Constructors

RandEnvelope::RandEnvelope(float frequency, float amplitude, float offset, float step)
				: Envelope(), mFrequency(frequency), mAmplitude(amplitude), 
				  mStep(step), mOffset(offset), mWalk(false) { 
	mSize = 0;
	if (mStep == 0)
		mStep = mAmplitude / 2.0;
	initSegment();
	mValues = 0;
	mSegments = 0;
}

// Prepare by putting an ending value in the line segment

void RandEnvelope::initSegment() { 
	mLastVal = fRandZ() * 2.0 - 1.0;
	mSegment.setEnd(mLastVal * mAmplitude + mOffset);
	nextSegment();
}

// To choose the next segment

void RandEnvelope::nextSegment() { 
	float next = 0.0f, randVal;
	mSegment.setStart(mSegment.end());
	if ( ! mWalk) {				// pick a new random and constrain it to the proper range
		randVal = (fRandZ() * 2.0 - 1.0) * (mStep / mAmplitude);
		next = mLastVal + randVal;
		if (next > 1.0)
			next = mLastVal - randVal;
		if (next < -1.0)
			next = mLastVal + randVal;
		mLastVal = next;
		next = next * mAmplitude + mOffset;
//		logMsg("RE %5.3f\n", next);
	} else {
					// what to do here?
	}
	mSegment.setEnd(next);
	mSegment.setDuration((float) (1.0 / mFrequency));
	mSegment.reset();
	mCurrentIndex = 0;
	mSegmentLength = (unsigned) (mSegment.duration() * mFrameRate);
}

// NextBuffer for rand

void RandEnvelope::nextBuffer(Buffer &outputBuffer, unsigned outBufNum) throw (CException) {
	SampleBuffer outPtr = outputBuffer.buffer(outBufNum);
	unsigned numFrames = outputBuffer.mNumFrames;
	unsigned endMark = mCurrentIndex + numFrames;
	DECLARE_SCALABLE_CONTROLS;	// declare the scale/offset buffers and values

				// if we can answer the request with the current line segment...
	if (endMark < mSegmentLength) {
		mSegment.nextBuffer(outputBuffer, outBufNum, scalePort, offsetPort);
		mCurrentIndex += numFrames;
		return;
	}		
					// else, do some, then create update the line segment
	unsigned tempFrames = mSegmentLength - mCurrentIndex;
	outputBuffer.mNumFrames = tempFrames;
	mSegment.nextBuffer(outputBuffer, outBufNum, scalePort, offsetPort);
					// update pointers
	nextSegment();
	outputBuffer.setBuffer(outBufNum, outputBuffer.buffer(outBufNum) + tempFrames);
	outputBuffer.mNumFrames = numFrames - tempFrames;
					// call the next line segment (must be long enough to fill the buffer)
	mSegment.nextBuffer(outputBuffer, outBufNum, scalePort, offsetPort);
					// clean up
	mCurrentIndex = numFrames - tempFrames;
	outputBuffer.setBuffer(outBufNum, outPtr);
	outputBuffer.mNumFrames = numFrames;
	return;
}
