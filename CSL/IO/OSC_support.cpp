//
//  OSC_support.cpp -- CSL support functions for use with OSC input via LibLo
//
//	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
//
// This file has a bunch of the "glue" code necessary to use OSC simply from within CSL.
// For an example, see OSC_main.cpp (which uses CSL instruymkents) GestureSensorTests.cpp, 
// which interfaces to the CREATE gesture sensors using OSC, or Server_Tests.cpp, which
// adds a few simple OSC commands for controlling the random bell textures.
//


#include "OSC_support.h"

#ifdef USE_LOSC							// liblo for OSC

#include "lo/lo.h"

// OSC globals

static int done = 0;						// running flag
static lo_server_thread sSrvThrd;		// the liblo thread guy

// error logger

void osc_error(int num, const char *msg, const char *path) {
    logMsg(kLogError, "OSC server error %d in path %s: %s\n", num, path, msg);
}

// Shut down and quit

void quitCSL() {
	done = 1;
	logMsg("OSC server exiting.");
	lo_server_thread_stop(sSrvThrd);
}

int quit_handler(const char *path, const char *types, lo_arg **argv, int argc,  void *data, void *user_data) {
	quitCSL();
	return 0;
}

// catch any incoming messages and display them

int generic_handler(const char *path, const char *types, lo_arg **argv, int argc, void *data, void *user_data) {
    logMsg("OSC message: <%s>", path);
    for (unsigned i=0; i<argc; i++) {
		printf("\t\targ %d '%c' ", i, types[i]);
		lo_arg_pp((lo_type)types[i], argv[i]);
		printf("\n");
    }
    printf("\n");
    fflush(stdout);
    return 1;
}

// Set up the LibLo OSC thread

extern "C" void initOSC(const char * thePort) {
	logMsg("initOSC");
	sSrvThrd = lo_server_thread_new(thePort, osc_error);
#ifdef CSL_DEBUG	// add method that will match any path and args
    lo_server_thread_add_method(sSrvThrd, NULL, NULL, generic_handler, NULL);
#endif				// add method that will match the path /q with no args and exit
    lo_server_thread_add_method(sSrvThrd, "/q", "", quit_handler, NULL);
}

// Main OSC loop -- select - read - dispatch

extern "C" void * mainOSCLoop(void * arg) {
	logMsg("mainOSCLoop starting");
	lo_server_thread_start(sSrvThrd);					// start lo!
	done = 0;
	while (!done)
		sleepSec(0.5);
	logMsg("mainOSCLoop returning");
	return NULL;
}

// Structure passed to OSC to hold a UGEN reference and a parameter selector

typedef struct {
	Instrument * instr;
	unsigned num;
	unsigned selector;
} Inst_Context;

// General-purpose type-checking parameter-setter methods for Instruments

int paramSetter(const char *path, const char *types, lo_arg **argv, int argc,  void *data, void *user_data) {
	Inst_Context * context = (Inst_Context *) user_data;
//	logMsg("OSC set %d/%d to (%d : %s) %g", (context->num + 1), context->selector, argc, types, ((float *) argv)[0]);
	context->instr->setParameter(context->selector, argc, (void **)argv, types);
	return 0;
}

// Play a note by re-triggering the main envelope

int playNote(const char *path, const char *types, lo_arg **argv, int argc,  void *data, void *user_data) {
	Inst_Context * context = (Inst_Context *) user_data;
	logMsg("OSC playNote on instr %d", (context->num + 1));
	context->instr->play();
	return 0;
}

// Play a note with arguments (passed to the instrument as a void *)

int playWithArgs(const char *path, const char *types, lo_arg **argv, int argc,  void *data, void *user_data) {
	Inst_Context * context = (Inst_Context *) user_data;
//	logMsg("OSC play_with_args on instr %d %s", (context->num + 1), types);
	context->instr->playOSC(argc, (void **) argv, types);
	return 0;
}

// Set up the OSC address space for an instrument library

extern "C" void setupOSCInstrLibrary(InstrumentVector library) {
	char iname[16];
	Inst_Context * the_context = NULL;
	unsigned num_instruments = library.size();
	logMsg("Setting up OSC address space for %d instruments", num_instruments);
					// Instrument reader loop
	for (unsigned i = 0; i < num_instruments; i++) {
		Instrument * instr = library[i]; 		// get the instrument and its accessors
		unsigned num_accessors = instr->numAccessors();
		AccessorVector accs = instr->getAccessors();
//		logMsg("Adding instrument %d (%s) %d accessors", i + 1, instr->name().c_str(), num_accessors);
									// Accessor method creation loop
		for (unsigned j = 0; j < num_accessors; j++) {
			the_context = (Inst_Context *) malloc(sizeof(Inst_Context));
			the_context->instr = instr;
			the_context->num = i;
			the_context->selector = accs[j]->mSelector;
			sprintf(iname, "/i%d/%s", i+1, accs[j]->mName.c_str());			// create address
#ifdef CSL_DEBUG
			if (i == 0)
				logMsg("\t\tAdding OSC method for %s (%d)", iname, accs[j]->mSelector);
#endif
			lo_server_thread_add_method(sSrvThrd, iname, NULL, paramSetter, the_context);		// float handles all other formats
		}
		sprintf(iname, "/i%d/p", i + 1);						// now add play commands
		lo_server_thread_add_method(sSrvThrd, iname, "", playNote, the_context);
		sprintf(iname, "/i%d/pn", i + 1);
		lo_server_thread_add_method(sSrvThrd, iname, NULL, playWithArgs, the_context);
	}
}

#endif
