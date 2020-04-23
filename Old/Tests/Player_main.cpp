//

//	Player_main.cpp --   A CSL streaming player program.

//	See the copyright notice and acknowledgment of authors in the file COPYRIGHT

//



// NOTE: STALE DON'T COMPILE THIS



#include "CSL_Includes.h"		// Basic CSL kernel include

#include "PAIO.h"			// include 3 IO option

#include "FileIO.h"

#ifndef CSL_WINDOWS

#include "CAIO.h"s

#endif

#include "TCP_RemoteStream.h"



using namespace csl;



PAIO io;				// PortAudio IO object

//CAIO io;				// CoreAudio IO object

//FileIO io("csl_test.aiff");		// File IO object



// MAIN -- read command-line arguments and play input from remote streams



// ToDo: cmd-line opts for hostname, socket, # channels, sRate, etc.



int main (int argc, const char * argv[]) {

	unsigned block_size = 1024;

	

	TCP_RemoteStream rfs("localhost", CSL_DEFAULT_REQUEST_PORT, CSL_DEFAULT_RESPONSE_PORT, 2, block_size);



	logMsg("CSL playing...");

	io.set_root(rfs);						// make some sound

	io.open();

	io.start();



	while(1)

		usleep((int) (1000000));				// sleep



	io.stop();								// close down

	io.close();	



	return 0;

}

