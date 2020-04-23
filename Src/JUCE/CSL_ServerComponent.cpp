/*
  ==============================================================================

  This is an automatically generated file created by the Jucer!

  Creation date:  11 Oct 2009 11:20:11 pm

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

#include "CSL_ServerComponent.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...

using namespace csl;

IO * theIO;									// global IO instance
Label* gCPULabel;							// CPU % label...
AudioDeviceManager * gAudioDeviceManager;	// global JUCE audio device mgr
Mixer * gMix;								// stereo mixer
Buffer * gFileBuffer = 0;					// global buffer for file cache
int gSampIndex = 0;							// write index into out buffer

// server function sets up an OSC address space on an instrument library

void oscServer() {
	const char *drumz[6] = {	"bassdrum_mono.aiff",	"stick_mono.aiff", 	// sample file names
								"midtom_mono.aiff",		"tymapnum_mono.aiff", 
								"splash_mono.aiff",		"triangle_mono.aiff" };
	InstrumentVector * lib = new InstrumentVector;	// this is the instrument vector
	gMix = new Mixer(2);		// Create the main stereo output mixer
	logMsg("Setting up instrument library");
								// Now add 16 instruments of your choice
	for (unsigned i = 0; i < 16; i++) {		// uncommnent the desired instrument below
		AdditiveInstrument * in = new AdditiveInstrument;
//		FMInstrument * in = new FMInstrument;
//		SndFileInstrument * in = new SndFileInstrument(CGestalt::dataFolder(), drumz[i % 6]);
		lib->push_back(in);
		gMix->addInput(*in);
	}
	Stereoverb * rev = new Stereoverb(*gMix);	// stereo reverb
	rev->setRoomSize(0.9);		// medium-length reverb
	
								// set up CSL IO
	theIO = new IO(CGestalt::frameRate(), CGestalt::blockSize(), -1, -1,
					CGestalt::numInChannels(), CGestalt::numOutChannels());
	theIO->setRoot(*rev);		// plug the mixer in as the IO client
	theIO->open();				// open the IO (no callbacks yet)
	theIO->start();				// start the IO (mixer inputs are not active() yet)

	char pNam[CSL_WORD_LEN];	// string for port number
	sprintf(pNam, "%d", CGestalt::outPort());

	initOSC(pNam);				// Set up OSC address space root

	setupOSCInstrLibrary(*lib);	// add the instrument library OSC
}

//[/MiscUserDefs]

//==============================================================================
CSLServerComponent::CSLServerComponent ()
    : playButton (0),
      stopButton (0),
      label (0),
      prefsButton (0),
      RecordButton (0),
      quitButton (0),
      cpuLabel (0),
      amplSlider (0),
      internalCachedImage1 (0)
{
    addAndMakeVisible (playButton = new TextButton (T("new button")));
    playButton->setButtonText (T("Play"));
    playButton->addButtonListener (this);

    addAndMakeVisible (stopButton = new TextButton (T("new button")));
    stopButton->setButtonText (T("Stop"));
    stopButton->addButtonListener (this);

    addAndMakeVisible (label = new Label (T("new label"),
                                          T("CSL Server")));
    label->setFont (Font (32.0000f, Font::plain));
    label->setJustificationType (Justification::centredLeft);
    label->setEditable (false, false, false);
    label->setColour (TextEditor::textColourId, Colours::black);
    label->setColour (TextEditor::backgroundColourId, Colour (0x0));

    addAndMakeVisible (prefsButton = new TextButton (T("new button")));
    prefsButton->setButtonText (T("Prefs"));
    prefsButton->addButtonListener (this);

    addAndMakeVisible (RecordButton = new ToggleButton (T("new toggle button")));
    RecordButton->setButtonText (T("Record"));
    RecordButton->addButtonListener (this);

    addAndMakeVisible (quitButton = new TextButton (T("new button")));
    quitButton->setButtonText (T("Quit"));
    quitButton->addButtonListener (this);

    addAndMakeVisible (cpuLabel = new Label (T("new label"),
                                             T("0.0 %")));
    cpuLabel->setFont (Font (15.0000f, Font::plain));
    cpuLabel->setJustificationType (Justification::centredLeft);
    cpuLabel->setEditable (false, false, false);
    cpuLabel->setColour (TextEditor::textColourId, Colours::black);
    cpuLabel->setColour (TextEditor::backgroundColourId, Colour (0x0));

    addAndMakeVisible (amplSlider = new Slider (T("new slider")));
    amplSlider->setRange (-8, 4, 0);
    amplSlider->setSliderStyle (Slider::LinearVertical);
    amplSlider->setTextBoxStyle (Slider::TextBoxLeft, true, 0, 20);
    amplSlider->addListener (this);

    internalCachedImage1 = ImageCache::getFromMemory (create_logo_sm_gif, create_logo_sm_gifSize);

    //[UserPreSize]
    //[/UserPreSize]

    setSize (300, 200);

    //[Constructor] You can add your own custom stuff here..

	oscServer();						// set up inst lib, OSC server & CSL IO

	const String error (mAudioDeviceManager.initialise (0,	/* no input */
													   2,	/* stereo output  */
													   0,	/* no XML defaults */
													   true /* select default device */));
	if (error.isNotEmpty())
		AlertWindow::showMessageBox (AlertWindow::WarningIcon,
									 T("CSL Demo"),
									 T("Couldn't open an output device!\n\n") + error);
										// get the audio device
//	AudioIODevice * audioIO = mAudioDeviceManager.getCurrentAudioDevice();	// get audio device
//	unsigned sRate = (unsigned) audioIO->getCurrentSampleRate();
//	unsigned bufSize = audioIO->getCurrentBufferSizeSamples();
	AudioDeviceManager::AudioDeviceSetup setup;
	mAudioDeviceManager.getAudioDeviceSetup(setup);			// get JUCE audio IO props
	setup.bufferSize = CGestalt::blockSize();
	setup.sampleRate = CGestalt::frameRate();
	mAudioDeviceManager.setAudioDeviceSetup(setup, true);	// set JUCE audio IO props
	mAudioDeviceManager.addAudioCallback(this);				// register me as callback

	gCPULabel = cpuLabel;
	gAudioDeviceManager = & mAudioDeviceManager;
	recrding = false;
	amplSlider->setValue(-0.2);

    //[/Constructor]
}

CSLServerComponent::~CSLServerComponent()
{
    //[Destructor_pre]. You can add your own custom destruction code here..

	gMix->deleteInputs();
	playing = false;
	mAudioDeviceManager.removeAudioCallback(this);
	if (gFileBuffer) {
		gFileBuffer->freeBuffers();
		delete gFileBuffer;
	}
    //[/Destructor_pre]

    deleteAndZero (playButton);
    deleteAndZero (stopButton);
    deleteAndZero (label);
    deleteAndZero (prefsButton);
    deleteAndZero (RecordButton);
    deleteAndZero (quitButton);
    deleteAndZero (cpuLabel);
    deleteAndZero (amplSlider);
    ImageCache::release (internalCachedImage1);

    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void CSLServerComponent::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (Colour (0xffd51f1f));

    g.setColour (Colours::black);
    g.drawImage (internalCachedImage1,
                 194, 156, 100, 40,
                 0, 0, internalCachedImage1->getWidth(), internalCachedImage1->getHeight());

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void CSLServerComponent::resized()
{
    playButton->setBounds (48, 48, 136, 56);
    stopButton->setBounds (48, 112, 136, 56);
    label->setBounds (48, 11, 150, 24);
    prefsButton->setBounds (208, 8, 86, 32);
    RecordButton->setBounds (208, 56, 72, 24);
    quitButton->setBounds (200, 88, 88, 56);
    cpuLabel->setBounds (8, 176, 64, 24);
    amplSlider->setBounds (8, 8, 23, 160);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void CSLServerComponent::buttonClicked (Button* buttonThatWasClicked)
{
    //[UserbuttonClicked_Pre]
    //[/UserbuttonClicked_Pre]

    if (buttonThatWasClicked == playButton)
    {
        //[UserButtonCode_playButton] -- add your button handler code here..

		playing = true;
		CGestalt::clearStopNow();							// clear flag
									// make a thread for the OSC server
		CThread * osc = CThread::MakeThread();
									// Run the OSC loop function (it exits on the quit command)
		osc->createThread(mainOSCLoop, NULL);
		sleepSec(0.5);				/// wait for it to start
		this->startTimer(1000);

        //[/UserButtonCode_playButton]
    }
    else if (buttonThatWasClicked == stopButton)
    {
        //[UserButtonCode_stopButton] -- add your button handler code here..

		playing = false;
		theIO->clearRoot();
		CGestalt::setStopNow();								// set flag to clear timers
		if (recrding)
			recordOnOff();
		sleepMsec(100);
		CGestalt::clearStopNow();		// clear flag

        //[/UserButtonCode_stopButton]
    }
    else if (buttonThatWasClicked == prefsButton)
    {
        //[UserButtonCode_prefsButton] -- add your button handler code here..
 		AudioDeviceSelectorComponent audioSettingsComp (mAudioDeviceManager,
														CGestalt::numInChannels(), CGestalt::numInChannels(),
														CGestalt::numOutChannels(), CGestalt::numOutChannels(),
														true, true,
														false, false);
											// ...and show it in a DialogWindow...
		audioSettingsComp.setSize (500, 400);
		DialogWindow::showModalDialog (T("Audio Settings"),
									   &audioSettingsComp,
									   this,
									   Colours::azure,
									   true);
        //[/UserButtonCode_prefsButton]
    }
    else if (buttonThatWasClicked == RecordButton)
    {
        //[UserButtonCode_RecordButton] -- add your button handler code here..

		bool wasRec = recrding;
		recordOnOff();
		if (wasRec)
			recrding = false;

        //[/UserButtonCode_RecordButton]
    }
    else if (buttonThatWasClicked == quitButton)
    {
        //[UserButtonCode_quitButton] -- add your button handler code here..

		playing = false;
		theIO->clearRoot();
		CGestalt::setStopNow();			// set flag to clear timers
		if (recrding)
			recordOnOff();
		sleepMsec(100);
		CGestalt::clearStopNow();		// clear flag
		JUCEApplication::quit();

        //[/UserButtonCode_quitButton]
    }

    //[UserbuttonClicked_Post]
    //[/UserbuttonClicked_Post]
}

void CSLServerComponent::sliderValueChanged (Slider* sliderThatWasMoved)
{
    //[UsersliderValueChanged_Pre]
    //[/UsersliderValueChanged_Pre]

    if (sliderThatWasMoved == amplSlider)
    {
        //[UserSliderCode_amplSlider] -- add your slider handling code here..

		amplValue = pow(2.0, amplSlider->getValue());

        //[/UserSliderCode_amplSlider]
    }

    //[UsersliderValueChanged_Post]
    //[/UsersliderValueChanged_Post]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...

void CSLServerComponent::audioDeviceIOCallback (const float** inputChannelData,
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
			outBuffer.mBuffers[i] = outputChannelData[i];

		try {					//////
								// Tell the IO to call its graph
								//////
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
			csl::sample * sPtr = gFileBuffer->mBuffers[0] + gSampIndex;
			memcpy(sPtr, outputChannelData[0], (numSamples * sizeof(csl::sample)));
			sPtr = gFileBuffer->mBuffers[1] + gSampIndex;
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

void CSLServerComponent::recordOnOff() {
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

void CSLServerComponent::audioDeviceAboutToStart (AudioIODevice* device) {
}

void CSLServerComponent::audioDeviceStopped() {
}

// timer call

void CSLServerComponent::timerCallback() {
	const MessageManagerLock mmLock;	// create the lock so we can call the GUI thread from this thread
	char msg[10];						// print the CPU usage
	sprintf(msg, "%5.2f%%", (gAudioDeviceManager->getCpuUsage() * 100.0));
	String msgS(msg);
	gCPULabel->setText(msgS, true);
}

// create the component -- called from main()

Component* createCSLComponent() {
    return new CSLServerComponent();
}

//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Jucer information section --

    This is where the Jucer puts all of its metadata, so don't change anything in here!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="CSLServerComponent" componentName=""
                 parentClasses="public Component, public Timer, public AudioIODeviceCallback"
                 constructorParams="" variableInitialisers="" snapPixels="8" snapActive="1"
                 snapShown="1" overlayOpacity="0.330000013" fixedSize="1" initialWidth="300"
                 initialHeight="200">
  <BACKGROUND backgroundColour="ffd51f1f">
    <IMAGE pos="194 156 100 40" resource="create_logo_sm_gif" opacity="1"
           mode="0"/>
  </BACKGROUND>
  <TEXTBUTTON name="new button" id="6ec6293830f1aecd" memberName="playButton"
              virtualName="" explicitFocusOrder="0" pos="48 48 136 56" buttonText="Play"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="new button" id="102da5923dcd3f0b" memberName="stopButton"
              virtualName="" explicitFocusOrder="0" pos="48 112 136 56" buttonText="Stop"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <LABEL name="new label" id="4871ba3a8fcadfd8" memberName="label" virtualName=""
         explicitFocusOrder="0" pos="48 11 150 24" edTextCol="ff000000"
         edBkgCol="0" labelText="CSL Server" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="32"
         bold="0" italic="0" justification="33"/>
  <TEXTBUTTON name="new button" id="c871e2c581386f9d" memberName="prefsButton"
              virtualName="" explicitFocusOrder="0" pos="208 8 86 32" buttonText="Prefs"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <TOGGLEBUTTON name="new toggle button" id="ced59fb08b7e7393" memberName="RecordButton"
                virtualName="" explicitFocusOrder="0" pos="208 56 72 24" buttonText="Record"
                connectedEdges="0" needsCallback="1" radioGroupId="0" state="0"/>
  <TEXTBUTTON name="new button" id="433653b5563ccc88" memberName="quitButton"
              virtualName="" explicitFocusOrder="0" pos="200 88 88 56" buttonText="Quit"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <LABEL name="new label" id="2379225f8cf773e2" memberName="cpuLabel"
         virtualName="" explicitFocusOrder="0" pos="8 176 64 24" edTextCol="ff000000"
         edBkgCol="0" labelText="0.0 %" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="15"
         bold="0" italic="0" justification="33"/>
  <SLIDER name="new slider" id="15fbaa985fc9c5a1" memberName="amplSlider"
          virtualName="" explicitFocusOrder="0" pos="8 8 23 160" min="-8"
          max="4" int="0" style="LinearVertical" textBoxPos="TextBoxLeft"
          textBoxEditable="0" textBoxWidth="0" textBoxHeight="20" skewFactor="1"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif

//==============================================================================
// Binary resources - be careful not to edit any of these sections!

// JUCER_RESOURCE: create_logo_sm_gif, 4973, "../../../CREATE/Doc/create.logo/CREATE.logo.sm.gif"
static const unsigned char resource_CSLServerComponent_create_logo_sm_gif[] = { 71,73,70,56,57,97,204,0,82,0,247,0,0,255,255,255,255,255,204,255,255,153,255,255,102,255,255,51,255,255,0,255,204,255,255,
204,204,255,204,153,255,204,102,255,204,51,255,204,0,255,153,255,255,153,204,255,153,153,255,153,102,255,153,51,255,153,0,255,102,255,255,102,204,255,102,153,255,102,102,255,102,51,255,102,0,255,51,255,
255,51,204,255,51,153,255,51,102,255,51,51,255,51,0,255,0,255,255,0,204,255,0,153,255,0,102,255,0,51,255,0,0,204,255,255,204,255,204,204,255,153,204,255,102,204,255,51,204,255,0,204,204,255,204,204,204,
204,204,153,204,204,102,204,204,51,204,204,0,204,153,255,204,153,204,204,153,153,204,153,102,204,153,51,204,153,0,204,102,255,204,102,204,204,102,153,204,102,102,204,102,51,204,102,0,204,51,255,204,51,
204,204,51,153,204,51,102,204,51,51,204,51,0,204,0,255,204,0,204,204,0,153,204,0,102,204,0,51,204,0,0,153,255,255,153,255,204,153,255,153,153,255,102,153,255,51,153,255,0,153,204,255,153,204,204,153,204,
153,153,204,102,153,204,51,153,204,0,153,153,255,153,153,204,153,153,153,153,153,102,153,153,51,153,153,0,153,102,255,153,102,204,153,102,153,153,102,102,153,102,51,153,102,0,153,51,255,153,51,204,153,
51,153,153,51,102,153,51,51,153,51,0,153,0,255,153,0,204,153,0,153,153,0,102,153,0,51,153,0,0,102,255,255,102,255,204,102,255,153,102,255,102,102,255,51,102,255,0,102,204,255,102,204,204,102,204,153,102,
204,102,102,204,51,102,204,0,102,153,255,102,153,204,102,153,153,102,153,102,102,153,51,102,153,0,102,102,255,102,102,204,102,102,153,102,102,102,102,102,51,102,102,0,102,51,255,102,51,204,102,51,153,
102,51,102,102,51,51,102,51,0,102,0,255,102,0,204,102,0,153,102,0,102,102,0,51,102,0,0,51,255,255,51,255,204,51,255,153,51,255,102,51,255,51,51,255,0,51,204,255,51,204,204,51,204,153,51,204,102,51,204,
51,51,204,0,51,153,255,51,153,204,51,153,153,51,153,102,51,153,51,51,153,0,51,102,255,51,102,204,51,102,153,51,102,102,51,102,51,51,102,0,51,51,255,51,51,204,51,51,153,51,51,102,51,51,51,51,51,0,51,0,
255,51,0,204,51,0,153,51,0,102,51,0,51,51,0,0,0,255,255,0,255,204,0,255,153,0,255,102,0,255,51,0,255,0,0,204,255,0,204,204,0,204,153,0,204,102,0,204,51,0,204,0,0,153,255,0,153,204,0,153,153,0,153,102,
0,153,51,0,153,0,0,102,255,0,102,204,0,102,153,0,102,102,0,102,51,0,102,0,0,51,255,0,51,204,0,51,153,0,51,102,0,51,51,0,51,0,0,0,255,0,0,204,0,0,153,0,0,102,0,0,51,238,0,0,221,0,0,187,0,0,170,0,0,136,
0,0,119,0,0,85,0,0,68,0,0,34,0,0,17,0,0,0,238,0,0,221,0,0,187,0,0,170,0,0,136,0,0,119,0,0,85,0,0,68,0,0,34,0,0,17,0,0,0,238,0,0,221,0,0,187,0,0,170,0,0,136,0,0,119,0,0,85,0,0,68,0,0,34,0,0,17,238,238,
238,221,221,221,187,187,187,170,170,170,136,136,136,119,119,119,85,85,85,68,68,68,34,34,34,17,17,17,0,0,0,33,249,4,0,0,0,67,0,44,0,0,0,0,204,0,82,0,0,8,254,0,1,8,28,72,176,160,193,131,8,19,42,92,200,176,
161,195,135,16,35,74,156,72,177,162,197,139,24,51,106,220,200,177,163,199,143,32,67,138,28,73,178,164,195,122,38,83,170,92,201,82,160,61,125,45,99,202,156,137,177,30,76,154,56,115,166,68,9,177,94,62,157,
64,131,122,172,135,47,226,75,161,72,147,86,180,114,143,33,79,129,62,149,74,157,186,176,30,191,166,11,139,14,60,154,176,158,149,138,246,168,138,237,104,143,213,207,133,95,7,70,85,168,245,96,189,176,8,237,
157,29,75,247,98,89,86,79,17,182,5,192,53,238,222,130,43,210,30,148,91,183,112,197,123,172,248,193,77,184,119,45,194,123,127,9,6,22,12,152,178,225,203,5,23,19,196,199,175,115,100,130,148,251,30,132,156,
48,112,32,132,129,44,99,198,92,15,107,65,124,137,247,169,222,76,208,241,232,207,46,173,232,203,59,48,245,234,223,80,63,231,99,149,24,183,64,61,4,237,33,47,109,220,180,230,129,250,102,3,175,187,188,160,
39,125,164,74,25,127,146,7,128,19,148,36,170,254,40,92,209,92,247,138,129,231,5,250,158,206,186,58,193,60,157,58,93,122,146,144,59,128,39,40,235,117,79,24,190,180,238,166,40,177,2,23,63,210,177,39,150,
123,3,121,18,95,39,226,61,214,73,61,115,132,85,15,125,9,169,64,225,96,249,240,19,72,61,251,232,195,74,122,252,220,100,32,77,19,62,84,15,32,7,197,119,73,39,23,26,244,68,39,247,133,69,194,126,8,145,224,
154,65,43,120,216,33,43,251,124,8,192,61,26,142,136,83,127,39,205,53,16,9,150,204,97,137,37,157,164,231,226,28,245,200,145,223,28,10,57,209,34,96,250,116,182,138,41,171,240,115,94,32,171,176,34,36,77,
68,54,100,155,64,72,42,201,164,147,5,61,1,165,148,0,64,88,229,149,146,101,201,207,150,93,126,25,230,152,51,217,99,156,65,250,25,36,71,39,75,202,97,137,19,8,61,193,201,125,245,144,224,4,141,7,145,192,38,
150,252,152,18,200,62,251,120,9,64,32,166,224,85,209,159,124,58,69,167,66,170,201,97,170,169,150,140,10,192,10,114,4,32,165,133,254,151,40,68,194,115,146,233,177,74,32,250,228,19,72,41,231,233,83,138,
166,12,89,17,153,136,172,225,147,41,113,137,225,138,15,173,80,213,147,215,91,247,48,133,207,10,206,202,90,5,111,246,204,241,132,149,36,92,9,158,133,42,204,145,135,61,121,140,203,236,143,171,236,51,80,
132,71,230,163,157,63,172,220,147,143,179,86,112,145,207,164,110,253,163,110,65,55,86,21,148,61,251,252,35,240,192,4,11,236,143,145,245,16,220,79,63,254,20,172,176,145,128,149,162,78,63,152,6,114,105,
103,196,249,195,176,63,28,255,67,143,192,173,80,172,207,21,122,92,209,33,177,4,237,163,206,63,69,145,11,151,133,121,204,81,197,199,22,235,163,143,197,53,231,147,79,174,60,239,188,115,63,250,110,250,79,
63,26,238,99,49,166,252,100,204,113,199,2,175,204,202,105,52,233,83,240,211,129,176,226,176,63,181,53,236,240,214,3,247,195,155,64,79,236,243,49,215,100,23,172,43,62,128,4,162,179,91,43,255,195,79,183,
157,192,53,135,30,85,204,93,246,221,3,171,27,72,254,219,120,147,77,147,213,3,11,88,91,192,4,79,234,85,193,187,181,134,15,208,4,171,67,107,21,122,216,83,207,10,129,32,158,143,21,86,4,102,15,100,186,18,
108,69,32,248,232,170,155,90,3,61,49,48,61,247,188,40,57,0,115,231,241,68,229,125,151,173,110,126,247,192,222,245,10,43,216,147,251,10,156,123,56,240,215,41,217,30,244,65,248,104,253,15,196,124,153,13,
64,183,2,1,30,184,65,115,232,163,85,194,4,231,154,199,110,130,77,155,207,192,249,236,3,200,30,152,238,38,208,190,0,16,46,176,30,240,61,33,222,19,163,60,241,4,52,224,252,147,154,21,151,99,206,143,194,246,
228,175,251,226,2,171,219,242,170,5,227,71,102,178,38,48,224,149,196,30,10,27,79,222,4,178,2,61,20,133,122,3,131,137,61,210,2,65,238,21,164,10,252,168,14,226,118,165,182,124,52,101,130,250,184,7,227,172,
176,15,83,232,42,83,63,185,199,62,136,82,15,227,201,111,14,157,176,130,204,228,83,133,39,196,111,31,79,0,68,41,56,168,27,130,9,16,61,254,126,234,223,143,0,80,133,220,77,173,32,153,3,77,1,99,226,66,124,
189,198,96,2,145,131,235,226,132,184,229,205,197,124,2,235,135,100,86,81,10,17,33,206,87,165,40,197,218,74,161,135,125,232,97,123,255,32,33,23,3,1,8,79,232,225,30,42,156,160,21,166,230,4,6,225,67,92,157,
184,7,32,254,161,142,23,149,34,83,252,232,162,212,6,246,67,129,164,174,30,241,19,19,76,240,1,153,0,34,241,57,31,51,224,72,132,71,62,82,9,44,138,157,64,78,5,143,199,151,180,200,203,97,139,209,3,113,162,
115,30,23,234,99,31,164,32,14,210,88,97,133,82,28,194,10,64,115,87,213,248,17,159,64,116,38,68,86,184,95,193,234,150,201,78,40,232,9,165,240,7,250,226,83,138,46,133,104,144,2,19,211,64,24,9,0,169,237,
3,96,86,208,67,106,142,8,154,73,73,77,146,33,89,65,193,206,101,144,123,136,239,18,156,104,89,21,251,3,57,124,128,50,55,165,120,90,6,235,193,183,127,216,172,152,153,218,82,41,210,134,43,64,176,66,15,128,
254,232,76,24,73,129,202,48,74,111,107,129,128,225,130,50,41,46,129,250,146,75,199,244,33,65,158,144,158,126,224,227,9,217,33,99,21,28,73,144,200,101,102,67,44,17,30,214,28,2,204,192,100,71,147,5,59,141,
10,138,18,77,115,22,236,60,47,41,38,43,210,9,8,39,184,240,82,158,64,22,43,110,117,179,57,144,66,16,187,242,68,151,144,69,138,75,116,201,102,255,240,7,50,131,202,157,5,41,168,19,240,241,37,49,183,4,136,
161,22,18,0,161,187,7,74,2,19,200,67,148,2,10,212,28,8,32,194,66,57,154,180,19,106,110,129,139,228,74,97,137,60,120,34,140,121,112,86,72,249,82,138,9,69,211,97,86,32,74,98,18,115,137,16,33,33,126,17,12,
4,41,146,70,28,68,116,143,21,114,72,103,103,178,35,83,82,196,84,54,86,11,132,54,9,198,203,163,14,52,62,10,34,5,151,128,74,72,130,248,234,114,0,144,102,152,92,105,210,202,18,36,16,97,185,166,76,16,88,61,
254,212,208,30,188,234,4,39,46,145,157,85,200,97,14,139,205,254,219,19,238,65,10,233,225,227,16,1,116,160,21,194,20,200,65,241,138,113,2,211,135,30,60,17,209,82,120,34,108,208,168,66,41,246,170,82,46,
245,232,143,171,144,222,199,170,128,4,231,233,75,92,73,85,144,118,147,218,137,95,237,65,120,252,120,139,61,158,208,25,64,106,168,83,43,237,108,50,11,2,90,0,220,143,155,36,153,35,193,246,178,152,9,149,
85,15,217,89,82,119,35,90,133,123,124,81,154,14,196,71,59,143,183,143,48,170,148,80,114,8,196,19,128,251,15,64,148,177,187,136,40,69,21,156,144,142,126,228,3,17,50,77,167,76,89,97,10,64,108,15,28,78,112,
130,30,10,86,84,199,26,117,65,92,236,194,80,89,185,143,12,133,9,144,51,189,165,108,218,41,64,201,73,78,31,212,106,24,124,71,50,212,127,176,73,14,112,1,132,126,87,65,220,163,238,179,182,254,173,30,62,162,
121,143,42,184,80,126,249,188,83,41,76,65,86,67,117,130,127,220,163,31,207,172,128,4,124,128,195,194,232,189,229,134,249,129,136,92,254,163,20,79,144,131,254,233,8,214,5,24,114,87,169,112,254,196,42,244,
48,226,192,65,46,59,5,46,166,158,3,25,200,42,224,53,153,184,19,214,146,163,37,176,29,131,100,86,88,252,199,61,206,163,130,36,148,43,31,122,32,235,138,246,185,195,149,238,83,15,242,205,155,206,124,135,
56,58,71,148,20,171,32,148,37,228,192,134,42,48,120,126,209,228,194,165,156,230,43,126,144,162,19,115,77,154,172,65,119,190,215,58,193,186,252,168,194,138,182,59,80,5,233,176,169,4,99,101,24,19,35,79,
14,199,216,20,51,30,219,208,150,188,233,75,13,108,82,134,182,136,163,244,1,8,242,22,204,10,196,5,167,37,206,74,220,229,102,199,213,58,13,36,23,201,168,94,217,69,19,62,197,45,5,130,71,77,130,39,147,77,
22,122,136,112,124,230,42,211,16,161,209,9,74,138,30,137,205,250,88,19,235,116,15,104,76,38,20,0,129,167,64,22,83,202,240,44,94,236,88,134,171,14,249,204,35,202,129,207,37,146,84,133,68,163,175,20,121,
176,196,138,10,220,221,193,30,57,105,97,116,112,185,254,53,198,96,162,230,67,65,151,8,119,168,39,62,106,57,32,193,133,254,184,165,152,227,103,225,164,117,66,178,97,146,169,108,238,199,15,39,72,17,9,107,
142,32,30,21,52,10,200,38,53,59,220,241,33,169,231,16,38,79,140,58,157,164,24,53,143,240,161,236,100,242,25,99,3,227,142,205,45,49,202,109,173,32,31,209,54,136,163,230,96,5,195,122,219,202,42,188,118,
32,186,187,92,111,183,61,49,197,13,119,32,243,49,209,234,169,111,193,10,67,155,47,65,205,15,5,45,105,212,108,112,2,131,47,23,179,166,70,231,30,26,115,151,152,145,37,235,125,152,84,193,238,115,2,98,130,
253,162,94,91,126,21,115,80,47,43,124,206,9,75,27,234,87,165,248,188,108,92,200,10,124,84,193,84,248,204,199,154,211,156,78,83,112,226,172,196,185,121,178,208,7,185,121,73,100,66,218,214,150,197,117,216,
221,126,88,245,200,130,45,46,97,201,93,48,79,239,44,164,145,254,21,151,252,110,40,83,189,252,207,55,227,107,98,8,196,143,154,51,126,204,157,235,91,99,79,254,124,84,79,240,67,14,117,78,38,18,228,80,87,
86,68,93,14,146,173,242,31,173,208,78,97,27,202,251,45,190,31,235,155,222,121,201,53,170,95,23,121,203,10,242,80,133,128,11,12,31,80,64,10,104,179,67,33,215,118,191,98,126,109,199,79,115,86,110,29,210,
70,65,215,52,144,179,83,175,246,119,205,71,2,214,181,7,58,164,103,43,117,51,253,144,37,27,182,37,250,84,5,214,85,54,10,150,93,111,118,115,165,144,121,148,247,90,91,66,10,114,0,9,169,20,122,150,144,29,
10,23,56,25,114,9,131,210,35,105,243,15,105,38,89,119,114,9,122,128,77,25,49,33,229,70,25,209,212,74,181,165,110,130,53,79,198,21,81,86,240,128,13,70,126,135,146,104,178,161,83,172,112,84,127,199,6,114,
112,107,4,163,7,134,50,113,132,210,9,82,228,15,175,197,9,73,162,95,20,184,102,79,232,58,121,112,57,88,102,48,117,148,93,39,198,79,34,22,108,227,215,9,92,242,9,166,162,103,134,114,89,12,86,122,180,132,
131,91,18,8,35,38,85,43,208,84,254,249,144,86,41,241,82,5,129,18,247,16,0,202,69,100,252,68,136,252,230,9,251,224,58,5,179,7,134,50,106,80,168,14,159,115,128,164,144,7,19,71,126,46,103,93,94,56,7,156,
32,31,171,245,90,122,128,138,151,48,7,166,146,113,43,194,9,115,144,15,234,0,98,176,216,9,209,163,15,121,32,60,105,4,67,254,70,76,161,55,7,10,131,111,121,208,37,161,184,109,157,97,9,108,144,29,176,164,
80,194,53,7,78,48,89,94,129,45,42,129,69,90,148,40,134,213,70,116,199,139,76,226,140,73,54,48,164,0,139,49,131,4,12,70,15,29,162,87,178,134,44,233,242,140,3,211,138,171,101,9,171,181,90,120,176,36,151,
128,7,78,192,15,251,48,40,73,114,9,92,88,10,117,116,9,102,101,88,121,16,126,2,19,8,117,4,89,72,181,32,251,224,9,72,224,127,155,231,62,149,50,142,129,149,46,163,214,9,163,23,108,63,146,58,42,80,10,73,84,
16,29,178,18,177,37,48,78,196,23,121,96,5,202,177,2,64,227,9,239,23,8,121,16,142,7,105,254,15,86,18,98,153,54,48,159,3,70,7,55,136,176,209,133,120,0,78,186,184,90,97,120,143,218,242,15,172,144,139,242,
232,4,2,83,55,51,232,9,163,144,15,221,229,127,30,51,126,76,162,144,190,180,10,121,80,135,129,243,4,146,179,7,135,240,75,85,64,101,134,18,105,238,40,48,2,244,20,153,247,28,254,85,73,38,161,75,193,197,31,
206,66,57,231,99,86,203,197,2,35,201,73,64,247,4,142,242,100,98,4,105,127,20,70,233,162,96,131,103,80,143,117,143,151,160,2,247,3,144,11,66,118,2,195,29,76,18,70,34,40,97,37,87,5,90,40,143,3,21,70,150,
128,4,123,20,56,60,193,25,170,119,30,221,147,36,157,160,54,12,246,84,172,67,43,247,131,50,7,84,48,66,184,2,99,19,77,92,4,8,70,68,48,243,194,60,78,128,4,61,230,99,232,98,152,92,4,105,147,247,142,120,32,
138,11,178,34,73,229,62,2,3,8,2,85,120,250,146,102,85,169,29,100,4,126,5,179,15,43,48,106,127,183,34,68,102,42,254,151,141,247,17,8,123,254,160,2,2,81,5,250,128,131,121,32,97,164,87,16,64,150,25,255,215,
18,53,169,150,153,209,54,244,160,92,129,244,21,117,249,21,204,227,40,80,24,52,43,96,10,245,16,70,246,86,15,166,136,138,132,114,143,186,200,29,108,160,2,140,147,102,2,121,40,166,240,15,122,224,38,141,89,
109,128,87,151,254,64,2,168,98,40,175,23,122,46,39,60,202,212,157,115,33,47,240,33,77,99,233,54,5,225,4,207,65,56,37,73,18,88,4,86,128,97,60,172,240,4,162,100,70,0,52,95,113,130,40,36,64,37,35,136,155,
65,200,69,250,64,2,69,97,93,116,39,141,176,56,7,218,226,117,206,227,38,180,136,151,241,147,58,84,32,144,102,149,102,114,112,15,72,192,150,2,147,15,36,48,153,159,167,15,33,70,2,88,180,81,134,132,48,232,
83,2,128,128,120,26,73,16,72,192,157,203,52,48,50,81,15,194,99,97,212,18,39,105,39,155,19,210,84,112,228,139,232,152,15,69,97,33,221,114,53,248,96,0,54,241,132,7,208,127,5,227,15,22,163,7,92,128,105,254,
128,160,27,252,160,108,254,224,115,234,35,74,31,67,32,79,128,7,220,49,94,119,153,15,85,23,84,133,154,4,125,232,58,101,84,114,242,227,65,147,3,41,203,243,4,42,144,15,123,80,169,106,35,104,152,163,51,251,
0,92,25,26,19,246,0,165,36,152,31,109,136,55,113,197,157,78,144,104,201,52,94,165,176,112,100,243,125,130,199,53,166,176,127,234,211,110,119,67,15,78,64,92,122,224,110,91,99,33,236,213,52,188,250,156,
57,241,18,172,224,110,20,99,123,3,129,79,20,131,43,116,170,101,199,226,26,72,0,36,54,19,174,152,178,2,192,84,94,53,67,109,24,8,8,70,19,174,23,147,52,253,0,13,253,208,138,76,135,75,156,243,61,92,144,4,
162,74,31,165,208,5,58,131,170,239,180,15,152,151,130,122,160,33,243,164,91,161,115,51,153,242,76,87,146,33,82,134,43,185,162,51,16,43,174,171,196,10,11,83,32,49,225,44,111,33,73,86,176,7,36,80,37,4,225,
4,100,122,16,135,248,23,167,130,122,248,192,19,36,128,178,33,166,45,87,233,254,38,34,90,58,96,99,33,248,7,21,106,230,114,129,149,113,43,48,147,108,192,77,238,131,16,64,135,40,161,130,16,86,240,165,252,
209,34,73,80,162,0,192,43,5,241,119,75,226,9,40,74,16,186,88,46,122,208,45,174,115,33,251,1,30,13,114,16,174,67,126,100,52,106,217,34,161,114,80,162,251,135,16,143,66,37,65,43,180,37,106,35,31,91,162,
245,176,10,6,97,143,250,245,180,3,177,34,117,211,40,5,217,164,235,146,28,170,2,33,114,208,109,210,9,32,75,194,77,86,146,16,49,115,182,8,33,132,128,161,42,113,34,183,102,104,134,75,162,158,2,1,31,226,130,
18,143,210,179,145,251,177,51,43,16,49,148,29,168,50,7,220,41,161,150,32,182,102,11,40,226,98,184,35,81,15,8,114,31,95,120,95,137,82,46,80,2,0,42,64,7,85,176,24,52,114,127,9,17,77,85,118,145,231,65,2,
98,88,162,234,163,16,163,107,186,31,97,19,7,65,161,73,146,16,186,22,51,97,129,4,83,4,179,91,161,184,202,209,118,248,101,5,37,96,146,158,254,80,162,228,50,30,194,27,18,103,34,16,106,210,9,168,89,58,108,
136,18,42,0,35,166,229,31,93,130,15,156,130,30,30,194,182,200,179,189,37,113,34,197,219,9,157,145,186,165,179,2,64,138,18,204,171,16,179,149,16,32,36,57,79,208,22,95,7,44,110,17,190,240,123,186,40,34,
40,244,155,65,245,129,191,175,187,191,245,145,185,157,164,15,0,44,192,25,194,182,6,124,192,220,251,25,173,164,32,160,186,80,244,161,149,222,241,193,4,161,163,254,17,190,43,160,92,220,68,20,26,156,19,117,
180,90,36,28,39,191,219,42,113,98,190,8,193,172,113,49,58,8,225,35,45,60,21,247,112,128,25,140,15,69,113,15,6,16,39,130,160,16,44,80,30,114,59,16,60,220,195,74,33,66,201,146,16,164,161,22,110,91,31,50,
224,31,10,209,196,78,140,20,43,192,87,22,75,95,144,59,16,83,44,178,22,27,94,91,44,21,176,65,192,7,65,25,86,49,30,5,18,24,164,114,198,82,177,2,153,146,21,131,163,16,99,140,35,22,43,199,83,145,33,104,145,
28,90,239,43,198,229,193,199,152,65,66,118,172,22,25,252,35,131,76,200,134,193,25,135,12,21,137,204,194,8,1,199,140,92,24,93,252,199,91,17,200,193,129,197,149,92,23,101,177,16,154,65,188,10,177,194,72,
219,201,66,209,198,38,146,200,166,60,38,136,11,200,171,252,202,113,162,202,176,220,195,162,60,203,149,44,201,182,156,203,91,28,16,0,59,0,0};

const char* CSLServerComponent::create_logo_sm_gif = (const char*) resource_CSLServerComponent_create_logo_sm_gif;
const int CSLServerComponent::create_logo_sm_gifSize = 4973;
