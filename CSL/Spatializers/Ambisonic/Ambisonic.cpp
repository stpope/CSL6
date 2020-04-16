//
//	Ambisonic.cpp -- Higher Order Ambisonic abstract base class for all Ambisonic effects and panners.
//	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
//	Higher Order Ambisonic classes written by Jorge Castellanos, Graham Wakefield, Florian Hollerweger, 2005
//
// AmbisonicUnitGenerator.cpp -- Higher Order Ambisonic superclass for all Ambisonic encoded framestreams
// See the copyright notice and acknowledgment of authors in the file COPYRIGHT
// Higher Order Ambisonic classes written by Jorge Castellanos, Graham Wakefield, Florian Hollerweger, 2005
//
// Mixin superclass for the ambisonic framestream classes (e.g. HOA_Encoder, HOA_Rotator, AmbisonicDecoder).
// Encapsulates parameters regarding the ambisonic order of the class (including hybrid orders)
//
// See HOA_AmbisonicFramestream.h for descriptions of the the class members.

#include "Ambisonic.h"
#include "AmbisonicUtilities.h"

#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <iostream>

#define AMBI_INVSQRT2 (1/1.414213562)
#define INV_SQRT2 (1/1.414213562)
#define AMBI_INVSQRT2 (1/1.414213562)

using std::cout;
using std::endl;

using namespace csl;

/*******************CONSTRUCTORS & DESTRUCTOR*******************/

AmbisonicUnitGenerator::AmbisonicUnitGenerator(unsigned torder) : mOrder(torder, torder) {
	initOrder();	// initialize
}

AmbisonicUnitGenerator::AmbisonicUnitGenerator(unsigned hOrder, unsigned vOrder) : mOrder(hOrder, vOrder) {
	mOrder.verticalOrder = vOrder;	
	initOrder();
}

AmbisonicUnitGenerator::AmbisonicUnitGenerator(AmbisonicOrder torder) : mOrder(torder) {
	initOrder();	// initialize
}

AmbisonicUnitGenerator::~AmbisonicUnitGenerator() { }


void AmbisonicUnitGenerator::initOrder() {

	setCopyPolicy(kIgnore); // This is needed so the default kCopy doesn't overide the multiple channel panning done here.

	mOrder.isUniform = (mOrder.horizontalOrder == mOrder.verticalOrder); // false for hybrid orders
	mNumChannels = orderToChannels(mOrder);		// derive necessary output channels from ambisonic order
	
}

void AmbisonicUnitGenerator::setOrder(AmbisonicOrder torder) {
	mOrder = torder;
	initOrder();
}

/*******************UTILITY FUNCTIONS*******************/

unsigned AmbisonicUnitGenerator::channelsToUniformOrder(const unsigned channels) {
	// M = floor(sqrt(N) - 1)
	return (unsigned)floor(sqrt((double)channels)-1);
}

unsigned AmbisonicUnitGenerator::greaterOrder(const AmbisonicOrder torder) {
	return torder.horizontalOrder > torder.verticalOrder ? torder.horizontalOrder : torder.verticalOrder; 
}

unsigned AmbisonicUnitGenerator::orderToChannels(const AmbisonicOrder torder) {
	// hybrid system version: N = 2*M_h + 1 + (M_v + 1)^2 - (2*M_v + 1)
	return (2 * torder.horizontalOrder + 1) + ((unsigned)pow(torder.verticalOrder + 1.f, 2) - (2 * torder.verticalOrder + 1));
}

unsigned AmbisonicUnitGenerator::orderToChannels(unsigned torder) {
	// uniform system version: N = (M+1)^2
	return (unsigned)pow(torder + 1.f, 2);
}

unsigned AmbisonicUnitGenerator::orderToHorizontalChannels(const AmbisonicOrder torder) {
	// N_h = 2*M_h + 1
	return (2 * torder.horizontalOrder + 1);
}

unsigned AmbisonicUnitGenerator::orderToVerticalChannels(const AmbisonicOrder torder) {
	// N_v = (M_v + 1)^2 - (2*M_v + 1)
	return ((unsigned)pow(torder.verticalOrder + 1.f, 2) - (2 * torder.verticalOrder + 1));
}


/*******************CHANNEL INDEXER FUNCTIONS*******************/

void AmbisonicUnitGenerator::channelIndexer(unsigned *indexArray) {
	AmbisonicOrder torder = mOrder;
	unsigned *channelIndex = indexArray;
	// how many channels it would need *if* it was a uniform order (hOrder == vOrder)
	unsigned numChannelsGreaterOrder = orderToChannels(greaterOrder(torder));
	
	// how many channels it actually needs for the hybrid order
	unsigned nnumChannels = orderToChannels(torder);
		
	unsigned n = 1;
	unsigned currentOrder = 1;
	unsigned hoaChannel = 1;
	unsigned numChannelsCurrentOrder;		
	
	// fill the channel indexer with zeroes
	for (unsigned i = 0; i < numChannelsGreaterOrder; i++)
		channelIndex[i] = 0;

	// work our way up the orders until we have exhausted all the required channels
	while (n < nnumChannels) {
		// horizontal indexing required?
		if (currentOrder <= torder.horizontalOrder) {
			// there are always 2 channels for horizontal encoding at each order
			channelIndex[hoaChannel++] = n++;
			channelIndex[hoaChannel++] = n++;
		} else {
			hoaChannel += 2;  // skip them
		}
		
		// how many channels are required up to the current order?
		numChannelsCurrentOrder = orderToChannels(currentOrder);
				
		// vertical indexing required?
		if (currentOrder <= torder.verticalOrder) {
			// do until all required vertical channels up to the current order are exhausted
			while(hoaChannel < numChannelsCurrentOrder) {
				channelIndex[hoaChannel++] = n++; // set and increment 
			}
		} else {			// skip them
			hoaChannel = numChannelsCurrentOrder;
		}
		// finished with the current order, increment and do again
		currentOrder++;
	}	
	return;
}


void AmbisonicUnitGenerator::invChannelIndexer(unsigned *indexArray) {

	AmbisonicOrder torder = mOrder;
	unsigned *channelIndex = indexArray;

	// how many channels it actually needs for the hybrid order
	unsigned nnumChannels = orderToChannels(torder);
	channelIndex[0] = 0; // w channel
	
	unsigned n = 1;
	unsigned currentOrder = 1;
	unsigned numChannelsCurrentOrder;		
	
	// work our way up the orders until we have exhausted all the required channels
	while (n < nnumChannels) {
		// horizontal indexing required?
		if (currentOrder <= torder.horizontalOrder) {
			// there are always 2 channels for horizontal encoding at each order
			channelIndex[n] = n++;
			channelIndex[n] = n++;
		} else {
			n += 2;				// skip them
		}
		// how many channels are required up to the current order?
		numChannelsCurrentOrder = orderToChannels(currentOrder);
		// vertical indexing required?
		if (currentOrder <= torder.verticalOrder) {
			// do until all required vertical channels up to the current order are exhausted
			while(n < numChannelsCurrentOrder) {
				channelIndex[n] = n++;  // set and increment 
			}
		} else {
			n = numChannelsCurrentOrder;  // skip them
		}			// finished with the current order, increment and do again
		currentOrder++;
	}	
	return;
}


//////////////////////////////////////////////
/*     AMBISONIC ENCODER IMPLEMENTATION     */
//////////////////////////////////////////////

/*******************CONSTRUCTORS & DESTRUCTOR*******************/
AmbisonicEncoder::AmbisonicEncoder() : AmbisonicUnitGenerator() { }
	
AmbisonicEncoder::AmbisonicEncoder(SpatialSource &input, unsigned order) : AmbisonicUnitGenerator(order), mInputPort(NULL) {
	setInput(input);
	initialize();
}

AmbisonicEncoder::AmbisonicEncoder(SpatialSource &input, unsigned horder, unsigned vorder) : AmbisonicUnitGenerator(horder, vorder), mInputPort(NULL) {
	setInput(input);
	initialize();
}

AmbisonicEncoder::~AmbisonicEncoder() {
	delete [] mWeights;				// de-allocate memory

	if (mInputPort != NULL) {
		delete mInputPort;
	}
}


/*******************INITIALIZE METHOD*******************/

void AmbisonicEncoder::initialize() {

	// allocate memory for encoding weights
	mWeights = new sample[mNumChannels];
	// initialize them
	mWeights[0] = AMBI_INVSQRT2;	// W channel weight is constant
	for (unsigned i = 1; i < mNumChannels; i++) { 
		mWeights[i] = 0.f;
	}	
}

/// Use to set the input to be encoded. 
void AmbisonicEncoder::setInput(SpatialSource &input) { 

	if (mInputPort == NULL)
		mInputPort = new Port(&input);

	if (mInputPort->mUGen != &input) {  // Make sure no setting the same input.
		mInputPort->mUGen->removeOutput(this); // Remove the encoder (slef) from the previous input ugen.
		mInputPort->mUGen =  &input; // mInputPort->setInput(input);
		
		input.addOutput(this);	// be sure to add me as an output of the other guy		
	}
#ifdef CSL_DEBUG
	logMsg("AmbisonicEncoder::setInput");
#endif		
}

/*******************NEXT_BUFFER METHOD*******************/

// To get my next buffer, get a buffer from the input, and then "process" it...
void AmbisonicEncoder::nextBuffer(Buffer &outputBuffer, unsigned outBufNum) throw(CException) {	
	
	unsigned numFrames = outputBuffer.mNumFrames;	// Block size
	Buffer *inputBuffer;
	outputBuffer.zeroBuffers();			// Initialize output buffer with zeros
#ifdef CSL_DEBUG
	logMsg("AmbisonicEncoder::nextBuffer");
#endif
	if (outputBuffer.mNumChannels < mNumChannels) // Make sure the buffer passed has enough channels
		logMsg(kLogError, "The AmbisonicEncoder requires %d output channels, found only %d ???\n", mNumChannels, outputBuffer.mNumChannels);
			
	// Get a pointer to the buffer of the sound source to be encoded
//	inputBuffer = mInputPort->pullInput(numFrames);	
// THE BLOCK BELOW WAS PULL_INPUT OF THE UGEN PORT...
	inputBuffer = mInputPort->mBuffer;				// else get the buffer
	inputBuffer->mNumFrames = numFrames;
	inputBuffer->mType = kSamples;
	mInputPort->mUGen->nextBuffer(*inputBuffer);					// and ask the UGen for nextBuffer()
	inputBuffer->mIsPopulated = true;

	SpatialSource *input = (SpatialSource *)mInputPort->mUGen;

//	if (input->positionChanged()) {
		// Get the encoding weights for this encoder
		fumaEncodingWeights(mWeights, mOrder, input->azimuth(), input->elevation());
//	}
	// Start encoding audio from zeroth order; higher orders cascade from this function
	for (unsigned i = 0; i < mNumChannels; i++) {
		SampleBuffer inPtr = inputBuffer->buffer(0);	// Reset the input pointer
		SampleBuffer outPtr = outputBuffer.buffer(i);	// Set a pointer to the output buffer
		
		for (unsigned j = 0; j < numFrames; j++) {		
						// Actual audio processing: encode sound sources following the Furse-Malham encoding convention
			*outPtr++ += (*inPtr++) * mWeights[i];	// encoding W channel
		}
	}
}


////////////////////////////////////////////
/*    AMBISONIC DECODER IMPLEMENTATION    */
////////////////////////////////////////////


/*******************CONSTRUCTORS & DESTRUCTOR*******************/
	
AmbisonicDecoder::AmbisonicDecoder(AmbisonicUnitGenerator &input, SpeakerLayout *layout, 
			AmbisonicDecoderMethod method, AmbisonicDecoderFlavour flavour) 
		: AmbisonicUnitGenerator(input.order()), mInputPort(NULL), mSpeakerLayout(layout), 
			mDecodingMethod(method), mDecoderFlavour(flavour) {	
	initialize(input, method, flavour);	
}

AmbisonicDecoder::AmbisonicDecoder(UnitGenerator &input, unsigned order, SpeakerLayout *layout, 
			AmbisonicDecoderMethod method, AmbisonicDecoderFlavour flavour)
		: AmbisonicUnitGenerator(order), mInputPort(NULL), 
			mDecodingMethod(method), mDecoderFlavour(flavour) {
	initialize(input, method, flavour);
}

AmbisonicDecoder::AmbisonicDecoder(UnitGenerator &input, unsigned hOrder, unsigned vOrder, 
			SpeakerLayout *layout, AmbisonicDecoderMethod method, AmbisonicDecoderFlavour flavour)
		: AmbisonicUnitGenerator(hOrder, vOrder), mInputPort(NULL), 
			mDecodingMethod(method), mDecoderFlavour(flavour) {
	initialize(input, method, flavour);
}

AmbisonicDecoder::~AmbisonicDecoder() {
	mInputPort->mUGen->removeOutput(this);
	delete mInputPort;
	delete [] mIOChannelMap;
	
	for (unsigned s = 0; s < mSpeakerLayout->numSpeakers(); s++)
		delete [] mDecodingMatrix[s];
	free(mDecodingMatrix);
}


/*******************INITIALIZE METHOD*******************/

void AmbisonicDecoder::initialize(UnitGenerator &input, AmbisonicDecoderMethod method, AmbisonicDecoderFlavour flavour) {

//	if (mSpeakerLayout == NULL) // if the user didn't set a layout then ask for the default layout.
//		mSpeakerLayout = SpeakerLayout::defaultSpeakerLayout();

	mInputPort = new Port(&input);
	input.addOutput(this);	// be sure to add me as an output of the other guy
		
	unsigned numSpeakers = mSpeakerLayout->numSpeakers(); // get a local copy of the number of speakers.
	AmbisonicOrder decodingOrder = mOrder;
	
	// are there enough speakers to decode this order?
//	if (orderToChannels(decodingOrder) > numSpeakers) {
//		// less speakers than necessary, so reduce decoding order by successively decrementing the vertical,
//		// then horizontal order, until L >= N criterium applies
//		while (orderToChannels(decodingOrder) > numSpeakers) {
//			if (decodingOrder.verticalOrder >= decodingOrder.horizontalOrder) {
//				--decodingOrder.verticalOrder;
//				if (orderToChannels(decodingOrder) <= numSpeakers)
//					break;
//			}
//			--decodingOrder.horizontalOrder;
//		}
//		
//		logMsg(kLogWarning, 
//			"Reducing decoding order because of available number of speakers (%d): horizontal order = %i, vertical order = %i\n", 
//			numSpeakers, decodingOrder.horizontalOrder, decodingOrder.verticalOrder);
//	}

	// are there enough encoded channels in the input to decode the specified order?
	if (orderToChannels(decodingOrder) > input.numChannels()) {
		// reduce order if not enough channels in the input ugen
		while (orderToChannels(decodingOrder) > input.numChannels()) {
			if (decodingOrder.verticalOrder >= decodingOrder.horizontalOrder) {
				--decodingOrder.verticalOrder;
				if (orderToChannels(decodingOrder) <= input.numChannels())
					break;
			}
			--decodingOrder.horizontalOrder;
		}
		logMsg(kLogWarning, 
			"Reducing decoding order because encoded input does not contain enough channels for specified order.\n Input channels:(%d): horizontal order = %i, vertical order = %i\n", 
			input.numChannels(), decodingOrder.horizontalOrder, decodingOrder.verticalOrder);
	}

	
	// recalculate the greater order & the uniformity
	decodingOrder.isUniform = (decodingOrder.horizontalOrder == decodingOrder.verticalOrder);
	mNumChannels = orderToChannels(decodingOrder); // how many input ambisonic channels needed at this order
	// how many channels it would need *if* it was a uniform order (hOrder == vOrder)
	unsigned numChannelsGreaterOrder = orderToChannels(greaterOrder(decodingOrder));
	unsigned channelIndex[numChannelsGreaterOrder];
	unsigned invChannelIndex[numChannelsGreaterOrder];

	// The invChannelIndexer doesn't go thru all channels, so make sure to set others to zero.
	memset(invChannelIndex , 0, numChannelsGreaterOrder * sizeof(unsigned));
	
	channelIndexer(channelIndex); // get a channel indexer based on the input order
	invChannelIndexer(invChannelIndex); // get an indexer from output framestream channel to 'ideal' ambisonic channel (after fixing the mOrder)

	mIOChannelMap = new int[mNumChannels];
	for (unsigned n = 0; n < mNumChannels; n++) {
//		cout << n << ": " << invChannelIndex[n] << endl;
		mIOChannelMap[n] = channelIndex[invChannelIndex[n]];
	}
	
	mDecodingMatrix = (sample**) malloc(numSpeakers * sizeof(sample*));
	for (unsigned s = 0; s < numSpeakers; s++)
		mDecodingMatrix[s] = new sample[mNumChannels];
	
	if (mDecodingMethod == kPSEUDOINVERSE)
		asPseudoInverse(); // build the Decoding matrix D using the pseudo inverse method
	else
		asProjection();  // build the Decoding matrix using the projection method

	
	// process the decoding matrix according to the desired decoder flavour
	if (mDecoderFlavour == kINPHASE) {
		if ( ! decodingOrder.isUniform) 
			logMsg(kLogWarning, "AmbisonicDecoder: the in-phase decoding method is only well defined for non-hybrid systems - you have asked for an in phase decoder for a hybrid ambisonic decoding.\n");
		makeInPhase(greaterOrder(decodingOrder));
	} 
	else if (mDecoderFlavour == kMAXRE) {
		if ( ! decodingOrder.isUniform)
			logMsg(kLogWarning, "AmbisonicDecoder: the max-rE decoding method is only well defined for non-hybrid systems - you have asked for a max rE decoder for a hybrid ambisonic decoding.\n");
		makeMaxRE(greaterOrder(decodingOrder));
	}

#ifdef CSL_DEBUG

	printf("Decoding from %i ambisonic channels to %i speakers", mNumChannels, numSpeakers);
	printf(mDecodingMethod == kPSEUDOINVERSE ? ", using pseudo inverse method" : ", using projection method");
	switch(mDecoderFlavour) {
		case 0: printf(" of basic flavour\n"); break;
		case 1: printf(" of in-phase flavour\n"); break;
		case 2: printf(" of max rE flavour\n"); break;
	}
	
	printf("Decoding Matrix:\n");
	for (unsigned s=0; s< numSpeakers; s++ ) {
		printf("speaker %d\t",s);
		for (unsigned n=0; n < mNumChannels; n++) {
			printf("%f\t\t", mDecodingMatrix[s][n]);
		}
		printf("\n");
	}
	printf("\n");
	
	printf("Created AmbisonicDecoder with horizontal order = %d, vertical order = %d\n", mOrder.horizontalOrder, mOrder.verticalOrder);

#endif

}


/*******************DECODING MATRIX METHODS*******************/
/*
B				... column vector of encoded Ambisonic channels, dimension N x 1
C				... re-encoding matrix, dimension N x L
C'				... transposed re-encoding matrix, dimension L x N
pinv(C)			... pseudoinverse of the re-encoding matrix C, dimension L x N
C * C'			... matrix product of the matrices C and C', dimension N x N
inv (C * C')	... inverse of C * C', dimension N x N
D				... decoding matrix, dimension L x N
p				... column vector of decoded loudspeaker driving signals, dimension L x 1

The matching condition (reference soundfield = synthesized soundfield): B = C * p
gives the decoding equation: p = D * B
Where D can be calculated by the projection method, or by the pseudoinverse method
*/

/// build the Decoding matrix D using the projection method D = (1/L) * C'
void AmbisonicDecoder::asProjection() {

	makeTransposedReEncodingMatrix(mDecodingMatrix);	// get C' before scaling it down by 1/L

	unsigned numSpeakers = mSpeakerLayout->numSpeakers();	
	sample invNumSpeakers = 1.f/((float)numSpeakers);	// inverse of number of speakers (1/L)
	
	// for each element in the matrix...
	for (unsigned s = 0; s < numSpeakers; s++) {
		for (unsigned i = 0; i < mNumChannels; i++) {
			mDecodingMatrix[s][i] *= invNumSpeakers; // ...scale down matrix element by 1/L:
		}
	}
}

/// Build the Decoding matrix D using the pseudo inverse method \n D = pinv(C) = C' * inv(C * C'). \n
/// Pseudo inverse code based on the matrix library found at: http://home1.gte.net/edwin2/Matrix/
void AmbisonicDecoder::asPseudoInverse()  {
	makeTransposedReEncodingMatrix(mDecodingMatrix);	// get C' before scaling it down by 1/L
	
	unsigned nnumChannels = mNumChannels;
	unsigned numSpeakers = mSpeakerLayout->numSpeakers();

				
	// build temporary 2D array of dimension N x N to be used in calculating the psuedo-inverse: (C * C') and inv(C * C')
	SampleBufferVector squareMatrix = (sample**) malloc(nnumChannels * sizeof(sample*));
	for (unsigned n = 0; n < nnumChannels; n++) {
		squareMatrix[n] = new sample[nnumChannels];
	}

	SampleBufferVector invSquareMatrix = (sample**) malloc(nnumChannels * sizeof(sample*));
	for (unsigned n = 0; n < nnumChannels; n++) {
		invSquareMatrix[n] = new sample[nnumChannels];
	}

	// create temporary matrices
	sample** transposedReEncodingMatrix = (sample**) malloc(numSpeakers * sizeof(sample*));
	for (unsigned s = 0; s < numSpeakers; s++)
		transposedReEncodingMatrix[s] = new sample[nnumChannels];

	 makeTransposedReEncodingMatrix(transposedReEncodingMatrix);

	// calculate (C * C'):
	for (unsigned n = 0; n < nnumChannels; n++) {  // for each row of (C * C')...
		
		for (unsigned j = 0; j < nnumChannels; j++ ) {  // for each column of (C * C')...
			squareMatrix[n][j] = 0.f;	// ... zero the new cell
			// matrix multiplication: stepping along the row of C and down the column of C', summing as it goes
			for (unsigned s = 0; s < numSpeakers; s++ ) {
				// (C * C') - we read C' as C by swapping the rows and columns of C' in the first term
				squareMatrix[n][j] += transposedReEncodingMatrix[s][n] * transposedReEncodingMatrix[s][j];
			}
		}
	}
	
	// ***start of matrix inversion: inv(C * C')***
	
	SampleBuffer uu	= new sample [nnumChannels * nnumChannels];	// more temporary variables
	SampleBuffer vv	= new sample [nnumChannels * nnumChannels];
	SampleBuffer w	= new sample [nnumChannels];
	SampleBufferVector u	= new sample*[nnumChannels];
	SampleBufferVector v	= new sample*[nnumChannels];
	
	for (unsigned i = 0; i < nnumChannels; i++) {
		u[i] = &(uu[nnumChannels*i]);		
		v[i] = &(vv[nnumChannels*i]);
		for (unsigned j = 0; j < nnumChannels; j++)
			u[i][j] = squareMatrix[i][j];
	}
	
	singularValueDecomposition(u, nnumChannels, nnumChannels, w, v);	// singular value decomposition
	sample wmax = 0.0;		// maximum singular value.

	for (unsigned j = 0; j < nnumChannels; j++) {
		if (w[j] > wmax)
			wmax = w[j];
	}
	
	sample wmin = wmax*1e-12; // epsilon
	for (unsigned k = 0; k < nnumChannels; k++)
		if (w[k] < wmin)
			w[k] = 0.0;
		else
			w[k] = 1.0/w[k];

	for (unsigned i = 0; i < nnumChannels; i++) {
		for (unsigned j = 0; j < nnumChannels; j++) {
			invSquareMatrix[i][j] = 0.0;
			for (unsigned k = 0; k < nnumChannels; k++)
				invSquareMatrix[i][j] += v[i][k]*w[k]*u[j][k];
		}
	}
									// clean up
	delete [] w;
	delete [] u;
	delete [] v;
	delete [] uu;
	delete [] vv;
	
	// ***end of matrix inversion***
	
	// get decoding matrix C by pseudoinverting it, i.e. multiply transposed re-encoding matrix by matrix
	// we just inverted: D = C' * inv(C * C')
	
	for (unsigned s = 0; s < numSpeakers; s++ ) {  // for each row of mDecodingMatrix...
		for (unsigned n = 0; n < nnumChannels; n++) {  // for each column of mDecodingMatrix...
			mDecodingMatrix[s][n] = 0.f; // ...zero the new cell
			for (unsigned j = 0; j < nnumChannels; j++) {  // for each column of C'...
				// ... do operations to obtainD = C' * inv(C * C')
				mDecodingMatrix[s][n] += transposedReEncodingMatrix[s][j] * invSquareMatrix[j][n];			
			}
		}
	}

	// Cleanup 
	for (unsigned n = 0; n < nnumChannels; n++) {
		delete [] squareMatrix[n];
		delete [] invSquareMatrix[n];
	}
	for (unsigned s = 0; s < numSpeakers; s++)
		delete [] transposedReEncodingMatrix[s];
	
	free(squareMatrix);
	free(invSquareMatrix);
	free(transposedReEncodingMatrix);
}


/*******************TRANSPOSED RE-ENCODING MATRIX GENERATION METHOD*******************/

void AmbisonicDecoder::makeTransposedReEncodingMatrix(float **transposeMatrix)
{
	unsigned numSpeakers = mSpeakerLayout->numSpeakers();
	Speaker *speaker;
	AmbisonicOrder order = mOrder;
	float **transposedReEncodingMatrix = transposeMatrix;
	
	// for each speaker
	for (unsigned s = 0; s < numSpeakers; s++) {		
		speaker = mSpeakerLayout->speakerAtIndex(s);							// get the details of this speaker
		
		// create a row in the LxN matrix containing the encoding weights (FuMa) for this speaker
		transposedReEncodingMatrix[s][0] = AMBI_INVSQRT2;			// W channel is always the same
		fumaEncodingWeights(transposedReEncodingMatrix[s], order, speaker->azimuth(), speaker->elevation());
	}
	
	return;
}


/*******************DECODING MATRIX FLAVOUR ADOPTION METHODS*******************/

/// Scales the decoding matrix according to the factors for max rE decoding
void AmbisonicDecoder::makeMaxRE(unsigned greaterOrder) {
	unsigned numSpeakers = mSpeakerLayout->numSpeakers();

	// hard-coded gain coefficients for in max-rE decoding (parameters specified up to 4th order):
	sample maxREParameters[][5] = { {1.0,	1.0,	1.0,	1.0,	1.0},		
	// n = 0, M = 0, 1, 2, 3, 4 
	{0.f,	0.577,	0.775,	0.861,	0.906},		
	// n = 1, M = 0, 1, 2, 3, 4 
	{0.f,	0.f,	0.4,	0.612,	0.732},		
	// n = 2, M = 0, 1, 2, 3, 4 
	{0.f,	0.f,	0.f,	0.305,	0.501},		
	// n = 3, M = 0, 1, 2, 3, 4 
	{0.f,	0.f,	0.f,	0.f,	0.246},		
	// n = 4, M = 0, 1, 2, 3, 4
	};
	
	// calculate gain factors for each order
	unsigned M = greaterOrder; // the order we are assuming as the context
	
	for (unsigned i = 1; i < mNumChannels; i++) { // skip W channel as it's factor is always 1
		for (unsigned l = 0; l < numSpeakers; l++) {
			// channelsToUniformOrder(i)+1 is a convenient way to get n (the order of each ambisonic channel), but it is not defined for W
			mDecodingMatrix[l][i] *= maxREParameters[channelsToUniformOrder(i)+1][M];
		}
	}
}

/// Scales the decoding matrix according to the factors for in-phase decoding
void AmbisonicDecoder::makeInPhase(unsigned tgreaterOrder) 
{
	unsigned numSpeakers = mSpeakerLayout->numSpeakers();
	
	// hard-coded gain coefficients for in phase decoding (parameters specified up to 4th order)
	
	sample inPhaseParameters[][5] = { // n = 0, 1, 2, 3, 4, M = 0, 1, 2, 3, 4 
				{1.0,	1.0,	1.0,	1.0,	1.0},		
				{0.f,	0.333,	0.5,	0.6,	0.667},		
				{0.f,	0.f,	0.1,	0.2,	0.286},		
				{0.f,	0.f,	0.f,	0.029,	0.071},		
				{0.f,	0.f,	0.f,	0.f,	0.008},		
	};
		
	// calculate gain factors for each order
	unsigned M = tgreaterOrder; // the order we are assuming as the context
	
	for (unsigned i = 1; i < mNumChannels; i++) { // skip W channel as its factor is always 1
		for (unsigned l = 0; l < numSpeakers; l++) {
			// channelsToUniformOrder(i)+1 is a convenient way to get n (the order of each ambisonic channel), but it is not defined for W
			mDecodingMatrix[l][i] *= inPhaseParameters[channelsToUniformOrder(i)+1][M];
		}
	}
}


/*******************NEXT_BUFFER METHOD*******************/

// To get my next buffer, get a buffer from the input, and then "process" it...
void AmbisonicDecoder::nextBuffer(Buffer &outputBuffer, unsigned outBufNum) throw(CException) {	

	unsigned numChannels = mNumChannels;
	unsigned numSpeakers = mSpeakerLayout->numSpeakers();
	unsigned numFrames = outputBuffer.mNumFrames;		// block size
	Buffer *inputBuffer;
	
	if (outputBuffer.mNumChannels < numSpeakers) { // USED TO BE numChannels. I think it makes more sense to be numSpeakers.
		logMsg(kLogError, "AmbisonicDecoder needs a buffer with %d channels, found only %d\n", numChannels, outputBuffer.mNumChannels);
		throw RunTimeError("Insufficient number of channels in buffer passed.");
	}
	
	outputBuffer.zeroBuffers(); // fill the output buffers with silence
		
//	inputBuffer = mInputPort->pullInput(numFrames);	
// THE BLOCK BELOW WAS PULL_INPUT OF THE UGEN PORT...
	inputBuffer = mInputPort->mBuffer;				// else get the buffer
	inputBuffer->mNumFrames = numFrames;
	inputBuffer->mType = kSamples;
	mInputPort->mUGen->nextBuffer(*inputBuffer);					// and ask the UGen for nextBuffer()
	inputBuffer->mIsPopulated = true;
	
	
	// since all equations for all speakers & encoded channels are the same, they can be merged into a single loop:
	for (unsigned n = 0; n < numChannels; n++) {  // for each input ambisonic channel to be used
		// get pointers to the encoded input channels (according to the channel indexer to accomodate hybrid orders)
		SampleBuffer inPtr = inputBuffer->buffer(mIOChannelMap[n]);	

		// for each speaker, for each order, for each sample & encoded channel
		for (unsigned l = 0; l < numSpeakers; l++) {
			SampleBuffer outPtr = outputBuffer.buffer(l); // get a pointer to the output channel
			
			// for each frame of the buffer
			for (unsigned i = 0; i < numFrames; i++) {
			
				*outPtr++ += mDecodingMatrix[l][n] * inPtr[i]; 
			}
		}
	}
			
	return;
}
