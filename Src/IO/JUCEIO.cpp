//
//  JUCEIO.cpp -- DAC IO using JUCE
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
//  printf("Audio IO rate %g; block size %d\n", mDevice->getCurrentSampleRate(), mDevice->getDefaultBufferSize());
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
    logMsg("JUCEIO::audioDeviceIOCallback");
	if ((mStatus != kIORunning) || (mGraph == 0)) {	// if we're off or there's no input graph,
													//  just play silence...
		for (unsigned i = 0; i < mNumInChannels; i++)
			bzero(outData[i], (numSamples * sizeof(sample)));
		return;
	}
	if (mNumInChannels > 0) {						// if any input
		if (mNumInChannels != numIns) {
			logMsg(kLogError, "Wrong # in channels: expected %d got %d", 
					mNumInChannels, numIns);
			return;
		}
		for (unsigned i = 0; i < mNumInChannels; i++)
			mInputBuffer.setBuffer(i, (SampleBuffer) inData[i]);
		mInputBuffer.mNumFrames = numSamples;
	}
	if (mNumOutChannels > 0) {
		if (mNumOutChannels != numOuts) {
			logMsg(kLogError, "Wrong # out channels: expected %d got %d", 
					mNumOutChannels, numOuts);
			return;
		}
		for (unsigned i = 0; i < numOuts; i++)
			mOutputBuffer.setBuffer(i, (SampleBuffer) outData[i]);
		mOutputBuffer.mNumFrames = numSamples;	
		pullInput(mOutputBuffer, NULL);
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
