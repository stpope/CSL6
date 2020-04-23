//
//  RtpReceiver.cpp -- a unit generator that listens on an RTP socket for incoming audio buffers.
//	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
//

#include "RtpReceiver.h"

using namespace csl;

#ifdef RTP_TIMING
static struct timeval then, now;			// used for getting the real time
static long timeVals, thisSec, timeSum;		// for printing run-time statistics
#endif

// RtpReceiver Constructor: socket-code heavy lifting
// Open up a remote frame stream using a socket to the given remote "server" ip/host

RtpReceiver :: RtpReceiver(unsigned chans)
	: mNumChans(chans),
	  mBufferFrames(RTP_BUFFER_SIZE),
	  CslRtpSession(mNumChans, mBufferFrames),
	  mRtpMutex() 
{
// The constructor should set up all of the transmission and session params, but
// should not do anything related to specific addresses

	// Create instances of our RTP classes
	mTransparams = new RTPUDPv4TransmissionParams;
	mSessparams = new RTPSessionParams;

#ifdef CSL_WINDOWS
	//MUST call WSAStartup() to use WS2_32.DLL
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD( 2, 2 ), &wsaData)!= 0)	 //WSACleanup called in destructor
		perror("Couldn't do WSAStartup");
#endif

	// create the RTP session, but don't add any destinations yet
	// Note that this also creates the thread that polls for incoming RTP packets
	if (!createRtpSession())	goto error;

	return;
error:
	logMsg(kLogError, "RtpReceiver init error");
	return;
}

// Destructor

RtpReceiver :: ~RtpReceiver() {

	BYEDestroy(RTPTime(10,0),0,0);	// Destroy the RTP session (wait 10 seconds for acknowledgement)

	if (mTransparams) delete mTransparams;
	if (mSessparams) delete mSessparams;
	if (mAddress) delete mAddress;

	#ifdef CSL_WINDOWS
		WSACleanup();		// Must be called because WSAStartup() called in class constructor
	#endif
}

bool RtpReceiver :: createRtpSession() {

	mSessparams->SetOwnTimestampUnit(1.0/mFrameRate);	// Our timestamp is 1/Fs
	mSessparams->SetUsePollThread(TRUE);				// This is the default behavior, but let's be verbose

<<<<<<< RtpReceiver.cpp
	mRtpSession->SetDefaultPayloadType(11);	// Default payload is a mono 16/44.1 stream (type 11)
	mRtpSession->SetDefaultMark(false);		// Sets the default marker to false
	mRtpSession->SetDefaultTimestampIncrement((unsigned) (CGestalt::frameRate() * 0.020));	// Default is 20 ms of samples per packet
=======
	SetDefaultPayloadType(11);	// Default payload is a mono 16/44.1 stream (type 11)
	SetDefaultMark(false);		// Sets the default marker to false
	SetDefaultTimestampIncrement(CGestalt::frameRate() * 0.020);	// Default is 20 ms of samples per packet
>>>>>>> 1.5

	mTransparams->SetPortbase(mLocalPort);
	int status = Create(*mSessparams,mTransparams);	
	printError(status);
	return true;
}

bool RtpReceiver :: addRtpSource(char * remoteIP, unsigned short remotePort) {

	mAddress = new RTPIPv4Address;

	// The inet_addr function returns a value in network byte order, but
	// we need the IP address in host byte order, so we use a call to
	// ntohl
	mRemoteIP = inet_addr(remoteIP);
	mRemoteIP = ntohl(mRemoteIP);

	// Storing passed parameters to class members
	mRemotePort = remotePort;

	// Set the  IP and port of the address we receive data from
	mAddress->SetIP(mRemoteIP);
	mAddress->SetPort(mRemotePort);

	int status = AddDestination(*mAddress);	// Adds a destination to the RTP stream (potential for multicasting)
	printError(status);
	return true;
}

bool RtpReceiver :: removeRtpSource() {
	// Destroy the RTP session, say bye to the server
	DeleteDestination(*mAddress);
	return true;
}

void RtpReceiver:: printError(int rtperr)
{
	if (rtperr < 0)
	{
		string errStr = "Error: " + RTPGetErrorString(rtperr) + "\n";
		
		// We need to convert the string to a char * before passing
		// it to logMsg...
		char * logStr = new char[errStr.length()+1];
		strcpy(logStr, errStr.c_str());

		logMsg(kLogError, logStr);
		exit(-1);
	}
}

//// nextBuffer method -- copy data and signal the semaphore when you need more ////
// TODO: This should actually deal with multi-channel nextBuffer calls, and support multi-channel RTP

void RtpReceiver :: nextBuffer(Buffer & outputBuffer, unsigned outBufNum) throw (Exception)
{
	float * outputBufferPtr = outputBuffer.monoBuffer(outBufNum);
	unsigned char * out, * buf;
	unsigned numFramesRequested = outputBuffer.mNumFrames;
	unsigned toCopy;						// # of bytes to copy per channel
	unsigned offset;
	
	mRtpMutex.lock();

	// TODO: multiple channels

	if (mBufferState == kBuffering) {		// If we haven't started playback yet (still buffering from an underrun)
		memset(outputBufferPtr, 0, numFramesRequested * sizeof(sample));	// Fill outBuf with zeroes
		mRtpMutex.unlock();
		return;
	}

	// Read the next part of rBuff ring buffer into outputBuffer
	mRtpBuffer.nextBuffer(outputBuffer);
	mRtpMutex.unlock();

	return;
}