//
//  RtpReceiver.h -- a unit generator that listens on an RTP socket for incoming audio buffers.
//
// The server is assumed to be on a remote machine, and is a CSL program that uses a RemoteIO
// port for output.
//
// The response packet format starts with the same header, followed by raw sample data as 
// non-interleaved floats, followed by the magic number as a packet footer.
//
// To set this up, the server must be a CSL program, and the RemoteIO object must know what port it 
// it listens to. The client (this RtpReceiver) needs to know the server's hostname and port.
// The client first sends the server an "introduction" packet so that the server can open a
// response socket. Then the client can send the server sample buffer requests.
//
// Note to self: The RTP receiver is triggered from a separate thread, and tries to put incoming
//    packets into the RingBuffer at the correct places.
//

#ifndef CSL_RTPRECEIVER_H
#define CSL_RTPRECEIVER_H

#include "CSL_Core.h"
#include "CslRtpSession.h"
#include "ThreadUtilities.h"



namespace csl {

// Useful macros for managing packet I/O
// TODO: This should be a variable that can be adjusted or set on the constructor
#define RTP_BUFFER_SIZE (4410)

// Default I/O port (ought to use PortMapper to pass port #s as cmd-line options)

#define CSL_DEFAULT_CLIENT_PORT 5004		// Default port for RTP listening (RTCP transmitted on port RTP+1)
#define CSL_DEFAULT_SERVER_PORT 5006	// Default port for sending RTP packets (RTCP xmitted on port RTP+1)

//#define RTP_TIMING					// Print out the timing code

// Remote commands

// The RTP read loop; this is spawned as a separate thread

extern "C" void * RTP_read_loop(void * inst);

// Thread utilities

typedef void * (*THREAD_START_ROUTINE)(void *);
extern "C" int CSL_CreateThread(THREAD_START_ROUTINE pfnThreadProc, void * pvParam);

//
////// RtpReceiver class //////
//
// Inherited from UnitGenerator:
// frameRate, numChannels

class RtpReceiver : public CslRtpSession, UnitGenerator {

public:

						/// Default constructor initializes an RtpReceiver with default 20ms bufSize
	RtpReceiver(unsigned chans = 1);				
	~RtpReceiver();		///< Class destructor

						/// Accessors used by the reader thread
	unsigned bufferSize() { return mBufferFrames; };
	unsigned remotePort() { return mRemotePort; };

	void nextBuffer(Buffer & outputBuffer, unsigned outBufNum) throw (Exception);

	void setLocalPort(unsigned localPort);
	void setBufferSize(unsigned bufferSize);

	bool addRtpSource(char* remoteIP, unsigned short remotePort);
	bool removeRtpSource();
	void printError(int rtperr);
	

protected:				// Internal ring buffer data
	bool createRtpSession();

	unsigned mNumChans;		// The default # of channels sent over RTP
	SynchPthread mRtpMutex;
	unsigned mBufferFrames;					///< the size of the RTP ring buffer (in FRAMES)

	unsigned long mRemoteIP;
	unsigned short mRemotePort;
	unsigned long mLocalIP;
	unsigned short mLocalPort;

	RtpBufferState mBufferState;
	
	RTPUDPv4TransmissionParams  * mTransparams;	// We are using UDP over IPv4
	RTPSessionParams			* mSessparams;
	RTPIPv4Address				* mAddress;
	
	unsigned mLastPacketNumber;					///< The ID of the last packet received
	



};

}

#endif

