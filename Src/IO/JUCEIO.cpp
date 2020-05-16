//
//  JUCEIO.cpp -- CSL audio IO using JUCE call-backs directly
//	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
//

#include "JUCEIO.h"

using namespace csl;

// JUCEIO Class

JUCEIO::JUCEIO(unsigned s_rate, unsigned b_size, 
			int in_device, int out_device, 
			unsigned in_chans, unsigned out_chans)
		: IO(s_rate, b_size, in_device, out_device, in_chans, out_chans) {
												// initialise the device manager with no settings
												// so that it picks a default device to use.
//	printf("Audio IO startup\n"); 
	const juce::String errorMsg (mAudioDeviceManager.initialise(
									(int) in_chans,		/* number of input channels */
									(int) out_chans,	/* number of output channels */
									nullptr,			/* no XML settings.. */
									true, {}, nullptr));/* select default device on failure */
	if (errorMsg.isNotEmpty()) {
		printf("JUCEIO error: %s\n", (char *) errorMsg.toRawUTF8());
        mDevice = 0;
//		AlertWindow::showMessageBox (AlertWindow::WarningIcon,
//									 String("CSL Test"),
//									 String("Couldn't open an output device!\n\n") + errorMsg);
        return;
	}
    mDevice = mAudioDeviceManager.getCurrentAudioDevice();
//  	logMsg("JUCEIO rate %g; block size %d", mDevice->getCurrentSampleRate(), mDevice->getDefaultBufferSize());
}

JUCEIO::~JUCEIO() {
    IO::stop();
    IO::close();
//	mDevice->removeAudioCallback(this);
}

///< open/close start/stop methods

void JUCEIO::open() throw(CException) {
    IO::open();
}

void JUCEIO::close() throw(CException) {
    IO::close();
}

void JUCEIO::start() throw(CException) {
    IO::open();
    IO::start();
    mAudioDeviceManager.addAudioCallback(this);
    mDevice->start(this);
}

void JUCEIO::stop() throw(CException) {
    IO::stop();
    mDevice->stop();
//	mDevice->removeAudioCallback(this);
}

// Audio playback callback & utilities

void JUCEIO::audioDeviceIOCallback (const float** inData, int numIns,
									float** outData, int numOuts,
									int numSamples) {
//    logMsg("JUCEIO::audioDeviceIOCallback");

	for (unsigned i = 0; i < mNumInChannels; i++)	//  fill the buffers with silence...
		bzero(outData[i], (numSamples * sizeof(sample)));
	if ((mStatus != kIORunning) || (mGraph == 0))	// if we're off or there's no input graph,
		return;
	if (mNumInChannels > 0) {						// if any input
		if (mNumInChannels != numIns) {
			logMsg(kLogError, "Wrong # in channels: expected %d got %d", mNumInChannels, numIns);
			return;
		}
		for (unsigned i = 0; i < mNumInChannels; i++)
			mInputBuffer.setBuffer(i, (SampleBuffer) inData[i]);
		mInputBuffer.mAreBuffersAllocated = true;
		mInputBuffer.mNumFrames = numSamples;
	}
	if (mNumOutChannels > 0) {						// process output
		if (mNumOutChannels != numOuts) {
			logMsg(kLogError, "Wrong # out channels: expected %d got %d", mNumOutChannels, numOuts);
			return;
		}
		mOutputBuffer.setSize(numOuts, numSamples);
		mOutputBuffer.mAreBuffersAllocated = true;
		for (unsigned i = 0; i < numOuts; i++)		// plug in output pointers
			mOutputBuffer.setBuffer(i, (SampleBuffer) outData[i]);
		try {					//////
								// Tell the IO to call its graph ------------------
								//////
			pullInput(mOutputBuffer, NULL);
		} catch (csl::CException e) {
			logMsg(kLogError, "Error running CSL: graph: %s\n", e.what());
		}
		mNumFramesPlayed += numSamples;
	}
	return;
}

//void JUCEIO::audioDeviceAboutToStart (AudioIODevice * dev) {
//
//};

//void JUCEIO::audioDeviceStopped() {
//
//};
