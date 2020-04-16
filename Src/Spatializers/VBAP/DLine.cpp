//
// DLine.h -- an Interpolating Delay Line
//	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
//
// This is a typical circular buffer with one writer and multiple readers (tap instances)

#include "DLine.h"
using namespace csl;

DLine :: DLine ( FrameStream &input, float max_delay ) : Processor(input),
		max_delay_time(max_delay), 
		delay_time(0.5 * max_delay), 
		max_delay_in_frames( (unsigned) (_sampleRate * max_delay + CGestalt::block_size() ) ),
		interp_type(kTruncate), 
		start_frame(0), 
//		end_frame(0),
		write_frame(0),
		ring_buffer(FrameStream::channels(), max_delay_in_frames)
{
//	max_delay_in_frames = _sampleRate * max_delay;
	ring_buffer.allocate_mono_buffers();
	ring_buffer.zero_buffers();
	
}

DLine :: ~DLine() {
	ring_buffer.freeMonoBuffers();
}

bool DLine :: init_delay_time( float dt ) {
	if  (dt > max_delay_time)
		return false;
	delay_time = dt;
	return set_target_delay_time(dt);
}

bool DLine :: set_target_delay_time( float tdt ) {
	if  (tdt > max_delay_time)
		return false;
	target_delay_time = tdt;
	return true;
}

bool DLine :: set_interp_type( InterpType it ) {
	interp_type = it;
	return true;
}

// Do the work

status DLine :: mono_next_buffer(Buffer & inputBuffer, Buffer & outputBuffer, unsigned inBufNum, unsigned outBufNum) {
	unsigned temp_write_frame = write_frame;
	sample * rbPtr = ring_buffer._monoBuffers[outBufNum] + temp_write_frame;
	sample * outPtr = outputBuffer._monoBuffers[outBufNum];
/*
	for (int i=0; i<outputBuffer._numFrames; i++){
	//	ring_buffer._monoBuffers[outBufNum][temp_write_frame] = outputBuffer._monoBuffers[outBufNum][i];
		*rbPtr++ = *outPtr++
		if (++temp_write_frame >= ring_buffer._numFrames)
			temp_write_frame = 0;
			rbPtr = ring_buffer._monoBuffers[outBufNum];
	}
*/
	sample *tempSamplePtr;
	outPtr = outputBuffer._monoBuffers[outBufNum];
	float ramp_factor = 0.0001;			// ??
	rbPtr = ring_buffer._monoBuffers[outBufNum];
	for (unsigned i = 0; i < outputBuffer._numFrames; i++) {
		delay_time = (1.0 - ramp_factor) * delay_time + ramp_factor * target_delay_time;
		float r_index_and_mant = (delay_time * _sampleRate);
		unsigned index = (unsigned) floor(r_index_and_mant);
		float mantissa = r_index_and_mant - index;
		int read_frame = temp_write_frame - index;
		while (read_frame < 0)
			read_frame += ring_buffer._numFrames;
		sample lower_read_value = *(rbPtr + read_frame);
		tempSamplePtr = ((unsigned)(rbPtr + read_frame + 1)) % ring_buffer._numFrames;
		sample upper_read_value = *tempSamplePtr;
		if ( ++temp_write_frame >= ring_buffer._numFrames)
			temp_write_frame -= ring_buffer._numFrames;
		*outPtr++ = ((1.0 - mantissa) * lower_read_value) + (mantissa * upper_read_value);
	}
	write_frame = temp_write_frame;
}

// Call inherited version

status DLine :: next_buffer(Buffer & inputBuffer, Buffer & outputBuffer){
	Processor :: pull_input( inputBuffer, outputBuffer );
	FrameStream :: next_buffer( inputBuffer, outputBuffer );
//	update_write_frame(outputBuffer._numFrames);
	write_frame += outputBuffer._numFrames;
	write_frame %= ring_buffer._numFrames;
	
}




