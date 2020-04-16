//
//	SimplePanner.h -- Basic spatial panner using a front/rear filter.
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
	SimplePanner(); // Default constructor
	~SimplePanner();

	void addSource(SpatialSource &s);		///< Implement Panner's addSource, inserting a panner to each source.
	void removeSource(SpatialSource &s);	///< Remove a sound source

	void nextBuffer(Buffer &outputBuffer /*, unsigned outBufNum */) throw (CException); ///< fill the buffer with the next buffer_length of values

protected:
	Mixer mDryMix;			///< direct (dry) send mixer
	Mixer mRevMix;			///< reverb send mixer
	Mixer mFiltMix;			///< LPF filter send mix
	
	Splitter mFiltSplit;	///< stereo-to-mono splitter (for filters)
	Butter mRLPF;			///< left &
	Butter mLLPF;			///< right lo-pass filters
	Joiner mFiltJoin;		///< mono-to-stereo joiner (for filters)
	
	Stereoverb mReverb;		///< stereo freeverb
	Mixer mOutMix;			///< master output mixer

	UGenIMap mPanners;	///< Vector of pointers to the panners
	UGenIMap mFanOuts;	///< Vector of pointers to the fan-outs

};

} // end namespace

#endif
