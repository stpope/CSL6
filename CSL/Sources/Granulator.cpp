//
//  Granulator.h -- CSL class for doing granular synthesis
//
//	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
//

#include "Granulator.h"

using namespace csl;
using namespace std;

// GrainPlayer implementation - pretty simple

GrainPlayer::GrainPlayer(GrainCloud * cloud) : UnitGenerator(), mCloud(cloud) {
	mNumChannels = 2;					// I'm always stereo
}

GrainPlayer::~GrainPlayer() {
	mCloud->isPlaying = false;
}

// The nextBuffer method does all the work of the synthesis, looping through the playing grains

void GrainPlayer::nextBuffer(Buffer & outputBuffer) throw (CException) {
	sample * out1;						// assumes stereo output -- ToDo: fix me
	sample * out2;
	int index, length;
	sample samp, env;
	float durHalf;
	unsigned numFrames = outputBuffer.mNumFrames;
	out1 = outputBuffer.buffer(0);
	out2 = outputBuffer.buffer(1);
	memset(out1, 0, numFrames * sizeof(sample));
	memset(out2, 0, numFrames * sizeof(sample));
		
	while (mCloud->gState != kFree) {	// wait until done creating grains in other thread
//		printf("!\n");
		csl::sleepUsec(100);
	}
	mCloud->gState = kDSP;				// set flag to keep scheduler from running while I'm calculating samples
										// grain loop
	for (Grain * curGrain = mCloud->mPlayingGrains; curGrain != 0; 
			curGrain = curGrain->nextGrain) {
//		printf(".");
		out1 = outputBuffer.buffer(0);							// assume stereo output
		out2 = outputBuffer.buffer(1);
		length = curGrain->numSamples;
		sample * sPtr = curGrain->samples;
		for (unsigned i = 0; i < numFrames; i++) {					// sample loop
			if (curGrain->time >= curGrain->duration)				// if grain has ended already
				break;
			if (curGrain->delay) {									// if grain hasn't started yet
				curGrain->delay--;
				continue;
			}
			index = (int)(curGrain->position);						// get index and wrap it
			while (index < 0)
				index += length;
			while (index >= length)
				index -= length;
			samp = sPtr[index];										// get sample value (should interpolate samples)
			durHalf = curGrain->duration * curGrain->env;			// really cheap triangle envelope
			if (curGrain->time < durHalf)							// if before middle
				env = (float) curGrain->time / durHalf;	
			else													// else after middle
				env = (float)(curGrain->duration - curGrain->time) 
						/ (curGrain->duration - durHalf);		
			samp *= curGrain->amplitude * env;						// scale by envelope
																	// Add to the current output samples
			*out1++ += samp * (1.0f - curGrain->pan);				// write to Left channel 
			*out2++ += samp * curGrain->pan;						// write to Right channel

			curGrain->position += curGrain->rate;					// Update position
			curGrain->time += 1.0f;									// Update time
		}
	}
	mCloud->gNow = C_TIME;											// increment time
	mCloud->gState = kFree;											// release lock
//	printf("\n");
}

// GrainCloud implementation -- grain mgmnt loops are forked as a separate threads

// Constructor allocates global pool of MAXGRAINS available grains

GrainCloud::GrainCloud() {
	Grain * newGrain;
	Grain * prevGrain;
	SAFE_MALLOC(prevGrain, Grain, sizeof(Grain));
	mSilentGrains = prevGrain;		// first in list
									// allocate a list of passive grains
	for (unsigned i = 0; i < MAXGRAINS - 1; i++) {
		SAFE_MALLOC(newGrain, Grain, sizeof(Grain));
		prevGrain->nextGrain = newGrain;
		prevGrain = newGrain;
	}
	prevGrain->nextGrain = 0;		// set last grain's Next to 0;
	unsigned count = 0;				// testing: count silent grains
	for (newGrain = mSilentGrains; newGrain != 0; newGrain = newGrain->nextGrain)
		count++;
	logMsg("Create grain lists: %d available", count);
	mPlayingGrains = 0;				// empty list of active grains
									// now set up threads
	spawnerThread = CThread::MakeThread();
	reaperThread = CThread::MakeThread();
	threadOn = false; 
	isPlaying = false;
	gState = kFree;
	mSamples = 0;
}

// stop threads and delete lists

GrainCloud::~GrainCloud() {
	spawnerThread->stopThread(100);
	reaperThread->stopThread(100);
	csl::sleepMsec(150);
}

// Grain creation loop: take the head item of silentGrains, fill it in based on the random ranges,
// and put it at the head of playingGrains

void * createGrains(void * theArg) {
	GrainCloud * cloud = (GrainCloud * ) theArg;
	Grain * newGrain;
	logMsg("Starting grain creation loop");
//	printf("\tG: ADDR  \t\tDUR     \tPOS    \tRATE\tPAN\n");
	while (true) {
		if ( ! cloud->isPlaying) {
			logMsg("Grain creation loop exits");
			return NULL;							// exit thread
		}
		while (cloud->gState != kFree)				// wait until done creating samples in other thread
			csl::sleepMsec(1);
		cloud->gState = kSched;						// set flag to keep from calculating samples while I'm creating grains
		
		if (cloud->isPlaying) {
			newGrain = cloud->mSilentGrains;		// take the first silent grain and put it into the temp variable
			if (newGrain == 0) {					// if no free grains, wait a bit
				printf("\tNo free grains\n");
//				cloud->reset();						// KLUDJ -- reset all if none free
				goto next;
			}
			cloud->mSilentGrains = newGrain->nextGrain;	// set the temp variable's Next to the head of silentGrains
													// fill it in from the GUI object's ranges
			newGrain->position = fRandB(cloud->mOffsetBase, cloud->mOffsetRange) * cloud->numSamples;
			newGrain->duration = fRandB(cloud->mDurationBase, cloud->mDurationRange) * CGestalt::frameRate();
			newGrain->rate = fRandB(cloud->mRateBase, cloud->mRateRange);
			newGrain->amplitude = fRandB(cloud->mVolumeBase, cloud->mVolumeRange) / cloud->mDensityBase;
			newGrain->time = 0.0f;
			newGrain->pan = fRandB(cloud->mWidthBase, cloud->mWidthRange);
			newGrain->env = fRandB(cloud->mEnvelopeBase, cloud->mEnvelopeRange);
													// delay within the next callback
			newGrain->delay = (unsigned) ((float) (C_TIME - cloud->gNow) * cloud->sampsPerTick);	
			newGrain->samples = cloud->mSamples;
			newGrain->numSamples = cloud->numSamples;
			newGrain->nextGrain = cloud->mPlayingGrains;	// set the temp variable's Next to the current head of playingGrains
			cloud->mPlayingGrains = newGrain;		// set the head of playingGrains to the temp variable
//			printf("\tG: %x\t\t%5.1f  \t%7.2f  \t%4.2f  \t%4.2f\n", 
//					newGrain, newGrain->duration, newGrain->position, newGrain->rate, newGrain->pan);
		} else {
			logMsg("Grain creation loop exits 2");
			return NULL;							// exit loop
		}

next:	cloud->gState = kFree;						// sleep for the inter-grain delay (may not be absolutely accurate)
		csl::sleepUsec(1000000.0f / fRandB(cloud->mDensityBase, cloud->mDensityRange));
	}
	return NULL;									// never reached
}

// Clean up completed grains, taking them off of mPlayingGrains and returning them to mSilentGrains

void * reapGrains(void *theArg) {
	GrainCloud * cloud = (GrainCloud * ) theArg;
	Grain *prevGrain, *curGrain, *nextGrain;
	
	logMsg("Starting grain culling loop");
	csl::sleepMsec(200);							// sleep a bit before starting
	while (true) {
		if ( ! cloud->isPlaying) {
			logMsg("Grain culling loop exits");
			return NULL;							// exit thread
		}
		while (cloud->gState != kFree)				// wait until done creating samples in other thread
			csl::sleepMsec(10);
		cloud->gState = kSched;						// set flag to keep from calculating samples while I'm creating grains
		if (cloud->isPlaying) {
			prevGrain = 0;
			curGrain = cloud->mPlayingGrains;
			while (curGrain != 0) {
				if (curGrain->time >= curGrain->duration) {				// if the position is at the end, free the grain
					nextGrain = curGrain->nextGrain;
//					printf("\tK: %x -> %x\n", curGrain, nextGrain);
					if (prevGrain == 0)									// If it's the first one
						cloud->mPlayingGrains = nextGrain;
					else												// else there is a grain before this one
						prevGrain->nextGrain = nextGrain;
					curGrain->nextGrain = cloud->mSilentGrains;			// put the free one on the silent list
					cloud->mSilentGrains = curGrain;
					curGrain = nextGrain;
					if (curGrain == 0)
						goto next;
				} else {
					prevGrain = curGrain;
					curGrain = curGrain->nextGrain;
				}
			}
		} else {
			logMsg("Grain culling loop exits 2");
			return NULL;							// exit thread
		}
next:	cloud->gState = kFree;
		csl::sleepMsec(500);						// sleep for 1/2 sec
	}
	return NULL;									// never reached
}

// Start the grain-creation thread

void GrainCloud::startThreads() {
	isPlaying = true;
	if (threadOn)
		return; 
	threadOn = true; 
	gNow = C_TIME;									// get start time
	sampsPerTick = (float) CGestalt::frameRate() / (float) C_TIME;
	logMsg("Starting grain creation/culling threads (%d)", C_TIME);
	spawnerThread->createThread(createGrains, this);
	reaperThread->createThread(reapGrains, this);
	csl::sleepMsec(100);						// sleep for a bit
	logMsg("Grain threads running");
}

// reset the list of grains to be all silent
	
void GrainCloud::reset() {
	unsigned live = 0;							// testing: count killed grains
	Grain * curGrain = mPlayingGrains;
	Grain * nextGrain;
	while (curGrain != 0) {
		nextGrain = curGrain->nextGrain;
		curGrain->nextGrain = mSilentGrains;	// put the grain on the silent list
		mSilentGrains = curGrain;
		curGrain = nextGrain;
		live++;
	}
	mPlayingGrains = 0;							// empty list
//	unsigned dead = 0;							// testing: count silent grains
//	for (curGrain = silentGrains; curGrain != 0; curGrain = curGrain->nextGrain)
//		dead++;
//	printf("\nReset grain lists (%d stopped, %d available)\n", live, dead);
}
