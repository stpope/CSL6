//
//	SimplePanner.h -- Simple spatial panner using a front/rear filter.
//	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
//

#ifndef CSL_SIMPLE_PANNER_H
#define CSL_SIMPLE_PANNER_H

#include "CSL_Includes.h"
#include "SpatialPanner.h"

namespace csl {

/// Simple Panner

class SimplePanner : public SpatialPanner {
public:
	SimplePanner();			///< Default constructor
	~SimplePanner();

	void addSource(SpatialSource &s);		///< Implement Panner's addSource, inserting a panner to each source.
	void removeSource(SpatialSource &s);	///< Remove a sound source

	virtual void nextBuffer(Buffer &outputBuffer) throw (CException); ///< fill the buffer with the next buffer_length of values

protected:
	Mixer mDryMix;			///< direct send mixer
	Mixer mFiltMix;			///< LPF filter send mix
	Mixer mRevMix;			///< reverb send mixer

	Splitter mFiltSplit;	///< stereo-to-mono splitter (for filters)
	Butter mRLPF;			///< left &
	Butter mLLPF;			///< right lo-pass filters
	Joiner mFiltJoin;		///< mono-to-stereo joiner (for filters)

	Stereoverb mReverb;		///< stereo freeverb
	Mixer mOutMix;			///< master output mixer

	UGenVector mPanners;	///< Vector of pointers to the panners
};

} // end namespace

#endif
