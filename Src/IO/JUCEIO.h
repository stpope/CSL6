//
//  JUCEIO.h -- Driver IO object for CSL on JUCE
//	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
//

#ifndef CSL_JUCEIO_H
#define CSL_JUCEIO_H

#include "CSL_Core.h"
//#include <juce.h>

namespace csl {

///
/// JUCEIO is an IO that runs as a JUCE
///

class JUCEIO : public IO, public juce::AudioIODeviceCallback {
public:				
											///< Constructor (stereo by default)
	JUCEIO(unsigned s_rate = CSL_mFrameRate, unsigned b_size = CSL_mBlockSize, 
			int in_device = 0, int out_device = 0, 
			unsigned in_chans = 0, unsigned out_chans = 2);
	virtual ~JUCEIO();

	void open() throw(CException);			///< open/close start/stop methods
	void close() throw(CException);
	void start() throw(CException);			///< start my timer thread
	void stop() throw(CException);			///< stop the timer thread

											///< Audio playback callback & utilities
	void audioDeviceIOCallback (const float** inputChannelData, int totalNumInputChannels,
									  float** outputChannelData, int totalNumOutputChannels,
									  int numSamples);
											/// JUCE methods
	void audioDeviceAboutToStart (juce::AudioIODevice*) { };
	void audioDeviceStopped() { };

protected:
    juce::AudioDeviceManager audioDeviceManager;	///< JUCE AudioDeviceManager

};

}	// end of namespace

#endif // CSL_JACKIO_H
