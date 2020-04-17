//
//  iphoneIO.cpp -- DAC IO using CoreAudio for the iphone
//		Abstract AUIO class and concrete CAIO class
//	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
//

#include "iphoneIO.h"

using namespace csl;

// This callback routine will be called by CoreAudio/AudioUnits

static OSStatus RenderCallback(void * userData, AudioUnitRenderActionFlags * ioActionFlags, 
								const AudioTimeStamp * inTimeStamp, UInt32 inOutputBusNumber, 
								UInt32 inNumberFrames, AudioBufferList * ioData) {
	AUIO * auio = (AUIO *) userData;					// cast uder data ptr
	unsigned numChannels = ioData->mBuffers[0].mNumberChannels;	// interleaved buffers
	Buffer * auBuffer = &auio->mOutputBuffer;			// temp buffer for passing ptrs to CSL
	
	if ( ! auio->mGraph)								// if graph not set
		return noErr;

	auBuffer->mNumChannels = numChannels;				// set up buffer
	auBuffer->mNumFrames = inNumberFrames;
	
	auio->pullInput(*auBuffer);							////// get data from the DSP graph //////
	
	auio->mNumFramesPlayed += inNumberFrames;
	
	sample * samp = auBuffer->mBuffers[0];				// buffer pointer
	SInt16 * outData = (SInt16 *) ioData->mBuffers[0].mData;
	SInt16 final;
	if (numChannels == 1) {								// mono case
		for (unsigned i = 0; i < inNumberFrames; i++) {	// output copy loop
			final = (SInt16) (*samp++ * 32768.0f);
		//	final += final << 16;						// ??
			*outData++ = final;
		}
	} else {											// stereo case; interleave samples
		sample * samp2 = auBuffer->mBuffers[1];
		for (unsigned i = 0; i < inNumberFrames; i++) {
			CHECK_SAMP(samp)
			final = (SInt16) (*samp++ * 32768.0f);
			*outData++ = final;
			CHECK_SAMP(samp2)
			final = (SInt16) (*samp2++ * 32768.0f);
			*outData++ = final;
		}
	}
	return noErr;	
}

// AUIO Constructors

AUIO :: AUIO() : IO() {
	mOutputBuffer.setSize(2, CGestalt::blockSize());		// allocate extra just for sure?
	mOutputBuffer.allocateBuffers();
}

AUIO :: AUIO(unsigned s_rate, unsigned b_size, int in_device, int out_device, 
			unsigned in_chans, unsigned out_chans)
: IO(s_rate, b_size, in_device, out_device, in_chans, out_chans) { }

AUIO :: ~AUIO() { }

void AUIO :: handleError(OSStatus err) throw(CException) {
	logMsg(kLogError, "An error occured while using the AudioUnit");
	logMsg(kLogError, "Error number: %d", (int) err);
	throw IOError("AUIO error");
}

// Open 

void AUIO :: open() throw(CException) { }

// Start/stop plugs in the callback function

void AUIO :: start() throw(CException) {
	mNumFramesPlayed = 0;
#ifdef DO_TIMING
	mThisSec = mTimeVals = mTimeSum = 0;
#endif
	OSStatus result;
	AURenderCallbackStruct renderCallback;
	
	renderCallback.inputProc = RenderCallback;
	renderCallback.inputProcRefCon = this;
												// plug in the audio renderer callback
	result = AudioUnitSetProperty (mAudioUnit, kAudioUnitProperty_SetRenderCallback, 
							kAudioUnitScope_Input, 0, &renderCallback, sizeof(renderCallback));
	if (noErr != result)
		handleError(result);
}

void AUIO :: stop() throw(CException) {
	OSStatus result;
	AURenderCallbackStruct renderCallback;
	
	renderCallback.inputProc = NULL;			//clear the callback pointer
	renderCallback.inputProcRefCon = this;
	
	result = AudioUnitSetProperty (mAudioUnit, kAudioUnitProperty_SetRenderCallback, 
							kAudioUnitScope_Input, 0, &renderCallback, sizeof(renderCallback));
	if (noErr != result)
		handleError(result);
}

// Close -- don't need to do anything 

void AUIO :: close() throw(CException) { }

// Answer the most recent input buffer

Buffer & AUIO ::  getInput() throw(CException) {
	return getInput(mInputBuffer.mNumFrames, mInputBuffer.mNumChannels);
}

// really get the desired format of input

Buffer & AUIO ::  getInput(unsigned numFrames, unsigned numChannels) throw(CException) {
	if (mNumInChannels == 0)
		throw IOError("Can't get unopened input");
	mInputBuffer.mIsPopulated = true;
	return(mInputBuffer);
}

/////////////////////// iPhoneIO IO /////////////////////////////////

// Constructor creates input/output device list

extern vector < IODevice *> gIODevices;			// Global list of all known IO devices

// iphoneIO constructor -- verbose version loads a table of the iphoneIO devices

iPhoneIO :: iPhoneIO() : AUIO() { }

// this version is not used

iPhoneIO :: iPhoneIO(unsigned s_rate, unsigned b_size, 
					int in_device, int out_device, unsigned in_chans, unsigned out_chans)
			: AUIO(s_rate, b_size, in_device, out_device, in_chans, out_chans) {
//	if (gIODevices.empty()) {					// if we've never set up before	
//		IODevice * newDev = new IODevice("iPhoneIO", 0, 1, 2, true, true);
//		newDev->mFrameRate = 44100;
//		gIODevices.push_back(newDev);			// push onto global list
//		newDev->dump();
//	} else {
//		for (unsigned i = 0; i < gIODevices.size(); i++)
//			gIODevices[i]->dump();
//	}
}

iPhoneIO :: ~iPhoneIO() { }

// Error function

void iPhoneIO :: handleError(OSStatus result) throw(CException) {
	AudioUnitUninitialize(mAudioUnit);
	AUIO::handleError(result);
}

// Open sets up iPhoneIO

void iPhoneIO :: open() throw(CException) {					// Get the output audio unit
	OSStatus result = noErr;
	AudioComponentDescription desc;
	desc.componentType = kAudioUnitType_Output;				// set up the component description
				// kAudioUnitType_Output = kAudioFormatFlagIsSignedInteger 
				//		+ kAudioFormatFlagIsPacked + kAudioFormatFlagIsNonInterleaved 
				//		+ (kAudioUnitSampleFractionBits << kLinearPCMFormatFlagsSampleFractionShift)
	desc.componentSubType = kAudioUnitSubType_RemoteIO; 
	desc.componentManufacturer = kAudioUnitManufacturer_Apple;
	desc.componentFlags = 0;
	desc.componentFlagsMask = 0;
															// find a component
	AudioComponent comp = AudioComponentFindNext(NULL, &desc);
	if (comp == NULL) {										// check it
		logMsg(kLogError, "An error occured while finding the AudioUnit default output\n");
		throw IOError("CoreAudio error in AudioComponentFindNext");
	}
	result = AudioComponentInstanceNew(comp, &mAudioUnit);	// open it up
	if (noErr != result)
		handleError(result);

	result = AudioUnitInitialize(mAudioUnit);				// AU init it
	if (noErr != result)
		handleError(result);

	AudioStreamBasicDescription audioFormat;
	audioFormat.mSampleRate = (Float64) CGestalt::frameRate();	// FS
	audioFormat.mFormatID = kAudioFormatLinearPCM;
	audioFormat.mFormatFlags = kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsPacked; 
	audioFormat.mFramesPerPacket = 1;
	audioFormat.mChannelsPerFrame = 2;			// stereo
	audioFormat.mBitsPerChannel = 16;			// 16-bit
	audioFormat.mBytesPerPacket = 4;
	audioFormat.mBytesPerFrame = 4;

	result = AudioUnitSetProperty(mAudioUnit,
							   kAudioUnitProperty_StreamFormat,
							   kAudioUnitScope_Output,
							   0,
							   &audioFormat,
							   sizeof(audioFormat));
	if (noErr != result)
		handleError(result);

//	result = AudioUnitInitialize (mAudioUnit);	// AU init it
//	if (noErr != result)
//		handleError(result);

	AUIO::open();								// AU open
//	logMsg("AUIO::open OK");
	
												// Initialize and configure the audio session
	AudioSessionInitialize(NULL, NULL, NULL, this);
	
												// set the audio category
	UInt32 audioCategory = kAudioSessionCategory_LiveAudio;
	AudioSessionSetProperty(kAudioSessionProperty_AudioCategory, sizeof(audioCategory), &audioCategory); 
	UInt32 getAudioCategory = sizeof(audioCategory);
	AudioSessionGetProperty(kAudioSessionProperty_AudioCategory, &getAudioCategory, &getAudioCategory);
	
												// set block rate 
	Float32 preferredBufferSize = (float) CGestalt::blockSize() / (float) CGestalt::frameRate(); 
	
	AudioSessionSetProperty (kAudioSessionProperty_PreferredHardwareIOBufferDuration, 
								sizeof(preferredBufferSize), &preferredBufferSize); 

												// set the audio session active
	AudioSessionSetActive(true);
	
				// ask whether we support stereo output
//	UInt32 numOutputChannels;
//	UInt32 dataSize = sizeof(UInt32);
//	result = AudioSessionGetProperty(kAudioSessionProperty_CurrentHardwareOutputNumberChannels, 
//			& dataSize, & numOutputChannels);
//	logMsg("AudioUnit outputs: %d\n", numOutputChannels);
}

void iPhoneIO :: start() throw(CException) {
	AUIO::start();	
	OSStatus result = AudioOutputUnitStart(mAudioUnit);		// Start 'er up!
	if (noErr != result)
		handleError(result);
	logMsg("iPhoneIO :: start");
}

void iPhoneIO :: stop() throw(CException) {
	AUIO::stop();
	OSStatus result = AudioOutputUnitStop(mAudioUnit);
	if (noErr != result)
		handleError(result);
	logMsg("iPhoneIO :: stop");
}

void iPhoneIO :: close() throw(CException) {
	AudioUnitUninitialize(mAudioUnit);
	//CloseComponent(mAudioUnit);
	AUIO::close();
}
