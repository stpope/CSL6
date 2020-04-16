//
//  iphoneIO.h -- DAC IO using CoreAudio for the iphone
//	Abstract AUIO class and concrete iPhoneIO class
//
//	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
//

#ifndef CSL_CAIO_H
#define CSL_CAIO_H

#include "CSL_Core.h"

#import <AudioToolbox/AudioToolbox.h>

using namespace std;

namespace csl {
	
///
/// General-purpose AudioUnit IO class
///

class AUIO : public IO {
public:	
	AUIO();
	AUIO(unsigned s_rate, unsigned b_size, int in_device, int out_device, 
			unsigned in_chans, unsigned out_chans);
	~AUIO();
	
	virtual void open() throw(CException);		///< open/close start/stop methods
	virtual void close() throw(CException);
	virtual void start() throw(CException);
	virtual void stop() throw(CException);
	
	void setAudioUnit(AudioUnit au) { mAudioUnit = au; };
	virtual Buffer & getInput() throw(CException);		///< get the current input buffer
												/// get the current input buffer
	virtual Buffer & getInput(unsigned numFrames, unsigned numChannels) throw(CException);
	
protected:
	AudioComponentInstance mAudioUnit;			///< The AudioUnit we play out
	void handleError(OSStatus result) throw(CException);
};

///
/// CoreAudio IO class for the iPhone
///

class iPhoneIO : public AUIO {
public:
	iPhoneIO();									/// Most verbose constructor -- specify everything
	iPhoneIO(unsigned s_rate, unsigned b_size, int in_device, int out_device, 
			unsigned in_chans, unsigned out_chans);
	~iPhoneIO();
	
	void open() throw(CException);				///< open/close start/stop methods
	void close() throw(CException);
	void start() throw(CException);
	void stop() throw(CException);
	
protected:
	void handleError(OSStatus result) throw(CException);
	
};
	
}

#endif
