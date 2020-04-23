//
//  RemoteIO.cpp -- CSL I/O Port for sending sample buffers out over sockets
//	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
//

#include "RemoteIO.h"

using namespace csl;

// Constructor

RemoteIO::RemoteIO(unsigned chans) {
	init_io(0, chans);
}

// Default is stereo

RemoteIO::RemoteIO() {
	init_io(0, 2);
}

// Initialization function

void RemoteIO::init_io(unsigned in, unsigned out) {
	_inputs = in;
	_outputs = out;
	_outputBuffer.set_size(_outputs, CGestalt::max_buffer_frames());		// dummy buffer
	_inputBuffer.set_size(_outputs, CGestalt::max_buffer_frames());
	_inputBuffer.allocateBuffers();
	_inputBuffer.zero_buffers();
	_inSock = 0;
	_outSock = 0;
	unsigned _bufferSize = CGestalt::max_buffer_frames();
	unsigned _numChannels = _outputs;		// needed by RFS macro below
	
											// Allocate my local sample buffer
	_buffer = (sample *) malloc(RFS_RESPONSE_PACKET_SIZE);
	if (_buffer <= 0)
		logMsg(kLogError, "Cannot allocate RFS output buffers.");
#ifdef CSL_WINDOWS
	WSADATA localWSA;
	if (WSAStartup(MAKEWORD(1,1),&localWSA)!= 0)
		perror("Couldn't do WSAStartup");
#endif
#ifdef DO_TIMING
	thisSec = timeVals = timeSum = 0;
#endif
}

// Destructor

RemoteIO::~RemoteIO() {
	if (_buffer != 0)
		free(_buffer);
}

// Open up the port

status RemoteIO::open() {
	return(this->open(CSL_DEFAULT_REQUEST_PORT));
}

// Really open and create the server read (input) socket

status RemoteIO::open(unsigned port) {
	struct sockaddr_in serv_addr;
							// Create TCP-IP socket
	if((_inSock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("RemoteIO open_client: bad _inSock");
		return cslErr;
	}
	logMsg("Open RemoteIO server read socket on port %d = %d", port, _inSock);	
	memset((char *)&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(port);
							// Bind the socket to my address
	if(bind(_inSock, (struct sockaddr *) &serv_addr, sizeof(struct sockaddr)) < 0) {
		perror("RemoteIO open_client: unable to  bind");
		return cslErr;
	}
	return cslOk;
}

// Close the port (doesn't work because I can't call the stdlib close...)

status RemoteIO::close() {
	if (_inSock)
		if (closesocket(_inSock) < 0)
			perror("Close _inSock");
	if (_outSock)
		if (closesocket(_outSock) < 0)
			perror("Close _outSock");
	return cslOk;
}

// Start playing -- this is the main loop and forks the reader thread

status RemoteIO::start() {
	socklen_t addrLen = sizeof(struct sockaddr_in);
	
//	logMsg("RemoteIO server listening to socket");	
	if (listen(_inSock, 2) < 0) {
		perror("\tTCP_RemoteIO listen");
		return cslErr;
	}
//	logMsg("RemoteIO server accepting socket connection");	
	if ((_outSock = accept(_inSock, (struct sockaddr *) & _clientAddr, & addrLen)) < 0) {
		perror("\tTCP_RemoteIO accept");
		return cslErr;
	}
	logMsg("RemoteIO server accepted client socket connection");	
						// now fork the IO thread
	CSL_CreateThread(RemoteIO_read_loop, (void *) this);
	return cslOk;
};

// This is the macro to cast the pointer to a RemoteIO instance reference.

#define THE_IO ((RemoteIO *) my_inst)

// This is the forked thread for the RemoteIO read loop.
// Note that this is "extern" because I fork it off as a separate thread; thus, it has to refer to
// the RemoteIO instance via the void * pointer passed in the thread creation call.

extern "C" void * RemoteIO_read_loop(void * my_inst) {
	ssize_t xfer_len;
	unsigned command, magic = RFS_PACKET_MAGIC;
	int sock = THE_IO->get_out_sock();
	sample * _buffer = THE_IO->get_buffer();
	unsigned char * b_str = (unsigned char *) _buffer;
	CSL_RFS_MSG * pkt_header = (CSL_RFS_MSG *) b_str;
	
	logMsg("Starting RemoteIO socket read loop (%d)", sock);
	while(TRUE) {					// Endless loop
	//	printf("\tRemoteIO reading from %d\n", _inSock);
		xfer_len = read(sock, (char *) _buffer, RFS_PACKET_SIZE);
		if (xfer_len < 0) {
			perror("RemoteIO loop read");
			goto next_loop;
		}
	//	logMsg("\tgot %d bytes = %x  (%d) %d  %d", xfer_len, pkt_header->magic, 
	//				(pkt_header->magic & 0x00ff), pkt_header->frames, pkt_header->channels);
		command = pkt_header->magic;
		if ((command & 0xffffff00) != magic) {  // Check header fields, ignore packet on any error
			logMsg(kLogError, "RemoteIO: bad request magic number: %x", command);
			goto next_loop;
		}
		command = command & 0x000000ff;			// Get the low-order byte (the actual command)
		switch (command) {						// Command packet processing switch
			case CSL_CMD_SET_CLIENT:			// Read the operands of the set client command
				if (xfer_len != RFS_PACKET_SIZE) {
					logMsg(kLogError, "RemoteIO: wrong size in client ID packet\n");
					break;
				}
				logMsg("RemoteIO set_client");
												// Echo the request packet back to the client
				xfer_len = write(sock, (char *) _buffer, RFS_PACKET_SIZE);
				if (xfer_len < 0)
					perror("RemoteIO set_client write");
				break;
			case CSL_CMD_NEXT_BUFFER:			// Process a buffer request packet
				THE_IO->process_request_packet();
				break;
			case CSL_CMD_STOP:					// Stop the server
				THE_IO->stop();
				THE_IO->close();
				exit(1);
			default:							// Default = ??
				logMsg(kLogError, "RemoteIO: Illegal command: %d", command);
				break;
		 }
next_loop:
		continue;
	}
	return NULL;								// Never reached
}

// This is the method to generate a sample buffer and send it back to the client

void RemoteIO::process_request_packet() {
	unsigned packet_size = 0;
	unsigned magic = RFS_PACKET_MAGIC;
	unsigned char * sbuf = (unsigned char *) _buffer;
	CSL_RFS_MSG * pkt_header = (CSL_RFS_MSG *) sbuf;
	unsigned frames = pkt_header->frames;
	ssize_t bytes_wrote;

	if ((frames < 32) || (frames > CGestalt::max_buffer_frames())) {
		logMsg(kLogError, "\tRemoteIO bad request frame count: %d", frames);
		return;
	}
	if (pkt_header->channels != _outputs) {
		logMsg(kLogError, "\tRemoteIO: bad request channel count: %d (%d)", 
						pkt_header->channels, _outputs);
		return;
	}								// Set up pointer for response samples
									// Send result buffer back to Remote client
	packet_size = RFS_PACKET_SIZE + (frames * _outputs * sizeof(sample));
	memcpy(&sbuf[packet_size], &magic, 4);
	packet_size += 4;
//	printf("RemoteIO sending client %d sample bytes\n\t", packet_size);
									// write away!
	bytes_wrote = write(_outSock, (char *) sbuf, packet_size);
	if (bytes_wrote < 0)
		perror("sendto");
									// now execute the graph for the next buffer
	if (_graph != 0) {	
		memset(((void *) & sbuf[RFS_PACKET_SIZE]), 0, (frames * _outputs * sizeof(sample)));
		_outputBuffer._numFrames = frames;
		_inputBuffer._numFrames = frames;			// this is empty for now
		for (unsigned i = 0; i < _outputs; i++)		// set output buffer sample pointers
			_outputBuffer._buffers[i] = (sample *) & sbuf[RFS_PACKET_SIZE 
										+ (i * frames * sizeof(sample))];
#ifdef DO_TIMING
		GET_TIME(&then);
#endif
									// Call the client's next_buffer function to do the work!
		_graph->next_buffer(_inputBuffer, _outputBuffer);

#ifdef DO_TIMING		
		GET_TIME(&now);
		this->print_time_statistics(&now, &then, &thisSec, &timeSum, &timeVals);
#endif
	}
}

// Stop playing

status RemoteIO::stop() {
	_outSock = 0;
	return cslOk;
}

