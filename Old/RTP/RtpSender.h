//
//  RemoteIO.h -- CSL I/O Port for sending sample buffers out over UDP or TCP-IP sockets
//		(in response to request packets sent from a RtpReceiver client)
//	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
//
// This is an output driver that receives call-backs from a remote client over a network socket
// and calls on its DSP graph to generate a buffer of samples.
// See the comment in RtpReceiver.h for a description of the packet format
//
// Note to self: The RTP sender is triggered from the nextBuffer call
//

#ifndef CSL_RTPSENDER_H
#define CSL_RTPSENDER_H

#include "CSL_Core.h"
//#include "CslRtpIncludes.h"
#include "CslRtpSession.h"
#include "ThreadUtilities.h"

namespace csl {

// the RemoteIO class

// TODO: This should be a variable that can be adjusted or set on the constructor
#define RTP_BUFFER_SIZE (4410)

// So this really isn't an "effect", but you connect it serially in the existing signal
// chain, thus it conforms to the Effect interface

class RtpSender : public Effect, public CslRtpSession {

public:						// These data members are public so that they can be used from the thread call
	RtpSender(unsigned chans = 1);
	~RtpSender();
	
					// The work method...
	void nextBuffer(Buffer &outputBuffer, unsigned outBufNum) throw (Exception);

	bool createRtpSession();

	bool addRtpDestination(char * remoteIP, unsigned short remotePort);
	bool removeRtpDestination();
	void printError(int rtperr);

protected:
	unsigned mNumChans;		// The default # of channels sent over RTP
	SynchPthread mRtpMutex;

	unsigned mBufferFrames;
	
	unsigned mRemotePort;						///< The network port I send to
	unsigned mLocalPort;							///< The network port I listen on
	unsigned mRemoteIP;							///< The IP address of the client in host byte order
	unsigned mLocalIP;						///< My IP address in host byte order
	string mCName;							///< The CName of my computer
	string mEmail;							///< My email I want registered with RTP
	int mLastPacketNumber;					///< The RTP ID of the last packet sent (or -1 if none)
	int mPayloadType;						///< The RTP payload type we are sending

	RTPUDPv4TransmissionParams  * mTransparams;	// We are using UDP over IPv4
	RTPSessionParams			* mSessparams;
	RTPIPv4Address				* mAddress;

#ifdef DO_TIMING					// This is for the performance timing code
	struct timeval mThen, mNow;		// Used for getting the real time
	long mTimeVals, mThisSec;			// Used for CPU usage statistics
	long mTimeSum;
#endif


};

}

#endif

