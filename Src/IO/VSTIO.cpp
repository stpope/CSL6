//
//  VSTIO.cpp -- DAC IO using VST
//	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
//

#include "VSTIO.h"

using namespace csl;

// Constructors

VSTIO * createEffectInstance (audioMasterCallback audioMaster) {
	return new VSTIO (audioMaster, CGestalt::frameRate(), CGestalt::blockSize(),
					CGestalt::numInChannels(), CGestalt::numOutChannels());
}

VSTIO::VSTIO() : IO(), AudioEffectX (audioMaster, 1, 1) { }
				
VSTIO::	VSTIO(audioMasterCallback audioMaster, 
					unsigned s_rate, unsigned b_size, 
					unsigned in_chans, unsigned out_chans)
			: IO(s_rate, b_size, 0, 0, in_chans, out_chans),
			  AudioEffectX (audioMaster, 1, 1) {		// 1 program, 1 parameter only
	setNumInputs(in_chans);		// stereo in
	setNumOutputs(out_chans);	// stereo out
	setUniqueID ('CSL_BP_Filter');	// identify
	canProcessReplacing ();		// supports replacing output
	mInterleaved = false;		// non-interleaved
	
								// set up InOut UGen to copy input to effect
	mInOut = new InOut(this, 2, 2, kNoProc, 1, 2);		// stereo i/o
								// create effect: a BPF filter
	mFilter = new Butter(*mInOut, BW_BAND_PASS, 1000.f, 200.f);
								// set its name
	mFilter->setName("CSL B-P Filter");
								// set it as the IO's root
	this->setRoot(*mFilter);
}

// Destructor

VSTIO::~VSTIO() { }

// open/close start/stop methods

void VSTIO::open() throw(CException) { }			

void VSTIO::close() throw(CException) { }

void VSTIO::start() throw(CException) { }			///< start my timer thread

void VSTIO::stop() throw(CException) { }			///< stop the timer thread

// Program

void VSTIO::setProgramName (char * name) {
	vst_strncpy ((char *)mGraph->name()->c_str(), name, kVstMaxProgNameLen);
}

void VSTIO::getProgramName (char * name) { 
	vst_strncpy (name, "CSL B-P Filter", kVstMaxProgNameLen);
//	vst_strncpy (name, (char *)mGraph->name()->c_str(), kVstMaxProgNameLen);
}

// Parameters

void VSTIO::setParameter (VstInt32 index, float value) {
	mFilter->setFrequency(value);
}

float VSTIO::getParameter (VstInt32 index) {
	Port * thePort = mFilter->getPort(CSL_FILTER_FREQUENCY);
	if (thePort)
		return thePort->nextValue();
	else
		return(0);
}

// Get a label from the Instrument

void VSTIO::getParameterLabel (VstInt32 index, char* label) { 
	vst_strncpy(label, "Hz", kVstMaxParamStrLen);
}

void VSTIO::getParameterDisplay (VstInt32 index, char* text) { 
	Hz2string(mFreq, text, kVstMaxParamStrLen);
}

void VSTIO::getParameterName (VstInt32 index, char* text) { 
	vst_strncpy(text, "Freq", kVstMaxParamStrLen);
}

bool VSTIO::getEffectName (char * name) { 
	vst_strncpy(name, "CSL B-P Filter", kVstMaxEffectNameLen);
	return true;
}

bool VSTIO::getVendorString (char * text) { 
	vst_strncpy(text, "CREATE / UCSB", kVstMaxVendorStrLen);
	return true;
}

bool VSTIO::getProductString (char * text) { 
	vst_strncpy(text, "CSL B-P Filter", kVstMaxProductStrLen);
	return true;
}

VstInt32 VSTIO::getVendorVersion () { 
	return 1000; 
}

// Processing

void VSTIO::processReplacing (float ** inputs, float ** outputs, VstInt32 sampleFrames) {
//	float* in1  =  inputs[0];
//	float* in2  =  inputs[1];
//	float* out1 = outputs[0];
//	float* out2 = outputs[1];
	
	if (mGraph) {
		mInputBuffer.setSize(mNumOutChannels, sampleFrames);
		mInputBuffer.checkBuffers();
								// copy data to mInputBuffer for the in-out
		for (unsigned i = 0; i < mNumOutChannels; i++)
			memcpy(mInputBuffer.buffer(i), inputs[i], sampleFrames * sizeof(VstInt32));
								// set up outbuf
		mOutputBuffer.setSize(mNumOutChannels, sampleFrames);
//		mOutputBuffer.checkBuffers();
								// copy JUCE data ptrs
		for (unsigned i = 0; i < mNumOutChannels; i++)
			mOutputBuffer.setBuffer(i, outputs[i]);
		try {					// Tell the IO to call its graph
			this->pullInput(mOutputBuffer);
		} catch (csl::CException e) {
			logMsg(kLogError, "Error running CSL: graph: %s\n", e.what());
		}
	}
}

///////////////////////////////////////////////////////////////////////////
//
// VSTInst

#if 0

#include "Instrument.h"
#include "BasicFMInstrument.h"
#include "AdditiveInstrument.h"

// Constructors

VSTInst * createEffectInstance (audioMasterCallback audioMaster) {
	Instrument * inst = new AdditiveInstrument;
	return new VSTInst (audioMaster, inst);
}

VSTInst::VSTInst() IO(), AudioEffectX (audioMaster, 1, 1) { }
				
VSTInst::	VSTInst(audioMasterCallback audioMaster, Instrument * theInstrument, 
					unsigned s_rate, unsigned b_size, 
					unsigned in_chans, unsigned out_chans)
			: IO(s_rate, b_size, 0, 0, in_chans, out_chans),
				AudioEffectX (audioMaster, 1, 1) {				// 1 program, 1 parameter only
	setNumInputs (in_chans);		// stereo in
	setNumOutputs (out_chans);		// stereo out
	setUniqueID ('Gain');	// identify
	canProcessReplacing ();	// supports replacing output
}

VSTInst::~VSTInst() { }
	
void VSTInst::open() throw(CException) { }			///< open/close start/stop methods

void VSTInst::close() throw(CException) { }

void VSTInst::start() throw(CException) { }			///< start my timer thread

void VSTInst::stop() throw(CException) { }			///< stop the timer thread

// Program

void VSTInst::setProgramName (char* name) { 
	vst_strncpy (mInstrument->mName, name, kVstMaxProgNameLen);
}

void VSTInst::getProgramName (char* name) { 
	vst_strncpy (name, mInstrument->mName, kVstMaxProgNameLen);
}

// Parameters

void VSTInst::setParameter (VstInt32 index, float value) {

}

float VSTInst::getParameter (VstInt32 index) {

}

// Get a label form the Instrument

void VSTInst::getParameterLabel (VstInt32 index, char* label) { 
	vst_strncpy (label, "dB", kVstMaxParamStrLen);
}

void VSTInst::getParameterDisplay (VstInt32 index, char* text) { 
	dB2string (fGain, text, kVstMaxParamStrLen);
}

void VSTInst::getParameterName (VstInt32 index, char* text) { 
	vst_strncpy (label, "Gain", kVstMaxParamStrLen);
}

bool VSTInst::getEffectName (char* name) { 
	vst_strncpy (name, "Gain", kVstMaxEffectNameLen);
	return true;
}

bool VSTInst::getVendorString (char* text) { 
	vst_strncpy (text, "CREATE / UCSB", kVstMaxVendorStrLen);
	return true;
}

bool VSTInst::getProductString (char* text) { 
	vst_strncpy (text, "Gain", kVstMaxProductStrLen);
	return true;
}

VSTInst::VstInt32 getVendorVersion () { 
	return 1000; 
}

// Processing

void VSTInst::processReplacing (float** inputs, float** outputs, VstInt32 sampleFrames) {
	float* in1  =  inputs[0];
	float* in2  =  inputs[1];
	float* out1 = outputs[0];
	float* out2 = outputs[1];
	
	while (--sampleFrames >= 0) {
		(*out1++) = (*in1++) * fGain;
		(*out2++) = (*in2++) * fGain;
	}
}

#endif
