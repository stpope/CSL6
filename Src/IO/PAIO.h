//
//  PAIO.h -- DAC IO using PortAudio
//	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
//

#ifndef CSL_PAIO_H
#define CSL_PAIO_H

#include "CSL_Core.h"

#include "portaudio.h"	// the portaudio header file in /usr/local/include

using namespace std;

namespace csl {

///
/// The PortAudio IO class
///

class PAIO : public IO {

public:
	PAIO(unsigned s_rate = CSL_mFrameRate, unsigned b_size = CSL_mBlockSize, 
			int in_device = -1, int out_device = -1, 
			unsigned in_chans = 0, unsigned out_chans = 2);
	~PAIO();					///< Destructor

	void open() throw(CException);			///< open the IO
	void start() throw(CException);			///< start the callbacks
	void stop() throw(CException);			///< stop the callbacks
	void close() throw(CException);			///< close the IO
	void test() throw(CException);			///< test the IO's graph

	PaStream * mStream;					///< the PortAudio stream we play out/get data from

protected:
	PaStreamParameters * mInputParameters;	///< PA IO stream parameters
	PaStreamParameters * mOutputParameters;
	
	PaDeviceIndex mInDev, mOutDev;			///< IO device numbers
	IODeviceVector mDevices;
											/// print the error message
	void handleError(PaError result) throw(CException);
											/// Actually initialize PortAudio driver
	void initialize(unsigned sr, unsigned bs, int is, int os, unsigned ic, unsigned oc);
};

}

#endif
