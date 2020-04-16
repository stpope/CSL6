//
//  CAIO.cpp -- DAC IO using CoreAudio
//	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
//

#include "CAIO.h"

#include "CAAudioHardwareDevice.h"
#include "CAAudioHardwareSystem.h"

using namespace csl;

// This callback routine will be called by CoreAudio/AudioUnits

static ComponentResult RenderCallback (void * userData, AudioUnitRenderActionFlags * ioActionFlags, 
						const AudioTimeStamp * inTimeStamp, UInt32 inOutputBusNumber, 
						UInt32 inNumberFrames, AudioBufferList * ioData) {
	AUIO * auio = (AUIO *) userData;		// cast uder data ptr
	unsigned numChannels = ioData->mNumberBuffers;				// assumes non-interleaved buffers
	Buffer * auBuffer = & auio->mOutputBuffer;			// temp buffer for passing ptrs to CSL
	for (unsigned i = 0; i < numChannels; i++)						// copy IO data ptrs to buffer
		auBuffer->mBuffers[i] = (sample *) ioData->mBuffers[i].mData;
	auBuffer->mAreBuffersAllocated = true;							// set up buffer
	auBuffer->mDidIAllocateBuffers = false;
	auBuffer->mIsPopulated = false;
	auio->mNumFramesPlayed += inNumberFrames;
	auio->pullInput( * auBuffer);
	return noErr;	
}

// AUIO Constructors

AUIO :: AUIO() : IO() { }

AUIO :: AUIO(unsigned s_rate, unsigned b_size, int in_device, int out_device, unsigned in_chans, unsigned out_chans)
				: IO(s_rate, b_size, in_device, out_device, in_chans, out_chans) { }

AUIO :: ~AUIO() { }

void AUIO :: handleError(OSStatus err) throw(CException) {
	logMsg(kLogError, "An error occured while using the AudioUnit default output\n");
	logMsg(kLogError, "Error number: %d\n", (int)err);
	throw IOError("CoreAudio error");
}

// Open plugs in the callback function

void AUIO :: open() throw(CException) {
#ifdef DO_TIMING
	mThisSec = mTimeVals = mTimeSum = 0;
#endif
	OSStatus result;
	AURenderCallbackStruct renderCallback;
	renderCallback.inputProc = RenderCallback;
	renderCallback.inputProcRefCon = this;
										// plug in the audio renderer callback
	result = AudioUnitSetProperty (mAudioUnit, kAudioUnitProperty_SetRenderCallback, kAudioUnitScope_Input, 0, 
							&renderCallback, sizeof(renderCallback));
	if (noErr != result)
		handleError(result);
}

// Start/stop

void AUIO :: start() throw(CException) {
	mNumFramesPlayed = 0;
}

void AUIO :: stop() throw(CException) {
	// don't need to do anything 
}

// Close stop the callback

void AUIO :: close() throw(CException) {
	OSStatus result;
	AURenderCallbackStruct renderCallback;
	renderCallback.inputProc = NULL;
	renderCallback.inputProcRefCon = this;
	result = AudioUnitSetProperty (mAudioUnit, kAudioUnitProperty_SetRenderCallback, kAudioUnitScope_Input, 0, 
							&renderCallback, sizeof(renderCallback));
	if (noErr != result)
		handleError(result);
}

// Answer the most recent input buffer

Buffer & AUIO ::  getInput() throw(CException) {
	return getInput(mInputBuffer.mNumFrames, mInputBuffer.mNumChannels);
}

// really get the desired format of input

Buffer & AUIO ::  getInput(unsigned numFrames, unsigned numChannels) throw(CException) {
	if (mNumInChannels == 0)
		throw IOError("Can't get unopened input");
//	Interleaver interleaver;
//	interleaver.deinterleave(mInputBuffer, mInputPointer, numFrames, numChannels);
	mInputBuffer.mIsPopulated = true;
	return(mInputBuffer);
}

/////////////////////// CoreAudio IO /////////////////////////////////

// Constructor creates input/output device list

extern vector < IODevice *> gIODevices;			// Global list of all known IO devices

// CAIO constructor -- verbose version loads a table of the CAIO devices

CAIO :: CAIO() : AUIO() { }

CAIO :: CAIO(unsigned s_rate, unsigned b_size, int in_device, int out_device, unsigned in_chans, unsigned out_chans)
				: AUIO(s_rate, b_size, in_device, out_device, in_chans, out_chans) {
	if (gIODevices.empty()) {					// if we've never set up before	
		UInt32 numDevs = CAAudioHardwareSystem::GetNumberDevices();
		if (numDevs > 0) {					// if you found any devices
			logMsg("");
			logMsg("IO Table");
			UInt32 devIndex = 0;
			AudioDeviceID defInID = CAAudioHardwareSystem::GetDefaultDevice(kAudioDeviceSectionInput, false);
			AudioDeviceID defOutID = CAAudioHardwareSystem::GetDefaultDevice(kAudioDeviceSectionOutput, false);
			while (devIndex < numDevs) {		// loop over devices			
				IODevice * newDev = new IODevice;
				CAAudioHardwareDevice theDevice(CAAudioHardwareSystem::GetDeviceAtIndex(devIndex));
				newDev->mIndex = theDevice.GetAudioDeviceID();			// copy index, name, rates, etc.
				CFStringGetCString(theDevice.CopyName(), newDev->mName, CSL_NAME_LEN, 0);
				newDev->mFrameRate = (float) theDevice.GetNominalSampleRate();
											// get # of I/O channels for device
				newDev->mMaxInputChannels = theDevice.GetTotalNumberChannels(kAudioDeviceSectionInput);
				newDev->mMaxOutputChannels = theDevice.GetTotalNumberChannels(kAudioDeviceSectionOutput);
				if (newDev->mIndex == defInID)
						newDev->mIsDefaultIn = 1;
				else		newDev->mIsDefaultIn = 0;
				if (newDev->mIndex == defOutID)
						newDev->mIsDefaultOut = 1;
				else		newDev->mIsDefaultOut = 0;
				gIODevices.push_back(newDev);					// push onto global list
				++devIndex;
									// Print out the CoreAudio device list
				newDev->dump();
			}
		}
		logMsg("");
	}
}

CAIO :: ~CAIO() { }

// Error fnuction

void CAIO :: handleError(OSStatus result) throw(CException) {
	AudioUnitUninitialize(mAudioUnit);
	CloseComponent(mAudioUnit);
	AUIO::handleError(result);
}

// Open sets up CAIO

void CAIO :: open() throw(CException) {					// Get the output audio unit
	OSStatus result = noErr;
	ComponentDescription desc;
						// get the cmoponent
	desc.componentType = kAudioUnitType_Output;
	desc.componentSubType = kAudioUnitSubType_DefaultOutput;
	desc.componentManufacturer = kAudioUnitManufacturer_Apple;
	desc.componentFlags = 0;
	desc.componentFlagsMask = 0;
	Component comp = FindNextComponent(NULL, &desc);
						// check it
	if (comp == NULL) 
		logMsg(kLogError, "An error occured while opening the AudioUnit default output\n");
						// open up
	result = OpenAComponent(comp, &mAudioUnit);
	if (noErr != result)
		handleError(result);
						// AU init!
	result = AudioUnitInitialize (mAudioUnit);
	if (noErr != result)
		handleError(result);
						// AU open
	AUIO::open();
}

void CAIO :: start() throw(CException) {
	OSStatus result = AudioOutputUnitStart(mAudioUnit);	// Start 'er up!
	if (noErr != result)
		handleError(result);
}

void CAIO :: stop() throw(CException) {
	OSStatus result = AudioOutputUnitStop(mAudioUnit);
	if (noErr != result)
		handleError(result);
}

void CAIO :: close() throw(CException) {
	AudioUnitUninitialize(mAudioUnit);
	CloseComponent(mAudioUnit);
}
