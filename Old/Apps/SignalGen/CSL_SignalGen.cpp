/*
  ==============================================================================

  This is an automatically generated file created by the Jucer!

  Creation date:  16 Nov 2007 6:07:32 pm

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

#include "CSL_SignalGen.h"

//[MiscUserDefs] You can add your own user definitions and misc code here...

// Globals

AudioDeviceManager * gAudioDeviceManager;
using namespace csl;

//[/MiscUserDefs]

//==============================================================================
CSLComponent::CSLComponent ()
    : startButton (0),
      quitButton (0),
      label (0),
      prefsButton (0),
      oscilloscope (0),
      fineFreq (0),
      coarseFreq (0),
      groupComponent (0),
      label2 (0),
      freqLabel (0),
      sineButton (0),
      squareButton (0),
      triangleButton (0),
      sawButton (0),
      whiteButton (0),
      pinkButton (0),
      levelSlider (0),
      levelLabel (0),
      hScaleSlider (0)
{
    addAndMakeVisible (startButton = new TextButton (T("startStop")));
    startButton->setButtonText (T("start"));
    startButton->addButtonListener (this);

    addAndMakeVisible (quitButton = new TextButton (T("quitAction")));
    quitButton->setButtonText (T("quit"));
    quitButton->addButtonListener (this);

    addAndMakeVisible (label = new Label (T("CSL test"),
                                          T("CSL Signal Generator")));
    label->setFont (Font (Font::getDefaultSerifFontName(), 20.0000f, Font::bold));
    label->setJustificationType (Justification::centredLeft);
    label->setEditable (false, false, false);
    label->setColour (Label::textColourId, Colour (0xff2013da));
    label->setColour (TextEditor::textColourId, Colours::black);
    label->setColour (TextEditor::backgroundColourId, Colour (0x0));

    addAndMakeVisible (prefsButton = new TextButton (T("prefsButton")));
    prefsButton->setButtonText (T("audio prefs"));
    prefsButton->addButtonListener (this);

    addAndMakeVisible (oscilloscope = new AudioWaveformDisplay());
    oscilloscope->setName (T("oscilloscope"));

    addAndMakeVisible (fineFreq = new Slider (T("fineFreq")));
    fineFreq->setRange (0.5, 2, 0);
    fineFreq->setSliderStyle (Slider::RotaryVerticalDrag);
    fineFreq->setTextBoxStyle (Slider::NoTextBox, false, 80, 20);
    fineFreq->addListener (this);

    addAndMakeVisible (coarseFreq = new Slider (T("coarseFreq")));
    coarseFreq->setRange (1, 10, 0);
    coarseFreq->setSliderStyle (Slider::RotaryVerticalDrag);
    coarseFreq->setTextBoxStyle (Slider::NoTextBox, false, 80, 20);
    coarseFreq->addListener (this);

    addAndMakeVisible (groupComponent = new GroupComponent (T("waveGroup"),
                                                            T("Waveform")));
    groupComponent->setTextLabelPosition (Justification::centredLeft);

    addAndMakeVisible (label2 = new Label (T("new label"),
                                           T("Freq: coarse/fine")));
    label2->setFont (Font (15.0000f, Font::plain));
    label2->setJustificationType (Justification::centredLeft);
    label2->setEditable (false, false, false);
    label2->setColour (TextEditor::textColourId, Colours::black);
    label2->setColour (TextEditor::backgroundColourId, Colour (0x0));

    addAndMakeVisible (freqLabel = new Label (T("freqLabel"),
                                              T("0.0 Hz")));
    freqLabel->setFont (Font (15.0000f, Font::plain));
    freqLabel->setJustificationType (Justification::centredLeft);
    freqLabel->setEditable (true, true, false);
    freqLabel->setColour (TextEditor::textColourId, Colours::black);
    freqLabel->setColour (TextEditor::backgroundColourId, Colour (0x0));
    freqLabel->addListener (this);

    addAndMakeVisible (sineButton = new ToggleButton (T("sineButton")));
    sineButton->setButtonText (T("Sine"));
    sineButton->setRadioGroupId (42);
    sineButton->addButtonListener (this);

    addAndMakeVisible (squareButton = new ToggleButton (T("squareButton")));
    squareButton->setButtonText (T("Square"));
    squareButton->setRadioGroupId (42);
    squareButton->addButtonListener (this);

    addAndMakeVisible (triangleButton = new ToggleButton (T("triangleButton")));
    triangleButton->setButtonText (T("Triangle"));
    triangleButton->setRadioGroupId (42);
    triangleButton->addButtonListener (this);

    addAndMakeVisible (sawButton = new ToggleButton (T("sawButton")));
    sawButton->setButtonText (T("Sawtooth"));
    sawButton->setRadioGroupId (42);
    sawButton->addButtonListener (this);

    addAndMakeVisible (whiteButton = new ToggleButton (T("whiteButton")));
    whiteButton->setButtonText (T("White"));
    whiteButton->setRadioGroupId (42);
    whiteButton->addButtonListener (this);

    addAndMakeVisible (pinkButton = new ToggleButton (T("pinkButton")));
    pinkButton->setButtonText (T("Pink"));
    pinkButton->setRadioGroupId (42);
    pinkButton->addButtonListener (this);

    addAndMakeVisible (levelSlider = new Slider (T("levelSlider")));
    levelSlider->setRange (0, 1, 0);
    levelSlider->setSliderStyle (Slider::LinearHorizontal);
    levelSlider->setTextBoxStyle (Slider::NoTextBox, true, 30, 20);
    levelSlider->addListener (this);

    addAndMakeVisible (levelLabel = new Label (T("levelLabel"),
                                               T("Level")));
    levelLabel->setFont (Font (15.0000f, Font::plain));
    levelLabel->setJustificationType (Justification::centredLeft);
    levelLabel->setEditable (false, false, false);
    levelLabel->setColour (TextEditor::textColourId, Colours::black);
    levelLabel->setColour (TextEditor::backgroundColourId, Colour (0x0));

    addAndMakeVisible (hScaleSlider = new Slider (T("hScaleSlider")));
    hScaleSlider->setRange (1, 5, 1);
    hScaleSlider->setSliderStyle (Slider::LinearVertical);
    hScaleSlider->setTextBoxStyle (Slider::NoTextBox, true, 80, 20);
    hScaleSlider->addListener (this);


    //[UserPreSize]
	oscilloscope->initialise(-1, 15, 2, true);		// channel, rate, window, zeroX
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

	gAudioDeviceManager = & mAudioDeviceManager;
	playThread = 0;

	source = NULL;
	sos = new csl::SumOfSines(kFrequency, 1, 1);	// use SOS for most sources
	sineButton->setToggleState (true, true);
    levelSlider->setValue(0.25, true);
    coarseFreq->setValue(4.0, true);
    fineFreq->setValue(1.0, true);
    hScaleSlider->setValue(2.0, true);
	this->setFreqLabel();
	this->setSource(sos);
	useFreq = true;
	isPlaying = false;
    //[/Constructor]
}

CSLComponent::~CSLComponent()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]

    deleteAndZero (startButton);
    deleteAndZero (quitButton);
    deleteAndZero (label);
    deleteAndZero (prefsButton);
    deleteAndZero (oscilloscope);
    deleteAndZero (fineFreq);
    deleteAndZero (coarseFreq);
    deleteAndZero (groupComponent);
    deleteAndZero (label2);
    deleteAndZero (freqLabel);
    deleteAndZero (sineButton);
    deleteAndZero (squareButton);
    deleteAndZero (triangleButton);
    deleteAndZero (sawButton);
    deleteAndZero (whiteButton);
    deleteAndZero (pinkButton);
    deleteAndZero (levelSlider);
    deleteAndZero (levelLabel);
    deleteAndZero (hScaleSlider);

    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void CSLComponent::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (Colour (0xffeecda2));

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void CSLComponent::resized()
{
    startButton->setBounds (208, 8, 176, 24);
    quitButton->setBounds (208, 368, 176, 24);
    label->setBounds (24, 368, 150, 24);
    prefsButton->setBounds (16, 8, 176, 24);
    oscilloscope->setBounds (24, 198, 368, 162);
    fineFreq->setBounds (312, 64, 64, 64);
    coarseFreq->setBounds (232, 64, 64, 64);
    groupComponent->setBounds (16, 40, 200, 150);
    label2->setBounds (248, 40, 120, 24);
    freqLabel->setBounds (272, 128, 96, 24);
    sineButton->setBounds (32, 72, 72, 24);
    squareButton->setBounds (120, 72, 72, 24);
    triangleButton->setBounds (32, 152, 88, 24);
    sawButton->setBounds (32, 112, 88, 24);
    whiteButton->setBounds (120, 112, 72, 24);
    pinkButton->setBounds (120, 152, 72, 24);
    levelSlider->setBounds (232, 168, 150, 24);
    levelLabel->setBounds (240, 150, 120, 24);
    hScaleSlider->setBounds (8, 192, 16, 168);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void CSLComponent::buttonClicked (Button* buttonThatWasClicked)
{
    //[UserbuttonClicked_Pre]
    //[/UserbuttonClicked_Pre]

    if (buttonThatWasClicked == startButton)
    {
        //[UserButtonCode_startButton] -- add your button handler code here..
		if (isPlaying) {
			oscilloscope->stop();
			startButton->setButtonText(T("start"));
			isPlaying = false;
//			theIO->stop();
		} else {
			startButton->setButtonText(T("stop"));
			oscilloscope->start();
			isPlaying = true;
//			theIO->start();
		}
        //[/UserButtonCode_startButton]
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
		oscilloscope->stop();			// stop scope display during dialog
		isPlaying = false;
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
		oscilloscope->start();			// when done, restart scope
		isPlaying = true;
        //[/UserButtonCode_prefsButton]
    }
    else if (buttonThatWasClicked == sineButton)
    {
        //[UserButtonCode_sineButton] -- add your button handler code here..
		useFreq = true;
		sos->clearPartials();
		sos->addPartial(1, 1);				// sine
        //[/UserButtonCode_sineButton]
    }
    else if (buttonThatWasClicked == squareButton)
    {
        //[UserButtonCode_squareButton] -- add your button handler code here..
		useFreq = true;
		sos->clearPartials();
		float maxHarm = ((float) csl::CGestalt::frameRate() / (this->frequency() * 2.0f ));
		for (float i = 1.0f; i < maxHarm; i += 2.0f)
			sos->addPartial(i, 0.5 / i);				// odd harmonics scaled by 1 / i
		this->setSource(sos);
        //[/UserButtonCode_squareButton]
    }
    else if (buttonThatWasClicked == triangleButton)
    {
        //[UserButtonCode_triangleButton] -- add your button handler code here..
		useFreq = true;								// plug in a sum-of-sines gen
		sos->clearPartials();
		float maxHarm = ((float) csl::CGestalt::frameRate() / (this->frequency() * 2.0f ));
		for (float i = 1.0f; i < maxHarm; i += 2.0f)
			sos->addPartial(i, 1 / (i * i));			// triangle is scaled by 1 / i**2
		this->setSource(sos);
        //[/UserButtonCode_triangleButton]
    }
    else if (buttonThatWasClicked == sawButton)
    {
        //[UserButtonCode_sawButton] -- add your button handler code here..
		useFreq = true;
		sos->clearPartials();
		float maxHarm = ((float) csl::CGestalt::frameRate() / (this->frequency() * 2.0f ));
		for (float i = 1.0f; i < maxHarm; i += 1.0f)
			sos->addPartial(i, 0.5 / i);			// saw is 1 / i
		this->setSource(sos);
        //[/UserButtonCode_sawButton]
    }
    else if (buttonThatWasClicked == whiteButton)
    {
        //[UserButtonCode_whiteButton] -- add your button handler code here..
		useFreq = false;
		this->setSource(new csl::WhiteNoise());			// plug in a noise gen
        //[/UserButtonCode_whiteButton]
    }
    else if (buttonThatWasClicked == pinkButton)
    {
        //[UserButtonCode_pinkButton] -- add your button handler code here..
		useFreq = false;
 		this->setSource(new csl::PinkNoise());			// plug in a noise gen
        //[/UserButtonCode_pinkButton]
    }

    //[UserbuttonClicked_Post]
    //[/UserbuttonClicked_Post]
}

void CSLComponent::sliderValueChanged (Slider* sliderThatWasMoved)
{
    //[UsersliderValueChanged_Pre]
    //[/UsersliderValueChanged_Pre]

    if (sliderThatWasMoved == fineFreq)
    {
        //[UserSliderCode_fineFreq] -- add your slider handling code here..
		this->setFreqLabel();
		if (useFreq)
			((csl::Phased *) source)->setFrequency(this->frequency());		// reset freq
        //[/UserSliderCode_fineFreq]
    }
    else if (sliderThatWasMoved == coarseFreq)
    {
        //[UserSliderCode_coarseFreq] -- add your slider handling code here..
		this->setFreqLabel();
		if (useFreq)
			((csl::Phased *) source)->setFrequency(this->frequency());		// reset freq
        //[/UserSliderCode_coarseFreq]
    }
    else if (sliderThatWasMoved == levelSlider)
    {
        //[UserSliderCode_levelSlider] -- add your slider handling code here..
		((csl::Scalable *) source)->setScale(this->amplitude());		// reset amplitude (linear)
        //[/UserSliderCode_levelSlider]
    }
    else if (sliderThatWasMoved == hScaleSlider)
    {
        //[UserSliderCode_hScaleSlider] -- add your slider handling code here..
		oscilloscope->setSamplesToAverage((unsigned) hScaleSlider->getValue());
        //[/UserSliderCode_hScaleSlider]
    }

    //[UsersliderValueChanged_Post]
    //[/UsersliderValueChanged_Post]
}

void CSLComponent::labelTextChanged (Label* labelThatHasChanged)
{
    //[UserlabelTextChanged_Pre]
	float frqVal;
    //[/UserlabelTextChanged_Pre]

    if (labelThatHasChanged == freqLabel)
    {
        //[UserLabelCode_freqLabel] -- add your label text handling code here..
		String val = freqLabel->getText(true);			// handle type-in of freq value
		frqVal = val.getFloatValue();
		frqVal = log2(frqVal / 10.0);					// log-2 for octave
		fineFreq->setValue(1.0, true);					// reset fine freq to 1
		coarseFreq->setValue(frqVal, true);				// set coarse freq to octave
		this->setFreqLabel();
		if (useFreq)
			((csl::Phased *) source)->setFrequency(this->frequency());	// reset osc freq
        //[/UserLabelCode_freqLabel]
    }

    //[UserlabelTextChanged_Post]
    //[/UserlabelTextChanged_Post]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...

float CSLComponent::frequency() {
	double octave = pow(2.0, coarseFreq->getValue());
	return ((float) (10.0 * octave * fineFreq->getValue()));
}

float CSLComponent::amplitude() {
	return ((float) levelSlider->getValue());
}

void CSLComponent::setFreqLabel() {
		char lab[16];
		sprintf(lab, "%8.2f Hz", this->frequency());
		freqLabel->setText(String(lab), false);
}

void CSLComponent::setSource(csl::UnitGenerator * src) {
	bool wasPlaying = isPlaying;
	isPlaying = false;
//	csl::UnitGenerator * tempSource = NULL;
//	if (source != NULL)
//		tempSource = source;		// store for later (to be safe)
	source = src;					// plug in new one
//	if (tempSource != NULL)			// now delete old one
//		delete tempSource;
	((csl::Scalable *) source)->setScale(this->amplitude());
	if (useFreq)
		((csl::Phased *) source)->setFrequency(this->frequency());
	isPlaying = wasPlaying;
}

void CSLComponent::audioDeviceIOCallback (const float** inputChannelData,
							int totalNumInputChannels,
							float** outputChannelData,
							int totalNumOutputChannels,
							int numSamples) {
					// put silence in the output buffers
	for (unsigned i = 0; i < totalNumOutputChannels; i++)
		memset(outputChannelData[i], 0, numSamples * sizeof(float));
					// set up CSL bufefr object
	if (isPlaying) {
		outBuffer.setSize(totalNumOutputChannels, numSamples);
		for (unsigned i = 0; i < totalNumOutputChannels; i++)
			outBuffer.mMonoBuffers[i] = outputChannelData[i];
		try {						// Get a buffer from the CSL graph
			source->nextBuffer(outBuffer);
		} catch (csl::CException e) {
			printf("Error running CSL graph\n");
		}
					// pass the audio callback on to the waveform display comp
		oscilloscope->audioDeviceIOCallback (inputChannelData, 0,
				outputChannelData, totalNumOutputChannels, numSamples);
	}
}

void CSLComponent::audioDeviceAboutToStart (double sampleRate, int numSamplesPerBlock) {
	oscilloscope->audioDeviceAboutToStart (sampleRate, numSamplesPerBlock);
	csl::CGestalt::setFrameRate((unsigned) sampleRate);		// set CSL globals
	csl::CGestalt::setBlockSize(numSamplesPerBlock);
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
  <BACKGROUND backgroundColour="ffeecda2"/>
  <TEXTBUTTON name="startStop" id="ed1811e776eea99b" memberName="startButton"
              virtualName="" explicitFocusOrder="0" pos="208 8 176 24" buttonText="start"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="quitAction" id="dbaf2871fd41de83" memberName="quitButton"
              virtualName="" explicitFocusOrder="0" pos="208 368 176 24" buttonText="quit"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <LABEL name="CSL test" id="a9876f115ab3c22e" memberName="label" virtualName=""
         explicitFocusOrder="0" pos="24 368 150 24" textCol="ff2013da"
         edTextCol="ff000000" edBkgCol="0" labelText="CSL Signal Generator"
         editableSingleClick="0" editableDoubleClick="0" focusDiscardsChanges="0"
         fontname="Default serif font" fontsize="20" bold="1" italic="0"
         justification="33"/>
  <TEXTBUTTON name="prefsButton" id="beeb15a1537fd4f6" memberName="prefsButton"
              virtualName="" explicitFocusOrder="0" pos="16 8 176 24" buttonText="audio prefs"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <GENERICCOMPONENT name="oscilloscope" id="4aa0f216430fecde" memberName="oscilloscope"
                    virtualName="" explicitFocusOrder="0" pos="24 198 368 162" class="AudioOutputWaveformDisplay"
                    params=""/>
  <SLIDER name="fineFreq" id="6e81021a69259355" memberName="fineFreq" virtualName=""
          explicitFocusOrder="0" pos="312 64 64 64" min="0.5" max="2" int="0"
          style="RotaryVerticalDrag" textBoxPos="NoTextBox" textBoxEditable="1"
          textBoxWidth="80" textBoxHeight="20" skewFactor="1"/>
  <SLIDER name="coarseFreq" id="66c0db9cce829f5c" memberName="coarseFreq"
          virtualName="" explicitFocusOrder="0" pos="232 64 64 64" min="1"
          max="10" int="0" style="RotaryVerticalDrag" textBoxPos="NoTextBox"
          textBoxEditable="1" textBoxWidth="80" textBoxHeight="20" skewFactor="1"/>
  <GROUPCOMPONENT name="waveGroup" id="dfc7af10f24194d7" memberName="groupComponent"
                  virtualName="" explicitFocusOrder="0" pos="16 40 200 150" title="Waveform"
                  textpos="33"/>
  <LABEL name="new label" id="5c653f8f3143c6cc" memberName="label2" virtualName=""
         explicitFocusOrder="0" pos="248 40 120 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Freq: coarse/fine" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="15" bold="0" italic="0" justification="33"/>
  <LABEL name="freqLabel" id="c492c186b6a35d50" memberName="freqLabel"
         virtualName="" explicitFocusOrder="0" pos="272 128 96 24" edTextCol="ff000000"
         edBkgCol="0" labelText="0.0 Hz" editableSingleClick="1" editableDoubleClick="1"
         focusDiscardsChanges="0" fontname="Default font" fontsize="15"
         bold="0" italic="0" justification="33"/>
  <TOGGLEBUTTON name="sineButton" id="5568f0c928443d2e" memberName="sineButton"
                virtualName="" explicitFocusOrder="0" pos="32 72 72 24" buttonText="Sine"
                connectedEdges="0" needsCallback="1" radioGroupId="42" state="0"/>
  <TOGGLEBUTTON name="squareButton" id="73d2b8d9cdb2cf3f" memberName="squareButton"
                virtualName="" explicitFocusOrder="0" pos="120 72 72 24" buttonText="Square"
                connectedEdges="0" needsCallback="1" radioGroupId="42" state="0"/>
  <TOGGLEBUTTON name="triangleButton" id="2f078f116065cd70" memberName="triangleButton"
                virtualName="" explicitFocusOrder="0" pos="32 152 88 24" buttonText="Triangle"
                connectedEdges="0" needsCallback="1" radioGroupId="42" state="0"/>
  <TOGGLEBUTTON name="sawButton" id="c81ecbff764d2cf0" memberName="sawButton"
                virtualName="" explicitFocusOrder="0" pos="32 112 88 24" buttonText="Sawtooth"
                connectedEdges="0" needsCallback="1" radioGroupId="42" state="0"/>
  <TOGGLEBUTTON name="whiteButton" id="38da59504af1d79f" memberName="whiteButton"
                virtualName="" explicitFocusOrder="0" pos="120 112 72 24" buttonText="White"
                connectedEdges="0" needsCallback="1" radioGroupId="42" state="0"/>
  <TOGGLEBUTTON name="pinkButton" id="57b1732d475daaee" memberName="pinkButton"
                virtualName="" explicitFocusOrder="0" pos="120 152 72 24" buttonText="Pink"
                connectedEdges="0" needsCallback="1" radioGroupId="42" state="0"/>
  <SLIDER name="levelSlider" id="57973641101accb8" memberName="levelSlider"
          virtualName="" explicitFocusOrder="0" pos="232 168 150 24" min="0"
          max="1" int="0" style="LinearHorizontal" textBoxPos="NoTextBox"
          textBoxEditable="0" textBoxWidth="30" textBoxHeight="20" skewFactor="1"/>
  <LABEL name="levelLabel" id="ae376c25d2a2fda1" memberName="levelLabel"
         virtualName="" explicitFocusOrder="0" pos="240 150 120 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Level" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="15"
         bold="0" italic="0" justification="33"/>
  <SLIDER name="hScaleSlider" id="497a2b3c00bd1d89" memberName="hScaleSlider"
          virtualName="" explicitFocusOrder="0" pos="8 192 16 168" min="1"
          max="5" int="1" style="LinearVertical" textBoxPos="NoTextBox"
          textBoxEditable="0" textBoxWidth="80" textBoxHeight="20" skewFactor="1"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif
