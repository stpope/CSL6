/*
  ==============================================================================

  This is an automatically generated file created by the Jucer!

  Creation date:  17 Sep 2009 3:29:44 pm

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Jucer version: 1.12

  ------------------------------------------------------------------------------

  The Jucer is part of the JUCE library - "Jules' Utility Class Extensions"
  Copyright 2004-6 by Raw Material Software ltd.

  ==============================================================================
*/

//[Headers] You can add your own extra header files here...

//[/Headers]

#include "CSL_AbstComponent.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...

using namespace csl;

// The main() function reads argv[1] as the suite selector (from the above list, 1-based)
// and argv[2] as the test within the suite, e.g., use "2 10" to select the 10th test in the
// srcTestList suite ("Vector IFFT" -- found at the bottom of Test_Sources.cpp).
// This is useful for debugging; set up the GUI to run the test you're working on.

// Globals are here

extern IO * theIO;							// global IO object accessed by other threads
juce::Label* gCPULabel;							// CPU % label...
juce::AudioDeviceManager * gAudioDeviceManager;	// global JUCE audio device mgr
//unsigned argCnt;							// globals for argc/v from cmd-line
//const char **argVals;
VoidFcnPtrN * gFcn;							// test function ptr

#define WRITE_TO_FILE						// support file recording
#ifdef WRITE_TO_FILE
	Buffer * gFileBuffer = 0;				// global buffer for file cache
	int gSampIndex = 0;						// write index into out buffer
#endif

//
// LThread run function spawns/loops its CThread and sleeps interruptably
//

void LThread::run() {
	while (1) {								// endless loop
		if (thr->isThreadRunning())			// sleep some
			if (csl::sleepMsec(250))		// interruptable sleep, check for failure
				goto turn_off;
		if (this->threadShouldExit())		// check flag
			goto turn_off;
		if ( ! thr->isThreadRunning()) {	// if off
			if (loop)
				thr->startThread();			// restart if looping
			else {
turn_off:		if (CGestalt::stopNow()) {			// if a timer was interrupted
					CGestalt::clearStopNow();		// clear the global flag
					thr->signalThreadShouldExit();
				}
				comp->playing = false;
				const juce::MessageManagerLock mmLock;	// create a lock to call the GUI thread here
				if (comp->isTimerRunning())
					comp->stopTimer();				// nasty to do this from here, but it freezes the GUI
//				if (comp->playButton)
//					comp->playButton->setButtonText(T("Play"));
				if (comp->recrding)
					comp->recordOnOff();
				return;
			}
		}
	}
}

//[/MiscUserDefs]

//==============================================================================
CSLAbstComponent::CSLAbstComponent() 
		: Component(),
		  playThread(0), loopThread(0), amplValue(1.0f), 
		  loop(false), recrding(false), playing(false), displayMode(true), changed(false) {

    //[UserPreSize]
    //[/UserPreSize]

    setSize (600, 400);

    //[Constructor] You can add your own custom stuff here..
	
////////////////////////////// Here we go! //////////////////////////////////////////

					// CSL Code starts here

					// initiali[zs]e the device manager so it picks a default device to use.
	const juce::String error (mAudioDeviceManager.initialise (CGestalt::numInChannels(),	/* stereo input */
													   CGestalt::numOutChannels(),	/* stereo output  */
													   0,	/* no XML defaults */
													   true /* select default device */));
	if (error.isNotEmpty())
		juce::AlertWindow::showMessageBox (juce::AlertWindow::WarningIcon,
									 "CSL 6 Demo",
									 "Couldn't open an output device!\n\n" + error);
										// get the audio device
	juce::AudioIODevice* audioIO = mAudioDeviceManager.getCurrentAudioDevice();
//	unsigned sRate = (unsigned) audioIO->getCurrentSampleRate();
//	unsigned bufSize = audioIO->getCurrentBufferSizeSamples();

#ifdef READ_IO_PROPS					// overwrite the system frame rate and block size from the
										//  selected hardware interface at startup time
	theIO = new csl::IO(sRate, bufSize, -1, -1,			// still use the default # IO channels
					CGestalt::numInChannels(), CGestalt::numOutChannels());
#else									// reset the HW frame rate & block size to the CSL definition
	juce::AudioDeviceManager::AudioDeviceSetup setup;
	mAudioDeviceManager.getAudioDeviceSetup(setup);
	setup.bufferSize = CGestalt::blockSize();
	setup.sampleRate = CGestalt::frameRate();
	mAudioDeviceManager.setAudioDeviceSetup(setup,true);
										// set up CSL IO
	theIO = new csl::IO(CGestalt::frameRate(), CGestalt::blockSize(), -1, -1,
					CGestalt::numInChannels(), CGestalt::numOutChannels());
#endif
	theIO->start();						// start IO and register callback
	mAudioDeviceManager.addAudioCallback(this);

	gAudioDeviceManager = & mAudioDeviceManager;
	playThread = 0;
	loopThread = 0;

    //[/Constructor]
}

CSLAbstComponent::~CSLAbstComponent()
{
    //[Destructor_pre]. You can add your own custom destruction code here..

	if (playing)			// CSL shut-down
		this->startStop(NULL);
	playing = false;
	mAudioDeviceManager.removeAudioCallback(this);
	if (gFileBuffer) {
		gFileBuffer->freeBuffers();
		delete gFileBuffer;
	}

    //[/Destructor_pre]


    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}


//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...

/////////////// CSL/JUCE Audio callback method ///////////////////

void CSLAbstComponent::audioDeviceIOCallback (const float** inputChannelData,
							int totalNumInputChannels,
							float** outputChannelData,
							int totalNumOutputChannels,
							int numSamples) {
								// put silence in the output buffers
	for (unsigned i = 0; i < totalNumOutputChannels; i++)
		memset(outputChannelData[i], 0, numSamples * sizeof(float));

	if ( ! playing)				// if off
		return;
	if (CGestalt::stopNow())	// if being interrupted
		return;
								// set up CSL buffer object
	if (theIO->mGraph) {
		outBuffer.setSize(totalNumOutputChannels, numSamples);
								// copy JUCE data ptrs
		for (unsigned i = 0; i < totalNumOutputChannels; i++)
			outBuffer.setBuffer(i, outputChannelData[i]);

		try {					//////
								// Set up the input pointers and
								// Tell the IO to call its graph
								//////
			theIO->mInputBuffer.setSize(totalNumInputChannels, numSamples);
			for (unsigned i = 0; i < totalNumInputChannels; i++)
				theIO->mInputBuffer.setBuffer(i, (sample *)inputChannelData[i]);
			theIO->pullInput(outBuffer);
		} catch (csl::CException e) {
			logMsg(kLogError, "Error running CSL: graph: %s\n", e.what());
		}
		if (amplValue != 1.0f) {	// scale by volume slider
								// channel loop
			for (unsigned i = 0; i < totalNumOutputChannels; i++) {
				float * sampPtr = outputChannelData[i];
								// frame loop
				for (unsigned j = 0; j < numSamples; j++)
					*sampPtr++ *= amplValue;
		}}
		if (recrding) {			// cache to (stereo) record buffer
			if (gSampIndex >= SAMPS_TO_WRITE)
				return;
			if ( ! gFileBuffer)
				return;
								// get cache ptrs & copy outbuf
			csl::sample * sPtr = gFileBuffer->buffer(0) + gSampIndex;
			memcpy(sPtr, outputChannelData[0], (numSamples * sizeof(csl::sample)));
			sPtr = gFileBuffer->buffer(1) + gSampIndex;
			memcpy(sPtr, outputChannelData[1], (numSamples * sizeof(csl::sample)));
								// update ptrs
			gSampIndex += numSamples;
			gFileBuffer->mNumFrames = gSampIndex;
		}
	} // if mGraph
}

// Handle file output in record mode; creates a remp file named "dataFolder(), OUT_SFILE_NAME"
//		such as /Users/stp/Code/CSL/CSL_Data/02_csl_out.aiff
// The template is XX_csl_out.aiff, could be like CSL_TakeXX.aiff.
// Increments the field XX in the name template with integers, wrapping at 100.
//

void CSLAbstComponent::recordOnOff() {
		if (recrding) {					// stop recording and write output file
			recrding = false;						// reset flag checked in audioDeviceIOCallback
			if (gFileBuffer->mNumFrames == 0)
				return;
			string outName = CGestalt::sndFileName();
													// write to snd file
			logMsg("Write %5.3f sec (%d ksamp) to file \"%s\"\n",
					((float) gSampIndex / CGestalt::frameRate()), gSampIndex / 1000, outName.c_str());
			SoundFile * ioFile = new SoundFile(outName);
			ioFile->openForWrite(kSoundFileFormatAIFF, 2);
			ioFile->writeBuffer(*gFileBuffer);		// snd file write of record buffer
			ioFile->close();
			delete ioFile;
		} else {									// start recording
			if ( ! gFileBuffer) {					// allocate buffer first time
				gFileBuffer = new Buffer(2, SAMPS_TO_WRITE + CGestalt::maxBufferFrames());
				gFileBuffer->allocateBuffers();
			}
			recrding = true;						// set flag checked in audioDeviceIOCallback
		}
		gSampIndex = 0;								// reset write ptr
		gFileBuffer->mNumFrames = gSampIndex;
}

// Audio device support

void CSLAbstComponent::audioDeviceAboutToStart (juce::AudioIODevice* device) {
//	oscilloscopeL->audioDeviceAboutToStart (device);
//	oscilloscopeR->audioDeviceAboutToStart (device);
//	csl::CGestalt::setBlockSize(numSamplesPerBlock);
}

void CSLAbstComponent::audioDeviceStopped() {
//	oscilloscope->audioDeviceStopped();
}

// timer call

void CSLAbstComponent::timerCallback() {
	if (! gCPULabel) return;
	const juce::MessageManagerLock mmLock;	// create the lock so we can call the GUI thread from this thread
	char msg[10];						// print the CPU usage
	sprintf(msg, "%5.2f%%", (gAudioDeviceManager->getCpuUsage() * 100.0));
	juce::String msgS(msg);
	gCPULabel->setText(msgS, juce::sendNotification);
}

// start/stop

void CSLAbstComponent::startStop(csl::VoidFcnPtrN * fcn) {
	if ( ! playing) {										// if not playing, start!
		playing = true;
		CGestalt::clearStopNow();							// clear flag
															// create a threadfcn that plays CSL
		ThreadFunc fcn = (void * (*)(void *)) fcn;

		playThread = new GThread(fcn);						// thread to call the CSL test function
		playThread->startThread();
		loopThread = new LThread(playThread, this, loop);	// thread to wait and/or loop it
		loopThread->startThread();
//		playButton->setButtonText (T("Stop"));
		this->startTimer(1000);								// 1-sec timer
	} else {												// if playing
		playing = false;
		theIO->clearRoot();	
		this->stopTimer();
		CGestalt::setStopNow();								// set flag to clear timers
		if (recrding)
			recordOnOff();
		sleepMsec(100);
		if (loop)
			loopThread->stopThread(1000);
		if (playThread->isThreadRunning())
			playThread->stopThread(1000);					// try to kill
		delete playThread;
		playThread = 0;
//		playButton->setButtonText (T("Play"));
		CGestalt::clearStopNow();		// clear flag
	}
}

// CSLSignalComponent constructor

CSLSignalComponent::CSLSignalComponent ()
	: CSLAbstComponent(),
	  playButton (0),
      quitButton (0),
      prefsButton (0),
      cpuLabel (0),
      VUMeterL (0),
      VUMeterR (0),
      amplitudeSlider (0),
      loopButton (0),
      recordButton (0)
{
    addAndMakeVisible (playButton = new juce::TextButton ("playNote"));
    playButton->setButtonText ("Play/Stop");
    playButton->addListener (this);

    addAndMakeVisible (quitButton = new juce::TextButton ("quitAction"));
    quitButton->setButtonText ("Quit");
    quitButton->addListener (this);

    addAndMakeVisible (prefsButton = new juce::TextButton ("new button"));
    prefsButton->setButtonText ("Audio Prefs");
    prefsButton->addListener (this);

    addAndMakeVisible (cpuLabel = new juce::Label("new label", "0.0%"));
    cpuLabel->setFont (juce::Font (juce::Font::getDefaultSansSerifFontName(), 15.0000f, juce::Font::bold));
    cpuLabel->setJustificationType (juce::Justification::centredRight);
    cpuLabel->setEditable (false, false, false);
    cpuLabel->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    cpuLabel->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x0));

    addAndMakeVisible (VUMeterL = new VUMeter());
    VUMeterL->setName ("new component");

    addAndMakeVisible (VUMeterR = new VUMeter());
    VUMeterR->setName ("new component");

	addAndMakeVisible (amplitudeSlider = new juce::Slider ("new slider"));
    amplitudeSlider->setRange (-5, 5, 0);
    amplitudeSlider->setSliderStyle (juce::Slider::LinearVertical);
    amplitudeSlider->setTextBoxStyle (juce::Slider::NoTextBox, true, 80, 20);
    amplitudeSlider->addListener (this);

    addAndMakeVisible (loopButton = new juce::ToggleButton ("new toggle button"));
    loopButton->setButtonText ("Loop");
    loopButton->addListener (this);

    addAndMakeVisible (recordButton = new juce::ToggleButton ("new toggle button"));
    recordButton->setButtonText ("Record");
    recordButton->addListener (this);
	
	amplitudeSlider->setValue(-0.2);	// GUI settings
	VUMeterL->setChannel(0);
	VUMeterR->setChannel(1);
    loopButton->setToggleState (false, false);
	gCPULabel = cpuLabel;				// component settings
}

CSLSignalComponent::~CSLSignalComponent()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]

    deleteAndZero (playButton);
    deleteAndZero (quitButton);
	deleteAndZero (prefsButton);
    deleteAndZero (cpuLabel);
    deleteAndZero (VUMeterL);
    deleteAndZero (VUMeterR);
    deleteAndZero (amplitudeSlider);
    deleteAndZero (loopButton);
    deleteAndZero (recordButton);

    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

void CSLSignalComponent::audioDeviceIOCallback (const float** inputChannelData,
							int totalNumInputChannels,
							float** outputChannelData,
							int totalNumOutputChannels,
							int numSamples) {
								// call the inherited version
	CSLAbstComponent::audioDeviceIOCallback (inputChannelData, totalNumInputChannels, 
									outputChannelData, totalNumOutputChannels, numSamples);
	if (theIO->mGraph) {
								// pass the audio callback on to the waveform display components
		VUMeterL->audioDeviceIOCallback (inputChannelData, totalNumInputChannels,
								outputChannelData, totalNumOutputChannels, numSamples);
		VUMeterR->audioDeviceIOCallback (inputChannelData, totalNumInputChannels,
								outputChannelData, totalNumOutputChannels, numSamples);

	} // if mGraph
}

void CSLSignalComponent::resized()
{
    playButton->setBounds (336, getHeight() - 39, 136, 32);
    quitButton->setBounds (getWidth() - 8 - 176, 8, 176, 32);
    prefsButton->setBounds (8, 8, 144, 32);
    cpuLabel->setBounds (getWidth() - 64, getHeight() - 35, 56, 24);
    VUMeterL->setBounds (25, 48, 15, juce::roundFloatToInt ((proportionOfHeight (0.3400f)) * 1.0000f));
    VUMeterR->setBounds (24, proportionOfHeight (0.5000f), 15, juce::roundFloatToInt ((proportionOfHeight (0.3400f)) * 1.0000f));
    amplitudeSlider->setBounds (0, 43, 20, proportionOfHeight (0.7475f));
    loopButton->setBounds ((336) + 144, getHeight() - 11 - 24, 64, 24);
    recordButton->setBounds ((getWidth() - 8 - 176) + -2 - 72, 37 - 24, 72, 24);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void CSLSignalComponent::buttonClicked (juce::Button* buttonThatWasClicked)
{
    //[UserbuttonClicked_Pre]
    //[/UserbuttonClicked_Pre]

    if (buttonThatWasClicked == playButton)
    {
        //[UserButtonCode_playButton] -- add your button handler code here..

		this->startStop(gFcn);

        //[/UserButtonCode_playButton]
    }
    else if (buttonThatWasClicked == quitButton)
    {
        //[UserButtonCode_quitButton] -- add your button handler code here..

		if (playing)
			this->startStop(gFcn);
		juce::JUCEApplication::quit();

        //[/UserButtonCode_quitButton]
    }
    else if (buttonThatWasClicked == prefsButton)
    {
        //[UserButtonCode_prefsButton] -- add your button handler code here..

					// Create an AudioDeviceSelectorComponent which contains the audio choice widgets...
//		if (displayMode) {
//			oscilloscopeL->stop();			// stop scope display during dialog
//			oscilloscopeR->stop();
//		} else {
//			spectrogam->stop();
//		}
//		   AudioDeviceSelectorComponent (AudioDeviceManager& deviceManager,
//                                  const int minAudioInputChannels,
//                                  const int maxAudioInputChannels,
//                                  const int minAudioOutputChannels,
//                                  const int maxAudioOutputChannels,
//                                  const bool showMidiInputOptions,
//                                  const bool showMidiOutputSelector,
//                                  const bool showChannelsAsStereoPairs,
//                                  const bool hideAdvancedOptionsWithButton);

		juce::AudioDeviceSelectorComponent audioSettingsComp (mAudioDeviceManager,
														CGestalt::numInChannels(), CGestalt::numInChannels(),
														CGestalt::numOutChannels(), CGestalt::numOutChannels(),
														true, true,
														false, false);
											// ...and show it in a DialogWindow...
		audioSettingsComp.setSize (500, 400);
		juce::DialogWindow::showModalDialog ("Audio Settings",
									   &audioSettingsComp,
									   this,
									   juce::Colours::azure,
									   true);
//		if (displayMode) {
//			oscilloscopeL->start();			// stop scope display during dialog
//			oscilloscopeR->start();
//		} else {
//			spectrogam->start();
//		}

        //[/UserButtonCode_prefsButton]
    }
    else if (buttonThatWasClicked == loopButton)
    {
        //[UserButtonCode_loopButton] -- add your button handler code here..

		if (loop) {
			if (loopThread) {
				loopThread->stopThread(100);
				delete loopThread;
				loopThread = 0;
			}
		}
		loop = ! loop;

        //[/UserButtonCode_loopButton]
    }
    else if (buttonThatWasClicked == recordButton)
    {
        //[UserButtonCode_recordButton] -- add your button handler code here..

		bool wasRec = recrding;
		recordOnOff();
		if (wasRec)
			recrding = false;

        //[/UserButtonCode_recordButton]
    }

    //[UserbuttonClicked_Post]
    //[/UserbuttonClicked_Post]
}

void CSLSignalComponent::comboBoxChanged (juce::ComboBox* comboBoxThatHasChanged)
{
    //[UsercomboBoxChanged_Pre]
    //[/UsercomboBoxChanged_Pre]

 
    //[UsercomboBoxChanged_Post]
    //[/UsercomboBoxChanged_Post]
}

void CSLSignalComponent::sliderValueChanged (juce::Slider* sliderThatWasMoved)
{
    //[UsersliderValueChanged_Pre]
    //[/UsersliderValueChanged_Pre]

	if (sliderThatWasMoved == amplitudeSlider)
    {
        //[UserSliderCode_amplitudeSlider] -- add your slider handling code here..

		amplValue = pow(2.0, amplitudeSlider->getValue());

        //[/UserSliderCode_amplitudeSlider]
    }

    //[UsersliderValueChanged_Post]
    //[/UsersliderValueChanged_Post]
}

//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Jucer information section --

    This is where the Jucer puts all of its metadata, so don't change anything in here!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="CSLAbstComponent" componentName=""
                 parentClasses="public Component, public AudioIODeviceCallback, public Timer"
                 constructorParams="" variableInitialisers="" snapPixels="8" snapActive="1"
                 snapShown="1" overlayOpacity="0.330000013" fixedSize="1" initialWidth="600"
                 initialHeight="400">
  <BACKGROUND backgroundColour="ffaeaeae"/>
  <TEXTBUTTON name="playNote" id="ed1811e776eea99b" memberName="playButton"
              virtualName="" explicitFocusOrder="0" pos="336 39R 136 32" buttonText="Play"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="quitAction" id="dbaf2871fd41de83" memberName="quitButton"
              virtualName="" explicitFocusOrder="0" pos="8Rr 8 176 32" buttonText="Quit"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <COMBOBOX name="test to run" id="bd1a5c541fbc8bc7" memberName="testCombo"
            virtualName="" explicitFocusOrder="0" pos="152 35R 176 24" editable="0"
            layout="33" items="" textWhenNonSelected="" textWhenNoItems="(no choices)"/>
  <GENERICCOMPONENT name="new component" id="d64c351a292a43a4" memberName="oscilloscopeL"
                    virtualName="" explicitFocusOrder="0" pos="44 48 52M 34%" class="AudioWaveformDisplay"
                    params=""/>
  <LABEL name="CSL test" id="a9876f115ab3c22e" memberName="label" virtualName=""
         explicitFocusOrder="0" pos="247c 10 158 28" textCol="fffffc00"
         edTextCol="ff000000" edBkgCol="0" labelText="CSL 5 Demos" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default serif font"
         fontsize="30" bold="1" italic="0" justification="33"/>
  <TEXTBUTTON name="new button" id="beeb15a1537fd4f6" memberName="prefsButton"
              virtualName="" explicitFocusOrder="0" pos="8 8 144 32" buttonText="Audio Prefs"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <LABEL name="new label" id="87d3c5b55ea75f76" memberName="cpuLabel"
         virtualName="" explicitFocusOrder="0" pos="64R 35R 56 24" edTextCol="ff000000"
         edBkgCol="0" labelText="0.0%" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default sans-serif font" fontsize="15"
         bold="1" italic="0" justification="34"/>
  <GENERICCOMPONENT name="new component" id="4aa0f216430fecde" memberName="oscilloscopeR"
                    virtualName="" explicitFocusOrder="0" pos="44 50% 52M 34%" class="AudioWaveformDisplay"
                    params=""/>
  <GENERICCOMPONENT name="new component" id="28308fd8ae783890" memberName="VUMeterL"
                    virtualName="" explicitFocusOrder="0" pos="25 48 15 100%" posRelativeH="d64c351a292a43a4"
                    class="VUMeter" params=""/>
  <GENERICCOMPONENT name="new component" id="643b07b4b6cf41d" memberName="VUMeterR"
                    virtualName="" explicitFocusOrder="0" pos="24 50% 15 100%" posRelativeH="4aa0f216430fecde"
                    class="VUMeter" params=""/>
  <SLIDER name="new slider" id="c62ea84a7afde2e3" memberName="scaleSlider"
          virtualName="" explicitFocusOrder="0" pos="8 62R 12M 24" min="0"
          max="1" int="0" style="LinearHorizontal" textBoxPos="NoTextBox"
          textBoxEditable="0" textBoxWidth="80" textBoxHeight="20" skewFactor="1"/>
  <SLIDER name="new slider" id="78c6bd39da739ba6" memberName="amplitudeSlider"
          virtualName="" explicitFocusOrder="0" pos="0 43 20 74.75%" min="-5"
          max="5" int="0" style="LinearVertical" textBoxPos="NoTextBox"
          textBoxEditable="0" textBoxWidth="80" textBoxHeight="20" skewFactor="1"/>
  <TOGGLEBUTTON name="new toggle button" id="2322f603f1796f48" memberName="loopButton"
                virtualName="" explicitFocusOrder="0" pos="144 11Rr 64 24" posRelativeX="ed1811e776eea99b"
                buttonText="Loop" connectedEdges="0" needsCallback="1" radioGroupId="0"
                state="0"/>
  <COMBOBOX name="test family" id="55af3a98f2faf643" memberName="familyCombo"
            virtualName="" explicitFocusOrder="0" pos="16 35R 128 24" editable="0"
            layout="33" items="Oscillators&#10;Sources&#10;Envelopes&#10;Effects&#10;Panners&#10;Controls"
            textWhenNonSelected="" textWhenNoItems="(no choices)"/>
  <TOGGLEBUTTON name="new toggle button" id="82a5a3f16d517231" memberName="recordButton"
                virtualName="" explicitFocusOrder="0" pos="-2r 37r 72 24" posRelativeX="dbaf2871fd41de83"
                buttonText="Record" connectedEdges="0" needsCallback="1" radioGroupId="0"
                state="0"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif
