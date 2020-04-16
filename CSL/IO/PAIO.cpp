//
//  PAIO.cpp -- DAC IO using PortAudio
//	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
//

#include "PAIO.h"
#include <iostream>

using namespace csl;

// Constructors

//PAIO::PAIO() : IO() { 
//	Pa_Initialize();
//	mStatus = kIONew;
//}

PAIO::PAIO(unsigned sr, unsigned bs, int in_stream, int out_stream, unsigned in_chans, unsigned out_chans) 
			: IO(sr, bs, in_stream, out_stream, in_chans, out_chans) {
	Pa_Initialize();
	mStatus = kIONew;
	this->initialize(sr, bs, in_stream, out_stream, in_chans, out_chans);
}

PAIO::~PAIO() {
//#ifdef CSL_DEBUG
	logMsg("PAIO::destructor");
//#endif		
	if (mStatus == kIORunning)
		stop();
	if (mStatus == kIOOpen)
		close();
	if (mInputParameters != NULL)
		free(mInputParameters);
	if (mOutputParameters != NULL)
		free(mOutputParameters);
}

// The PortAudio callback function

static int pa_callback (const void * inputPointer, void * outputPointer, 
						unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo *outTime, 
						PaStreamCallbackFlags statusFlags, void * userData) {
	PAIO * paio = (PAIO *) userData;		// cast the user data pointer as a PAIO guy
	sample * out = (float *) outputPointer;
	if (paio->mStatus != kIORunning)
		return 0;
	if (paio->mGraph == 0) {			// if there's no input graph, just play silence...
//		memset(out, 0, (framesPerBuffer * numOutChannels * sizeof(sample)));
		return 0;
        }
	Buffer * outBuffer = & (paio->mOutputBuffer);
//	unsigned numOutChannels = outBuffer->mNumChannels;
	outBuffer->mNumFrames = framesPerBuffer;
#ifdef CSL_DEBUG
	logMsg("PAIO::callback (%x)", paio->mGraph);
#endif
	paio->mNumFramesPlayed += framesPerBuffer;
	if (paio->mNumInChannels > 0) {
		paio->mInputPointer = (float *) inputPointer;		// get the I/O buffer pointers
		paio->mInputBuffer.mNumFrames = framesPerBuffer;	// interleaved
	}
	paio->pullInput(*outBuffer, out);
	return 0;	
}

// PA error handler

void PAIO::handleError(PaError err) throw(CException) {
	Pa_Terminate();
	logMsg(kLogError, "An error occured while using PortAudio");
	logMsg(kLogError, "Error number: %d", err);
	logMsg(kLogError, "Error message: %s", Pa_GetErrorText(err));	
	throw IOError("Portaudio error");
}

// Open port audio

void PAIO::open() throw(CException) {
	PaError err;
	if (mStatus == kIOOpen)					// already open
		return;
	if (mStatus != kIOInit) {
		logMsg(kLogError, "Error opening PortAudio in state %d\n", mStatus);
		return;
	}						// Allocate the IO buffers
	if (mInputBuffer.mAreBuffersAllocated) 
		mInputBuffer.freeBuffers();
	mInputBuffer.allocateBuffers();	
	if (mOutputBuffer.mAreBuffersAllocated) 
		mOutputBuffer.freeBuffers();
	mOutputBuffer.allocateBuffers();	
//#ifdef CSL_DEBUG
	logMsg("PAIO::open");
//#endif
	err = Pa_OpenStream (&mStream,		// the big open_stream call
			mInputParameters,			// set up input device
			mOutputParameters,			// set up output device
			(double) CGestalt::frameRate(), 
			CGestalt::blockSize(),	
			0,							// stream flags
			pa_callback,					// our callback function (see above)
			this);						// user data = this PAIO object

	if (err != paNoError) {	
#ifdef CSL_DEBUG
	logMsg("Error opening PortAudio: %s", Pa_GetErrorText(err ));
#endif		
		throw IOError("Error opening PortAudio");		
	}
#ifdef DO_TIMING
	mThisSec = mTimeVals = mTimeSum = 0;
#endif
	mStatus = kIOOpen;
}

// Start PA IO

void PAIO::start() throw(CException) {
	PaError err;
	if (mStatus == kIORunning)			// already running
		return;
	if (mStatus != kIOOpen) {				// wrong state
		logMsg(kLogError, "Error starting PortAudio in state %d\n", mStatus);
		return;
	}
//#ifdef CSL_DEBUG
	logMsg("PAIO::start");
//#endif		
	err = Pa_StartStream(mStream);
	if (err != paNoError)
		handleError(err);
	mStatus = kIORunning;
}

// Stop

void PAIO::stop() throw(CException) {
	PaError err;
	if (mStatus != kIORunning) {
		logMsg(kLogError, "Error stopping PortAudio in state %d\n", mStatus);
		return;
	}
//#ifdef CSL_DEBUG
	logMsg("PAIO::stop");
//#endif		
	err = Pa_StopStream(mStream);
	if (err != paNoError)
		handleError(err);
	mStatus = kIOOpen;
}

// Close

void PAIO::close() throw(CException) {
	PaError err;
	if (mStatus != kIOOpen) {
		logMsg(kLogError, "Error closing PortAudio in state %d\n", mStatus);
		return;
	}
//#ifdef CSL_DEBUG
	logMsg("PAIO::close");
//#endif		
	err = Pa_CloseStream(mStream);
	if (err != paNoError)
		return handleError(err);
	if (mOutputBuffer.mAreBuffersAllocated) 
		mOutputBuffer.freeBuffers();
	if (mInputBuffer.mAreBuffersAllocated) 
		mInputBuffer.freeBuffers();
	Pa_Terminate();
	mStatus = kIOInit;
}

// test the IO's graph

void PAIO::test() throw(CException) {
	Buffer outBuffer(CGestalt::numOutChannels(), CGestalt::blockSize());
	outBuffer.allocateBuffers();
#ifdef CSL_DEBUG
	logMsg("PAIO::test");
#endif		
	if (mGraph != 0) {
		try {	
			mGraph->nextBuffer(outBuffer);
		} catch (CException ex) {	// handler: log error and play silence
			logMsg(kLogError, "An error occured in the CSL nextBuffer method: %s\n", ex.mMessage.c_str());	
			throw ex;		// swallow the error
		}
	}
}

// Protected init method does all the set up and device allocation

void PAIO::initialize(unsigned sr, unsigned bs, int is, int os, unsigned ic, unsigned oc) {
	const PaDeviceInfo *pdi;
	IODevice * devPtr;
										// set the global frame rate and block size
//#ifdef CSL_DEBUG
	logMsg("PAIO::init");
//#endif		
	if (sr != CGestalt::frameRate())
		CGestalt::setFrameRate(sr);
	if (bs != CGestalt::blockSize())
		CGestalt::setBlockSize(bs);
										// set the global # IO channels
	if (ic != CGestalt::numInChannels())
		CGestalt::setNumInChannels(ic);
	mNumInChannels = ic;
	if (oc != CGestalt::numOutChannels())
		CGestalt::setNumOutChannels(oc);
	mNumOutChannels = oc;
	
	if (is < 0)
			is = (int)Pa_GetDefaultInputDevice();
	if (os < 0)
			os = (int)Pa_GetDefaultOutputDevice();
			
	mChannelMap = new unsigned[mNumOutChannels]; // Allocate memory for the channel map
	
	for(unsigned i = 0; i < mNumOutChannels; i++) // initialize mapping all channels to themselves.
		mChannelMap[i] = i;

	mInputBuffer.setSize(ic, bs);			// set receiver's buffers
	mOutputBuffer.setSize(oc, bs);

	mNumRealInChannels = 0;
	mNumRealOutChannels = 0;
	PaDeviceIndex numDevices = Pa_GetDeviceCount();			// count the PA devices
	if (numDevices < 0 )
		logMsg(kLogFatal, "Pa_CountDevices returned 0x%x\n", numDevices);
										// iterate over the devices, adding PADevices to the mDevices vector
	for (int i = 0; i < numDevices; i++) {
		pdi = Pa_GetDeviceInfo(i);
		devPtr = new IODevice(pdi->name, i, 
						pdi->maxInputChannels, pdi->maxOutputChannels,
						(i == (int)Pa_GetDefaultInputDevice()), (i == (int)Pa_GetDefaultOutputDevice()));
		devPtr->mFrameRates.push_back(pdi->defaultSampleRate);		// V19 mod
		mDevices.push_back(devPtr);
		mNumRealInChannels += pdi->maxInputChannels;
		mNumRealOutChannels += pdi->maxOutputChannels;
	}

	// set the receiver's state variables
	if (ic == 0) {					// Port Audio can't open a device with 0 channels
		mInDev = paNoDevice;		// In such case, "No device" has to be specified.
		mInputParameters = NULL;
	} else {
		mInDev = is;			
		// setup input/output params. portaudio V19 change
		mInputParameters = (PaStreamParameters*)malloc(sizeof(PaStreamParameters));
		mInputParameters->channelCount = csl_min(mNumRealInChannels, CGestalt::numInChannels());
		mInputParameters->device = mInDev;
		mInputParameters->sampleFormat = paFloat32;
		mInputParameters->suggestedLatency = (PaTime) 0;
		mInputParameters->hostApiSpecificStreamInfo = NULL;
	}
	if (oc == 0) {						// Port Audio can't open a device with 0 channels
		mOutDev = paNoDevice;		// In such case, "No device" has to be specified.
		mOutputParameters = NULL;
	} else {
		mOutDev = os;
		mOutputParameters = (PaStreamParameters*)malloc(sizeof(PaStreamParameters));
		mOutputParameters->channelCount = csl_min(mNumRealOutChannels, CGestalt::numOutChannels());
		mOutputParameters->device = mOutDev;
		mOutputParameters->sampleFormat = paFloat32;
		mOutputParameters->suggestedLatency = (PaTime) 0;
		mOutputParameters->hostApiSpecificStreamInfo = NULL;
	}
									// print out the device table
	logMsg("PAIO set-up %d in, %d out", mNumInChannels, mNumOutChannels);
	mStatus = kIOInit;					// set status flag
//#ifdef CSL_DEBUG
	logMsg("	Found %d PortAudio devices: %d in, %d out", numDevices, mNumRealInChannels, mNumRealOutChannels);
	for (unsigned i = 0; i < mDevices.size(); i++) {
		mDevices[i]->dump();
	}
//#endif
}
