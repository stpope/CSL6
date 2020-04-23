/*
  ==============================================================================

  This is an automatically generated file created by the Jucer!

  Creation date:  30 Oct 2007 6:53:19 pm

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Jucer version: 1.11

  ------------------------------------------------------------------------------

  The Jucer is part of the JUCE library - "Jules' Utility Class Extensions"
  Copyright 2004-6 by Raw Material Software ltd.

  ==============================================================================
*/

//[Headers] You can add your own extra header files here...
//[/Headers]

#include "CSL_Component.h"

//[MiscUserDefs] You can add your own user definitions and misc code here...

//==============================================================================
// Waveform display class

extern csl::testStruct theTestList[];

csl::IO * theIO;
Label* GcpuLabel;
AudioDeviceManager * gAudioDeviceManager;

// oscilloscope class

class AudioOutputWaveformDisplay : public Component,
                                   public Timer,
                                   public AudioIODeviceCallback {
public:
						// Constructor
	AudioOutputWaveformDisplay() {
		bufferPos = 0;
		bufferSize = 2048;
		circularBuffer = (float*) juce_calloc (sizeof (float) * bufferSize);
		currentOutputLevel = 0.0f;
		numSamplesIn = 0;
		setOpaque (true);
	}

    ~AudioOutputWaveformDisplay() {
        juce_free (circularBuffer);
    }

	void initialise(unsigned rate, unsigned window, bool zeroX) {
		delayInMsec = 1000/rate;
		samplesToAverage = window;
		zeroCross = zeroX;
		startTimer(delayInMsec);  // repaint every so-many msec	
	}
	
    void paint (Graphics& g) {
        g.fillAll (Colours::black);
        g.setColour (Colours::lightgreen);

        const float halfHeight = getHeight() * 0.5f;
        int bp = bufferPos;
        unsigned wid = getWidth();

        for (int x = wid; --x >= 0;) {
            const int samplesAgo = wid - x;
            const float level = circularBuffer [(bp + bufferSize - samplesAgo) % bufferSize];

            if (level > 0.01f)
                g.drawLine ((float) x, halfHeight - (halfHeight * level),
                            (float) x, halfHeight + (halfHeight * level));
        }
		char cpuPercent[16];
		sprintf(cpuPercent, "%4.1f%%", gAudioDeviceManager->getCpuUsage() * 100.0);
		GcpuLabel->setText(String(cpuPercent), true);
    }

    void timerCallback() {
        repaint();
    }

    void addSample (const float sample) {
        currentOutputLevel += fabsf (sample);
        if (++numSamplesIn > samplesToAverage) {
            circularBuffer [bufferPos++ % bufferSize] = currentOutputLevel / samplesToAverage;
            numSamplesIn = 0;
            currentOutputLevel = 0.0f;
        }
    }

    void audioDeviceIOCallback (const float** inputChannelData,
                                int totalNumInputChannels,
                                float** outputChannelData,
                                int totalNumOutputChannels,
                                int numSamples) {
        for (int i = 0; i < totalNumOutputChannels; ++i) {
            if (outputChannelData[i] != 0) {
                for (int j = 0; j < numSamples; ++j)
                    addSample (outputChannelData[i][j]);
                break;
            }
        }
    }

    void audioDeviceAboutToStart (double sampleRate, int numSamplesPerBlock) {
        zeromem (circularBuffer, sizeof (float) * bufferSize);
    }

    void audioDeviceStopped() {
        zeromem (circularBuffer, sizeof (float) * bufferSize);
    }

private:
    float* circularBuffer;
    float currentOutputLevel;
    int volatile bufferPos, bufferSize, numSamplesIn;
	unsigned delayInMsec;
	unsigned samplesToAverage;
	bool zeroCross;
};

//[/MiscUserDefs]

//==============================================================================
CSLComponent::CSLComponent ()
    : playButton (0),
      quitButton (0),
      testCombo (0),
      oscilloscope (0),
      label (0),
      prefsButton (0),
      cpuLabel (0),
      decrButton (0),
      incrButton (0),
      oscilloscope2 (0)
{
    addAndMakeVisible (playButton = new TextButton (T("playNote")));
    playButton->setButtonText (T("play test"));
    playButton->addButtonListener (this);

    addAndMakeVisible (quitButton = new TextButton (T("quitAction")));
    quitButton->setButtonText (T("quit"));
    quitButton->addButtonListener (this);

    addAndMakeVisible (testCombo = new ComboBox (T("test to run")));
    testCombo->setEditableText (false);
    testCombo->setJustificationType (Justification::centredLeft);
    testCombo->setTextWhenNothingSelected (String::empty);
    testCombo->setTextWhenNoChoicesAvailable (T("(no choices)"));
    testCombo->addListener (this);

    addAndMakeVisible (oscilloscope = new AudioOutputWaveformDisplay());
    oscilloscope->setName (T("new component"));

    addAndMakeVisible (label = new Label (T("CSL test"),
                                          T("CSL test")));
    label->setFont (Font (Font::getDefaultSerifFontName(), 20.0000f, Font::bold));
    label->setJustificationType (Justification::centredLeft);
    label->setEditable (false, false, false);
    label->setColour (Label::textColourId, Colour (0xff2013da));
    label->setColour (TextEditor::textColourId, Colours::black);
    label->setColour (TextEditor::backgroundColourId, Colour (0x0));

    addAndMakeVisible (prefsButton = new TextButton (T("new button")));
    prefsButton->setButtonText (T("audio prefs"));
    prefsButton->addButtonListener (this);

    addAndMakeVisible (cpuLabel = new Label (T("new label"),
                                             T("0.0%")));
    cpuLabel->setFont (Font (Font::getDefaultSansSerifFontName(), 15.0000f, Font::bold));
    cpuLabel->setJustificationType (Justification::centredRight);
    cpuLabel->setEditable (false, false, false);
    cpuLabel->setColour (TextEditor::textColourId, Colours::black);
    cpuLabel->setColour (TextEditor::backgroundColourId, Colour (0x0));

    addAndMakeVisible (decrButton = new TextButton (T("new button")));
    decrButton->setButtonText (T("-"));
    decrButton->addButtonListener (this);

    addAndMakeVisible (incrButton = new TextButton (T("new button")));
    incrButton->setButtonText (T("+"));
    incrButton->addButtonListener (this);

    addAndMakeVisible (oscilloscope2 = new AudioOutputWaveformDisplay());
    oscilloscope2->setName (T("new component"));


    //[UserPreSize]
	oscilloscope->initialise(20, 64, false);
	oscilloscope2->initialise(20, 4, true);
    //[/UserPreSize]

    setSize (400, 400);

    //[Constructor] You can add your own custom stuff here..
					// and initialise the device manager with no settings so that it picks a
					// default device to use.
	const String error (mAudioDeviceManager.initialise (0, /* number of input channels */
													   2, /* number of output channels */
													   0, /* no XML settings.. */
													   true  /* select default device on failure */));
	if (error.isNotEmpty())
		AlertWindow::showMessageBox (AlertWindow::WarningIcon,
									 T("CSL Demo"),
									 T("Couldn't open an output device!\n\n") + error);
	else
		mAudioDeviceManager.setAudioCallback (this);
							// set up CSL IO
	unsigned nIn = mAudioDeviceManager.getInputChannels().countNumberOfSetBits();
	unsigned nOut= mAudioDeviceManager.getOutputChannels().countNumberOfSetBits();
	AudioIODevice* audioIO = mAudioDeviceManager.getCurrentAudioDevice();
	unsigned sRate = (unsigned) audioIO->getCurrentSampleRate();
	unsigned bufSize = audioIO->getCurrentBufferSizeSamples();
	theIO = new csl::IO(sRate, bufSize, -1, -1, nIn, nOut);
	theIO->start();

	for (unsigned i = 0; theTestList[i].name != NULL; i++) {
//		printf("\tAdd \"%s\"\n", theTestList[i].name);
		testCombo->addItem(String(theTestList[i].name), i + 1);
	}
	testCombo->setSelectedId(1);
	GcpuLabel = cpuLabel;
	gAudioDeviceManager = & mAudioDeviceManager;
	playThread = 0;
    //[/Constructor]
}

CSLComponent::~CSLComponent()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]

    deleteAndZero (playButton);
    deleteAndZero (quitButton);
    deleteAndZero (testCombo);
    deleteAndZero (oscilloscope);
    deleteAndZero (label);
    deleteAndZero (prefsButton);
    deleteAndZero (cpuLabel);
    deleteAndZero (decrButton);
    deleteAndZero (incrButton);
    deleteAndZero (oscilloscope2);

    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void CSLComponent::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (Colour (0xfff7d2d2));

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void CSLComponent::resized()
{
    playButton->setBounds (240, 352, 150, 24);
    quitButton->setBounds (200, 8, 176, 24);
    testCombo->setBounds (8, 352, 168, 24);
    oscilloscope->setBounds (8, 48, 384, 136);
    oscilloscope2->setBounds (8, 200, 384, 136);
    label->setBounds (8, 378, 150, 24);
    prefsButton->setBounds (16, 8, 176, 24);
    cpuLabel->setBounds (336, 378, 54, 24);
    decrButton->setBounds (182, 352, 24, 24);
    incrButton->setBounds (210, 352, 24, 24);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void CSLComponent::buttonClicked (Button* buttonThatWasClicked)
{
    //[UserbuttonClicked_Pre]
    //[/UserbuttonClicked_Pre]

    if (buttonThatWasClicked == playButton)
    {
        //[UserButtonCode_playButton] -- add your button handler code here..
		int which = testCombo->getSelectedId();
		if (which == 0)
			return;
		playThread = csl::Thread::MakeThread();
		void * (*fcn)(void *) = (void * (*)(void *)) theTestList[which - 1].fcn;
		playThread->createThread(fcn, NULL);			// call the CSL test function
        //[/UserButtonCode_playButton]
    }
    else if (buttonThatWasClicked == quitButton)
    {
        //[UserButtonCode_quitButton] -- add your button handler code here..
		JUCEApplication::quit();
        //[/UserButtonCode_quitButton]
    }
    else if (buttonThatWasClicked == prefsButton)
    {
        //[UserButtonCode_prefsButton] -- add your button handler code here..
			   // Create an AudioDeviceSelectorComponent which contains the audio choice widgets...
		AudioDeviceSelectorComponent audioSettingsComp (mAudioDeviceManager,
														0, 1,
														2, 2,
														true,
														false);
		// ...and show it in a DialogWindow...
		audioSettingsComp.setSize (500, 400);
		DialogWindow::showModalDialog (T("Audio Settings"),
									   &audioSettingsComp,
									   this,
									   Colours::azure,
									   true);
        //[/UserButtonCode_prefsButton]
    }
    else if (buttonThatWasClicked == decrButton)
    {
        //[UserButtonCode_decrButton] -- add your button handler code here..
		int which = testCombo->getSelectedId() - 1;
		if (which == 0)
			which = testCombo->getNumItems();
		testCombo->setSelectedId(which);
        //[/UserButtonCode_decrButton]
    }
    else if (buttonThatWasClicked == incrButton)
    {
        //[UserButtonCode_incrButton] -- add your button handler code here..
		int which = testCombo->getSelectedId() + 1;
		if (which > testCombo->getNumItems())
			which = 1;
		testCombo->setSelectedId(which);
        //[/UserButtonCode_incrButton]
    }

    //[UserbuttonClicked_Post]
    //[/UserbuttonClicked_Post]
}

void CSLComponent::comboBoxChanged (ComboBox* comboBoxThatHasChanged)
{
    //[UsercomboBoxChanged_Pre]
    //[/UsercomboBoxChanged_Pre]

    if (comboBoxThatHasChanged == testCombo)
    {
        //[UserComboBoxCode_testCombo] -- add your combo box handling code here..
		printf("\tSelect test %d\n", testCombo->getSelectedId());
        //[/UserComboBoxCode_testCombo]
    }

    //[UsercomboBoxChanged_Post]
    //[/UsercomboBoxChanged_Post]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...

void CSLComponent::audioDeviceIOCallback (const float** inputChannelData,
							int totalNumInputChannels,
							float** outputChannelData,
							int totalNumOutputChannels,
							int numSamples) {
					// put silence in the output buffers
	for (unsigned i = 0; i < totalNumOutputChannels; i++)
		memset(outputChannelData[i], 0, numSamples * sizeof(float));
					// set up CSL bufefr object
	if (theIO->mGraph) {
		outBuffer.setSize(totalNumOutputChannels, numSamples);
		for (unsigned i = 0; i < totalNumOutputChannels; i++)
			outBuffer.mMonoBuffers[i] = outputChannelData[i];
		try {		// Tell the IO to call its graph
			theIO->pullInput(outBuffer);
		} catch (csl::CException e) {
			printf("Error running CSL: graph\n");
		}
					// pass the audio callback on to the waveform display comp
		oscilloscope->audioDeviceIOCallback (inputChannelData, totalNumInputChannels,
				outputChannelData, totalNumOutputChannels, numSamples);
		oscilloscope2->audioDeviceIOCallback (inputChannelData, totalNumInputChannels,
				outputChannelData, totalNumOutputChannels, numSamples);
	} // else
//		if (playThread != NULL)
//			delete playThread;
}

void CSLComponent::audioDeviceAboutToStart (double sampleRate, int numSamplesPerBlock) {
	oscilloscope->audioDeviceAboutToStart (sampleRate, numSamplesPerBlock);
	oscilloscope2->audioDeviceAboutToStart (sampleRate, numSamplesPerBlock);
}

void CSLComponent::audioDeviceStopped() {
//	oscilloscope->audioDeviceStopped();
}

Component* createCSLComponent() {
    return new CSLComponent();
}


//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Jucer information section --

    This is where the Jucer puts all of its metadata, so don't change anything in here!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="CSLComponent" componentName=""
                 parentClasses="public Component, public AudioIODeviceCallback"
                 constructorParams="" variableInitialisers="" snapPixels="8" snapActive="1"
                 snapShown="1" overlayOpacity="0.330000013" fixedSize="1" initialWidth="400"
                 initialHeight="400">
  <BACKGROUND backgroundColour="fff7d2d2"/>
  <TEXTBUTTON name="playNote" id="ed1811e776eea99b" memberName="playButton"
              virtualName="" explicitFocusOrder="0" pos="240 352 150 24" buttonText="play test"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="quitAction" id="dbaf2871fd41de83" memberName="quitButton"
              virtualName="" explicitFocusOrder="0" pos="200 8 176 24" buttonText="quit"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <COMBOBOX name="test to run" id="bd1a5c541fbc8bc7" memberName="testCombo"
            virtualName="" explicitFocusOrder="0" pos="8 352 168 24" editable="0"
            layout="33" items="" textWhenNonSelected="" textWhenNoItems="(no choices)"/>
  <GENERICCOMPONENT name="new component" id="d64c351a292a43a4" memberName="oscilloscope"
                    virtualName="" explicitFocusOrder="0" pos="8 48 384 136" class="AudioOutputWaveformDisplay"
                    params=""/>
  <LABEL name="CSL test" id="a9876f115ab3c22e" memberName="label" virtualName=""
         explicitFocusOrder="0" pos="8 378 150 24" textCol="ff2013da"
         edTextCol="ff000000" edBkgCol="0" labelText="CSL test" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default serif font"
         fontsize="20" bold="1" italic="0" justification="33"/>
  <TEXTBUTTON name="new button" id="beeb15a1537fd4f6" memberName="prefsButton"
              virtualName="" explicitFocusOrder="0" pos="16 8 176 24" buttonText="audio prefs"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <LABEL name="new label" id="87d3c5b55ea75f76" memberName="cpuLabel"
         virtualName="" explicitFocusOrder="0" pos="336 378 54 24" edTextCol="ff000000"
         edBkgCol="0" labelText="0.0%" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default sans-serif font" fontsize="15"
         bold="1" italic="0" justification="34"/>
  <TEXTBUTTON name="new button" id="713dc52e44a09fd9" memberName="decrButton"
              virtualName="" explicitFocusOrder="0" pos="182 352 24 24" buttonText="-"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="new button" id="b492edebe654bcba" memberName="incrButton"
              virtualName="" explicitFocusOrder="0" pos="210 352 24 24" buttonText="+"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <GENERICCOMPONENT name="new component" id="4aa0f216430fecde" memberName="oscilloscope2"
                    virtualName="" explicitFocusOrder="0" pos="8 200 384 136" class="AudioOutputWaveformDisplay"
                    params=""/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif
