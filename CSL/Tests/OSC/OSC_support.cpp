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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "lo/lo.h"

#include "BasicFMInstrument.h"
//#include "SndFileInstrument.h"
//#include "OSC_cpp.h"

int done = 0;

void error(int num, const char *m, const char *path);
int generic_handler(const char *path, const char *types, lo_arg **argv,
		    int argc, void *data, void *user_data);
int foo_handler(const char *path, const char *types, lo_arg **argv, int argc,
		 void *data, void *user_data);
int quit_handler(const char *path, const char *types, lo_arg **argv, int argc,
		 void *data, void *user_data);

#define MAX_ACCESSORS 64			// max number of instrument accessors

using namespace csl;

// Prototypes for "glue" code

extern "C" void initOSCReceive();
extern "C" void initOSCAddrSpace();
extern "C" void setupOSCInstrLibrary(std::vector<Instrument *> library);
#define addOSCMethod(name, nfunction)			\
	lo_server_thread_add_method(sSrvThrd, nname, NULL, nfunction, NULL)

extern "C" void addOSCMethod2(name, nfunction, void * user_data);

					/* add method that will match the path /foo/bar, with two numbers, coerced
					 * to float and int */
    lo_server_thread_add_method(sSrvThrd, "/foo/bar", "fi", foo_handler, NULL);


extern "C" void mainOSCLoop(int port);
extern "C" void quitCSL();

// OSC globals

static lo_server_thread sSrvThrd;

void error(int num, const char *msg, const char *path)
{
    printf("liblo server error %d in path %s: %s\n", num, path, msg);
    fflush(stdout);
}

/* catch any incoming messages and display them. returning 1 means that the
 * message has not been fully handled and the server should try other methods */
int generic_handler(const char *path, const char *types, lo_arg **argv, int argc, void *data, void *user_data) {
    int i;
    printf("path: <%s>\n", path);
    for (i=0; i<argc; i++) {
	printf("arg %d '%c' ", i, types[i]);
	lo_arg_pp(types[i], argv[i]);
	printf("\n");
    }
    printf("\n");
    fflush(stdout);

    return 1;
}

int foo_handler(const char *path, const char *types, lo_arg **argv, int argc,	 void *data, void *user_data) {
    /* example showing pulling the argument values out of the argv array */
    printf("%s <- f:%f, i:%d\n\n", path, argv[0]->f, argv[1]->i);
    fflush(stdout);

    return 0;
}

int quit_handler(const char *path, const char *types, lo_arg **argv, int argc,  void *data, void *user_data) {
    done = 1;
    printf("quiting\n\n");
    fflush(stdout);
				// Shut down and quit
	logMsg("OSC server exiting.");
	lo_server_thread_stop(sSrvThrd);
	IO * io = IO :: theInstance();
	io->stop();			// stop it
	io->close();	
	exit(0);
}

// OSC support functions

// Set up the receive Q

extern "C" void initOSCReceive() {

					/* start a new server on port 7770 */
	sSrvThrd = lo_server_thread_new("7770", error);
}

// Main OSC loop -- select - read - dispatch

extern "C" void mainOSCLoop(int port) {
					/* add method that will match any path and args */
    lo_server_thread_add_method(sSrvThrd, NULL, NULL, generic_handler, NULL);
					/* add method that will match the path /quit with no args */
    lo_server_thread_add_method(sSrvThrd, "/quit", "", quit_handler, NULL);
					// start lo!
    lo_server_thread_start(st);

    while (!done) {
#ifdef WIN32
    Sleep(1);
#else
	usleep(1000);
#endif
    }
quit:
	quitCSL();
    return 0;
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


// IOSC setup methods

extern "C" void initOSCAddrSpace() {
}

extern "C" void addOSCMethod(char * name, OSC_CALL_BACK function ) {
}

extern "C" void addOSCMethod2(char * name, OSC_CALL_BACK function, void * user_data) {
}

// Set up the OSC address space for an instrument library

extern "C" void setupOSCInstrLibrary(std::vector<Instrument *> library) {
	char * iname = NULL;
	Inst_Context * the_context = NULL;
	unsigned num_instruments = library.size();
	logMsg("Setting up OSC address space for %d instruments\n", num_instruments);
	
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
