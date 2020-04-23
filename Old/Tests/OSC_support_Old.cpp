//
//  OSC_support.cpp -- CSL support functions for use with OSC input.
//	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
//
// This file has a bunch of the "glue" code necessary to use OSC simply from within CSL.
// For an example, see OSC_main.cpp (which uses CSL instruymkents) GestureSensorTests.cpp, 
// which interfaces to the CREATE gesture sensors using OSC, or Server_Tests.cpp, which
// adds a few simple OSC commands for controlling the random bell textures.
//
// To use this in your own main, add the following:
//							// setup call for OSC addr space mgmnt
//	initOSCAddrSpace();	
//							// add an OSC command callback
//	addOSCMethod(mCmd, callback);
//
//							// or use an instrument library
//	setupOSCInstrLibrary(vector<Instrument *> library);
//
//							// Run the main loop function (doesn't return)
//	mainOSCLoop(udp_port);	
//
// Functions


#include "BasicFMInstrument.h"
//#include "SndFileInstrument.h"
#include "OSC_cpp.h"

#define MAX_ACCESSORS 64			// max number of instrument accessors
typedef int FileDescriptor;			// used by OSC

using namespace csl;

// Prototypes for "glue" code

extern "C" FileDescriptor initUdp(int port);
extern "C" void closeUdp(int sockfd);
extern "C" void receiveOSCPacket(int sockfd);

extern "C" void *MyInitTimeMalloc(int numBytes);
extern "C" void *MyRealTimeMalloc(int numBytes);

extern "C" void initOSCReceive();
extern "C" void initOSCAddrSpace();
extern "C" void setupOSCInstrLibrary(std::vector<Instrument *> library);
extern "C" void addOSCMethod(char * name, OSC_CALL_BACK function);
extern "C" void addOSCMethod2(char * name, OSC_CALL_BACK function, void * user_data);

extern "C" void mainOSCLoop(int port);
extern "C" void quitCSL();

// OSC globals

OSCcontainer OSCTopLevelContainer = NULL;
struct OSCContainerQueryResponseInfoStruct cqinfo;
struct OSCMethodQueryResponseInfoStruct QueryResponseInfo;
FileDescriptor sockfd;				// OSC socket file descriptor	

// OSC support functions

// Set up the receive Q

extern "C" void initOSCReceive() {
	struct OSCReceiveMemoryTuner rt;
	rt.InitTimeMemoryAllocator = MyInitTimeMalloc;
	rt.RealTimeMemoryAllocator = MyRealTimeMalloc;
	rt.receiveBufferSize = 1000;
	rt.numReceiveBuffers = 50;
	rt.numQueuedObjects = 100;
	rt.numCallbackListNodes = 100;
//	logMsg("Init OSC receive Q\n");	
	if (OSCInitReceive(&rt) == FALSE)
		fatal_error("OSCInitReceive returned FALSE!\n");
}

// Main OSC loop -- select - read - dispatch

extern "C" void mainOSCLoop(int port) {
	fd_set read_fds, write_fds;
	
	logMsg("Starting OSC read/dispatch loop");
	
	sockfd = initUdp(port);
	if (sockfd < 0) {
		perror("initudp");
		return;
	}
	initOSCReceive();		// Set up the receive buffers with reasonable default sizes

	while(TRUE) {					// Endless loop
		OSCInvokeAllMessagesThatAreReady(OSCTT_PlusSeconds(OSCTT_CurrentTime(),
						(csl :: CGestalt :: blockSize() / csl :: CGestalt :: frameRate())));
								// Set up the select file descriptor lists
		FD_ZERO(&read_fds);			// clear read_fds
		FD_ZERO(&write_fds);			// clear write_fds
		FD_SET(sockfd, &read_fds);		// read from the OSC socket
		FD_SET(0, &read_fds);			// and from stdin
								// Do the "select" call on the socket
	//	printf("\tOSC: calling input select\n");
		if (select(sockfd + 1, &read_fds, &write_fds, (fd_set * )0, (struct timeval *)0) < 0) {
			perror("OSC: bad select");    	// if select reported an error
			goto quit;
		}
	//	printf("\tOSC: input select returned\n");
		if(FD_ISSET(sockfd, &read_fds))		// if there's a message coming in
			receiveOSCPacket(sockfd);		// Call the receive/dispatch function
		if (FD_ISSET(0, &read_fds)) {		// is there a character in the queue?
			char c = getchar();			// this will never block
			if (c == 'q')				// quit
				break;
		}
	}
quit:
	quitCSL();
}

/////////////////////////// OSC Helper Functions /////////////////////////////

// Structure passed to OSC to hold a UGEN reference and a parameter selector

typedef struct {
	Instrument * instr;
	unsigned num;
	unsigned selector;
} Inst_Context;

// General-purpose type-checking parameter-setter methods for Instruments

void floatParamSetter(void * con, int arglen, const void * vargs, OSCTimeTag when, NetworkReturnAddressPtr ra) {
	char * error;
	Inst_Context * context = (Inst_Context *) con;
	char * typeString = (char *) vargs;
	void * otherArgs = (void *) OSCDataAfterAlignedString(typeString, typeString+arglen, &error);
	if (strcmp(typeString, ",f")) {
		logMsg(kLogError, "ERROR: Invalid type string in OSC message, expected \",f\" got \"%s\"", typeString);
		fflush(stdout);
		return;
	}
	logMsg("OSC set float param %d/%d to %g", (context->num + 1), context->selector, ((float *) otherArgs)[0]);
	fflush(stdout);
	context->instr->setParameter(context->selector, otherArgs);
}

void intParamSetter(void * con, int arglen, const void * vargs, OSCTimeTag when, NetworkReturnAddressPtr ra) {
	char * error;
	Inst_Context * context = (Inst_Context *) con;
	char * typeString = (char *) vargs;
	void * otherArgs = (void *) OSCDataAfterAlignedString(typeString, typeString+arglen, &error);
	if (strcmp(typeString, ",i")) {
		logMsg(kLogError, "ERROR: Invalid type string in OSC message, expected \",i\" got \"%s\"", typeString);
		return;
	}
	logMsg("OSC set int param %d/%d to %d", (context->num + 1), context->selector, ((int *) otherArgs)[0]);
	fflush(stdout);
	context->instr->setParameter(context->selector, otherArgs);
}

void stringParamSetter(void * con, int arglen, const void * vargs, OSCTimeTag when, NetworkReturnAddressPtr ra) {
	char * error;
	Inst_Context * context = (Inst_Context *) con;
	char * typeString = (char *) vargs;
	void * otherArgs = (void *) OSCDataAfterAlignedString(typeString, typeString+arglen, &error);
	if (strcmp(typeString, ",s")) {
		logMsg(kLogError, "ERROR: Invalid type string in OSC message, expected \",s\" got \"%s\"", typeString);
		return;
	}
	logMsg("OSC set string param %d/%d to %s", (context->num + 1), context->selector, (char *) otherArgs);
	context->instr->setParameter(context->selector, otherArgs);
}

// Play a note by re-triggering the main envelope

void playNote(void * con, int arglen, const void * vargs, OSCTimeTag when, NetworkReturnAddressPtr ra) {
	Inst_Context * context = (Inst_Context *) con;
	logMsg("OSC play_note on instr %d", (context->num + 1));
	context->instr->play();
}

// Play a note with arguments (passed to the instrument as a void *)

void playWithArgs(void * con, int arglen, const void * vargs, OSCTimeTag when, NetworkReturnAddressPtr ra) {
	Inst_Context * context = (Inst_Context *) con;
	char * error;
	char * typeString = (char *) vargs;
	char * endOfArgs = (char *) vargs + arglen;
	void * otherArgs = OSCDataAfterAlignedString(typeString, typeString+arglen, &error);
	logMsg("OSC play_with_args on instr %d %s", (context->num + 1), typeString);
	context->instr->playOSC(typeString, otherArgs, endOfArgs);
}

// Shut down and quit

void quitMethod(void *dummy, int arglen, const void *args, OSCTimeTag when, NetworkReturnAddressPtr ra) {
	quitCSL();
}

void quitCSL() {
	logMsg("OSC server exiting.");
	fflush(stdout);
	IO * io = IO :: theInstance();
	io->stop();			// stop it
	io->close();	
	closeUdp(sockfd);
	exit(0);
}

// IOSC setup methods

extern "C" void initOSCAddrSpace() {
					// This is so that you can call this method multilpe times
	if (OSCTopLevelContainer != NULL)
		return;
					// Memory tuner parameters
	struct OSCAddressSpaceMemoryTuner tun;
	tun.initNumContainers = 8;
	tun.initNumMethods = 120;
	tun.InitTimeMemoryAllocator = MyInitTimeMalloc;
	tun.RealTimeMemoryAllocator = MyRealTimeMalloc;
	OSCTopLevelContainer = OSCInitAddressSpace(&tun);	
	
	OSCInitContainerQueryResponseInfo(&cqinfo);
	OSCInitMethodQueryResponseInfo(&QueryResponseInfo);
}

extern "C" void addOSCMethod(char * name, OSC_CALL_BACK function ) {
	OSCNewMethod(name, OSCTopLevelContainer, function, NULL, &QueryResponseInfo);
}

extern "C" void addOSCMethod2(char * name, OSC_CALL_BACK function, void * user_data) {
	OSCNewMethod(name, OSCTopLevelContainer, function, user_data, &QueryResponseInfo);
}

// Set up the OSC address space for an instrument library

extern "C" void setupOSCInstrLibrary(std::vector<Instrument *> library) {
	OSCcontainer temp_cont;
	char * iname = NULL;
	Inst_Context * the_context = NULL;
	unsigned num_instruments = library.size();
	logMsg("Setting up OSC address space for %d instruments\n", num_instruments);
	
	OSCNewMethod("q", OSCTopLevelContainer, quitMethod, 0, &QueryResponseInfo);
					// Instrument reader loop
	for (unsigned i = 0; i < num_instruments; i++) {
		iname = (char *) malloc(8);
		sprintf(iname, "i%d", i+1);			// create container for instrument
		temp_cont = OSCNewContainer(iname, OSCTopLevelContainer, &cqinfo);
		Instrument * instr = library[i]; 		// get the instrument and its accessors
		unsigned num_accessors = instr->numAccessors();
		AccessorVector accs = instr->getAccessors();
		logMsg("Adding instrument \"%s\" (%s); %d accessors", iname, instr->name().c_str(), num_accessors);

									// Accessor method creation loop
		for (unsigned j = 0; j < num_accessors; j++) {
			the_context = (Inst_Context *) malloc(sizeof(Inst_Context));
			the_context->instr = instr;
			the_context->num = i;
			the_context->selector = accs[j]->mSelector;
		//	if (i == 0)
		//		printf("\tAdding OSC method for %s (%d)\n", accessors[j]->name, accessors[j]->selector);
			switch(accs[j]->mType) {
			case CSL_INT_TYPE:
				OSCNewMethod(accs[j]->mName.c_str(), temp_cont, intParamSetter, the_context, &QueryResponseInfo);
				break;
			case CSL_FLOAT_TYPE:
				OSCNewMethod(accs[j]->mName.c_str(), temp_cont, floatParamSetter, the_context, &QueryResponseInfo);
				break;
			case CSL_STRING_TYPE:
				OSCNewMethod(accs[j]->mName.c_str(), temp_cont, stringParamSetter, the_context, &QueryResponseInfo);
			}
		}
		OSCNewMethod("p", temp_cont, playNote, the_context, &QueryResponseInfo);
		OSCNewMethod("pn", temp_cont, playWithArgs, the_context, &QueryResponseInfo);
	}
	fflush(stdout);
//Debugging
//	printf("\nPrinting whole address space as published\n");
//	OSCPrintWholeAddressSpace();
}
