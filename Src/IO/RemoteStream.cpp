//
//  RemoteStream.cpp -- a frame stream that sends RFS requests to its "server" to get buffers.
//	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
//

#include "RemoteStream.h"

#ifndef CSL_WINDOWS
#include <sys/param.h>
#endif

using namespace csl;

#ifdef RS_TIMING
static struct timeval then, now;			// used for getting the real time
static long timeVals, thisSec, timeSum;		// for printing run-time statistics
#endif

// RemoteStream Constructor: socket-code heavy lifting
// Open up a remote frame stream using a socket to the given remote "server" ip/host

RemoteStream::RemoteStream(char * server_name, unsigned short server_port, unsigned chans, unsigned bufSize) {
	unsigned waitTime = (unsigned) (1000000.0f * (float) bufSize / (float) CGestalt::sample_rate());
	mNumChannels = chans;
	mBufferSize = bufSize;
	mCurrentBuffer = 0;
	mCurrentFrame = 0;
#ifdef CSL_WINDOWS
	WSADATA localWSA;
	if (WSAStartup(MAKEWORD(1,1),&localWSA)!= 0)
		perror("Couldn't do WSAStartup");
#endif
									// set up the packet storage buffers
	for (int i = 0; i < 2; i++) {
		mIoBuffers[i] = (sample *) malloc(RS_RESPONSE_PACKET_SIZE);
	}
									// call the setup methods
	if (initSockets(serverName, serverPort))
		goto error;
	initPacket();
	if (connectToServer())
		goto error;
									// Now set up the background server semaphore and thread
	mServerSemaphore = (sem_t *) sem_open("RFS_read_sem", O_CREAT, 0644, 0);
	if (mServerSemaphore == (sem_t *) SEM_FAILED)
		perror("RFS: sem_open");
									// Create the reader thread
	CSL_CreateThread(RS_read_loop, (void *) this);
									// Now try to fill the input input buffers
	for (unsigned i = 0; i < 2; i++) {
		sem_post(mServerSemaphore);		// Trigger the server reader thread
		sleep_usec(waitTime);		// sleep
		mCurrentBuffer = 1 - mCurrentBuffer;
	}
	return;
error:
	logMsg(kLogError, "RemoteStream socket open error");
	if (mSocket)
		if (closesocket(mSocket) < 0)
			perror("Close mSocket");
	mSocket = -1;
}

// Now for the socket creation

int RemoteStream::initSockets(char * serverName, unsigned short serverPort) {
	struct hostent * he;
													// Set up the background thread's semaphore
	mServerSemaphore = (sem_t *) sem_open("RFS_read_sem", O_CREAT, 0644, 0);
	if (mServerSemaphore == (sem_t *) SEM_FAILED)
		perror("RemoteStream: sem_open");

#ifdef CSL_DEBUG
	logMsg("Open RemoteStream output socket");		// Open request socket
#endif

	if ((mSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("RemoteStream: socket");
		return 1;
	}
	if ((he = gethostbyname(serverName)) == NULL) {	// get the server's IP host info
		perror("\tRemoteStream: gethostbyname");
		return 1;
	}
	memset((char *) &mServerAddr, 0, sizeof(mServerAddr));	// Set up server address structure
	mServerAddr.sin_family = AF_INET;
	mServerAddr.sin_port = htons(serverPort);
	mServerAddr.sin_addr = *((struct in_addr *) he->h_addr);
	logMsg("RemoteStream open output socket to %s:%d (= %d) (sz = %d)", 
		inet_ntoa(mServerAddr.sin_addr), serverPort, mSocket, mBufferSize);
	return 0;
}

// Set up request packet header

void RemoteStream::initPacket() {
	unsigned char * bString;
									// Write the structure into the buffer
	mHeader.magic = RS_PACKET_MAGIC | CSL_CMD_SET_CLIENT;	
	mHeader.frames = mBufferSize;
	mHeader.channels = mNumChannels;
	for (int i = 0; i < 2; i++) {
		bString = (unsigned char *) mIoBuffers[i];
		memcpy(bString, &mHeader, RS_PACKET_SIZE);		// Copy the header to the packets
	}
}

// Now send out the "introduction" packet

int RemoteStream::connectToServer() {
	CSL_RS_MSG * packetHeader;
	ssize_t xferCnt;
	unsigned command;
									// connect the socket to the server
	if (connect(mSocket, (struct sockaddr *) & mServerAddr, sizeof(mServerAddr)) < 0) {
		perror("\tRemoteStream connect");
		return 1;
	}								// write the intro packet
	xferCnt = write(mSocket, (char *) mIoBuffers[0], RS_PACKET_SIZE);
	if (xferCnt != RS_PACKET_SIZE) {
		perror("\tRemoteStream intro packet write");
		return 1;	
	}								// read the response
	xferCnt = read(mSocket, (char *) mIoBuffers[0], RS_PACKET_SIZE);
	if (xferCnt != RS_PACKET_SIZE) {
		logMsg(kLogError, "RemoteStream: intro packet read (%d)", xfer_cnt);
		perror("RemoteStream connect");
		return 1;	
	}								// check the response packet
	packetHeader = (CSL_RS_MSG *) mIoBuffers[0];			// Cast buffer into a packet header
	command = packetHeader->magic;
	if ((command & 0xffffff00) != RS_PACKET_MAGIC) {	// Check header fields
		logMsg(kLogError, "RemoteStream: bad request magic number: %x\n", command);
		return 1;
	}
	logMsg("RemoteStream received server confirmation");
	return 0;
}

// Destructor

RemoteStream::~RemoteStream() {
	free(mIoBuffers[0]);
	free(mIoBuffers[1]);
	if (mSocket)
		if (closesocket(mSocket) < 0)
			perror("RemoteStream: close _sock");
}

//// nextBuffer method -- copy data and signal the semaphore when you need more ////

void RemoteStream::nextBuffer(Buffer &outputBuffer) throw(CException) {
	unsigned char * out, * buf;
	unsigned frames = outputBuffer.mNumFrames;
	unsigned toCopy;						// # of bytes to copy per channel
	unsigned offset;
	bool split;								// whether we're splitting output buffers
	
	if ((mCurrentFrame + frames) <= mBufferSize) {
		split = false;
		toCopy = frames * sizeof(sample);
	} else {								// else we have to read from both buffers
		split = true;
		toCopy = (mBufferSize - mCurrentFrame) * sizeof(sample);
	}
											// Copy the output buffer samples
	for (unsigned i = 0; i < outputBuffer.mNumChannels; i++) {
		out = (unsigned char *) outputBuffer.buffer(i];
		offset = RS_PACKET_SIZE + (mCurrentFrame * sizeof(sample)) 
						+ (i * mBufferSize * sizeof(sample));
		buf = ((unsigned char *) (mIoBuffers[mCurrentBuffer])) + offset;
		memcpy(out, buf, toCopy);
	}
	mCurrentFrame += frames;
	if (mCurrentFrame >= mBufferSize) {		// If we're done with this buffer
		mCurrentBuffer = 1 - mCurrentBuffer;
		mCurrentFrame = 0;
		sem_post(mServerSemaphore);				// Trigger the server reader thread!
	}
	if (split) {							// if we have to read the start of the other buffer
		toCopy = (frames * sizeof(sample)) - toCopy;
		for (unsigned i = 0; i < outputBuffer.mNumChannels; i++) {
			out = (unsigned char *) outputBuffer.buffer(i];
			offset = RS_PACKET_SIZE + (i * mBufferSize * sizeof(sample));
			buf = ((unsigned char *) (mIoBuffers[mCurrentBuffer])) + offset;
			memcpy(out, buf, toCopy);
		}
		mCurrentFrame = toCopy / sizeof(sample);		
	}
	return;
}

// Here's the reader thread function; it waits on a semaphore to send a packet to the server

extern "C" void * RS_Read_Loop(void * inst) {
	RemoteStream * rfs = (RemoteStream *) inst;
	ssize_t bytes_xfer;
	unsigned command, packet_size;
	unsigned _numChannels = rfs->channels();
	unsigned which, _bufferSize;
	unsigned char * b_str;
	CSL_RFS_MSG * pkt_header;
	int sock = rfs->get_sock();
#ifdef RS_TIMING
	thisSec = timeVals = timeSum = 0L;
#endif

	logMsg("\tRemoteStream starting read_loop");
	while(1) {							// -- ENDLESS LOOP --
		sem_wait(rfs->get_semaphore());			// Wait on the semaphore sent from the reader thread
												// Set up request packet header
		which = rfs->get_buffer_switch();
		_bufferSize = rfs->get_buffer_size();
		b_str = (unsigned char *) ((rfs->get_io_buffers())[1 - which]);
		pkt_header = (CSL_RFS_MSG *) b_str;		// Cast buffer into a packet header
		pkt_header->magic = RFS_PACKET_MAGIC | CSL_CMD_NEXT_BUFFER;
		pkt_header->frames = _bufferSize;
		pkt_header->channels = _numChannels;
#ifdef RS_TIMING
		GET_TIME(&then);
#endif											// Send a request packet
		if (write(sock, (char *) b_str, RFS_PACKET_SIZE) != (int) RFS_PACKET_SIZE) {
			logMsg(kLogError, "RemoteStream read_loop write");
			perror("\t\t");
			continue;
		}										// Read a response packet
		packet_size = RFS_RESPONSE_PACKET_SIZE;
		bytes_xfer = read(sock, (char *) b_str, packet_size);
		if (bytes_xfer != (int) packet_size) {
			logMsg(kLogError, "RemoteStream read_loop read: %d", bytes_xfer);
			perror("\t\t");
			continue;
		}
#ifdef RS_TIMING
		GET_TIME(&now);
		rfs->print_time_statistics(&now, &then, &thisSec, &timeSum, &timeVals);
#endif											// Now process the response buffer
		command = pkt_header->magic;			// Check header/footer fields, ignore packet on error
		if ((command & 0xffffff00) != RFS_PACKET_MAGIC) {
			logMsg(kLogError, "RemoteStream: bad response magic number: %x", command);
			continue;
		}
		memcpy(& command, & b_str[packet_size - 4], 4);
		if (command != RFS_PACKET_MAGIC) {	// Check trailing magic number
			logMsg(kLogError, "RemoteStream: bad response footer: %x", command);
			continue;
		}
	}
}

// Private thread management function

extern "C" int CSL_CreateThread(THREAD_START_ROUTINE pfnThreadProc, void * pvParam) {
	pthread_t tid;
	pthread_attr_t attr;
	int ret;
 
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);
 
	ret = pthread_create(&tid, &attr, pfnThreadProc, pvParam);
	return (ret == 0);
}
