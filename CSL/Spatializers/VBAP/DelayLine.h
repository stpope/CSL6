//
// DelayLine.h -- an Interpolating Delay Line
//	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
//
// This is a typical circular buffer with one writer and multiple readers (tap instances)

#ifndef CSL_DelayLine_H
#define CSL_DelayLine_H

#include "CSL_Core.h"
#include "RingBuffer.h"

namespace csl {

// The delay line takes its input and write a delayed output
class DelayLine : public UnitGenerator, public Effect {
public:
	DelayLine(unsigned maxDelayInSamples);

	float delayTime();
	unsigned delayLength();
	
	float setDelayTime(float delayInMiliseconds);	
	unsigned setDelayLength(unsigned delayInSamples);
//	void setInterpolationKind();
	
	void nextBuffer(Buffer &output) throw(CException);

protected:
	RingBuffer mRingBuffer;
	unsigned mMaxDelayInSamples;
	unsigned mTotalDelayInSamples;

};

}

#endif

//typedef enum {
//	kTruncate,
//	kLinear,
//	kAllPass
//} InterpType;
//
//class DLine : public FrameStream, public Processor {
//
//private:	
//	Buffer ring_buffer;
//
//	float max_delay_time;
//	float delay_time;
//	float target_delay_time;
//	unsigned max_delay_in_frames;
//	InterpType interp_type;
//	unsigned start_frame;
////	unsigned end_frame;
//	unsigned write_frame;
//
//public:
//	DLine( FrameStream &input, float max_delay );
//	~DLine();
//
//	bool set_target_delay_time( float tdt );
//	bool init_delay_time( float dt );
//	bool set_interp_type( InterpType );
////	update_write_frame( unsigned how_many_frames);
//
//	status mono_next_buffer(Buffer & inputBuffer, Buffer & outputBuffer, unsigned inBufNum, unsigned outBufNum);
//	status next_buffer(Buffer & inputBuffer, Buffer & outputBuffer);
//	
//};
