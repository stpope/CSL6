//
//  RemoteIO.cpp -- CSL I/O Port for sending sample buffers out over sockets
//	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
//

#include "RtpSender.h"

using namespace csl;

// Constructor

RtpSender :: RtpSender(unsigned chans) 
	: Effect(),
	  mNumChans(chans),
	  mBufferFrames(RTP_BUFFER_SIZE),
	  CslRtpSession(chans, RTP_BUFFER_SIZE),
	  mRtpMutex()
{
// The constructor should set up all of the transmission and session params, but
// should not do anything related to specific addresses

	mLastPacketNumber = -1;		// We haven't received a packet yet, so -1 indicates this

	mPayloadType = 11; // Default payload is a mono 16/44.1 stream (type 11)
	SetDefaultPayloadType(mPayloadType);	// And set it in the session

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
	logMsg(kLogError, "RtpSender init error");
	return;
}

// Destructor

RtpSender :: ~RtpSender() {
	BYEDestroy(RTPTime(10,0),0,0);	// Destroy the RTP session (wait 10 seconds for acknowledgement)

	if (mTransparams) delete mTransparams;
	if (mSessparams) delete mSessparams;
	if (mAddress) delete mAddress;

	#ifdef CSL_WINDOWS
		WSACleanup();		// Must be called because WSAStartup() called in class constructor
	#endif
}

bool RtpSender :: createRtpSession() {

	mSessparams->SetOwnTimestampUnit(1.0/mFrameRate);	// Our timestamp is 1/Fs
	mSessparams->SetUsePollThread(TRUE);				// This is the default behavior, but let's be verbose

<<<<<<< RtpSender.cpp
	mRtpSession->SetDefaultPayloadType(11);	// Default payload is a mono 16/44.1 stream (type 11)
	mRtpSession->SetDefaultMark(false);		// Sets the default marker to false
	mRtpSession->SetDefaultTimestampIncrement((unsigned) (CGestalt::frameRate() * 0.020));	// Default is 20 ms of samples per packet
=======
	SetDefaultMark(false);		// Sets the default marker to false
	SetDefaultTimestampIncrement(CGestalt::frameRate() * 0.020);	// Default is 20 ms of samples per packet
>>>>>>> 1.4

	mTransparams->SetPortbase(mLocalPort);
	int status = Create(*mSessparams,mTransparams);			// Create the RTP Session
	printError(status);
	return true;
}

bool RtpSender :: addRtpDestination(char * remoteIP, unsigned short remotePort) {

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

bool RtpSender :: removeRtpDestination() {
	// Destroy the RTP session, say bye to the server
	DeleteDestination(*mAddress);
	return true;
}

void RtpSender :: printError(int rtperr)
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

void RtpSender :: nextBuffer(Buffer &outputBuffer, unsigned outBufNum) throw (Exception)
// This nextBuffer function call should get the input buffer, put the data in RtpSender's
// internal RingBuffer, and echo the data to the outputBuffer
// TODO: This should actually deal with multi-channel nextBuffer calls, and support multi-channel RTP
{	
	if (outBufNum != 0) return;	// We want to work with the multi-channel buffers only

	Port * inPort = mInputs[CSL_INPUT_L];
	this->pullInput(inPort, outputBuffer.mNumFrames);
	mInputPtr = inPort->mBuffer->mMonoBuffers[0];
//	UnitGenerator :: nextBuffer(outputBuffer);

	sample * outputPtr = outputBuffer.monoBuffer(outBufNum);
	sample * inputPtr = mInputPtr;
	unsigned numFrames = outputBuffer.mNumFrames;

	outputBuffer.copySamplesFrom(*(this->inPort()->mBuffer)); // Copy input to output
//	mRtpBuffer.writeBuffer(*(this->inPort()->mBuffer));	// And put the inputBuffer onto RtpBuffer
	// TODO: the writeBuffer function NEEDS to return a buffer status indicator, or an int representing
	// the actual # of samples written

	return;
}
