//
// DLine.h -- an Interpolating Delay Line
//	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
//
// This is a typical circular buffer with one writer and multiple readers (tap instances)

#ifndef INCLUDE_DLine_H
#define INCLUDE_DLine_H

#include "CSL_Core.h"

namespace csl {

typedef enum {
	kTruncate,
	kLinear,
	kAllPass
} InterpType;

class DLine : public FrameStream, public Processor {

private:	
	Buffer ring_buffer;

	float max_delay_time;
	float delay_time;
	float target_delay_time;
	unsigned max_delay_in_frames;
	InterpType interp_type;
	unsigned start_frame;
//	unsigned end_frame;
	unsigned write_frame;

public:
	DLine( FrameStream &input, float max_delay );
	~DLine();

	bool set_target_delay_time( float tdt );
	bool init_delay_time( float dt );
	bool set_interp_type( InterpType );
//	update_write_frame( unsigned how_many_frames);

	status mono_next_buffer(Buffer & inputBuffer, Buffer & outputBuffer, unsigned inBufNum, unsigned outBufNum);
	status next_buffer(Buffer & inputBuffer, Buffer & outputBuffer);
	
};

}

#endif
