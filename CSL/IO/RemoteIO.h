//
//  RemoteIO.h -- CSL I/O Port for sending sample buffers out over UDP or TCP-IP sockets
//		(in response to request packets sent from a RemoteStream client)
//	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
//
// This is an output driver that receives call-backs from a remote client over a network socket
// and calls on its DSP graph to generate a buffer of samples.
// See the comment in RemoteStream.h for a description of the packet format
//

#ifndef INCLUDE_REMOTEIO_H
#define INCLUDE_REMOTEIO_H

#include "CSL_Includes.h"
#include "ThreadUtilities.h"
#include "RemoteStream.h"

namespace csl {

// Driver thread function

extern "C" void * RemoteIO_read_loop(void * inst);

///
/// the RemoteIO class
///

class RemoteIO : public IO {

protected:
	unsigned _inputs, _outputs;		// The default # of I/O channels
	Buffer _outputBuffer;			// My output buffer (proxy for the remote frame stream client)
	Buffer _inputBuffer;			// empty input buffer (since we don't pass input across the network [yet])
	
	struct sockaddr_in _clientAddr, _myAddr;	// Socket addresses for the remote client and me
	int _inSock;					// The RFS socket I listen to
	int _outSock;					// The RFS socket I read/write from/to
	
	sample * _buffer;				// My local packet buffer (used for io and for the DSP graph)
#ifdef DO_TIMING					// This is for the performance timing code
	struct timeval then, now;		// Used for getting the real time
	long timeVals, thisSec;			// Used for CPU usage statistics
	long timeSum;
#endif
	void init_io(unsigned in, unsigned out);

public:						// These data members are public so that they can be used from the thread call
	RemoteIO();
	RemoteIO(unsigned chans);
	~RemoteIO();
	
	status open();
	status open(unsigned port);
	virtual status start();
	status stop();
	status close();
	
	void process_request_packet();
	int get_out_sock() { return(_outSock); };
	sample * get_buffer() { return(_buffer); };
	struct sockaddr_in * get_client_addr() { return(&_clientAddr); };
};

}

#endif

