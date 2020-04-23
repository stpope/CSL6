///
///  Granulator.h -- CSL class for doing granular synthesis
///
///	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
///

#ifndef CSL_GRAN_H
#define CSL_GRAN_H

#include "CSL_Core.h"		// my superclass
#include "ThreadUtilities.h"

namespace csl {				// my namespace

#define MAXGRAINS 200

/// Grain data structure
/// This implementation uses a linked list data structure.
/// You might want to add a few more members to this for flexibility.

typedef struct Grain {
	float amplitude;		///< amplitude scale (0 - 1)
	float rate;				///< playback rate (1.0 for normal pitch, < 0 reads backwards)
	float duration;			///< duration in samples
	float time;				///< current time (index) in samples
	float pan;				///< stereo pan 0 - 1
	float env;				///< envelope: 0 = perc, 0.5 = triangle, 1 = reverse perc
	float position;			///< running sample index
	unsigned delay;			///< initial delay in samples
	float * samples;		///< sample buffer pointer
	unsigned numSamples;	///< length of sample vector
	Grain * nextGrain;		///< A pointer to the next grain in the linked list
} Grain;

/// This flag is for the app state, so that we don't change the grain lists while calculating samples

typedef enum {			
	kFree,				///< free state
	kDSP,				///< calculating audio samples
	kSched,				///< scheduling grains
} GrainulatorState;

/// GrainCloud -- routine for playing clouds under GUI control.
/// This could be called a cloud or a stream.
/// You could also add a few more variables to make more flexible clouds.

class GrainCloud {
public:
	GrainCloud();				///< simple constructor
	~GrainCloud();
			
	void startThreads();		///< method to start-up the create/reap threads
	void reset();				///< reset all grains to silent
							// public data members (set from GUI)
	float mRateBase;			///< grain rate base
	float mRateRange;			///< rate random range
	float mOffsetBase;			///< starting index offset base
	float mOffsetRange;			///< offset range
	float mDensityBase;			///< grain density base
	float mDensityRange;		///< grain density range
	float mDurationBase;		///< grain duration base
	float mDurationRange;		///< grain duration range
	float mWidthBase;			///< stereo width
	float mWidthRange;			///< stereo width
	float mVolumeBase;			///< amplitude scale
	float mVolumeRange;			///< amplitude range
	float mEnvelopeBase;		///< envelope base: 0 = perc, 0.5 = triangle, 1 = reverse perc
	float mEnvelopeRange;		///< envelope range

	float * mSamples;			///< sample buffer pointer
	unsigned numSamples;		///< # of samples in buffer
	bool isPlaying;				///< whether I'm on or off

	Grain * mSilentGrains;		///< shared grain lists - ptr to the free pool (silent)
	Grain * mPlayingGrains;		///< ptr to the list of active grains
	GrainulatorState gState;	///< granulator state flag
	long gNow;					///< clock for accurate timing
	float sampsPerTick;			///< resolution of hi-res clock(s-rate / 1 billion)

protected:
	CThread * spawnerThread;	///< thread to create grains
	CThread * reaperThread;		///< thread to kill finished grains
	bool threadOn;				///< if the thread's running
};

/// GrainPlayer -- low-level granular synthesis generator, uses a list of current grains.

class GrainPlayer : public UnitGenerator {
public:
	GrainPlayer(GrainCloud * cloud);
	~GrainPlayer();
								/// this sums up the list of live grains -- very simple
	void nextBuffer(Buffer & outputBuffer) throw (CException);
	
	GrainCloud * mCloud;		///< the cloud I play
};

}		// end of namespace

#endif
