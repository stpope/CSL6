//
//	HOA_Utilities.cpp -- Higher Order Ambisonic utility classes
//	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
//	Higher Order Ambisonic classes written by Jorge Castellanos, Graham Wakefield, Florian Hollerweger, 2005
//
//	Utility classes used by HOA_Encoder, MOA_Encoder, AmbisonicRotator, MOA_Rotator, HOA_Decoder

#include "AmbisonicUtilities.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

using namespace csl;

////////////////////////////////////////////
/*     AMBISONIC MIXER IMPLEMENTATION     */
////////////////////////////////////////////

/*******************CONSTRUCTORS & DESTRUCTOR*******************/

AmbisonicMixer::AmbisonicMixer(unsigned norder) : AmbisonicUnitGenerator(norder) {
	initialize();
}

AmbisonicMixer::AmbisonicMixer(unsigned hOrder, unsigned vOrder) : AmbisonicUnitGenerator(hOrder, vOrder) {
	initialize();
}

AmbisonicMixer::~AmbisonicMixer() { }


/*******************INITIALIZE METHOD*******************/

void AmbisonicMixer::initialize()
{
	// allocate memory for source signal buffers according to number of channels required at this order
	mInBuffer = new Buffer(mNumChannels, CGestalt::blockSize());
	mInBuffer->allocateBuffers();

#ifdef CSL_DEBUG
	logMsg("Created an AmbisonicMixer with horizontal order = %d, vertical order = %d\n", mOrder.horizontalOrder, mOrder.verticalOrder);
#endif		

}

void AmbisonicMixer::addInput(UnitGenerator &input) {
		
	if (mNumChannels <= input.numChannels()) {  // Make sure the input has at least the number of channels needed for the order specified
		
		mInputs.push_back(&input); // add the new input
	
		// calculate inverse of the number of input sound sources (used for normalization)
		mInvNumInputs = 1.f/(float)mInputs.size();

#ifdef CSL_DEBUG
	logMsg("AmbisonicPanner: added %ith input\n", mInputs.size());
#endif
	} else {
		logMsg(kLogError, "AmbisonicMixer: cannot add input to a mixer of %ix%i order.\n", mOrder.horizontalOrder, mOrder.verticalOrder);
	}
	return;
}

void AmbisonicMixer::addInput(AmbisonicUnitGenerator &input) {
	
	AmbisonicOrder inputOrder = input.order();
	if (inputOrder.horizontalOrder >= mOrder.horizontalOrder && inputOrder.verticalOrder >= mOrder.verticalOrder) {
		mInputs.push_back(&input); // add the new input
	
		// calculate inverse of the number of input sound sources (used for normalization)
		mInvNumInputs = 1.f/(float)mInputs.size();

#ifdef CSL_DEBUG
	logMsg("AmbisonicPanner: added %ith input\n", mInputs.size());
#endif

	} else {
		logMsg(kLogError, "AmbisonicMixer: cannot add input of %ix%i order to a mixer of %ix%i order.\n", inputOrder.horizontalOrder, inputOrder.verticalOrder, mOrder.horizontalOrder, mOrder.verticalOrder);
	}
	return;

}

/*******************NEXT_BUFFER METHOD*******************/

// To get my next buffer, get a buffer from the input, and then "process" it...
void AmbisonicMixer::nextBuffer(Buffer &outputBuffer, unsigned outBufNum) throw(CException) {		
	SampleBuffer outPtr, inPtr;
	unsigned i, c, k, numFrames;
	unsigned numInputs = mInputs.size();
	
	mInBuffer->mNumFrames = numFrames = outputBuffer.mNumFrames;	// block size
	
	outputBuffer.zeroBuffers();			// initialize output buffer with zeros

#ifdef CSL_DEBUG
	logMsg("AmbisonicMixer::nextBuffer");
#endif
		
	if (numInputs) {	// if there are no inputs, then the outputbuffer can remain as zeroes.
		
		if (outputBuffer.mNumChannels < mNumChannels) 
			logMsg(kLogError, "AmbisonicMixer requires %d output channels, found only %d ???\n", mNumChannels, outputBuffer.mNumChannels);
				
		// loop through each input in turn, adding it's next_buffer output onto the outputbuffer channel by channel
		for (i = 0; i < numInputs; i++) {	
			// pointer to the current input framestream
			UnitGenerator * input = mInputs[i];
			
			if (input->isActive()) {		// if active input found, get a buffer and sum it into the output
				
				// fill up mInBuffer with the ambisonic encoded audio from the current input
				input->nextBuffer(*mInBuffer);
				
				for (c = 0; c < mNumChannels; c++) {			// loops through channels
					outPtr = outputBuffer.buffer(c);
					inPtr = mInBuffer->buffer(c);
					for (k = 0; k < numFrames; k++)	// loops through sample buffers
						*outPtr++ += *inPtr++;	
				}
			}
		} // end of input loop
		
		// after mixing, scale down our encoded output by the number of encoded sources to avoid clipping:
		// for each actual audio output channel (not each ambisonic channel of the greater order)...
		for (c = 0; c < mNumChannels; c++) {
			outPtr = outputBuffer.buffer(c); // reset the outPtr to the beginning of this channel in the outputbuffer

			for (i = 0; i < numFrames; i++) // ...scale this channels' output buffer down
				*outPtr++ *= mInvNumInputs; 
		}
	} // end of if (numInputs)
	
	return;
}


////////////////////////////////////////////
/*    AMBISONIC ROTATOR IMPLEMENTATION    */
////////////////////////////////////////////


/*******************CONSTRUCTORS & DESTRUCTOR*******************/
	
AmbisonicRotator::AmbisonicRotator(AmbisonicUnitGenerator &input) : AmbisonicUnitGenerator(input.order()) {
	
	initialize(input);
}

AmbisonicRotator::AmbisonicRotator(UnitGenerator &input, unsigned order) : AmbisonicUnitGenerator(order) {
	
	initialize(input);
}

AmbisonicRotator::AmbisonicRotator(UnitGenerator &input, unsigned horder, unsigned vorder) : AmbisonicUnitGenerator(horder, vorder) {
	
	initialize(input);
}

AmbisonicRotator::~AmbisonicRotator(void) {

	mInputPort->mUGen->removeOutput(this);
	delete mInputPort;

	delete [] mSinAngle;
	delete [] mCosAngle;
	
	delete [] mInputChannelIndex;
	delete [] mChannelIndex;
	
	for (int i = 0; i < mNumChannels; i++) {							// one sample pointer for each ambisonic output channel
		if (mOutPtr[i])
            delete mOutPtr[i];
		if(mInPtr[i])
            delete mInPtr[i];
	}
	
	free(mOutPtr);
	free(mInPtr);
	
}


/*******************INITIALIZE METHOD*******************/

void AmbisonicRotator::initialize(UnitGenerator &input) {
	mInputPort = new Port(&input);
	input.addOutput(this);	// be sure to add me as an output of the other guy

	// are there enough encoded channels in the input to rotate the specified order?
	if (orderToChannels(mOrder) > input.numChannels()) {
		// reduce order if not enough channels in the input ugen
		while (orderToChannels(mOrder) > input.numChannels()) {
			if (mOrder.verticalOrder >= mOrder.horizontalOrder) {
				--mOrder.verticalOrder;
				if (orderToChannels(mOrder) <= input.numChannels())
					break;
			}
			
			--mOrder.horizontalOrder;
		}
		
		logMsg(kLogWarning, "Reducing decoding order because encoded input does not contain enough channels for specified order.\n Input channels:(%d): horizontal order = %i, vertical order = %i\n", 
							input.numChannels(), mOrder.horizontalOrder, mOrder.verticalOrder);
	}

	// recalculate the greater order & the uniformity (overriding values calculated in inherited HOA_AmbisonicFramestream constructor)
	mGreaterOrder = greaterOrder(mOrder);
	mOrder.isUniform = (mOrder.horizontalOrder == mOrder.verticalOrder);
	mNumChannels = orderToChannels(mOrder);		// derive necessary output channels from ambisonic order
	mNumChannelsGreaterOrder = orderToChannels(mGreaterOrder); // channels required for uniform greater order

	mInputChannelIndex = new unsigned[mNumChannelsGreaterOrder];
	channelIndexer(mInputChannelIndex); // get a channel indexer based on the input order
	
	mChannelIndex = new unsigned[mNumChannelsGreaterOrder];
	channelIndexer(mChannelIndex); // get a channel indexer based on the output order (after fixing the mOrder)

	mShouldRotate = mShouldTurn = mShouldTilt = false;		// default initialise to no rotational activity
			
	mOutPtr = (sample**) malloc(mNumChannels * sizeof(sample*));	// Allocating memory for audio output pointers.
	mInPtr = (sample**) malloc(mNumChannels * sizeof(sample*));	// Allocating memory for audio output pointers.
	for (unsigned i = 0; i < mNumChannels; i++) {							// one sample pointer for each ambisonic output channel
		mOutPtr[i] = new sample;
		mInPtr[i] = new sample;
	}

	mSinAngle = new sample[mGreaterOrder];		// create arrays of samples to store sin & cosines of tilt, tumble & rotate angles 
	mCosAngle = new sample[mGreaterOrder];		// used in the rotator DSP loop

#ifdef CSL_DEBUG
	logMsg("Created AmbisonicRotator with horizontal order = %d, vertical order = %d\n", mOrder.horizontalOrder, mOrder.verticalOrder);
#endif
}


/*******************SET_N-TH_INPUT METHOD*******************/

//  function to set framestream sources for rotation, til tand tumble
void AmbisonicRotator::setNthInput(float amount, Axes axis)
{
	// for example, use setNthInput(sine, kTILT);
	switch (axis) {
		case kTILT:
			setTilt(amount);
			break;
		case kTUMBLE: 
			setTumble(amount);
			break;
		case kROTATE:
			setRotate(amount);
			break;
		default:
			logMsg(kLogError, "AmbisonicRotator; Attempted to add control signal to invalid axis channel ???\n");			
	}
}

void AmbisonicRotator::setTilt(float amount) { 
	if (mOrder.isUniform) {
		mTilt = amount;
	if (amount)
		mShouldTilt = true;
	else
		mShouldTilt = false; // if amount == 0 then don't even do the operations. Nothing to be changed!		
	} else {
		logMsg(kLogWarning, "Attempt to set Tilt control for a hybrid order rotator failed.\n");
	}
}
void AmbisonicRotator::setTumble(float amount) { 
	if (mOrder.isUniform) {
		mTumble = amount;
		
		if (amount)
			mShouldTurn = true;
		else
			mShouldTurn = false; // if amount == 0 then don't even do the operations. Nothing to be changed!
	} else {
		logMsg(kLogWarning, "Attempt to set Tumble control for a hybrid order rotator failed.\n");
	}
}
void AmbisonicRotator::setRotate(float amount) { 

	mRotate = amount;

	if (amount)
		mShouldRotate = true;
	else
		mShouldRotate = false; // if amount == 0 then don't even do the operations. Nothing to be changed!

}


/*******************NEXT_BUFFER METHOD*******************/

// Overriding Framestream::nextBuffer(); To get my next buffer, get a buffer from the input, and then "process" it...
void AmbisonicRotator::nextBuffer(Buffer &outputBuffer, unsigned outBufNum) throw(CException) {	
	unsigned numFrames = mNumFrames = outputBuffer.mNumFrames;		// block size
	unsigned numChannels = mNumChannels;
	unsigned greaterOrder = mGreaterOrder;
	unsigned i;		// to be used as the index in several for loops.
	Buffer *inputBuffer;

	
	if (outputBuffer.mNumChannels < numChannels) { // make sure that enough output channels were provided
		logMsg(kLogError, "AmbisonicRotator needs a buffer with %d channels, found only %d\n", numChannels, outputBuffer.mNumChannels);
		throw RunTimeError("Insufficient number of channels in buffer passed.");
	}

//	inputBuffer = mInputPort->pullInput(numFrames);	
// THE BLOCK BELOW WAS PULL_INPUT OF THE UGEN PORT...
	inputBuffer = mInputPort->mBuffer;				// else get the buffer
	inputBuffer->mNumFrames = numFrames;
	inputBuffer->mType = kSamples;
	mInputPort->mUGen->nextBuffer(*inputBuffer);					// and ask the UGen for nextBuffer()
	inputBuffer->mIsPopulated = true;

	// Get a buffer from the input signal
//	_input->nextBuffer(outputBuffer);	
	
	// assign our in & out pointers to their respective channels of input & output buffers
	for (i = 0; i < mNumChannelsGreaterOrder; i++) {
		mOutPtr[i] =  outputBuffer.buffer(mChannelIndex[i]);	
	}

	for (i = 0; i < numChannels; i++) {
		mInPtr[i] =  inputBuffer->buffer(mInputChannelIndex[i]);
	}
	
	//	First, copy the input encoded audio to the output, ready for rotations if required
	for (i = 0; i < numFrames; i++)  //
		for (unsigned j = 0; j < numChannels; j++)	
			*mOutPtr[j]++ = *mInPtr[j]++;

	// do tilt processing if required
	if (mShouldTilt) {	
		//	reset outPtr to the begining of the buffer
		for (i = 0; i < numChannels; i++)
			mOutPtr[i] = outputBuffer.buffer(mChannelIndex[i]);

		// set the sin & cosines of i multiples of the tilt angles
		for (i = 0; i < greaterOrder; i++) {
			mSinAngle[i] = sinf((i+1) * mTilt);
			mCosAngle[i] = cosf((i+1) * mTilt);
		}
		
		// do the actual calculations
		tiltFirstOrder();
		
	}
	
	// do tumble processing if required
	if (mShouldTurn) {
		//	reset outPtr to the begining of the buffer
		for (i = 0; i < numChannels; i++)
			mOutPtr[i] = outputBuffer.buffer(mChannelIndex[i]);

		// set the sin & cosines of i multiples of the tilt angles
		for (i = 0; i < greaterOrder; i++) {
			mSinAngle[i] = sinf((i+1) * mTumble);
			mCosAngle[i] = cosf((i+1) * mTumble);
		}
		
		// do the actual calculations
		tumbleFirstOrder();
		
	}
	
	// do the rotation processing if required
	if (mShouldRotate) {
		//	reset outPtr to the begining of the buffer
		for (i = 0; i < numChannels; i++)
			mOutPtr[i] = outputBuffer.buffer(mChannelIndex[i]);

		// set the sin & cosines of i multiples of the tilt angles
		for (i = 0; i < greaterOrder; i++) {
			mSinAngle[i] = sinf((i+1) * mRotate);
			mCosAngle[i] = cosf((i+1) * mRotate);
		}
			
		// do the actual calculations - checking for hyrbid orders:
		if (mOrder.horizontalOrder)
			rotateFirstOrderHorizontal();
		// note that there is no 1st order vertical calculation necessary for rotation
		if (mOrder.verticalOrder > 1)
			rotateSecondOrderVertical();
	
	}
	
	// done
	return;
	
}

/*******************TILT, TUMBLE, ROTATE METHODS*******************/

void AmbisonicRotator::tiltFirstOrder() {
	SampleBufferVector outPtr = mOutPtr;			// create local copy of output pointer
	sample temp1;
	
	for (unsigned i = 0; i < mNumFrames; i++)   {
		// W channel (0) remains unchanged
		// X channel (1) remains unchanged
		temp1 = *outPtr[2]; // needed to avoid recursive error
		*outPtr[2]++ = mCosAngle[0] * (*outPtr[2]) - mSinAngle[0] * (*outPtr[3]);
		*outPtr[3]++ = mSinAngle[0] * temp1 + mCosAngle[0] * (*outPtr[3]);
		
	}

}

void AmbisonicRotator::tiltSecondOrder()
{
	SampleBufferVector outPtr = mOutPtr;			// create local copy of output pointer
	sample temp1, temp2, temp3; // needed to avoid recursive error
	
	for (unsigned i = 0; i < mNumFrames; i++)  {
		temp1 = *outPtr[4]; // needed to avoid recursive error
		temp2 = *outPtr[5];
		temp3 = *outPtr[7];

		*outPtr[4]++ = 0.5 * (mCosAngle[1] - 1) * (*outPtr[8]) + 0.5 * mSinAngle[1] * (*outPtr[7]) + 0.25 * (mCosAngle[1] + 3) * (*outPtr[4]);
		*outPtr[5]++ = -mSinAngle[0]  * (*outPtr[6]) + mCosAngle[0]  * (*outPtr[5]);
		*outPtr[6]++ = mCosAngle[0]  * (*outPtr[6]) + mSinAngle[0]  * temp2;
		*outPtr[7]++ = -mSinAngle[1]  * (*outPtr[8]) + mCosAngle[1]  * (*outPtr[7]) - (0.5 * mSinAngle[1])  * temp1;
		*outPtr[8]++ = 0.25 * (1 + 3 * mCosAngle[1]) * (*outPtr[8]) + 0.75 * mSinAngle[1] * temp3 + 0.375 * (mCosAngle[1] - 1) * temp1;
		
	}

	if (mGreaterOrder > 2) tiltThirdOrder();
	
}

void AmbisonicRotator::tiltThirdOrder()
{
	SampleBufferVector outPtr = mOutPtr;			// create local copy of output pointer
	sample temp1, temp2, temp3, temp4, temp5;	
	
	for (unsigned i = 0; i < mNumFrames; i++) {
		// temporaries needed to avoid recursive error
		temp1 = *outPtr[9];
		temp2 = *outPtr[10];
		temp3 = *outPtr[11];
		temp4 = *outPtr[12];
		temp5 = *outPtr[14];
		
		// The source for the 3rd order tilt matrix is Zmoelnig's thesis, taking into consideration his different channel naming convention!
		*outPtr[9]++  = 0.125 * (3 * mCosAngle[1] + 5) * (*outPtr[9]) + 1.5 * mSinAngle[1] * (*outPtr[12])	+ 0.515625 * (mCosAngle[1] - 1) * (*outPtr[13]);																														 // P channel
		*outPtr[10]++ = 0.0625 * (15 * mCosAngle[0] + mCosAngle[2]) * (*outPtr[10]) - 0.375 * (5 * mSinAngle[0] + mSinAngle[2]) * (*outPtr[11]) - 0.2578125 * (mCosAngle[0] - mCosAngle[2]) * (*outPtr[14]) + 0.25 * (3 * mSinAngle[0] - mSinAngle[2])  * (*outPtr[15]);			 // Q channel
		*outPtr[11]++ = 0.0625 * (5 * mSinAngle[0] + mSinAngle[2]) * temp2 + 0.125 * (5 * mCosAngle[0] + 3 * mCosAngle[2]) * (*outPtr[11]) + 0.0859375 * (- mSinAngle[0] + 3 * mSinAngle[2]) * (*outPtr[14]) + 0.25 * (- mCosAngle[0] + mCosAngle[2]) * (*outPtr[15]);			 // N channel
		*outPtr[12]++ = - 0.25 * mSinAngle[1] * temp1 + mCosAngle[1] * (*outPtr[12]) - 0.34375 * mSinAngle[1] * (*outPtr[13]);																																					 // O channel
		*outPtr[13]++ = 0.454545454545 * (mCosAngle[1] - 1) * temp1 +  + 1.818181818182 * mSinAngle[1] * temp4 + 0.125 * (3 + 5 * mCosAngle[1]) * (*outPtr[13]);																													 // L channel
		*outPtr[14]++ = 0.227272727273 * (- mCosAngle[0] + mCosAngle[2]) * temp2 + 0.454545454545 * (mSinAngle[0] - 3 * mSinAngle[2]) * temp3 + 0.0625 * (mCosAngle[0] + 15 * mCosAngle[2]) * (*outPtr[14]) - 0.181818181818 * (mSinAngle[0] + 5 * mSinAngle[2]) * (*outPtr[15]); // M channel
		*outPtr[15]++ = 0.15625 * (- 3 * mSinAngle[0] + mSinAngle[2]) * temp2 + 0.9375 * (- mCosAngle[0] + mCosAngle[2]) * temp3 + 0.12890625 * (mSinAngle[0] + 5 * mSinAngle[2]) * temp5 + 0.125 * (3 * mCosAngle[0] + 5 * mCosAngle[2]) * (*outPtr[15]);						 // K channel
		
	}
	// if (mOrder.greaterOrder > 3) tiltFourthOrder();
}


void AmbisonicRotator::tumbleFirstOrder() {
	SampleBufferVector outPtr = mOutPtr;			// create local copy of output pointer
	sample temp1;
	
	for (unsigned i = 0; i < mNumFrames; i++) {
		temp1 = *outPtr[1]; // needed to avoid recursive error
		// W channel (0) remains unchanged
		*outPtr[1]++ = mCosAngle[0] * (*outPtr[1]) - mSinAngle[0] * (*outPtr[3]);
		// Y channel (2) remains unchanged
		*outPtr[3]++ = mSinAngle[0] * temp1 + mCosAngle[0] * (*outPtr[3]);
	}
}

void AmbisonicRotator::tumbleSecondOrder() {

	SampleBufferVector outPtr = mOutPtr;			// create local copy of output pointer
	sample temp1, temp2, temp3;

	for (unsigned i = 0; i < mNumFrames; i++) {
		// temporaries needed to avoid recursive error
		temp1 = *outPtr[4];
		temp2 = *outPtr[5];
		temp3 = *outPtr[6];

		*outPtr[4]++ = 0.5 * (1 - mCosAngle[1]) * (*outPtr[8]) - 0.5 * mSinAngle[1] * (*outPtr[6]) + 0.25 * (3 + mCosAngle[1]) * (*outPtr[5]);
		*outPtr[5]++ = -mSinAngle[0]  * (*outPtr[7]) + mCosAngle[0]  * (*outPtr[5]);
		*outPtr[6]++ = -mSinAngle[1]  * (*outPtr[8]) + mCosAngle[1]  * (*outPtr[6]) + 0.5 * mSinAngle[1] * temp1;
		*outPtr[7]++ = mCosAngle[0]  * (*outPtr[7]) + mSinAngle[0]  * temp2;
		*outPtr[8]++ = 0.25 * (1 + 3 * mCosAngle[1]) * (*outPtr[8]) + 0.75 * mSinAngle[1] * temp3 + 0.375 * (1 - mCosAngle[1]) * temp1;
	}
	if (mGreaterOrder > 2) tumbleThirdOrder();
}

void AmbisonicRotator::tumbleThirdOrder()
{
	SampleBufferVector outPtr = mOutPtr;			// create local copy of output pointer
	sample temp1, temp2, temp3, temp4, temp5;
	
	for (unsigned i = 0; i < mNumFrames; i++) {
		// temporaries needed to avoid recursive error
		temp1 = *outPtr[9];
		temp2 = *outPtr[10];
		temp3 = *outPtr[11];
		temp4 = *outPtr[12];
		temp5 = *outPtr[13];
		
		// The source for the 3rd order tumble matrix is Zmoelnig's thesis, taking into consideration his different channel naming convention!
		*outPtr[9]++  = 0.0625 * (15 * mCosAngle[0] + mCosAngle[2]) * (*outPtr[9]) - 0.375 * (5 * mSinAngle[0] + mSinAngle[2]) * (*outPtr[11]) + 0.2578125 * (mCosAngle[0] - mCosAngle[2]) * (*outPtr[13]) + 0.25 * (- 3 * mSinAngle[0] + mSinAngle[2]) * (*outPtr[15]);		 // P channel
		*outPtr[10]++ = 0.125 * (5 + 3 * mCosAngle[1]) * (*outPtr[10]) - 1.5 * mSinAngle[1] * (*outPtr[12]) + 0.515625 * (1 - mCosAngle[1]) * (*outPtr[14]);																														 // Q channel
		*outPtr[11]++ = 0.0625 * (5 * mSinAngle[0] + mSinAngle[2]) * temp1 + 0.125 * (5 * mCosAngle[0] + 3 * mCosAngle[2]) * (*outPtr[11])+ 0.0859375 * (mSinAngle[0] - 3 * mSinAngle[2]) * (*outPtr[13]) + 0.25 * (mCosAngle[0] - mCosAngle[2]) * (*outPtr[15]);				 // N channel
		*outPtr[12]++ = 0.25 * mSinAngle[1] * temp2 + mCosAngle[1] * (*outPtr[12]) - 0.34375 * mSinAngle[1] * (*outPtr[14]);																																						 // O channel
		*outPtr[13]++ = 0.227272727273 * (mCosAngle[0] - mCosAngle[2]) * temp1 + 0.454545454545 * (- mSinAngle[0] + 3 * mSinAngle[2]) * temp3 + 0.0625 * (mCosAngle[0] + 15 * mCosAngle[2]) * (*outPtr[13]) - 0.181818181818 * (mSinAngle[0] + 5 * mSinAngle[2]) * (*outPtr[15]); // L channel
		*outPtr[14]++ = 0.454545454545 * (1 - mCosAngle[1]) * temp2 + 1.818181818182 * mSinAngle[1] * temp4 + 0.125 * (3 + 5 * mCosAngle[1]) * (*outPtr[14]);																														 // M channel
		*outPtr[15]++ = 0.15625 * (3 * mSinAngle[0] - mSinAngle[2]) * temp1 + 0.9375 * (mCosAngle[0] - mCosAngle[2]) * temp3 + 0.12890625 * (mSinAngle[0] + 5 * mSinAngle[2]) * temp5 + 0.125 * (3 * mCosAngle[0] + 5 * mCosAngle[2]) * (*outPtr[15]);							 // K channel
								
	}
	//if (mOrder.greaterOrder > 3) tumbleFourthOrder();
}



void AmbisonicRotator::rotateFirstOrderHorizontal()
{
	SampleBufferVector outPtr = mOutPtr;			// create local copy of output pointer
	sample temp1;
	
	for (unsigned i = 0; i < mNumFrames; i++) {
		temp1 = *outPtr[1]; // needed to avoid recursive error
		// W channel (0) remains unchanged
		*outPtr[1]++ = mCosAngle[0] * (*outPtr[1]) - mSinAngle[0] * (*outPtr[2]);
		*outPtr[2]++ = mSinAngle[0] * temp1 + mCosAngle[0] * (*outPtr[2]);
		// Z channel (3) remains unchanged		
	}

	if (mOrder.horizontalOrder > 1) rotateSecondOrderHorizontal();
}

// note that 1st order vertical for rotation is not required

void AmbisonicRotator::rotateSecondOrderHorizontal() 
{
	SampleBufferVector outPtr = mOutPtr;			// create local copy of output pointer
	sample temp1;
	
	for (unsigned i = 0; i < mNumFrames; i++) {
		temp1 = *outPtr[4]; // needed to avoid recursive error
		*outPtr[4]++ = mCosAngle[1] * (*outPtr[4]) - mSinAngle[1] * (*outPtr[5]);  // U Channel
		*outPtr[5]++ = mSinAngle[1] * temp1 + mCosAngle[1] * (*outPtr[5]);			// V Channel
	}
	if (mOrder.horizontalOrder > 2) rotateThirdOrderHorizontal();
}

void AmbisonicRotator::rotateSecondOrderVertical() 
{
	SampleBufferVector outPtr = mOutPtr;			// create local copy of output pointer
	sample temp1;
	
	for (unsigned i = 0; i < mNumFrames; i++) {
		temp1 = *outPtr[6]; // needed to avoid recursive error
		*outPtr[6]++ = mCosAngle[0] * (*outPtr[6]) - mSinAngle[0] * (*outPtr[7]);	// S channel
		*outPtr[7]++ = mSinAngle[0] * temp1 + mCosAngle[0] * (*outPtr[7]);			// T channel
		// R channel (8) remains unchanged
	}
	if (mOrder.verticalOrder > 2) rotateThirdOrderVertical();
}

void AmbisonicRotator::rotateThirdOrderHorizontal() 
{
	SampleBufferVector outPtr = mOutPtr;			// create local copy of output pointer
	sample temp1;
	
	for (unsigned i = 0; i < mNumFrames; i++) {
		temp1 = *outPtr[9]; // needed to avoid recursive error
		
		// The source for the 3rd order tilt matrix is Zmoelnig's thesis, taking into consideration his different channel naming convention!
		// This matrix seems to be different from the one that Malham specifies in his thesis!
		*outPtr[9]++  = mCosAngle[2] * (*outPtr[9]) - mSinAngle[2] * (*outPtr[10]); // P channel
		*outPtr[10]++ = mSinAngle[2] * temp1 + mCosAngle[2] * (*outPtr[10]); // Q channel
	}
	// if (mOrder.horizontalOrder > 3) rotateFourthOrderHorizontal();
}

void AmbisonicRotator::rotateThirdOrderVertical() 
{
	SampleBufferVector outPtr = mOutPtr;			// create local copy of output pointer
	sample temp2, temp3;
	
	for (unsigned i = 0; i < mNumFrames; i++) {
		temp2 = *outPtr[11];// needed to avoid recursive error
		temp3 = *outPtr[13];
		
		// The source for the 3rd order tilt matrix is Zmoelnig's thesis, taking into consideration his different channel naming convention!
		// This matrix seems to be different from the one that Malham specifies in his thesis!
		*outPtr[11]++ = mCosAngle[1] * (*outPtr[11]) - mSinAngle[1] * (*outPtr[12]); // N channel
		*outPtr[12]++ = mSinAngle[1] * temp2 + mCosAngle[1] * (*mOutPtr[12]); // O channel
		*outPtr[13]++ = mCosAngle[0] * (*outPtr[13]) - mSinAngle[0] * (*outPtr[14]); // L channel
		*outPtr[14]++ = mSinAngle[0] * temp3 + mCosAngle[0] * (*outPtr[14]); // M channel
		// K channel (15) remains unchanged
	}
	// if (mOrder.verticalOrder > 3) rotateFourthOrderVertical();
}



/*******************MATRIX INVERSION UTILITY FUNCTION*******************/

// code based on the matrix library found at: http://home1.gte.net/edwin2/Matrix/

/*
PYTHAG computes sqrt(a^{2} + b^{2}) without destructive overflow or underflow.
*/
static double at, bt, ct;
#define PYTHAG(a, b) ((at = fabs(a)) > (bt = fabs(b)) ? \
(ct = bt/at, at*sqrt(1.0+ct*ct)): (bt ? (ct = at/bt, bt*sqrt(1.0+ct*ct)): 0.0))

static double maxarg1, maxarg2;
#define MAX(a, b) (maxarg1 = (a), maxarg2 = (b), (maxarg1) > (maxarg2) ? \
(maxarg1) : (maxarg2))

#define SIGN(a, b) ((b) < 0.0 ? -fabs(a): fabs(a))

/** \relates AmbisonicUnitGenerator
Given a matrix a[m][n], this routine computes its singular value
decomposition, A = U*W*V^{T}.  The matrix U replaces a on output.
The diagonal matrix of singular values W is output as a vector w[n].
The matrix V (not the transpose V^{T}) is output as v[n][n].
m must be greater or equal to n;  if it is smaller, then a should be
filled up to square with zero rows.
*/
void csl::singularValueDecomposition(sample** a, int m, int n, sample* w, sample** v)
{
    int flag, i, its, j, jj, k, l, nm;
    double c, f, h, s, x, y, z;
    double anorm = 0.0, g = 0.0, scale = 0.0;

   // if (m < n)
//      error("SingularValueDecomposition: Matrix A must be augmented with extra rows of zeros.");
    double* rv1 = new double [n];

    /* Householder reduction to bidiagonal form.			*/
    for (i = 0; i < n; i++) {
		l = i + 1;
        rv1[i] = scale*g;
        g = s = scale = 0.0;
        if (i < m) {
			for (k = i; k < m; k++)
			scale += fabs(a[k][i]);
			if (scale) {
				for (k = i; k < m; k++) {
					a[k][i] /= scale;
					s += a[k][i]*a[k][i];
				};
				f = a[i][i];
				g = -SIGN(sqrt(s), f);
				h = f*g - s;
				a[i][i] = f - g;
				if (i != n - 1) {
					for (j = l; j < n; j++) {
						for (s  = 0.0, k = i; k < m; k++)
							s += a[k][i]*a[k][j];
						f = s/h;
						for (k = i; k < m; k++)
							a[k][j] += f*a[k][i];
					};
				};
				for (k = i; k < m; k++)
					a[k][i] *= scale;
			};
		};
		w[i] = scale*g;
        g = s= scale = 0.0;
        if (i < m && i != n - 1) {
			for (k = l; k < n; k++)
				scale += fabs(a[i][k]);
			if (scale) {
				for (k = l; k < n; k++) {
					a[i][k] /= scale;
					s += a[i][k]*a[i][k];
				};
				f = a[i][l];
				g = -SIGN(sqrt(s), f);
				h = f*g - s;
				a[i][l] = f - g;
				for (k = l; k < n; k++)
					rv1[k] = a[i][k]/h;
				if (i != m - 1) {
					for (j = l; j < m; j++) {
						for (s = 0.0, k = l; k < n; k++)
							s += a[j][k]*a[i][k];
						for (k = l; k < n; k++)
							a[j][k] += s*rv1[k];
					};
				};
				for (k = l; k < n; k++)
					a[i][k] *= scale;
			};
		};
        anorm = MAX(anorm, (fabs(w[i]) + fabs(rv1[i])));
	};
    /* Accumulation of right-hand transformations.			*/
    for (i = n - 1; 0 <= i; i--)
      {
	if (i < n - 1)
	  {
	    if (g)
	      {
		for (j = l; j < n; j++)
		  v[j][i] = (a[i][j]/a[i][l])/g;
		  /* Double division to avoid possible underflow:	*/
		for (j = l; j < n; j++)
		  {
		    for (s = 0.0, k = l; k < n; k++)
		      s += a[i][k]*v[k][j];
		    for (k = l; k < n; k++)
		      v[k][j] += s*v[k][i];
		  };
	      };
	    for (j = l; j < n; j++)
	      v[i][j] = v[j][i] = 0.0;
	  };
	v[i][i] = 1.0;
	g = rv1[i];
	l = i;
      };
    /* Accumulation of left-hand transformations.			*/
    for (i = n - 1; 0 <= i; i--)
      {
	l = i + 1;
	g = w[i];
	if (i < n - 1)
	  for (j = l; j < n; j++)
	    a[i][j] = 0.0;
	if (g)
	  {
	    g = 1.0/g;
	    if (i != n - 1)
	      {
		for (j = l; j < n; j++)
		  {
		    for (s = 0.0, k = l; k < m; k++)
		      s += a[k][i]*a[k][j];
		    f = (s/a[i][i])*g;
		    for (k = i; k < m; k++)
		      a[k][j] += f*a[k][i];
		  };
	       };
	    for (j = i; j < m; j++)
	      a[j][i] *= g;
	  }
	else
	  for (j = i; j < m; j++)
	    a[j][i] = 0.0;
	++a[i][i];
      };
    /* Diagonalization of the bidiagonal form.				*/
    for (k = n - 1; 0 <= k; k--)	/* Loop over singular values.	*/
      {
	for (its = 0; its < 30; its++)	/* Loop over allowed iterations.*/
	  {
	    flag = 1;
	    for (l = k; 0 <= l; l--)	/* Test for splitting:		*/
	      {
		nm = l - 1;		/* Note that rv1[0] is always zero.*/
		if (fabs(rv1[l]) + anorm == anorm)
		  {
		    flag = 0;
		    break;
		  };
		if (fabs(w[nm]) + anorm == anorm)
		  break;
	      };
	    if (flag)
	      {
		c = 0.0;		/* Cancellation of rv1[l], if l>0:*/
		s = 1.0;
		for (i = l; i <= k; i++) {
		    f = s*rv1[i];
		    if (fabs(f) + anorm != anorm)
		      {
			g = w[i];
			h = PYTHAG(f, g);
			w[i] = h;
			h = 1.0/h;
			c = g*h;
			s = (-f*h);
			for (j = 0; j < m; j++)
			  {
			    y = a[j][nm];
			    z = a[j][i];
			    a[j][nm] = y*c + z*s;
			    a[j][i]  = z*c - y*s;
			  };
		      };
		  };
	      };
	    z = w[k];
	    if (l == k)		/* Convergence.				*/
	      {
		if (z < 0.0)	/* Singular value is made non-negative.	*/
		  {
		    w[k] = -z;
		    for (j = 0; j < n; j++)
		      v[j][k] = (-v[j][k]);
		  };
		break;
	      };
//	    if (its == 29)
//	      error("No convergence in 30 SingularValueDecomposition iterations.");
	    x = w[l];		/* Shift from bottom 2-by-2 minor.	*/
	    nm = k - 1;
	    y = w[nm];
	    g = rv1[nm];
	    h = rv1[k];
	    f = ((y - z)*(y + z) + (g - h)*(g + h))/(2.0*h*y);
	    g = PYTHAG(f, 1.0);
	    f = ((x - z)*(x + z) + h*((y/(f + SIGN(g, f))) - h))/x;
	    /* Next QR transformation:					*/
	    c = s = 1.0;
	    for (j = l; j <= nm; j++)
	      {
		i = j + 1;
		g = rv1[i];
		y = w[i];
		h = s*g;
		g = c*g;
		z = PYTHAG(f, h);
		rv1[j] = z;
		c = f/z;
		s = h/z;
		f = x*c + g*s;
		g = g*c - x*s;
		h = y*s;
		y = y*c;
		for (jj = 0; jj < n;  jj++)
		  {
		    x = v[jj][j];
		    z = v[jj][i];
		    v[jj][j] = x*c + z*s;
		    v[jj][i] = z*c - x*s;
		  };
		z = PYTHAG(f, h);
		w[j] = z;	/* Rotation can be arbitrary if z = 0.	*/
		if (z)
		  {
		    z = 1.0/z;
		    c = f*z;
		    s = h*z;
		  };
		f = (c*g) + (s*y);
		x = (c*y) - (s*g);
		for (jj = 0; jj < m; jj++)
		  {
		    y = a[jj][j];
		    z = a[jj][i];
		    a[jj][j] = y*c + z*s;
		    a[jj][i] = z*c - y*s;
		  };
	      };
	    rv1[l] = 0.0;
	    rv1[k] = f;
	    w[k] = x;
	  };
      };
    delete [] rv1;
  }

/// \relates AmbisonicUnitGenerator
void csl::fumaEncodingWeights(SampleBuffer weights, const AmbisonicOrder &order, float azimuth, float elevation) {

	float x,y,z,x2,y2,z2;
	// assume default location of 0,0, i.e directly in front on the plane
	x= x2 = 1.f;
	y = z = y2 = z2 = 0.f;
	
	unsigned h = order.horizontalOrder;
	unsigned v = order.verticalOrder;
	unsigned channel = 1;
	
	// skip this line, do it in the initialization of weights instead, because it never changes
	//weights[0] = AMBI_INVSQRT2;	// W channel, shouldn't it be already defined?
	
	if (h > 0) {
		float cosel = cosf(elevation);
		x = cosf(azimuth) * cosel;
		y = sinf(azimuth) * cosel;
		
		weights[channel++] = x;	// X = cos(A)cos(E)	
		weights[channel++] = y;	// Y = sin(A)cos(E)
		
	}
		
	if (v > 0) {
		z = sinf(elevation);
		weights[channel++] = z;	// Z = sin(E)
	}
	
	if (h > 1) {
		x2 = x*x;
		y2 = y*y;

		weights[channel++] = x2 - y2;	// U = cos(2A)cos2(E) = xx-yy
		weights[channel++] = 2.f * x * y;	// V = sin(2A)cos2(E) = 2xy
		
	}

	if (v > 1) {
		z2 = z*z;
		
		weights[channel++] = 2.f * z * x;	// S = cos(A)sin(2E) = 2zx
		weights[channel++] = 2.f * z * y;	// T = sin(A)sin(2E) = 2yz
		weights[channel++] = (1.5f * z2) - 0.5f;	// R = 1.5sin2(E)-0.5 = 1.5zz-0.5
		
	}	

	if (h > 2) {
		weights[channel++] = x * (x2 - 3.f*y2);	// P = cos(3A)cos3(E) = X(X2-3Y2)
		weights[channel++] = y * (y2 - 3.f*x2);	// Q = sin(3A)cos3(E) = Y(3X2-Y2)
	}

	if (v > 2) {
		float pre = 8.f/11.f;
		
		weights[channel++] = z * (x2-y2) * 0.5f;	// N = cos(2A)sin(E)cos2(E) = Z(X2-Y2)/2
		weights[channel++] = x * y * z;	// O = sin(2A)sin(E)cos2(E) = XYZ
		weights[channel++] = pre * x * (5.f*z2 - 1.f);	// L = 8cos(A)cos(E)(5sin2(E) - 1)/11 = 8X(5Z2-1)/11
		weights[channel++] = pre * y * (5.f*z2 - 1.f);	// M = 8sin(A)cos(E)(5sin2(E) - 1)/11 = 8Y(5Z2-1)/11
		weights[channel++] = z * 0.5 * (5.f*z2 - 3.f); // K = sin(E)(5sin2(E) - 3)/2 = Z(5Z2-3)/2
	}

}

/// \relates AmbisonicUnitGenerator
void csl::fumaIndexedEncodingWeights(SampleBuffer weights, const AmbisonicOrder &order, sample &azimuth, sample &elevation)
{
	float x,y,z,x2,y2,z2;
	// assume default location of 0,0, i.e directly in front on the plane
	x= x2 = 1.f;
	y= z = y2 = z2 = 0.f;
	
	
	unsigned h = order.horizontalOrder;
	unsigned v = order.verticalOrder;
	
	// skip this line, do it in the initialization of weights instead, because it never changes
	//weights[0] = AMBI_INVSQRT2;	// W channel, shouldn't it be already defined?
	
	if (h > 0) {
		float cosel = cosf(elevation);
		x = cosf(azimuth) * cosel;
		y = sinf(azimuth) * cosel;
		
		weights[1] = x;	// X = cos(A)cos(E)	
		weights[2] = y;	// Y = sin(A)cos(E)
		
	}
		
	if (v > 0) {
		z = sinf(elevation);
		weights[3] = z;	// Z = sin(E)
	}
	
	if (h > 1) {
		x2 = x*x;
		y2 = y*y;

		weights[4] = x2 - y2;	// U = cos(2A)cos2(E) = xx-yy
		weights[5] = 2.f * x * y;	// V = sin(2A)cos2(E) = 2xy
		
		if (h > 2) {
			weights[9] = x * (x2 - 3.f*y2);	// P = cos(3A)cos3(E) = X(X2-3Y2)
			weights[10]= y * (y2 - 3.f*x2);	// Q = sin(3A)cos3(E) = Y(3X2-Y2)
		}
	}
	
	if (v > 1) {
		z2 = z*z;
		
		weights[6] = 2.f * z * x;	// S = cos(A)sin(2E) = 2zx
		weights[7] = 2.f * z * y;	// T = sin(A)sin(2E) = 2yz
		weights[8] = (1.5f * z2) - 0.5f;	// R = 1.5sin2(E)-0.5 = 1.5zz-0.5
		
		if (v > 2) {
			float pre = 8.f/11.f;
			
			weights[11] = z * (x2-y2) * 0.5f;	// N = cos(2A)sin(E)cos2(E) = Z(X2-Y2)/2
			weights[12] = x * y * z;	// O = sin(2A)sin(E)cos2(E) = XYZ
			weights[13] = pre * x * (5.f*z2 - 1.f);	// L = 8cos(A)cos(E)(5sin2(E) - 1)/11 = 8X(5Z2-1)/11
			weights[14] = pre * y * (5.f*z2 - 1.f);	// M = 8sin(A)cos(E)(5sin2(E) - 1)/11 = 8Y(5Z2-1)/11
			weights[15] = z * 0.5 * (5.f*z2 - 3.f); // K = sin(E)(5sin2(E) - 3)/2 = Z(5Z2-3)/2
		}
	}
}  
