/*
  ==============================================================================

  This is an automatically generated file created by the Jucer!

  Creation date:  27 Oct 2009 10:19:22 pm

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

#include "CSLMIDIComponent.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...

#include "MIDIIOJ.h"
#include <iostream>
using namespace csl;

// Globals are here

extern IO * theIO;								// global IO object accessed by other threads
Label* gCPULabel = 0;							// CPU % label
AudioDeviceManager * gAudioDeviceManager = 0;	// global JUCE audio device mgr
unsigned argCnt = 0;							// globals for argc/v from cmd-line
char **argVals;
VoidFcnPtrN * gFcn = 0;							// test function ptr

//[/MiscUserDefs]

//==============================================================================
CSLMIDIComponent::CSLMIDIComponent ()
    : quitButton (0),
      label (0),
      textEditor (0),
      clearButton (0),
      audioDeviceSelector (0),
      filtButton (0)
{
    addAndMakeVisible (quitButton = new TextButton (T("new button")));
    quitButton->setButtonText (T("Quit"));
    quitButton->addButtonListener (this);

    addAndMakeVisible (label = new Label (T("new label"),
                                          T("CSL MIDI Input Dump")));
    label->setFont (Font (24.0000f, Font::plain));
    label->setJustificationType (Justification::centredLeft);
    label->setEditable (false, false, false);
    label->setColour (TextEditor::textColourId, Colours::black);
    label->setColour (TextEditor::backgroundColourId, Colour (0x0));

    addAndMakeVisible (textEditor = new TextEditor (T("new text editor")));
    textEditor->setMultiLine (false);
    textEditor->setReturnKeyStartsNewLine (false);
    textEditor->setReadOnly (false);
    textEditor->setScrollbarsShown (true);
    textEditor->setCaretVisible (true);
    textEditor->setPopupMenuEnabled (true);
    textEditor->setText (String::empty);

    addAndMakeVisible (clearButton = new TextButton (T("new button")));
    clearButton->setButtonText (T("Clear"));
    clearButton->addButtonListener (this);

//    addAndMakeVisible (audioDeviceSelector = new AudioDeviceSelectorComponent());
//    audioDeviceSelector->setName (T("new component"));

    addAndMakeVisible (filtButton = new ToggleButton (T("new toggle button")));
    filtButton->setButtonText (T("Filter Note On/Off"));
    filtButton->addButtonListener (this);


    //[UserPreSize]
    //[/UserPreSize]

    setSize (650, 350);

    //[Constructor] You can add your own custom stuff here..

	filt = false;
	cLin = new char[CSL_STR_LEN];
	cLin[0] = 0;
	textEditor->setMultiLine (true, true);
	textEditor->setReturnKeyStartsNewLine (true);
	textEditor->setText (T("MIDI Messages\n\n"));
	textEditor->setPasswordCharacter(0);

//	   AudioDeviceSelectorComponent (AudioDeviceManager& deviceManager,
//                                  const int minAudioInputChannels,
//                                  const int maxAudioInputChannels,
//                                  const int minAudioOutputChannels,
//                                  const int maxAudioOutputChannels,
//                                  const bool showMidiInputOptions,
//                                  const bool showMidiOutputSelector,
//                                  const bool showChannelsAsStereoPairs,
//                                  const bool hideAdvancedOptionsWithButton);

    addAndMakeVisible (audioDeviceSelector = new AudioDeviceSelectorComponent
					(mAudioDeviceManager,
					 0, 0, 0, 0,
					 true, true, false, false));
    audioDeviceSelector->setName (T("new component"));
	audioDeviceSelector->setSize (500, 450);

/// Create a MIDI in and attach a filtering listener to it

	mIn = new MIDIIn;					// create the midi in
	mIn->open(DEFAULT_MIDI_IN);			// open it
//	lst.mPeriod = 0.25;
//	this->mKey = kNoteOn;				// filter noteOn events
	mIn->attachObserver(this);			// attach this as observer of input
	logMsg("Start MIDI listener");
	mIn->start();						// start midi in
	this->startTimer(100);				// 0.1-sec timer

    //[/Constructor]
}

CSLMIDIComponent::~CSLMIDIComponent()
{
    //[Destructor_pre]. You can add your own custom destruction code here..

	logMsg("done.");
	mIn->stop();
	mIn->close();

    //[/Destructor_pre]

    deleteAndZero (quitButton);
    deleteAndZero (label);
    deleteAndZero (textEditor);
    deleteAndZero (clearButton);
    deleteAndZero (audioDeviceSelector);
    deleteAndZero (filtButton);

    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void CSLMIDIComponent::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (Colour (0xff829bb1));

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void CSLMIDIComponent::resized()
{
    quitButton->setBounds (272, 312, 104, 32);
    label->setBounds (16, 288, 224, 24);
    textEditor->setBounds (392, 8, 248, 336);
    clearButton->setBounds (272, 272, 102, 32);
//    audioDeviceSelector->setBounds (16, 16, 360, 200);
    filtButton->setBounds (232, 232, 144, 24);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void CSLMIDIComponent::buttonClicked (Button* buttonThatWasClicked)
{
    //[UserbuttonClicked_Pre]
    //[/UserbuttonClicked_Pre]

    if (buttonThatWasClicked == quitButton)
    {
        //[UserButtonCode_quitButton] -- add your button handler code here..
		
 		JUCEApplication::quit();
		
        //[/UserButtonCode_quitButton]
    }
    else if (buttonThatWasClicked == clearButton)
    {
        //[UserButtonCode_clearButton] -- add your button handler code here..
		
		textEditor->clear();
		
        //[/UserButtonCode_clearButton]
    }
    else if (buttonThatWasClicked == filtButton)
    {
        //[UserButtonCode_filtButton] -- add your button handler code here..

		filt = ! filt;
	
        //[/UserButtonCode_filtButton]
    }

    //[UserbuttonClicked_Post]

    //[/UserbuttonClicked_Post]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...

// update() method takes a MIDI message and prints it

void CSLMIDIComponent::update(void * arg) {
	CMIDIMessage * msg = (CMIDIMessage *) arg;
	mIn->clear();
//	logMsg("MIDI msg %d  k %d  v %d", msg->message, msg->getNote(), msg->getVelocity());
	if (filt && (! msg->isNoteOnOff()))
		return;
	char * cptr = cLin + strlen(cLin);
	sprintf(cptr, "    %.2d  k %.2d  v %.2d\n", msg->message, msg->getNote(), msg->getVelocity());
	changed = true;							// set changed flag for timer thread
}

// timer call checks changed and prints messages

void CSLMIDIComponent::timerCallback() {
//	CSLAbstComponent::timerCallback();
	if ( ! changed) return;
	String lStr(cLin);
	const MessageManagerLock mmLock;	// create the lock so we can call the GUI from this thread
	textEditor->setCaretPosition(textEditor->getTotalNumChars());
	textEditor->insertTextAtCursor(lStr);
	cLin[0] = 0;				// reset string
	changed = false;			// reset flag
}

// create the component -- called from main()

Component* createCSLComponent() {
    return new CSLMIDIComponent();
}

//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Jucer information section --

    This is where the Jucer puts all of its metadata, so don't change anything in here!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="CSLMIDIComponent" componentName=""
                 parentClasses="public CSLAbstComponent" constructorParams=""
                 variableInitialisers="" snapPixels="8" snapActive="1" snapShown="1"
                 overlayOpacity="0.330000013" fixedSize="1" initialWidth="650"
                 initialHeight="350">
  <BACKGROUND backgroundColour="ff829bb1"/>
  <TEXTBUTTON name="new button" id="c0692c09dc89d8ab" memberName="quitButton"
              virtualName="" explicitFocusOrder="0" pos="272 312 104 32" buttonText="Quit"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <LABEL name="new label" id="7dccc1a8e08b292d" memberName="label" virtualName=""
         explicitFocusOrder="0" pos="16 288 224 24" edTextCol="ff000000"
         edBkgCol="0" labelText="CSL MIDI Input Dump" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="24" bold="0" italic="0" justification="33"/>
  <TEXTEDITOR name="new text editor" id="2df8e41197d78c6c" memberName="textEditor"
              virtualName="" explicitFocusOrder="0" pos="392 8 248 336" initialText=""
              multiline="0" retKeyStartsLine="0" readonly="0" scrollbars="1"
              caret="1" popupmenu="1"/>
  <TEXTBUTTON name="new button" id="fec4a7f49a098957" memberName="clearButton"
              virtualName="" explicitFocusOrder="0" pos="272 272 102 32" buttonText="Clear"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <GENERICCOMPONENT name="new component" id="b1bc16d7b6e9ef79" memberName="audioDeviceSelector"
                    virtualName="" explicitFocusOrder="0" pos="16 16 360 200" class="AudioDeviceSelectorComponent"
                    params=""/>
  <TOGGLEBUTTON name="new toggle button" id="9ff31f17342ed08f" memberName="filtButton"
                virtualName="" explicitFocusOrder="0" pos="232 232 144 24" buttonText="Filter Note On/Off"
                connectedEdges="0" needsCallback="1" radioGroupId="0" state="0"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif
