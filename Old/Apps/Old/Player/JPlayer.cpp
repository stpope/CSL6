/*
  ==============================================================================

  This is an automatically generated file created by the Jucer!

  Creation date:  28 Nov 2007 12:36:42 pm

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

#include "JPlayer.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...

// Globals

AudioDeviceManager * gAudioDeviceManager;
using namespace csl;

//[/MiscUserDefs]

//==============================================================================
CSLComponent::CSLComponent ()
    : label (0),
      oscilloscope (0),
      VUMeterL (0),
      VUMeterR (0),
      levelSlider (0),
      directoryTree (0),
      fileList (0),
      scopeButton (0)
{
    addAndMakeVisible (label = new Label (T("CSL test"),
                                          T("CSL Player")));
    label->setFont (Font (Font::getDefaultSerifFontName(), 30.0000f, Font::bold));
    label->setJustificationType (Justification::centredLeft);
    label->setEditable (false, false, false);
    label->setColour (Label::textColourId, Colours::blue);
    label->setColour (TextEditor::textColourId, Colours::black);
    label->setColour (TextEditor::backgroundColourId, Colour (0x0));

    addAndMakeVisible (oscilloscope = new AudioWaveformDisplay());
    oscilloscope->setName (T("oscilloscope"));

    addAndMakeVisible (VUMeterL = new VUMeter());
    VUMeterL->setName (T("VUMeterL"));

    addAndMakeVisible (VUMeterR = new VUMeter());
    VUMeterR->setName (T("VUMeterR"));

    addAndMakeVisible (levelSlider = new Slider (T("new slider")));
    levelSlider->setRange (-2, 6, 0);
    levelSlider->setSliderStyle (Slider::LinearHorizontal);
    levelSlider->setTextBoxStyle (Slider::NoTextBox, true, 80, 20);
    levelSlider->addListener (this);


    addAndMakeVisible (fileList = new TableListBox());
    fileList->setName (T("new component"));

    addAndMakeVisible (scopeButton = new TextButton (T("new button")));
    scopeButton->setButtonText (T("scope"));
    scopeButton->addButtonListener (this);


    //[UserPreSize]
//////////////////////////////////////////////////////////////////
	oscilloscope->initialise(-1, 10, 2, true);	// unsigned rate, unsigned window, bool zeroX
												// Create the VU meters
	VUMeterL = new VUMeter(0, VUMeter::Vertical, 32);
	VUMeterL->setDecay(33,2);
	addAndMakeVisible(VUMeterL);

	VUMeterR = new VUMeter(1, VUMeter::Vertical, 32);
	VUMeterR->setDecay(33,2);
	addAndMakeVisible(VUMeterR);

//////////////////////////////////////////////////////////////////
    //[/UserPreSize]

    setSize (600, 600);

    //[Constructor] You can add your own custom stuff here..
					// and initialise the device manager with no settings so that it picks a
					// default device to use.
//////////////////////////////////////////////////////////////////
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

				// set up CSL granulator objects
	isPlaying = false;
			// set slider defaults
	levelSlider->setValue(0);
	
	OwnedArray <File> roots;
	File::findFileSystemRoots (roots);
	directoryList = new DirectoryContentsList (0, thread);
	directoryList->setDirectory (*roots[0], true, true);
    addAndMakeVisible (directoryTree = new FileTreeComponent (directoryList));


//////////////////////////////////////////////////////////////////
    //[/Constructor]
}

CSLComponent::~CSLComponent()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]

    deleteAndZero (label);
    deleteAndZero (oscilloscope);
    deleteAndZero (VUMeterL);
    deleteAndZero (VUMeterR);
    deleteAndZero (levelSlider);
    deleteAndZero (directoryTree);
    deleteAndZero (fileList);
    deleteAndZero (scopeButton);

    //[Destructor]. You can add your own custom destruction code here..
        delete directoryList;
    //[/Destructor]
}

//==============================================================================
void CSLComponent::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (Colour (0xffc1d5d5));

        if (directoryTree != 0)
            g.drawRect (directoryTree->getX(), directoryTree->getY(),
                        directoryTree->getWidth(), directoryTree->getHeight());

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void CSLComponent::resized()
{
    label->setBounds (8, 568, 152, 29);
    oscilloscope->setBounds (8, 416, 536, 144);
    VUMeterL->setBounds (552, 416, 16, 144);
    VUMeterR->setBounds (576, 416, 16, 144);
    levelSlider->setBounds (248, 568, 344, 24);
    directoryTree->setBounds (8, 8, 232, 400);
    fileList->setBounds (248, 8, 344, 400);
    scopeButton->setBounds (152, 568, 88, 24);
    //[UserResized] Add your own custom resize handling here..
            fileTreeComp->setBoundsRelative (0.05f, 0.07f, 0.9f, 0.9f);
    //[/UserResized]
}

void CSLComponent::sliderValueChanged (Slider* sliderThatWasMoved)
{
    //[UsersliderValueChanged_Pre]
    //[/UsersliderValueChanged_Pre]

    if (sliderThatWasMoved == levelSlider)
    {
        //[UserSliderCode_levelSlider] -- add your slider handling code here..
        //[/UserSliderCode_levelSlider]
    }

    //[UsersliderValueChanged_Post]
    //[/UsersliderValueChanged_Post]
}

void CSLComponent::buttonClicked (Button* buttonThatWasClicked)
{
    //[UserbuttonClicked_Pre]
    //[/UserbuttonClicked_Pre]

    if (buttonThatWasClicked == scopeButton)
    {
        //[UserButtonCode_scopeButton] -- add your button handler code here..
        //[/UserButtonCode_scopeButton]
    }

    //[UserbuttonClicked_Post]
    //[/UserbuttonClicked_Post]
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
					// set up CSL buffer object
	if (isPlaying) {
					// pass the audio callback on to the waveform display & VU metercompoments
		oscilloscope->audioDeviceIOCallback (inputChannelData, 0,
				outputChannelData, totalNumOutputChannels, numSamples);
		VUMeterL->audioDeviceIOCallback (inputChannelData, 0,
				outputChannelData, totalNumOutputChannels, numSamples);
		VUMeterR->audioDeviceIOCallback (inputChannelData, 0,
				outputChannelData, totalNumOutputChannels, numSamples);
	}
}

void CSLComponent::audioDeviceAboutToStart (double sampleRate, int numSamplesPerBlock) {
	oscilloscope->audioDeviceAboutToStart (sampleRate, numSamplesPerBlock);
	csl::CGestalt::setBlockSize(numSamplesPerBlock);
}

void CSLComponent::audioDeviceStopped() {
//	oscilloscope->audioDeviceStopped();
}

Component* createCSLComponent() {
    return new CSLComponent();
}

// put up a file dialog to select an AIFF file

void CSLComponent::loadFile() {
        FileChooser myChooser ("Please select the file to granulate...",
                               File::getSpecialLocation (File::userHomeDirectory),
                               "*.aiff");
        if (myChooser.browseForFileToOpen()) {
            File sndFile (myChooser.getResult());
					// open and read in the file
			AiffAudioFormat waf;
			AudioFormatReader * theSource = waf.createReaderFor(sndFile.createInputStream(), false);
			unsigned numSamples = theSource->lengthInSamples;
			unsigned numChannels = theSource->numChannels;
					// allocate storage buffers
			int ** destSamples = (int** ) juce_calloc(numChannels * sizeof(int *));
			for (unsigned i = 0; i < numChannels; i++)
				destSamples[i] = (int*) juce_calloc(numSamples * sizeof(int));
			AudioSubsectionReader theReader(theSource, 0, numSamples, true);
			theReader.read(destSamples, 0l, numSamples);
					// now convert it to floats
			float * fSamples = (float * ) juce_calloc(numSamples * sizeof(float));
			for (unsigned i = 0; i < numSamples; i++)
				fSamples[i] = ((float) destSamples[0][i]) / (float) 0x80000000;
  			cloud->mSamples = fSamples;
			cloud->numSamples = numSamples;
      }
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
                 snapShown="1" overlayOpacity="0.330000013" fixedSize="1" initialWidth="600"
                 initialHeight="600">
  <BACKGROUND backgroundColour="ffc1d5d5"/>
  <LABEL name="CSL test" id="a9876f115ab3c22e" memberName="label" virtualName=""
         explicitFocusOrder="0" pos="8 568 152 29" textCol="ff0000ff"
         edTextCol="ff000000" edBkgCol="0" labelText="CSL Player" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default serif font"
         fontsize="30" bold="1" italic="0" justification="33"/>
  <GENERICCOMPONENT name="oscilloscope" id="4aa0f216430fecde" memberName="oscilloscope"
                    virtualName="" explicitFocusOrder="0" pos="8 416 536 144" class="AudioWaveformDisplay"
                    params=""/>
  <GENERICCOMPONENT name="VUMeterL" id="2d6560d1efd79b93" memberName="VUMeterL" virtualName=""
                    explicitFocusOrder="0" pos="552 416 16 144" class="VUMeter" params=""/>
  <GENERICCOMPONENT name="VUMeterR" id="a532c6b2609a9799" memberName="VUMeterR" virtualName=""
                    explicitFocusOrder="0" pos="576 416 16 144" class="VUMeter" params=""/>
  <SLIDER name="new slider" id="7d42c91d70e1546f" memberName="levelSlider"
          virtualName="" explicitFocusOrder="0" pos="248 568 344 24" min="-2"
          max="6" int="0" style="LinearHorizontal" textBoxPos="NoTextBox"
          textBoxEditable="0" textBoxWidth="80" textBoxHeight="20" skewFactor="1"/>
  <TREEVIEW name="new treeview" id="df7f02a4b6898b40" memberName="directoryTree"
            virtualName="" explicitFocusOrder="0" pos="8 8 232 400" rootVisible="1"
            openByDefault="0"/>
  <GENERICCOMPONENT name="new component" id="c3cc264986436ddb" memberName="fileList"
                    virtualName="" explicitFocusOrder="0" pos="248 8 344 400" class="TableListBox"
                    params=""/>
  <TEXTBUTTON name="new button" id="398a81546310f9f5" memberName="scopeButton"
              virtualName="" explicitFocusOrder="0" pos="152 568 88 24" buttonText="scope"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif
