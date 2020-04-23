//
//  JackIO.h -- Driver IO object for CSL on Jack
//	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
//

#ifndef CSL_JACKIO_H
#define CSL_JACKIO_H

#include "CSL_Core.h"

#include <jack/jack.h>

namespace csl {

//extern "C" int JackCallback (jack_nframes_t nframes, void *arg);

///
/// JackIO is an IO that uses a thread and a timer to call its graph's nextBuffer(); it doesn't do
/// anything with the resulting buffer, but assumes that other objects (observers of the graph)
/// will handle some actual IO. 
///
class JackIO : public IO {
public:				
	JackIO();						///< Constructor
	JackIO(unsigned s_rate, unsigned b_size, 
		int in_device = 0, int out_device = 0, 
		unsigned in_chans = 0, unsigned out_chans = 2);
	virtual ~JackIO();
	
	void open() throw(CException);			///< open/close start/stop methods
	void close() throw(CException);
	void start() throw(CException);			///< start my timer thread
	void stop() throw(CException);			///< stop the timer thread

	jack_port_t * mInput_port;
	jack_port_t * mOutput_port;
	jack_client_t * mClient;
};


}	// end of namespace

#endif CSL_JACKIO_H

