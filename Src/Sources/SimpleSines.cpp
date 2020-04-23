//
//  SimpleSines.cpp -- implementation of the tutorial example oscillator classes
//	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
//

#include "SimpleSines.h"			// include the class header
#include <math.h>					// this is for sin()

using namespace csl;				// use the CSL namespace

//
// SimpleSine -- The simplest CSL sine oscillator class
//

// Constructors call UnitGenerator constructor and initialize the member variables

SimpleSine::SimpleSine() : UnitGenerator(), mFrequency(220.0), mPhase(0.0) { }	// default freq = 220 Hz, phase = 0

SimpleSine::SimpleSine(float frequency) : UnitGenerator(), mFrequency(frequency), mPhase(0.0) { }

SimpleSine::SimpleSine(float frequency, float phase) : UnitGenerator(), mFrequency(frequency), mPhase(phase) { }

SimpleSine::~SimpleSine() { }			///< Destructor is a no-op

//
// The SimpleSine monoNextBuffer method plays one channel into the given buffer at a fixed frequency
//

void SimpleSine::nextBuffer(Buffer & outputBuffer, unsigned outBufNum) throw (CException) {

	sample * buffer = outputBuffer.buffer(outBufNum);			// get pointer to the selected output channel

	float phaseIncrement = mFrequency * CSL_TWOPI / mFrameRate;	// calculate the phase increment

#ifdef CSL_DEBUG
	logMsg("SimpleSine nextBuffer");								// for verbose debugging (define CSL_DEBUG in CSL_Core.h)
#endif

	for (unsigned i = 0; i < outputBuffer.mNumFrames; i++) {		// this is the sample computation loop
		*buffer++ = sin(mPhase);									// compute and store sine value
		mPhase += phaseIncrement;									// increment the phase by the phase increment
	}	
	while (mPhase >= CSL_TWOPI)										// reduce phase to < 2pi radians when done
		mPhase -= CSL_TWOPI;
}																	// that's all there is to it!


//
// SineAsPhased -- A sine oscillator that uses the Phased mix-in class
//
										/// Constructors call UnitGenerator and Phased constructors
SineAsPhased::SineAsPhased() : UnitGenerator(), Phased(220.0) { }

SineAsPhased::SineAsPhased(float frequency) : UnitGenerator(), Phased(frequency) { }

SineAsPhased::SineAsPhased(float frequency, float phase) : UnitGenerator(), Phased(frequency, phase) { }

SineAsPhased::~SineAsPhased() { }		///< Destructor is a no-op

// This monoNextBuffer method looks the same as above, but handles dynamic frequency using
// the input port map (i.e., the mInputs[CSL_FREQUENCY] expression, which returns a Port object pointer)

#ifdef NOT_THIS_WAY		// This is the really verbose way; see below for how to use the macros to make this easier

void SineAsPhased::nextBuffer(Buffer & outputBuffer, unsigned outBufNum) throw (CException) {
	sample * buffer = outputBuffer.buffer(outBufNum);		// get pointer to the selected output channel
	float rateRecip = CSL_TWOPI / mFrameRate;					// Calculate the phase increment multiplier
	
	Port * freqPort = mInputs[CSL_FREQUENCY];					// get the frequency control port from the map
	UnitGenerator * freqUG = freqPort->mUGen;					// get its unit generator
	bool freqDyn = (freqUG != NULL);							// if it's NULL, we have a fixed freq.
	sample * freq = NULL;										// pointer to freq buffer (if used)
	float freqC;												// current frequency value

	if (freqDyn) {												// if we have a dynamic freq.
		this->pullInput(freqPort, outputBuffer);				// pull its buffer (this calls its nextBuffer method)
		freq = freqPort->mBuffer->buffer(0);				// get the pointer to its buffer
		freqC = *freq++;										// grab the first value
	} else														// otherwise
		freqC = freqPort->mValue;								// get the port's constant value

	for (unsigned i = 0; i < outputBuffer.mNumFrames; i++) {		// the sample loop
		*buffer++ = sin(mPhase);								// compute and store sine value
		mPhase += (freqC * rateRecip);							// increment phase by possibly dynamic frequency
		if (freqDyn)											// if the freq. is dynamic
			freqC = *freq++;									// update it from the control input's buffer pointer
	}	
	while (mPhase >= CSL_TWOPI)									// wrap around after 2pi radians
		mPhase -= CSL_TWOPI;
}

#endif

// This is the right way to do it (the terse version that uses these macros defined for Phased in CSL_Core.h).
//	The added calls above are handled by #defines in the CSL_Core.h header file, so you can say things like
//			DECLARE_PHASED_CONTROLS;
//			LOAD_PHASED_CONTROLS;
//			UPDATE_PHASED_CONTROLS;

void SineAsPhased::nextBuffer(Buffer & outputBuffer, unsigned outBufNum) throw (CException) {
	sample * buffer = outputBuffer.buffer(outBufNum);		// get pointer to the selected output channel
	unsigned numFrames = outputBuffer.mNumFrames;				// the number of frames to fill
	float rateRecip = CSL_TWOPI / mFrameRate;					// compute the phase increment scale
	
	DECLARE_PHASED_CONTROLS;					// declare the frequency buffer and value as above
#ifdef CSL_DEBUG
	logMsg("SineAsPhased nextBuffer");			// for verbose debugging
#endif	
	LOAD_PHASED_CONTROLS;						// load the freqC from the constant or dynamic value
	
	for (unsigned i = 0; i < numFrames; i++) {	// sample loop
		*buffer++ = sin(mPhase);				// compute and store sine value
		mPhase += (freqValue * rateRecip);		// increment phase
		UPDATE_PHASED_CONTROLS;					// update the dynamic frequency
	}	
	while (mPhase > CSL_TWOPI)					// wraparound after 2pi radians
		mPhase -= CSL_TWOPI;
}

// Pretty-print the receiver for debugging

void SineAsPhased::dump() {
	logMsg("a SineAsPhased");		// print a message
	Phased::dump();				// this dumps the input port map
}

///
/// SineAsScaled -- A sine oscillator with scale and offset
///
										/// Constructors call UnitGenerator, Phased, and Scalable constructors
SineAsScaled::SineAsScaled() : UnitGenerator(), Phased(220.0, 0.0), Scalable(1.0, 0.0) { }

SineAsScaled::SineAsScaled(float frequency) : UnitGenerator(), Phased(frequency, 0.0), Scalable(1.0, 0.0) { }

SineAsScaled::SineAsScaled(float frequency, float phase) : UnitGenerator(), Phased(frequency, phase), Scalable(1.0, 0.0) { }

SineAsScaled::SineAsScaled(float frequency, float phase, float ampl, float offset) : UnitGenerator(), Phased(frequency, phase), Scalable(ampl, offset) { }

SineAsScaled::~SineAsScaled() { }		///< Destructor is a no-op
	
// SineAsScaled monoNextBuffer method uses the dynamic frequency, scale, and offset

#ifdef NOT_THIS_WAY		// This is the really verbose way; see below for how to use the macros to make this easier

void SineAsScaled::nextBuffer(Buffer & outBuf, unsigned outBufNum) throw (CException) {
	sample * buffer = outputBuffer.buffer(outBufNum);		// get pointer to the selected output channel
	float rateRecip = CSL_TWOPI / mFrameRate;					// Calculate the phase increment multiplier

	Port * freqPort = mInputs[CSL_FREQUENCY];					// get the frequency control port from the map
	Port * scalePort = mInputs[CSL_SCALE];						// scale control port
	Port * offsetPort = mInputs[CSL_OFFSET];					// offset control port

	UnitGenerator * freqUG = freqPort->mUGen;					// get its unit generator
	UnitGenerator * scaleUG = scalePort->mUGen;					// scale unit generator
	UnitGenerator * offsetUG = offsetPort->mUGen;				// offset UGen

	bool freqDyn = (freqUG != NULL);							// if it's NULL, we have a fixed freq.
	bool scaleDyn = (scaleUG != NULL);							
	bool offsetDyn = (offsetUG != NULL);					

	sample * freq = NULL;										// pointer to freq buffer (if used)
	sample * scale = 0;											// scale buffer pointer
	sample * offset = 0;										// offset buffer pointer
	float freqC, scaleC, offsetC								// constant values

	if (freqDyn) {												// if we have a dynamic freq.
		this->pullInput(freqPort, outputBuffer);				// pull its buffer (this calls its nextBuffer method)
		freq = freqPort->mBuffer->buffer(0);				// get the pointer to its buffer
		freqC = *freq++;										// grab the first value
	} else														// otherwise
		freqC = freqPort->mValue;								// get the port's constant value

	if (scaleDyn) {												// if we have a dynamic scale
		this->pullInput(scalePort, outputBuffer);
		scale = scalePort->mBuffer->buffer(0);
		scaleC = *scale++;	
	} else
		scaleC = scalePort->mValue;

	if (offsetDyn) {											// if we have a dynamic offset							
		this->pullInput(offsetPort, outputBuffer);
		offset = offsetPort->mBuffer->buffer(0);
		offsetC = *offset++;
	} else
		offsetC = offsetPort->mValue;

	for (unsigned i = 0; i < outputBuffer.mNumFrames; i++) {	// the sample loop
		*buffer++ = sin(mPhase);				// compute and store sine value
		mPhase += (freqC * rateRecip);			// increment phase by possibly dynamic frequency

		if (freqDyn)							// if the freq. is dynamic
			freqC = *freq++;					// update it from the control input's buffer pointer
		if (scaleDyn)							// if the scale is dynamic
			scaleC = *scale++;		
		if (offsetDyn)							// if the offset is dynamic
			offsetC = *offset++
	}	
	while (mPhase >= CSL_TWOPI)					// wrap a 
	while (mPhase >= CSL_TWOPI)								// wraparound after 2pi radians
		mPhase -= CSL_TWOPI;
}

#endif

// Again, the added calls above are handled by macros defined in the CSL_Core.h header file, so you can say things like
//			DECLARE_SCALABLE_CONTROLS;
//			LOAD_SCALABLE_CONTROLS;
//			UPDATE_SCALABLE_CONTROLS;

void SineAsScaled::nextBuffer(Buffer & outputBuffer, unsigned outBufNum) throw (CException) {
	sample * buffer = outputBuffer.buffer(outBufNum);	// get pointer to the selected output channel
	float rateRecip = CSL_TWOPI / mFrameRate;
	unsigned numFrames = outputBuffer.mNumFrames;			// the number of frames to fill
	DECLARE_PHASED_CONTROLS;								// declare the frequency buffer and value as above
	DECLARE_SCALABLE_CONTROLS;								// declare the scale/offset buffers and values as above
#ifdef CSL_DEBUG
	logMsg("SineAsScaled nextBuffer");
#endif		
	LOAD_PHASED_CONTROLS;									// load the freqC from the constant or dynamic value
	LOAD_SCALABLE_CONTROLS;									// load the scaleC and offsetC from the constant or dynamic value
	
	for (unsigned i = 0; i < numFrames; i++) {					// sample loop
		*buffer++ = (sin(mPhase) * scaleValue) + offsetValue;	// compute and store (scaled and offset) sine value
		mPhase += (freqValue * rateRecip);						// increment phase
		UPDATE_PHASED_CONTROLS;								// update the dynamic frequency
		UPDATE_SCALABLE_CONTROLS;							// update the dynamic scale/offset
	}	
	while (mPhase >= CSL_TWOPI)								// wrap around after 2pi radians
		mPhase -= CSL_TWOPI;
}

// Pretty-print the receiver for debugging

void SineAsScaled::dump() {
	logMsg("a SineAsScaled");		// print a message
	Scalable::dump();				// this dumps the input port map
}
