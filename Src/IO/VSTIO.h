//
//  VSTIO.h -- IO using VSTIO 
//
//	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
//

#ifndef CSL_VSTIO_H
#define CSL_VSTIO_H

#include "CSL_Core.h"
#include "Instrument.h"

#include "audioeffectx.h"

namespace csl {

///
/// VSTIO is an IO that answers the VST processReplacing() call by calling its CSL graph
///

class VSTIO : public IO, public AudioEffectX {
public:				
	VSTIO ();						///< Constructor
	VSTIO (audioMasterCallback audioMaster, 
					unsigned s_rate = 44100, unsigned b_size = 512, 
					unsigned in_chans = 2, unsigned out_chans = 2);
	virtual ~VSTIO();

	void open() throw(CException);			///< open/close start/stop methods
	void close() throw(CException);
	void start() throw(CException);			///< start my timer thread
	void stop() throw(CException);			///< stop the timer thread

					// Processing
	virtual void processReplacing (float** inputs, float** outputs, VstInt32 sampleFrames);
	virtual void processDoubleReplacing (double** inputs, double** outputs, VstInt32 sampleFrames) { };

					// Program
	virtual void setProgramName (char* name);
	virtual void getProgramName (char* name);

					// Parameters
	virtual void setParameter (VstInt32 index, float value);
	virtual float getParameter (VstInt32 index);
	virtual void getParameterLabel (VstInt32 index, char* label);
	virtual void getParameterDisplay (VstInt32 index, char* text);
	virtual void getParameterName (VstInt32 index, char* text);

	virtual bool getEffectName (char* name);
	virtual bool getVendorString (char* text);
	virtual bool getProductString (char* text);
	virtual VstInt32 getVendorVersion ();
	
protected:
	InOut * mInOut;							///< the in-out object
	Filter * mFilter;						///< BPF filter
	float mFreq;							///< Example: filter center freq
	
};

#if 0

/// VSTInst is an instrument, get it?

class VSTInst : public IO, public AudioEffectX {
public:				
	VSTInst ();						///< Constructor
	VSTInst (audioMasterCallback audioMaster, Instrument * theInstrument, 
					unsigned s_rate = 44100, unsigned b_size = 512, 
					unsigned in_chans = 0, unsigned out_chans = 2);
	virtual ~VSTInst();
	
	void open() throw(CException);			///< open/close start/stop methods
	void close() throw(CException);
	void start() throw(CException);			///< start my timer thread
	void stop() throw(CException);			///< stop the timer thread

					// Processing
	virtual void processReplacing (float** inputs, float** outputs, VstInt32 sampleFrames);
	virtual void processDoubleReplacing (double** inputs, double** outputs, VstInt32 sampleFrames) { };

					// Program
	virtual void setProgramName (char* name);
	virtual void getProgramName (char* name);

					// Parameters
	virtual void setParameter (VstInt32 index, float value);
	virtual float getParameter (VstInt32 index);
	virtual void getParameterLabel (VstInt32 index, char* label);
	virtual void getParameterDisplay (VstInt32 index, char* text);
	virtual void getParameterName (VstInt32 index, char* text);

	virtual bool getEffectName (char* name);
	virtual bool getVendorString (char* text);
	virtual bool getProductString (char* text);
	virtual VstInt32 getVendorVersion ();

protected:
	Instrument * mInstrument;
};

#endif

}	// end of namespace

#endif
