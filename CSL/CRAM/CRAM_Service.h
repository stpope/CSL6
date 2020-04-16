//
// CSL interface and main() function for use with CRAM: the CREATE Real-time Applications Manager
// See the copyright and acknowledgment of authors in the file COPYRIGHT
//
//  The CSLService class is the wrapper used to manage CSL programs from CRAM.
//  This file has the main() function that starts any of a number of different CSL services under the control of CRAM.
//
// Usage:
//  CSL_Server service_name listener_port emergency_port CSL_function [-o UDP_IO-port] [-i RemoteFrameStream-in-IP/port]
//		The name and l/e port arguments are used by the CRAM management system
//		The CSL function is a string to select from the built-in play routines (see the list below)
// Examples
//  CSL_Server CSL31 12345 12355 bells				-- play bells sending sound out to the local server
//  CSL_Server CSL31 12345 12355 speakers -o 33123	-- play voices listening for client requests on port 33123
//													-- mix stereo sample streams from the two given servers
//  CSL_Server CSL31 12345 12355 mixer -i 128.111.92.51 12445 -i 128.111.92.60 12445
//
// The CSL_Function selects from the various available services:
//		bells		texture of random FM bells
//		noise		swept filtered noise
//		speakers	phoneme streams
//		mixer		8-in 2-out network mixer
//
// The optional arguments 
//		-o port-num -- for UDP_IO clients, to tell them their listener ports
//		-i client-IP client-port -- for processors, to identify their inputs
//		-a  -- for auto-start (run stand-alone without waiting for CRAM start message)
//
// The mixer takes additional arguments to describe its clients
//

#ifndef __CSL_CRAM_INCLUDE__
#define __CSL_CRAM_INCLUDE__

#include "Server.h"
#include <stdarg.h> 	// for va_arg, va_list, etc.

// Logger * mLogger;

namespace csl {

using namespace std;
using namespace cram;

// The CSL CRAM Service class

class CSLService : public Server {

public:		
	void start(void);			// start method
	void stop(void);			// stop method	
	void suspend(void);			// suspend method
	void resume(void);			// resume method
								// set an option value
	void set_opt(const char * nam, const char * val);

						// Constructors
	CSLService(char * name, unsigned short lport, unsigned short eport, char * which);
	~CSLService();
	
	pthread_t mThread;			// CSL test thread
	char * mWhich;				// type of CSL service
	
};

}

#endif

