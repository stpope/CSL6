//
// Header file to include the OSC functions in CSL as externs
//
// See the copyright notice and acknowledgment of authors in the file COPYRIGHT
//

#ifndef OSC_CPP_INCLUDE
#define OSC_CPP_INCLUDE

#include "CSL_Includes.h"
#include "Instrument.h"

#ifdef USE_LOSC							// liblo for OSC

using namespace csl;

										/// init function
extern "C" void initOSC(const char * port);

										// setup instr library address space
extern "C" void setupOSCInstrLibrary(InstrumentVector library);

										// OSC read/exec loop
extern "C" void mainOSCLoop(void *);

#endif

#endif
