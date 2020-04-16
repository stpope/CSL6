//
//	Ambisonic.h -- Higher Order Ambisonic abstract base class for all Ambisonic effects and panners.
//	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
//	Higher Order Ambisonic classes written by Jorge Castellanos, Graham Wakefield, Florian Hollerweger, 2005
//
//	Encapsulates parameters regarding the ambisonic order of the class (including hybrid orders)
//
//	CONVENTIONS USED IN THIS CODE:
//	------------------------------
//
//	***coordinate system***
//	Left oriented
//	Azimuth = 0 on the x/z plane and increasing towards the positive y direction
//	Elevation = 0 on x/y plane and increasing towards the positive z direction
//	Internal angle representation: spherical radians
//
//	***encoding convention***
//	following the Furse-Malham set (Ambisonic channel weighting for uniform energy distribution)
//
//	***abbreviations used in code and comments***
//	M	... Ambisonic order (in uniform order system)
//	M_h	... horizontal Ambisonic order (in hybrid order systems)
//	M_v	... horizontal Ambisonic order (in hybrid order systems)
//	N	... total number of Ambisonic encoded channels (horizontal and vertical)
//	N_h	... number of horizontal Ambisonic channels (in hybrid order systems)
//	N_v	... number of vertical Ambisonic channels (in hybrid order systems)
//	L	... number of available loudspeakers
//
//	***ordering and naming of Ambisonic channels = spherical harmonics***
//	These conventions follow the ones used in the thesis by Jerome Daniel.
//	The 3rd order naming convention (which Daniel doesn't provide) follows the one used in the thesis of David Malham.
//	Watch out for different conventions in other papers!
//	index			0	1	2	3	4	5	6	7	8	9	10	11	12	13	14	15
//	M (order)		0	1	1	1	2	2	2	2	2	3	3	3	3	3	3	3
//	m (=M)			0	1	1	1	2	2	2	2	2	3	3	3	3	3	3	3
//	n				0	1	1	0	2	2	1	1	0	3	3	2	2	1	1	0
//	sigma			'1'	1	-1	'1'	1	-1	1	-1	'1'	1	-1	1	-1	1	-1	'1'
//	name			W	X	Y	Z	U	V	S	T	R	P	Q	N	O	L	M	K
//	hor/vert/omni	om	h	h	v	h	h	v	v	v	h	h	v	v	v	v	v
//

#ifndef CSL_AMBISONIC_H
#define CSL_AMBISONIC_H

#include "CSL_Core.h"
#include "CPoint.h"
#include "SpatialSource.h"
#include "SpeakerLayout.h"

#define HOA_MAX_ORDER 2 // maximum order that the CSL HOA will handle

namespace csl {

/// Ambisonic order structure (separate definition for horizontal and vertical order):
/// \relates AmbisonicUnitGenerator

class AmbisonicOrder { 
public:
	AmbisonicOrder(unsigned hOrder = 0, unsigned vOrder = 0) : horizontalOrder(hOrder), verticalOrder(vOrder) { };
	~AmbisonicOrder() { };
	
	unsigned horizontalOrder;
	unsigned verticalOrder;		
	bool isUniform;				///< Returns true if horizontal and verical orders are identical.
};

/// Ambisonic Abstract Base Class.

class AmbisonicUnitGenerator : public UnitGenerator {
public:
	
	// Constructors & destructor:
	AmbisonicUnitGenerator(unsigned order = 0);					///< Initialize with uniform Ambisonic order. Defaults to zeroth order.
	AmbisonicUnitGenerator(unsigned horder, unsigned vorder);	///< Initialize with hybrid Ambisonic order
	AmbisonicUnitGenerator(AmbisonicOrder order); ///< 
	virtual ~AmbisonicUnitGenerator();
	
	AmbisonicOrder order() { return mOrder; }; ///< Returns the Ambisonic order.

protected:
	void setOrder(AmbisonicOrder order);
	
	AmbisonicOrder mOrder;				///< the order of the Unit Generator

	// returns the Ambisonic order from the number of Ambisonic channels: M = floor(sqrt(N) - 1)
	// NOTE - only works for uniform horizontal and vertical order (whereas ambiguity occurs in hybrid order systems)!
	unsigned channelsToUniformOrder(const unsigned channels);

	/// Compares the horizontal and vertical Ambisonic order of a hybrid order and returns the largest
	unsigned greaterOrder(const AmbisonicOrder order);

	/// Returns the number of Ambisonic channels from a hybrid Ambisonic order: N = 2*M_h + 1 + (M_v + 1)^2 - (2*M_v + 1)
	unsigned orderToChannels(const AmbisonicOrder order);

	/// Returns the number of Ambisonic channels corresponding to a uniform Ambisonic order: N = (M+1)^2
	unsigned orderToChannels(unsigned order);

	/// Returns the number of horizontal Ambisonic channels from a hybrid Ambisonic order: N_h = 2*M_h + 1
	unsigned orderToHorizontalChannels(const AmbisonicOrder order);

	/// Returns the number of vertical Ambisonic channels from a hybrid Ambisonic order: N_v = (M_v + 1)^2 - (2*M_v + 1)
	unsigned orderToVerticalChannels(const AmbisonicOrder order);

	/// Calculates a lookup table to map Ambisonic channel index to actually used UnitGenerator channel
	void channelIndexer(unsigned *indexArray);

	/// Calculates a lookup table to map actually used UnitGenerator channel to Ambisonic channel index
	void invChannelIndexer(unsigned *indexArray);
	
private:

	void initOrder();	// generic initialisation function

};

//	AmbisonicEncoder -- Higher Order Ambisonic encoding class
//
//	Higher Order Ambisonic class for positioning mono sound sources into an Ambisonic encoded soundfield.
//	The position information is fed to the encoder as two multichannel framestreams (azimuth and elevation). 
//	It is possible to specify either one uniform Ambisonic order or to define the horizontal and vertical 
//	order separately (hybrid order encoding). If no order(s) is/are specified, first order Ambisonic will be used (B-Format).

class AmbisonicEncoder : public AmbisonicUnitGenerator {
public:
	
	AmbisonicEncoder();			///< Default constructor
	AmbisonicEncoder(SpatialSource &input, unsigned order = 1);	///< Initialize with uniform Ambisonic order
	AmbisonicEncoder(SpatialSource &input, unsigned horder, unsigned vorder); ///< Initialize with hybrid Ambisonic order
	
	virtual ~AmbisonicEncoder();	///< Destructor

	void setInput(SpatialSource &input);	///< Set my input
	SpatialSource *input() { return (SpatialSource *)mInputPort->mUGen; };
	
	/// Does the DSP processing for the Ambisonic Encoder.
	virtual void nextBuffer(Buffer &outputBuffer, unsigned outBufNum) throw (CException);
		
protected:
	SampleBuffer mWeights;			///< Encoding weights for each order (per source)
	Port *mInputPort;		///< Holds the input to be encoded.
	void initialize();			///< Initializing method called by constructors
};

/// Flag for the decoding method

typedef enum {
	kPSEUDOINVERSE = 0, 
	kPROJECTION
} AmbisonicDecoderMethod;

/// Flag for the decoder flavour

typedef enum {
	kBASIC = 0, 
	kINPHASE, 
	kMAXRE
} AmbisonicDecoderFlavour;

//
//	AmbisonicDecoder -- Higher Order Ambisonic Decoding class
//
//	Higher Order Ambisonic class for decoding an Ambisonic encoded soundfield (e.g. the output of the AmbisonicEncoder or
//	the AmbisonicRotator) to a loudspeaker layout specified by SpeakerLayout. The Ambisonic order at which the decoder
//	oprates is derived by the following criteria: If no order is specified in the constructor, the order of the Ambisonic
//	encoded input is used. This order can be downgraded either by specifying a lower order in the constructor
//	(obviously, it cannot be increased). The decoder might further decrease the given order to match the maximum
//	order reproducable on the available loudspeaker layout. One out of two different decoding methods can be chosen by
//	using the flags kPROJECTION, kPSEUDOINVERSE in the constructor. One out of three available decoder flavors can
//	be applied by using the flags kBASIC, kINPHASE, kMAXRE.
//

class AmbisonicDecoder : public AmbisonicUnitGenerator {
public:
	// Constructors & destructor:
	/// Defaults to standard speaker layout as defined in "HOA_SpeakerLayout" class and to Ambisonic order of encoded input
	AmbisonicDecoder(AmbisonicUnitGenerator &input, 
					SpeakerLayout *layout = SpeakerLayout::defaultSpeakerLayout(), 
					AmbisonicDecoderMethod method = kPROJECTION, 
					AmbisonicDecoderFlavour flavour = kBASIC);
	/// Initializes with uniform Ambisonic order, defaults to standard speaker layout as defined in "HOA_SpeakerLayout" class
	AmbisonicDecoder(UnitGenerator &input, unsigned order, 
					SpeakerLayout *layout = SpeakerLayout::defaultSpeakerLayout(), 
					AmbisonicDecoderMethod method = kPROJECTION, 
					AmbisonicDecoderFlavour flavour = kBASIC);
	/// Initializes with hybrid Ambisonic order
	AmbisonicDecoder(UnitGenerator &input, unsigned hOrder, unsigned vOrder, 
					SpeakerLayout *layout = SpeakerLayout::defaultSpeakerLayout(), 
					AmbisonicDecoderMethod method = kPROJECTION, 
					AmbisonicDecoderFlavour flavour = kBASIC);
	
	~AmbisonicDecoder();	///< Destructor
		
	/// Does the DSP processing for the Ambisonic Decoder.
	virtual void nextBuffer(Buffer &outputBuffer, unsigned outBufNum) throw (CException);

protected:

	Port *mInputPort;
	float mNumChannelsInv;		// inverse of the numbers of loudspeakers (used for normalization)
	int *mIOChannelMap;			// lookup table from index of actually used Ambisonic channel input Ambisonic channel index
	SampleBufferVector mDecodingMatrix;	// deccoding matrix D
	SpeakerLayout *mSpeakerLayout;				// loudspeaker layout provided by "HOA_SpeakerLayout" class
	AmbisonicDecoderMethod mDecodingMethod;		// decoding method: projection or pseudoinverse
	AmbisonicDecoderFlavour mDecoderFlavour;	// decoder flavour: basic, in-phase or max-rE


	/// initializing method called by constructors
	void initialize(UnitGenerator &input, AmbisonicDecoderMethod method, AmbisonicDecoderFlavour flavour);
	
	void asProjection();	///< Create the decoding matrix "D" using the projection method
	void asPseudoInverse(); ///< create the decoding matrix "D" using the pseudoinverse method
		
	void makeInPhase(unsigned greaterOrder);	///< Adjusts the decoding matrix D for in-phase flavour
	void makeMaxRE(unsigned greaterOrder);		///< Adjusts the decoding matrix D for Max rE flavour

	void makeTransposedReEncodingMatrix(float **transposeMatrix); ///< Utility method that creates the transposed re-encoding matrix C'

};

}

#endif
