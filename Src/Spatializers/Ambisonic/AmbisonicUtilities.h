//
//	AmbisonicUtilities.h -- Higher Order Ambisonic utility classes
//	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
//	Higher Order Ambisonic classes written by Jorge Castellanos, Graham Wakefield, Florian Hollerweger, 2005
//
//	Utility classes used by HOA_Encoder, MOA_Encoder, AmbisonicRotator, MOA_Rotator, HOA_Decoder

#ifndef AMBISONIC_UTILITIES_H
#define AMBISONIC_UTILITIES_H

#include "CSL_Core.h"
#include "Ambisonic.h"

#define AMBI_INVSQRT2 (1/1.414213562)

namespace csl {

/// Utility function used in calculating the inverse of a matrix, used in AmbisonicDecoder for the pseudoinverse method

void singularValueDecomposition(sample** a, int m, int n, sample* w, sample** v);

/// Utility function that calculates fuma encoding weights for a given order, azimuth and elevation.

void fumaEncodingWeights(SampleBuffer weights, const AmbisonicOrder &order, sample azimuth, sample elevation);

/// Utility function that calculates fuma encoding weights for a given order, azimuth and elevation.

void fumaIndexedEncodingWeights(SampleBuffer weights, const AmbisonicOrder &order, sample &azimuth, sample &elevation);


//	AmbisonicMixer -- Higher Order Ambisonic mixing class
//
//	Higher Order Ambisonic class for mixing encoded ambisonic encoded audio streams.  All incoming streams should have
//	the same order; the order used is derived from the first stream added.

class AmbisonicMixer : public AmbisonicUnitGenerator {
public:
	
	// Constructors & destructor:
	AmbisonicMixer(unsigned order = 1); // Default constructor
	AmbisonicMixer(unsigned hOrder, unsigned vOrder); // Default constructor
	
	//// Initialize with one input, derive the Mixer ambisonic order from this input.
//	AmbisonicMixer(UnitGenerator &input);
	
	~AmbisonicMixer();  ///< Destructor

	/// Initializing method called by constructors
	void initialize();
	
	/// methods for adding/removing inputs to the mixer.
	void addInput(AmbisonicUnitGenerator &input);
	void addInput(UnitGenerator &input);
		
	unsigned numInputs() { return mInputs.size(); }; 	///< Number of active inputs

	// nextBuffer(). Does the DSP processing for the Ambisonic Mixer.
	virtual void nextBuffer(Buffer &outputBuffer, unsigned outBufNum) throw(CException);

protected:
	UGenVector mInputs;						///< vector of pointers to the loudspeakers
	Buffer *mInBuffer;						///< buffer for the input framestream
	float mInvNumInputs;					///< the inverse of the number of inputs (used for normalization)
	
};

typedef enum {
	kTILT = 0, 
	kTUMBLE, 
	kROTATE	// to be used with the "setNthInput" method to add control sources
} Axes;										// tilt -> x axis, tumble -> y axis, rotate -> z axis

//	AmbisonicRotator.h -- Higher Order Ambisonic rotator class
//	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
//	Higher Order Ambisonic classes written by Jorge Castellanos, Graham Wakefield, Florian Hollerweger, 2005
//
//	Higher Order Ambisonic class for rotating an Ambisonic encoded sound field (e.g. the output of the HOA_Encoder)
//	around any combination of x, y and z axes. Rotation is applied by using the method "setNthInput" with the flags
//	HOA_TILT, HOA_TUMBLE, HOA_ROTATE as desired. The order of the incoming Ambisonic framestream can be degraded to
//	match the maximum order of the available decoding system (number of available speakers). It is possible to specify
//	either one uniform Ambisonic order or to define the horizontal and vertical order separately (hybrid order rotating).
//	If no order(s) is/are specified, the order(s) of the Ambisonic encoded input framestream will be used.

class AmbisonicRotator : public AmbisonicUnitGenerator {
public:
				// Constructors & destructor:
				/// initializes with no rotation
	AmbisonicRotator(AmbisonicUnitGenerator &input);
				/// initializes with uniform Ambisonic order and no rotation
	AmbisonicRotator(UnitGenerator &input, unsigned order);
				/// initializes with hybrid Ambisonic  order and no rotation
	AmbisonicRotator(UnitGenerator &input, unsigned vorder, unsigned horder);

	~AmbisonicRotator(); // destructor
	
	
	// set control input sources according axis flag
	void setNthInput(float amount, Axes axis);
	void setTilt(float amount);
	void setTumble(float amount);
	void setRotate(float amount);
	
	// Overriding Framestream::next_buffer(). Does the DSP processing for the Ambisonic Rotator.
	virtual void nextBuffer(Buffer &outputBuffer, unsigned outBufNum) throw(CException);
				
protected:
	unsigned mNumFrames;
	unsigned mGreaterOrder;
	unsigned mNumChannelsGreaterOrder;
	unsigned *mChannelIndex;		// index to the output buffer in the encoding functions.
	unsigned *mInputChannelIndex;	// index to the input buffer in the encoding functions (in case input order != output order)
	bool mShouldRotate, mShouldTurn, mShouldTilt;	// flags to note whether rotation in each axis is being used

private:
	Port *mInputPort;

	sample mRotate, mTumble, mTilt;					// the current amount of rotation in each axis (in radians)
	SampleBuffer mSinAngle, mCosAngle;				// Used to hold sines/cosines of tilt, tumble, rotate angles during audio processing
	SampleBufferVector mOutPtr, mInPtr;				// pointers to Ambisonic encoded input and Ambisonic encoded & rotated output buffers
	
	void initialize(UnitGenerator &input);	
	
	// methods for tilt, tumble & rotate at each order
	void tiltFirstOrder();
	void tiltSecondOrder();
	void tiltThirdOrder();
	
	void tumbleFirstOrder();
	void tumbleSecondOrder();
	void tumbleThirdOrder();
	
	// methods for rotation at each order
	// separated by horizontal & vertical for hybrid order systems (which can be rotated, but not tilted or tumbled)
	void rotateZerothOrder();
	void rotateFirstOrderHorizontal();	
	void rotateSecondOrderHorizontal();
	void rotateThirdOrderHorizontal();
	void rotateFirstOrderVertical();	
	void rotateSecondOrderVertical();
	void rotateThirdOrderVertical();
};

}

#endif
