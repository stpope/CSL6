//
//  RemoteStream.h -- a frame stream that sends network requests to a server to get buffers.
//	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
//
// The server is assumed to be on a remote machine, and is a CSL program that uses a RemoteIO
// port for output. The request packet sent to the server from this client causes it to call 
// its DSP graph's next_buffer() method and return the sample buffer via the TCP-IP socket.
//
// The request packet format is given in the structure CSL_RFS_MSG.
// The response packet format starts with the same header, followed by raw sample data as 
// non-interleaved floats, followed by the magic number as a packet footer.
//
// To set this up, the server must be a CSL program, and the RemoteIO object must know what port it 
// it listens to. The client (this RemoteStream) needs to know the server's hostname and port.
// The client first sends the server an "introduction" packet so that the server can open a
// response socket. Then the client can send the server sample buffer requests.
//
// Note that the RemoteStream's packet size will typically be larger than the buffer size of the 
// client's DSP graph. In the tests, we use a packet size of 8000 samples and a client block size
// of 500.
//

#ifndef CSL_RemoteStream_H
#define CSL_RemoteStream_H

#include "CSL_Includes.h"
#include "ThreadUtilities.h"

#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>

#ifdef CSL_WINDOWS		// MS-Windows socket includes

#include <winsock.h>
typedef unsigned int in_addr_t;
typedef int ssize_t;
#define MAXHOSTNAMELEN 256
#include <fcntl.h>
#define SEM_FAILED ((sem_t*) 0)	// For pthread semaphores -- why do we need this?

#else					// UNIX socket includes

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/param.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define closesocket(x)  ::close(x)

#endif

namespace csl {

// CSL RS protocol message header structure (8 bytes)

typedef struct {
	unsigned magic;				// Magic number (=0x004200XX, whereby XX is the command)
	unsigned short frames;		// Number of frames requested
	unsigned short channels;		// Number of channels requested
} CSL_RS_MSG;

// Useful macros for managing packet I/O

#define RS_PACKET_MAGIC 0x00420000						// RS packet magic number
#define RS_PACKET_SIZE (sizeof(CSL_RFS_MSG))			// Size of RS request packets
#define RS_BUFFER_SIZE (mBufferSize * mNumChannels * sizeof(sample))
#define RS_RESPONSE_PACKET_SIZE (RFS_PACKET_SIZE + RS_BUFFER_SIZE + 4)

// Default I/O port (ought to use PortMapper to pass port #s as cmd-line options)

#define CSL_DEFAULT_REQUEST_PORT 57574	// Default port for RS requests (server listener)

//#define RS_TIMING					// Print out the timing code

// Remote commands

#define CSL_CMD_SET_CLIENT		42		// Set up the server's response socket to the client
#define CSL_CMD_NEXT_BUFFER 	43		// Request a buffer of samples from the server
//#define CSL_CMD_NEXT_SAMPLE 	44		// Request a single sample from the server
//#define CSL_CMD_GET_RATE		45		// Query the server's sample rate
//#define CSL_CMD_GET_CHANNELS	46		// Query the server's # of channels
#define CSL_CMD_STOP			47		// Stop the server

// The RS read loop; this is spawned as a separate thread

extern "C" void * RS_read_loop(void * inst);

// Thread utilities

typedef void * (*THREAD_START_ROUTINE)(void *);
extern "C" int CSL_CreateThread(THREAD_START_ROUTINE pfnThreadProc, void * pvParam);

///
/// RemoteStream class
///

class RemoteStream : public UnitGenerator {

public:					/// Constructor
	RemoteStream(char * clientName, unsigned short clientPort, unsigned ch, unsigned bufSize);
	~RemoteStream();

						/// Accessors used by the reader thread
	sample **ioBuffers() { return mIoBuffers; };
	int socket() { return mSocket; };
	unsigned short requestSize() { return (RFS_BUFFER_SIZE); };
	unsigned bufferSwitch() { return mCurrentBuffer; };
	unsigned bufferIndex() { return mCurrentFrame; };
	unsigned bufferSize() { return mBufferSize; };
	sem_t *semaphore() { return mServerSemaphore; };
	
						/// Get a buffer of frames
	void nextBuffer(Buffer &outputBuffer) throw(CException);

protected:				// Basic data
	unsigned mBufferSize;				///< the size of the input ring buffer (in FRAMES)
	sample * mIoBuffers[2];				///< My IO buffers (2 for dbl-buffering; mBufferSize frames in size)
	unsigned mCurrentBuffer;			///< current IO buffer in use
	unsigned mCurrentFrame;				///< current position in buffer
	
						// Socket-related members
	int mSocket;						///< The socket I send to
	CSL_RS_MSG mHeader;				///< The request packet header
	struct sockaddr_in mServerAddr;		///< Socket addresses for the remote server and for me
	sem_t * mServerSemaphore;			///< Semaphore to trigger call to server for samples
	
						// Protected low-level setup methods
	int initSockets(char * serverName, unsigned short serverPort);
	void initPacket();
	virtual int connectToServer();

};

}

#endif

