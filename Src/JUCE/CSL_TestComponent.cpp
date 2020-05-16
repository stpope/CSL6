/*
  ==============================================================================

  This is an automatically generated GUI class created by the Projucer!

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Created with Projucer version: 6.0.0

  ------------------------------------------------------------------------------

  The Projucer is part of the JUCE library.
  Copyright (c) 2017 - ROLI Ltd.

  ==============================================================================
*/

//[Headers] You can add your own extra header files here...

#include "Test_Support.h"

//[/Headers]

#include "CSL_TestComponent.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...

using namespace csl;

// the CSL test suite lists

// These are structs that are declared in the individual CSL test suite files
//		struct testStruct { char * name;   void (* fcn)();  };
// Each test suite has a list of its functions

extern testStruct oscTestList[];
extern testStruct srcTestList[];
extern testStruct envTestList[];
extern testStruct effTestList[];
extern testStruct panTestList[];
#ifdef USE_JMIDI
extern testStruct ctrlTestList[];
#endif
extern testStruct audioTestList[];

testStruct * allTests[] = {
	oscTestList,
	srcTestList,
	envTestList,
	effTestList,
	panTestList,
#ifdef USE_JMIDI
	ctrlTestList,
#endif
	audioTestList
};

const char * allTestNames[] = {
	"Oscillator Tests",
	"Source Tests",
	"Envelope Tests",
	"Effect Tests",
	"Panner Tests",
#ifdef USE_JMIDI
	"Control Tests",
#endif
	"Audio Tests"
};

const char * allTestFiles[] = {
	"Test_Oscillators.cpp",
	"Test_Sources.cpp",
	"Test_Envelopes.cpp",
	"Test_Effects.cpp",
	"Test_Panners.cpp",
#ifdef USE_JMIDI
	"Test_Control.cpp",
#endif
	"Test_Audio.cpp"
};

testStruct * gTestList;				// global menu

// Pretty-print the test menu

void dumpTestList() {
	printf("\nMenu List\n");
#ifdef USE_JMIDI
	unsigned numSuites = 7;
#else
	unsigned numSuites = 6;
#endif
	for (unsigned i = 0; i < numSuites; i++) {
		gTestList = allTests[i];
		const char * testName = allTestNames[i];
		printf("\n%s  -  %s\n", testName, allTestFiles[i]);
//		printf("</ul>\n<li>%s</li>\n<ul>", testName);
		for (unsigned j = 0; gTestList[j].name != NULL; j++) {
			printf("%24s	%s\n", gTestList[j].name, gTestList[j].comment);
//			printf("<li>%s -- %s</li>\n", gTestList[j].name, gTestList[j].comment);
		}
	}
	printf("\n\n");
}

// The main() function reads argv[1] as the suite selector (from the above list, 1-based)
// and argv[2] as the test within the suite, e.g., use "2 10" to select the 10th test in the
// srcTestList suite ("Vector IFFT" -- found at the bottom of Test_Sources.cpp).
// This is useful for debugging; set up the GUI to run the test you're working on.

// Globals are here

extern IO_CLASS * theIO;						// global IO object accessed by other threads
juce::Label * gCPULabel;						// CPU % label...
juce::AudioDeviceManager * gAudioDeviceManager;	// global JUCE audio device mgr
unsigned argCnt;							    // globals for argc/v from cmd-line
const char ** argVals;

#define WRITE_TO_FILE						    // support file recording
#ifdef WRITE_TO_FILE
	Buffer * gFileBuffer = 0;				    // global buffer for file cache
	int gSampIndex = 0;						    // write index into out buffer
#endif

//
// LThread run function spawns/loops its CThread and sleeps interruptably
//

void LThread::run() {
	while (1) {								    // endless loop
		if (thr->isThreadRunning())			    // sleep some
			if (csl::sleepMsec(250))		    // interruptable sleep, check for failure
				goto turn_off;
		if (this->threadShouldExit())		    // check flag
			goto turn_off;
		if ( ! thr->isThreadRunning()) {	    // if off
			if (loop)
				thr->startThread();			    // restart if looping
			else {
turn_off:		if (CGestalt::stopNow()) {		// if a timer was interrupted
					CGestalt::clearStopNow();   // clear the global flag
					thr->signalThreadShouldExit();
				}
				comp->playing = false;
				const juce::MessageManagerLock mmLock;	// create a lock to call the GUI thread here
				if (comp->isTimerRunning())
					comp->stopTimer();				    // nasty to do this from here, but it freezes the GUI
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
CSLComponent::CSLComponent ()
{
    //[Constructor_pre] You can add your own custom stuff here..
    //[/Constructor_pre]

    playButton.reset (new TextButton ("playNote"));
    addAndMakeVisible (playButton.get());
    playButton->setButtonText (TRANS("Play/Stop"));
    playButton->addListener (this);

    quitButton.reset (new TextButton ("quitAction"));
    addAndMakeVisible (quitButton.get());
    quitButton->setButtonText (TRANS("Quit App"));
    quitButton->addListener (this);

    testCombo.reset (new ComboBox ("test to run"));
    addAndMakeVisible (testCombo.get());
    testCombo->setEditableText (false);
    testCombo->setJustificationType (Justification::centredLeft);
    testCombo->setTextWhenNothingSelected (String());
    testCombo->setTextWhenNoChoicesAvailable (TRANS("(no choices)"));
    testCombo->addListener (this);

    oscilloscopeL.reset (new AudioWaveformDisplay());
    addAndMakeVisible (oscilloscopeL.get());
    oscilloscopeL->setName ("new component");

    label.reset (new Label ("CSL test",
                            TRANS("CSL 6.0 Demos - select from the menus at the lower-left")));
    addAndMakeVisible (label.get());
    label->setFont (Font (Font::getDefaultSerifFontName(), 24.00f, Font::plain).withTypefaceStyle ("Bold"));
    label->setJustificationType (Justification::centred);
    label->setEditable (false, false, false);
    label->setColour (Label::textColourId, Colour (0xfffffc00));
    label->setColour (TextEditor::textColourId, Colours::black);
    label->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    prefsButton.reset (new TextButton ("new button"));
    addAndMakeVisible (prefsButton.get());
    prefsButton->setButtonText (TRANS("Set Audio Prefs"));
    prefsButton->addListener (this);

    prefsButton->setBounds (8, 8, 154, 32);

    cpuLabel.reset (new Label ("new label",
                               TRANS("0.0%")));
    addAndMakeVisible (cpuLabel.get());
    cpuLabel->setFont (Font (Font::getDefaultSansSerifFontName(), 15.00f, Font::plain).withTypefaceStyle ("Bold"));
    cpuLabel->setJustificationType (Justification::centredRight);
    cpuLabel->setEditable (false, false, false);
    cpuLabel->setColour (TextEditor::textColourId, Colours::black);
    cpuLabel->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    oscilloscopeR.reset (new AudioWaveformDisplay());
    addAndMakeVisible (oscilloscopeR.get());
    oscilloscopeR->setName ("new component");

    VUMeterL.reset (new VUMeter());
    addAndMakeVisible (VUMeterL.get());
    VUMeterL->setName ("new component");

    VUMeterR.reset (new VUMeter());
    addAndMakeVisible (VUMeterR.get());
    VUMeterR->setName ("new component");

    scaleSlider.reset (new Slider ("new slider"));
    addAndMakeVisible (scaleSlider.get());
    scaleSlider->setRange (0, 1, 0);
    scaleSlider->setSliderStyle (Slider::LinearHorizontal);
    scaleSlider->setTextBoxStyle (Slider::NoTextBox, true, 80, 20);
    scaleSlider->addListener (this);

    amplitudeSlider.reset (new Slider ("new slider"));
    addAndMakeVisible (amplitudeSlider.get());
    amplitudeSlider->setRange (-5, 5, 0);
    amplitudeSlider->setSliderStyle (Slider::LinearVertical);
    amplitudeSlider->setTextBoxStyle (Slider::NoTextBox, true, 8, 20);
    amplitudeSlider->addListener (this);

    loopButton.reset (new ToggleButton ("new toggle button"));
    addAndMakeVisible (loopButton.get());
    loopButton->setButtonText (TRANS("Loop"));
    loopButton->addListener (this);

    familyCombo.reset (new ComboBox ("test family"));
    addAndMakeVisible (familyCombo.get());
    familyCombo->setEditableText (false);
    familyCombo->setJustificationType (Justification::centredLeft);
    familyCombo->setTextWhenNothingSelected (String());
    familyCombo->setTextWhenNoChoicesAvailable (TRANS("(no choices)"));
    familyCombo->addItem (TRANS("Oscillators"), 1);
    familyCombo->addItem (TRANS("Sources"), 2);
    familyCombo->addItem (TRANS("Envelopes"), 3);
    familyCombo->addItem (TRANS("Effects"), 4);
    familyCombo->addItem (TRANS("Panners"), 5);
    familyCombo->addItem (TRANS("Controls"), 6);
    familyCombo->addItem (TRANS("Audio"), 7);
    familyCombo->addListener (this);

    recordButton.reset (new ToggleButton ("new toggle button"));
    addAndMakeVisible (recordButton.get());
    recordButton->setButtonText (TRANS("Record"));
    recordButton->addListener (this);

    label2.reset (new Label ("new label",
                             TRANS("Category")));
    addAndMakeVisible (label2.get());
    label2->setFont (Font (15.00f, Font::plain).withTypefaceStyle ("Regular"));
    label2->setJustificationType (Justification::centredLeft);
    label2->setEditable (false, false, false);
    label2->setColour (TextEditor::textColourId, Colours::black);
    label2->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    label2->setBounds (24, 664, 100, 24);

    label3.reset (new Label ("new label",
                             TRANS("Demo")));
    addAndMakeVisible (label3.get());
    label3->setFont (Font (15.00f, Font::plain).withTypefaceStyle ("Regular"));
    label3->setJustificationType (Justification::centredLeft);
    label3->setEditable (false, false, false);
    label3->setColour (TextEditor::textColourId, Colours::black);
    label3->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    label3->setBounds (264, 664, 80, 24);


    //[UserPreSize]
	oscilloscopeL->initialise(0, 20, 2, true);		// channel, rate, window, zeroX
	oscilloscopeR->initialise(1, 20, 2, true);
    //[/UserPreSize]

    setSize (1000, 700);

#pragma mark set-up

    //[Constructor] You can add your own custom stuff here..

////////////////////////////// Here we go! //////////////////////////////////////////
					// CSL code starts here
//	dumpTestList();					    // print out the demo/test menu

					// initiali[zs]e the device manager so it picks a default device to use.
	const juce::String error (mAudioDeviceManager.initialise(2,	    // stereo input
													   2,	        // stereo output
													   0,	        // no XML defaults
													   true ));     // select default device
	if (error.isNotEmpty())
		juce::AlertWindow::showMessageBox (juce::AlertWindow::WarningIcon,
									 "CSL Demo",
									 "Couldn't open an output device!\n\n" + error);

#ifdef READ_IO_PROPS					// overwrite the system frame rate and block size from the
										// get the audio device
	juce::AudioIODevice * audioIO = mAudioDeviceManager.getCurrentAudioDevice();
										//  selected hardware interface at startup time
	unsigned sRate = (unsigned) audioIO->getCurrentSampleRate();
	unsigned bufSize = audioIO->getCurrentBufferSizeSamples();
	theIO = new csl::IO(sRate, bufSize, -1, -1,			// still use the default # IO channels
					CGestalt::numInChannels(),
					CGestalt::numOutChannels());

#else									// reset the HW frame rate & block size to the CSL definition
//	juce::AudioDeviceManager::AudioDeviceSetup setup;
//	mAudioDeviceManager.getAudioDeviceSetup(setup);
//	setup.bufferSize = CGestalt::blockSize();
//	setup.sampleRate = CGestalt::frameRate();
//	mAudioDeviceManager.setAudioDeviceSetup(setup, true);
										// set up IO
	theIO = new IO_CLASS(CGestalt::frameRate(),
					CGestalt::blockSize(),
					-1, -1,				// use default I/O devices
					2, 2);              // stereo I/O by default
#endif
	theIO->start();						// start IO and register callback
	mAudioDeviceManager.addAudioCallback(this);

	gCPULabel = cpuLabel.get();			// component settings
	gAudioDeviceManager = & mAudioDeviceManager;
	playThread = 0;
	loopThread = 0;
	loop = false;
	playing = false;
	displayMode = true;
	recrding = false;

	amplitudeSlider->setValue(0.8);	    // GUI settings
	scaleSlider->setValue(0.0);
	oscilloscopeL->start();
	oscilloscopeR->start();
	VUMeterL->setChannel(0);
	VUMeterR->setChannel(1);
    loopButton->setToggleState (false, false);
//	spectrogam->setVisible(false);

	int whichSuite = 1;				    // set default suite/test
	int whichTest = 1;
									    // try to read init file
	string initMsg(CGestalt::initFileText('T'));
	if (initMsg.size() > 0) {
		sscanf(initMsg.c_str(), "%d %d", & whichSuite, & whichTest);
		printf("Select suite %d, test %d\n", whichSuite, whichTest);
	}
//	if (argCnt > 1)					    // cmd-line args select test suite and test
//		whichSuite = atoi(argVals[1]);
//	if (argCnt > 2)
//		whichTest = atoi(argVals[2]);
//	if (argCnt > 1)
//		printf("Select suite %d, test %d\n", whichSuite, whichTest);
									// combo menu selections
	familyCombo->setSelectedId(whichSuite, true);
	this->setComboLabels(whichSuite - 1);
	testCombo->setSelectedId(whichTest, true);

    //[/Constructor]
}

CSLComponent::~CSLComponent()
{
    //[Destructor_pre]. You can add your own custom destruction code here..

	if (playing)			// CSL shut-down
		this->startStop();
	playing = false;
	mAudioDeviceManager.removeAudioCallback(this);
	if (gFileBuffer) {
		gFileBuffer->freeBuffers();
		delete gFileBuffer;
	}

    //[/Destructor_pre]

    playButton = nullptr;
    quitButton = nullptr;
    testCombo = nullptr;
    oscilloscopeL = nullptr;
    label = nullptr;
    prefsButton = nullptr;
    cpuLabel = nullptr;
    oscilloscopeR = nullptr;
    VUMeterL = nullptr;
    VUMeterR = nullptr;
    scaleSlider = nullptr;
    amplitudeSlider = nullptr;
    loopButton = nullptr;
    familyCombo = nullptr;
    recordButton = nullptr;
    label2 = nullptr;
    label3 = nullptr;


    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void CSLComponent::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (Colours::grey);

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void CSLComponent::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    playButton->setBounds (590, getHeight() - 39, 170, 32);
    quitButton->setBounds (getWidth() - 8 - 176, 8, 176, 32);
    testCombo->setBounds (320, getHeight() - 36, 220, 24);
    oscilloscopeL->setBounds (44, 48, getWidth() - 52, proportionOfHeight (0.4200f));
    label->setBounds (440 - (558 / 2), 10, 558, 28);
    cpuLabel->setBounds (getWidth() - 64, getHeight() - 35, 56, 24);
    oscilloscopeR->setBounds (44, proportionOfHeight (0.5000f), getWidth() - 52, proportionOfHeight (0.4200f));
    VUMeterL->setBounds (25, 48, 15, roundToInt (proportionOfHeight (0.4200f) * 1.0000f));
    VUMeterR->setBounds (24, proportionOfHeight (0.5000f), 15, roundToInt (proportionOfHeight (0.4200f) * 1.0000f));
    scaleSlider->setBounds (36, getHeight() - 60, getWidth() - 36, 24);
    amplitudeSlider->setBounds (0, 42, 20, proportionOfHeight (0.8700f));
    loopButton->setBounds (590 + 170 - -54, getHeight() - 11 - 24, 64, 24);
    familyCombo->setBounds (102, getHeight() - 35, 140, 24);
    recordButton->setBounds ((getWidth() - 8 - 176) + -12 - 72, 37 - 24, 72, 24);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void CSLComponent::buttonClicked (Button* buttonThatWasClicked)
{
    //[UserbuttonClicked_Pre]
    //[/UserbuttonClicked_Pre]

    if (buttonThatWasClicked == playButton.get())
    {
        //[UserButtonCode_playButton] -- add your button handler code here..

		this->startStop();

        //[/UserButtonCode_playButton]
    }
    else if (buttonThatWasClicked == quitButton.get())
    {
        //[UserButtonCode_quitButton] -- add your button handler code here..

		if (playing)
			this->startStop();
		juce::JUCEApplication::quit();

        //[/UserButtonCode_quitButton]
    }
    else if (buttonThatWasClicked == prefsButton.get())
    {
        //[UserButtonCode_prefsButton] -- add your button handler code here..

					// Create an AudioDeviceSelectorComponent which contains the audio choice widgets...
//		if (displayMode) {
			oscilloscopeL->stop();			// stop scope display during dialog
			oscilloscopeR->stop();
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
			oscilloscopeL->start();			// stop scope display during dialog
			oscilloscopeR->start();
//		} else {
//			spectrogam->start();
//		}

        //[/UserButtonCode_prefsButton]
    }
    else if (buttonThatWasClicked == loopButton.get())
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
    else if (buttonThatWasClicked == recordButton.get())
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

void CSLComponent::comboBoxChanged (ComboBox* comboBoxThatHasChanged)
{
    //[UsercomboBoxChanged_Pre]
    //[/UsercomboBoxChanged_Pre]

    if (comboBoxThatHasChanged == testCombo.get())
    {
        //[UserComboBoxCode_testCombo] -- add your combo box handling code here..

//		printf("\tSelect test %d\n", testCombo->getSelectedId());

        //[/UserComboBoxCode_testCombo]
    }
    else if (comboBoxThatHasChanged == familyCombo.get())
    {
        //[UserComboBoxCode_familyCombo] -- add your combo box handling code here..

//		printf("\tSelect suite %d\n", familyCombo->getSelectedId());
		this->setComboLabels(familyCombo->getSelectedId() - 1);

        //[/UserComboBoxCode_familyCombo]
    }

    //[UsercomboBoxChanged_Post]
    //[/UsercomboBoxChanged_Post]
}

void CSLComponent::sliderValueChanged (Slider* sliderThatWasMoved)
{
    //[UsersliderValueChanged_Pre]
    //[/UsersliderValueChanged_Pre]

    if (sliderThatWasMoved == scaleSlider.get())
    {
        //[UserSliderCode_scaleSlider] -- add your slider handling code here..

		unsigned stepValue = (unsigned) (scaleSlider->getValue() * 10.0) + 1;
		oscilloscopeL->setSamplesToAverage(stepValue);
		oscilloscopeR->setSamplesToAverage(stepValue);

        //[/UserSliderCode_scaleSlider]
    }
    else if (sliderThatWasMoved == amplitudeSlider.get())
    {
        //[UserSliderCode_amplitudeSlider] -- add your slider handling code here..

		amplValue = pow(2.0, amplitudeSlider->getValue());

        //[/UserSliderCode_amplitudeSlider]
    }

    //[UsersliderValueChanged_Post]
    //[/UsersliderValueChanged_Post]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...

//
/////////////// CSL/JUCE Audio callback method ///////////////////
//

void CSLComponent::audioDeviceIOCallback (const float** inputChannelData,
							int totalNumInputChannels,
							float** outputChannelData,
							int totalNumOutputChannels,
							int numSamples) {
								// put silence in the output buffers
    for (unsigned i = 0; i < totalNumOutputChannels; i++)
        bzero(outputChannelData[i], numSamples * sizeof(float));
	if ( ! playing)				// if off
		return;
	if (CGestalt::stopNow())	// if being interrupted
		return;
								// set up CSL buffer object
	if (theIO->mGraph) {
		outBuffer.setSize(totalNumOutputChannels, numSamples);
		outBuffer.mAreBuffersAllocated = true;
								// copy JUCE data ptrs into outBuffer
		for (unsigned i = 0; i < totalNumOutputChannels; i++)
			outBuffer.setBuffer(i, outputChannelData[i]);
        outBuffer.mAreBuffersAllocated = true;
        outBuffer.mNumAlloc = numSamples;
                                // copy over the input data
        if (CGestalt::numInChannels() > 0) {
            for (unsigned i = 0; i < CGestalt::numInChannels(); i++) {
                float * inDPtr = (float *) inputChannelData[i];
                theIO->mInputBuffer.setBuffer(i, inDPtr);
        } }
        theIO->mInputBuffer.mAreBuffersAllocated = true;
        theIO->mInputBuffer.mNumAlloc = numSamples;
		try {					//////
								// Tell the IO to call its graph ------------------
								//////
			theIO->pullInput(outBuffer);
			outBuffer.mIsPopulated = true;
			outBuffer.mAreBuffersZero = false;

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
								// pass the audio callback on to the waveform display components
		oscilloscopeL->audioDeviceIOCallback (inputChannelData, totalNumInputChannels,
								outputChannelData, totalNumOutputChannels, numSamples);
		oscilloscopeR->audioDeviceIOCallback (inputChannelData, totalNumInputChannels,
								outputChannelData, totalNumOutputChannels, numSamples);
		VUMeterL->audioDeviceIOCallback (inputChannelData, 0,
								outputChannelData, totalNumOutputChannels, numSamples);
		VUMeterR->audioDeviceIOCallback (inputChannelData, 0,
								outputChannelData, totalNumOutputChannels, numSamples);
//		spectrogam->audioDeviceIOCallback (inputChannelData, totalNumInputChannels,
//								outputChannelData, totalNumOutputChannels, numSamples);

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

void CSLComponent::recordOnOff() {
		if (recrding) {					// stop recording and write output file
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

void CSLComponent::audioDeviceAboutToStart (juce::AudioIODevice* device) {
	oscilloscopeL->audioDeviceAboutToStart (device);
	oscilloscopeR->audioDeviceAboutToStart (device);
//	spectrogam->audioDeviceAboutToStart (device);
//	csl::CGestalt::setBlockSize(numSamplesPerBlock);
}

void CSLComponent::audioDeviceStopped() {
//	oscilloscope->audioDeviceStopped();
}

// Set up the combo box from the individual test suite methods

void CSLComponent::setComboLabels(unsigned which) {
	testCombo->clear();
	gTestList = allTests[which];
	for (unsigned i = 0; gTestList[i].name != NULL; i++) {
//		printf("\tAdd \"%s\"\n", gTestList[i].name);
		testCombo->addItem(juce::String(gTestList[i].name), i + 1);
	}
	testCombo->setSelectedId(1);
}

// create the component -- called from main()

juce::Component* createCSLComponent() {
    return new CSLComponent();
}

// timer call

void CSLComponent::timerCallback() {
	const juce::MessageManagerLock mmLock;	// create the lock so we can call the GUI thread from this thread
	char msg[10];						// print the CPU usage
	sprintf(msg, "%5.2f%%", (gAudioDeviceManager->getCpuUsage() * 100.0));
	juce::String msgS(msg);
	gCPULabel->setText(msgS, juce::sendNotification);
}

// start/stop

void CSLComponent::startStop() {
	if ( ! playing) {										// if not playing, start!
		int which = testCombo->getSelectedId();
		if (which == 0)
			return;
		char initmsg[16];									// store test to file
		sprintf(initmsg, "%d %d", familyCombo->getSelectedId(), testCombo->getSelectedId());
		CGestalt::storeToInitFile('T', string(initmsg));

		playing = true;
		CGestalt::clearStopNow();							// clear flag
															// create a threadfcn that plays CSL
		ThreadFunc fcn = (void * (*)(void *)) gTestList[which - 1].fcn;

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

//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Projucer information section --

    This is where the Projucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="CSLComponent" componentName=""
                 parentClasses="public Component, public AudioIODeviceCallback, public Timer"
                 constructorParams="" variableInitialisers="" snapPixels="8" snapActive="1"
                 snapShown="1" overlayOpacity="0.330" fixedSize="1" initialWidth="1000"
                 initialHeight="700">
  <BACKGROUND backgroundColour="ff808080"/>
  <TEXTBUTTON name="playNote" id="ed1811e776eea99b" memberName="playButton"
              virtualName="" explicitFocusOrder="0" pos="590 39R 170 32" buttonText="Play/Stop"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="quitAction" id="dbaf2871fd41de83" memberName="quitButton"
              virtualName="" explicitFocusOrder="0" pos="8Rr 8 176 32" buttonText="Quit App"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <COMBOBOX name="test to run" id="bd1a5c541fbc8bc7" memberName="testCombo"
            virtualName="" explicitFocusOrder="0" pos="320 36R 220 24" editable="0"
            layout="33" items="" textWhenNonSelected="" textWhenNoItems="(no choices)"/>
  <GENERICCOMPONENT name="new component" id="d64c351a292a43a4" memberName="oscilloscopeL"
                    virtualName="" explicitFocusOrder="0" pos="44 48 52M 42%" class="AudioWaveformDisplay"
                    params=""/>
  <LABEL name="CSL test" id="a9876f115ab3c22e" memberName="label" virtualName=""
         explicitFocusOrder="0" pos="440c 10 558 28" textCol="fffffc00"
         edTextCol="ff000000" edBkgCol="0" labelText="CSL 6.0 Demos - select from the menus at the lower-left"
         editableSingleClick="0" editableDoubleClick="0" focusDiscardsChanges="0"
         fontname="Default serif font" fontsize="24.0" kerning="0.0" bold="1"
         italic="0" justification="36" typefaceStyle="Bold"/>
  <TEXTBUTTON name="new button" id="beeb15a1537fd4f6" memberName="prefsButton"
              virtualName="" explicitFocusOrder="0" pos="8 8 154 32" buttonText="Set Audio Prefs"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <LABEL name="new label" id="87d3c5b55ea75f76" memberName="cpuLabel"
         virtualName="" explicitFocusOrder="0" pos="64R 35R 56 24" edTextCol="ff000000"
         edBkgCol="0" labelText="0.0%" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default sans-serif font" fontsize="15.0"
         kerning="0.0" bold="1" italic="0" justification="34" typefaceStyle="Bold"/>
  <GENERICCOMPONENT name="new component" id="4aa0f216430fecde" memberName="oscilloscopeR"
                    virtualName="" explicitFocusOrder="0" pos="44 50% 52M 42%" class="AudioWaveformDisplay"
                    params=""/>
  <GENERICCOMPONENT name="new component" id="28308fd8ae783890" memberName="VUMeterL"
                    virtualName="" explicitFocusOrder="0" pos="25 48 15 100%" posRelativeH="d64c351a292a43a4"
                    class="VUMeter" params=""/>
  <GENERICCOMPONENT name="new component" id="643b07b4b6cf41d" memberName="VUMeterR"
                    virtualName="" explicitFocusOrder="0" pos="24 50% 15 100%" posRelativeH="4aa0f216430fecde"
                    class="VUMeter" params=""/>
  <SLIDER name="new slider" id="c62ea84a7afde2e3" memberName="scaleSlider"
          virtualName="" explicitFocusOrder="0" pos="36 60R 36M 24" min="0.0"
          max="1.0" int="0.0" style="LinearHorizontal" textBoxPos="NoTextBox"
          textBoxEditable="0" textBoxWidth="80" textBoxHeight="20" skewFactor="1.0"
          needsCallback="1"/>
  <SLIDER name="new slider" id="78c6bd39da739ba6" memberName="amplitudeSlider"
          virtualName="" explicitFocusOrder="0" pos="0 42 20 87%" min="-5.0"
          max="5.0" int="0.0" style="LinearVertical" textBoxPos="NoTextBox"
          textBoxEditable="0" textBoxWidth="8" textBoxHeight="20" skewFactor="1.0"
          needsCallback="1"/>
  <TOGGLEBUTTON name="new toggle button" id="2322f603f1796f48" memberName="loopButton"
                virtualName="" explicitFocusOrder="0" pos="-54R 11Rr 64 24" posRelativeX="ed1811e776eea99b"
                buttonText="Loop" connectedEdges="0" needsCallback="1" radioGroupId="0"
                state="0"/>
  <COMBOBOX name="test family" id="55af3a98f2faf643" memberName="familyCombo"
            virtualName="" explicitFocusOrder="0" pos="102 35R 140 24" editable="0"
            layout="33" items="Oscillators&#10;Sources&#10;Envelopes&#10;Effects&#10;Panners&#10;Controls&#10;Audio"
            textWhenNonSelected="" textWhenNoItems="(no choices)"/>
  <TOGGLEBUTTON name="new toggle button" id="82a5a3f16d517231" memberName="recordButton"
                virtualName="" explicitFocusOrder="0" pos="-12r 37r 72 24" posRelativeX="dbaf2871fd41de83"
                buttonText="Record" connectedEdges="0" needsCallback="1" radioGroupId="0"
                state="0"/>
  <LABEL name="new label" id="23b1e228fb0b5e55" memberName="label2" virtualName=""
         explicitFocusOrder="0" pos="24 664 100 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Category" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="15.0"
         kerning="0.0" bold="0" italic="0" justification="33"/>
  <LABEL name="new label" id="5a2d4a92827c7701" memberName="label3" virtualName=""
         explicitFocusOrder="0" pos="264 664 80 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Demo" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="15.0"
         kerning="0.0" bold="0" italic="0" justification="33"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]

