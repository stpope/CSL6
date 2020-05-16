//
// Header file to include the OSC functions in CSL as externs
//
// See the copyright notice and acknowledgment of authors in the file COPYRIGHT
//

#ifndef OSC_CPP_INCLUDE
#define OSC_CPP_INCLUDE

#include "CSL_Includes.h"
#include "Instrument.h"

//
// READ_CSL_OPTS macro -- Parse the standard CSL cmd-line options and set globals based on their values
//

#define READ_CSL_OPTS									\
unsigned val;											\
if (find_option(argc, argv, 'r', &val) > 0) {			\
	CGestalt::setFrameRate(atoi(argv[val]));			\
	printf("Setting frame rate to: %d\n",				\
	CGestalt::frameRate()); }							\
if (find_option(argc, argv, 'b', &val) > 0) {			\
	CGestalt::setBlockSize(atoi(argv[val]));			\
	printf("Setting block size to: %d\n",				\
	CGestalt::blockSize()); }							\
if (find_option(argc, argv, 'o', &val) > 0) {			\
	CGestalt::setNumOutChannels(atoi(argv[val]));		\
	printf("Setting numOutChannels to: %d\n",			\
	CGestalt::numOutChannels()); }						\
if (find_option(argc, argv, 'i', &val) > 0) {			\
	CGestalt::setNumInChannels(atoi(argv[val]));		\
	printf("Setting numInChannels to: %d\n",			\
	CGestalt::numInChannels()); }						\
if (find_option(argc, argv, 'l', &val) > 0) {			\
	CGestalt::setLoggingPeriod(atoi(argv[val]));		\
	printf("Setting loggingPeriod to: %d\n",			\
	CGestalt::loggingPeriod()); }						\
if (find_option(argc, argv, 'p', &val) > 0) {			\
	CGestalt::setOutPort(atoi(argv[val]));				\
	printf("Setting outPort to: %d\n",					\
	CGestalt::outPort()); }								\
if ((find_option(argc, argv, 'u', &val) >= 0) ||		\
	(find_option(argc, argv, 'h', &val) >= 0)) {		\
		usage(argv[0]);									\
		exit(0); }

//if (find_option(argc, argv, 'v', &val) > 0)				\
//	printf("Setting CSL verbosity\n");					\
//	if (argv[val][0] == '-')							\
//		CGestalt::setVerbosity(2);						\
//	else												\
//		CGestalt::setVerbosity(atoi(argv[val]))


int find_option(int argc, const char ** argv, char flag, unsigned * datum);

void usage(const char * name);

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
