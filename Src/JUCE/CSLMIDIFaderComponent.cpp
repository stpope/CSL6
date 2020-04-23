/*
  ==============================================================================

  This is an automatically generated file created by the Jucer!

  Creation date:  24 Oct 2009 11:43:13 pm

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

#include "CSLMIDIFaderComponent.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...

#include "MIDIIOJ.h"
#include <iostream>
using namespace csl;

// Globals are here

extern IO * theIO;								// global IO object accessed by other threads
Label* gCPULabel = 0;							// CPU % label...
AudioDeviceManager * gAudioDeviceManager = 0;	// global JUCE audio device mgr
unsigned argCnt = 0;							// globals for argc/v from cmd-line
char **argVals;
VoidFcnPtrN * gFcn = 0;							// test function ptr


/// Test mixer -- really slow additive synthesis

void playSineMixer() {
	Mixer mix(2);					// create a stereo mixer
	for (int i = 0; i < 16; i++) {
		Osc * vox = new Osc(fRandM(100, 140), 0.05);		// create scaled sine waves
		Panner * pan = new Panner(*vox, ((float) i / 8.0f) - 1.0f);		// panner on the sine
		mix.addInput(*pan);					// add the panners to the mixer
	}
	logMsg("playing mix of 16 sines...");

	theIO->open();						// open & start it
	theIO->start();
	theIO->setRoot(mix);				// make some sound
	sleepSec(60);						// sleep for the desired duration
	theIO->clearRoot();					// make some silence
	theIO->stop();						// stop & close PortAudio
	theIO->close();	
	sleepSec(0.1);						// wait for it to stop
	logMsg("mix done.\n");
}

//[/MiscUserDefs]

//==============================================================================
CSLMIDIComponent::CSLMIDIComponent ()
    : quitButton (0),
      selButton (0),
      clearButton (0),
      audioDeviceSelector (0), changed(false), playing(false),
      slider (0),
      slider2 (0),
      slider3 (0),
      slider4 (0),
      slider5 (0),
      slider6 (0),
      slider7 (0),
      slider8 (0),
      slider9 (0),
      slider10 (0),
      slider11 (0),
      slider12 (0),
      slider13 (0),
      slider14 (0),
      slider15 (0),
      slider16 (0),
      label2 (0)
{
    addAndMakeVisible (quitButton = new TextButton (T("new button")));
    quitButton->setButtonText (T("Quit"));
    quitButton->addButtonListener (this);

    addAndMakeVisible (selButton = new TextButton (T("new button")));
    selButton->setButtonText (T("Select"));
    selButton->addButtonListener (this);

    addAndMakeVisible (clearButton = new TextButton (T("new button")));
    clearButton->setButtonText (T("Clear"));
    clearButton->addButtonListener (this);

//    addAndMakeVisible (audioDeviceSelector = new AudioDeviceSelectorComponent());
//    audioDeviceSelector->setName (T("new component"));

    addAndMakeVisible (slider = new Slider (T("new slider")));
    slider->setRange (0, 127, 0);
    slider->setSliderStyle (Slider::LinearVertical);
    slider->setTextBoxStyle (Slider::NoTextBox, true, 0, 20);
    slider->addListener (this);

    addAndMakeVisible (slider2 = new Slider (T("new slider")));
    slider2->setRange (0, 127, 0);
    slider2->setSliderStyle (Slider::LinearVertical);
    slider2->setTextBoxStyle (Slider::NoTextBox, true, 0, 20);
    slider2->addListener (this);

    addAndMakeVisible (slider3 = new Slider (T("new slider")));
    slider3->setRange (0, 127, 0);
    slider3->setSliderStyle (Slider::LinearVertical);
    slider3->setTextBoxStyle (Slider::NoTextBox, true, 0, 20);
    slider3->addListener (this);

    addAndMakeVisible (slider4 = new Slider (T("new slider")));
    slider4->setRange (0, 127, 0);
    slider4->setSliderStyle (Slider::LinearVertical);
    slider4->setTextBoxStyle (Slider::NoTextBox, true, 0, 20);
    slider4->addListener (this);

    addAndMakeVisible (slider5 = new Slider (T("new slider")));
    slider5->setRange (0, 127, 0);
    slider5->setSliderStyle (Slider::LinearVertical);
    slider5->setTextBoxStyle (Slider::NoTextBox, true, 0, 20);
    slider5->addListener (this);

    addAndMakeVisible (slider6 = new Slider (T("new slider")));
    slider6->setRange (0, 127, 0);
    slider6->setSliderStyle (Slider::LinearVertical);
    slider6->setTextBoxStyle (Slider::NoTextBox, true, 0, 20);
    slider6->addListener (this);

    addAndMakeVisible (slider7 = new Slider (T("new slider")));
    slider7->setRange (0, 127, 0);
    slider7->setSliderStyle (Slider::LinearVertical);
    slider7->setTextBoxStyle (Slider::NoTextBox, true, 0, 20);
    slider7->addListener (this);

    addAndMakeVisible (slider8 = new Slider (T("new slider")));
    slider8->setRange (0, 127, 0);
    slider8->setSliderStyle (Slider::LinearVertical);
    slider8->setTextBoxStyle (Slider::NoTextBox, true, 0, 20);
    slider8->addListener (this);

    addAndMakeVisible (slider9 = new Slider (T("new slider")));
    slider9->setRange (0, 127, 0);
    slider9->setSliderStyle (Slider::LinearVertical);
    slider9->setTextBoxStyle (Slider::NoTextBox, true, 0, 20);
    slider9->addListener (this);

    addAndMakeVisible (slider10 = new Slider (T("new slider")));
    slider10->setRange (0, 127, 0);
    slider10->setSliderStyle (Slider::LinearVertical);
    slider10->setTextBoxStyle (Slider::NoTextBox, true, 0, 20);
    slider10->addListener (this);

    addAndMakeVisible (slider11 = new Slider (T("new slider")));
    slider11->setRange (0, 127, 0);
    slider11->setSliderStyle (Slider::LinearVertical);
    slider11->setTextBoxStyle (Slider::NoTextBox, true, 0, 20);
    slider11->addListener (this);

    addAndMakeVisible (slider12 = new Slider (T("new slider")));
    slider12->setRange (0, 127, 0);
    slider12->setSliderStyle (Slider::LinearVertical);
    slider12->setTextBoxStyle (Slider::NoTextBox, true, 0, 20);
    slider12->addListener (this);

    addAndMakeVisible (slider13 = new Slider (T("new slider")));
    slider13->setRange (0, 127, 0);
    slider13->setSliderStyle (Slider::LinearVertical);
    slider13->setTextBoxStyle (Slider::NoTextBox, true, 0, 20);
    slider13->addListener (this);

    addAndMakeVisible (slider14 = new Slider (T("new slider")));
    slider14->setRange (0, 127, 0);
    slider14->setSliderStyle (Slider::LinearVertical);
    slider14->setTextBoxStyle (Slider::NoTextBox, true, 0, 20);
    slider14->addListener (this);

    addAndMakeVisible (slider15 = new Slider (T("new slider")));
    slider15->setRange (0, 127, 0);
    slider15->setSliderStyle (Slider::LinearVertical);
    slider15->setTextBoxStyle (Slider::NoTextBox, true, 0, 20);
    slider15->addListener (this);

    addAndMakeVisible (slider16 = new Slider (T("new slider")));
    slider16->setRange (0, 127, 0);
    slider16->setSliderStyle (Slider::LinearVertical);
    slider16->setTextBoxStyle (Slider::NoTextBox, true, 0, 20);
    slider16->addListener (this);

    addAndMakeVisible (label2 = new Label (T("new label"),
                                           T("CSL 5.0 MIDI Faders")));
    label2->setFont (Font (24.0000f, Font::plain));
    label2->setJustificationType (Justification::centredLeft);
    label2->setEditable (false, false, false);
    label2->setColour (TextEditor::textColourId, Colours::black);
    label2->setColour (TextEditor::backgroundColourId, Colour (0x0));


    //[UserPreSize]
    //[/UserPreSize]

    setSize (400, 300);

    //[Constructor] You can add your own custom stuff here..
	
	sliders.push_back(slider);		values.push_back(0); 	nextVals.push_back(0);
	sliders.push_back(slider2);		values.push_back(0); 	nextVals.push_back(0);
	sliders.push_back(slider3);		values.push_back(0); 	nextVals.push_back(0);
	sliders.push_back(slider4);		values.push_back(0); 	nextVals.push_back(0);
	sliders.push_back(slider5);		values.push_back(0); 	nextVals.push_back(0);
	sliders.push_back(slider6);		values.push_back(0); 	nextVals.push_back(0);
	sliders.push_back(slider7);		values.push_back(0); 	nextVals.push_back(0);
	sliders.push_back(slider8);		values.push_back(0); 	nextVals.push_back(0);
	sliders.push_back(slider9);		values.push_back(0); 	nextVals.push_back(0);
	sliders.push_back(slider10);	values.push_back(0); 	nextVals.push_back(0);
	sliders.push_back(slider11);	values.push_back(0); 	nextVals.push_back(0);
	sliders.push_back(slider12);	values.push_back(0); 	nextVals.push_back(0);
	sliders.push_back(slider13);	values.push_back(0); 	nextVals.push_back(0);
	sliders.push_back(slider14);	values.push_back(0); 	nextVals.push_back(0);
	sliders.push_back(slider15);	values.push_back(0); 	nextVals.push_back(0);
	sliders.push_back(slider16);	values.push_back(0); 	nextVals.push_back(0);
	
	mIn = new MIDIIn;					// create the midi in
	mIn->open(DEFAULT_MIDI_IN);			// open it
//	lst.mPeriod = 0.25;
//	this->mKey = kNoteOn;				// filter noteOn events
	mIn->attachObserver(this);			// attach this as observer of input
	logMsg("Start MIDI listener");
	mIn->start();						// start midi in
	offset = 48;
	this->startTimer(100);				// 0.1-sec timer

    //[/Constructor]
}

CSLMIDIComponent::~CSLMIDIComponent()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]

    deleteAndZero (quitButton);
    deleteAndZero (selButton);
    deleteAndZero (clearButton);
//    deleteAndZero (audioDeviceSelector);
    deleteAndZero (slider);
    deleteAndZero (slider2);
    deleteAndZero (slider3);
    deleteAndZero (slider4);
    deleteAndZero (slider5);
    deleteAndZero (slider6);
    deleteAndZero (slider7);
    deleteAndZero (slider8);
    deleteAndZero (slider9);
    deleteAndZero (slider10);
    deleteAndZero (slider11);
    deleteAndZero (slider12);
    deleteAndZero (slider13);
    deleteAndZero (slider14);
    deleteAndZero (slider15);
    deleteAndZero (slider16);
    deleteAndZero (label2);

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
    quitButton->setBounds (328, 8, 150, 24);
    selButton->setBounds (328, 40, 150, 24);
    clearButton->setBounds (328, 272, 150, 24);
//    audioDeviceSelector->setBounds (312, 72, 176, 192);
    slider->setBounds (16, 72, 15, 200);
    slider2->setBounds (32, 72, 15, 200);
    slider3->setBounds (48, 72, 15, 200);
    slider4->setBounds (64, 72, 15, 200);
    slider5->setBounds (120, 72, 15, 200);
    slider6->setBounds (88, 72, 15, 200);
    slider7->setBounds (104, 72, 15, 200);
    slider8->setBounds (136, 72, 15, 200);
    slider9->setBounds (192, 72, 15, 200);
    slider10->setBounds (160, 72, 15, 200);
    slider11->setBounds (176, 72, 15, 200);
    slider12->setBounds (208, 72, 15, 200);
    slider13->setBounds (264, 72, 15, 200);
    slider14->setBounds (232, 72, 15, 200);
    slider15->setBounds (248, 72, 15, 200);
    slider16->setBounds (280, 72, 15, 200);
    label2->setBounds (24, 24, 208, 24);
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
        //[/UserButtonCode_quitButton]
    }
    else if (buttonThatWasClicked == selButton)
    {
        //[UserButtonCode_selButton] -- add your button handler code here..
        //[/UserButtonCode_selButton]
    }
    else if (buttonThatWasClicked == clearButton)
    {
        //[UserButtonCode_clearButton] -- add your button handler code here..
        //[/UserButtonCode_clearButton]
    }

    //[UserbuttonClicked_Post]
    //[/UserbuttonClicked_Post]
}

void CSLMIDIComponent::sliderValueChanged (Slider* sliderThatWasMoved)
{
    //[UsersliderValueChanged_Pre]
    //[/UsersliderValueChanged_Pre]

    if (sliderThatWasMoved == slider)
    {
        //[UserSliderCode_slider] -- add your slider handling code here..
        //[/UserSliderCode_slider]
    }
    else if (sliderThatWasMoved == slider2)
    {
        //[UserSliderCode_slider2] -- add your slider handling code here..
        //[/UserSliderCode_slider2]
    }
    else if (sliderThatWasMoved == slider3)
    {
        //[UserSliderCode_slider3] -- add your slider handling code here..
        //[/UserSliderCode_slider3]
    }
    else if (sliderThatWasMoved == slider4)
    {
        //[UserSliderCode_slider4] -- add your slider handling code here..
        //[/UserSliderCode_slider4]
    }
    else if (sliderThatWasMoved == slider5)
    {
        //[UserSliderCode_slider5] -- add your slider handling code here..
        //[/UserSliderCode_slider5]
    }
    else if (sliderThatWasMoved == slider6)
    {
        //[UserSliderCode_slider6] -- add your slider handling code here..
        //[/UserSliderCode_slider6]
    }
    else if (sliderThatWasMoved == slider7)
    {
        //[UserSliderCode_slider7] -- add your slider handling code here..
        //[/UserSliderCode_slider7]
    }
    else if (sliderThatWasMoved == slider8)
    {
        //[UserSliderCode_slider8] -- add your slider handling code here..
        //[/UserSliderCode_slider8]
    }
    else if (sliderThatWasMoved == slider9)
    {
        //[UserSliderCode_slider9] -- add your slider handling code here..
        //[/UserSliderCode_slider9]
    }
    else if (sliderThatWasMoved == slider10)
    {
        //[UserSliderCode_slider10] -- add your slider handling code here..
        //[/UserSliderCode_slider10]
    }
    else if (sliderThatWasMoved == slider11)
    {
        //[UserSliderCode_slider11] -- add your slider handling code here..
        //[/UserSliderCode_slider11]
    }
    else if (sliderThatWasMoved == slider12)
    {
        //[UserSliderCode_slider12] -- add your slider handling code here..
        //[/UserSliderCode_slider12]
    }
    else if (sliderThatWasMoved == slider13)
    {
        //[UserSliderCode_slider13] -- add your slider handling code here..
        //[/UserSliderCode_slider13]
    }
    else if (sliderThatWasMoved == slider14)
    {
        //[UserSliderCode_slider14] -- add your slider handling code here..
        //[/UserSliderCode_slider14]
    }
    else if (sliderThatWasMoved == slider15)
    {
        //[UserSliderCode_slider15] -- add your slider handling code here..
        //[/UserSliderCode_slider15]
    }
    else if (sliderThatWasMoved == slider16)
    {
        //[UserSliderCode_slider16] -- add your slider handling code here..
        //[/UserSliderCode_slider16]
    }

    //[UsersliderValueChanged_Post]
    //[/UsersliderValueChanged_Post]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...


// update() method takes a MIDI message and prints it

void CSLMIDIComponent::update(void * arg) {
	CMIDIMessage * msg = (CMIDIMessage *) arg;
	mIn->clear();
//	logMsg("MIDI msg %d  k %d  v %d", msg->command, msg->getNote(), msg->getVelocity());
	if ( ! msg->isControlChange())
		return;
	nextVals[msg->getNote() - offset] = msg->getVelocity();	
	changed = true;							// set changed flag for timer thread
}

// timer call checks changed and redraws sliders

void CSLMIDIComponent::timerCallback() {
//	CSLAbstComponent::timerCallback();
	if ( ! changed) return;
	for (unsigned i = 0; i < sliders.size(); i++) {
		if (values[i] != nextVals[i]) {
			sliders[i]->setValue(nextVals[i]);
			values[i] = nextVals[i];
		}
	}
	changed = false;			// reset flag
}


/////////////// CSL/JUCE Audio callback method ///////////////////

void CSLMIDIComponent::audioDeviceIOCallback (const float** inputChannelData,
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
	} // if mGraph
}

// Audio device support

void CSLMIDIComponent::audioDeviceAboutToStart (AudioIODevice* device) {
}

void CSLMIDIComponent::audioDeviceStopped() {
}


// start/stop

void CSLMIDIComponent::startStop() {
	if ( ! playing) {										// if not playing, start!		
		playing = true;
		CGestalt::clearStopNow();							// clear flag
															// create a threadfcn that plays CSL
		ThreadFunc fcn = (void * (*)(void *)) playSineMixer;

		playThread = new GThread(fcn);						// thread to call the CSL test function
		playThread->startThread();
//		playButton->setButtonText (T("Stop"));
	} else {												// if playing
		playing = false;
		theIO->clearRoot();	
		this->stopTimer();
		CGestalt::setStopNow();								// set flag to clear timers
		sleepMsec(100);
		if (playThread->isThreadRunning())
			playThread->stopThread(1000);					// try to kill
		delete playThread;
		playThread = 0;
//		playButton->setButtonText (T("Play"));
		CGestalt::clearStopNow();		// clear flag
	}
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
                 overlayOpacity="0.330000013" fixedSize="1" initialWidth="500"
                 initialHeight="300">
  <BACKGROUND backgroundColour="ff829bb1"/>
  <TEXTBUTTON name="new button" id="c0692c09dc89d8ab" memberName="quitButton"
              virtualName="" explicitFocusOrder="0" pos="328 8 150 24" buttonText="Quit"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="new button" id="9e849843391ee895" memberName="selButton"
              virtualName="" explicitFocusOrder="0" pos="328 40 150 24" buttonText="Select"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="new button" id="fec4a7f49a098957" memberName="clearButton"
              virtualName="" explicitFocusOrder="0" pos="328 272 150 24" buttonText="Clear"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <GENERICCOMPONENT name="new component" id="b1bc16d7b6e9ef79" memberName="audioDeviceSelector"
                    virtualName="" explicitFocusOrder="0" pos="312 72 176 192" class="AudioDeviceSelectorComponent"
                    params=""/>
  <SLIDER name="new slider" id="48c2dd1972fbb1" memberName="slider" virtualName=""
          explicitFocusOrder="0" pos="16 72 15 200" min="0" max="127" int="0"
          style="LinearVertical" textBoxPos="NoTextBox" textBoxEditable="0"
          textBoxWidth="0" textBoxHeight="20" skewFactor="1"/>
  <SLIDER name="new slider" id="fcb4de8fba017278" memberName="slider2"
          virtualName="" explicitFocusOrder="0" pos="32 72 15 200" min="0"
          max="127" int="0" style="LinearVertical" textBoxPos="NoTextBox"
          textBoxEditable="0" textBoxWidth="0" textBoxHeight="20" skewFactor="1"/>
  <SLIDER name="new slider" id="3f29e68d9b1bdba1" memberName="slider3"
          virtualName="" explicitFocusOrder="0" pos="48 72 15 200" min="0"
          max="127" int="0" style="LinearVertical" textBoxPos="NoTextBox"
          textBoxEditable="0" textBoxWidth="0" textBoxHeight="20" skewFactor="1"/>
  <SLIDER name="new slider" id="b43f10e2bfb52bf2" memberName="slider4"
          virtualName="" explicitFocusOrder="0" pos="64 72 15 200" min="0"
          max="127" int="0" style="LinearVertical" textBoxPos="NoTextBox"
          textBoxEditable="0" textBoxWidth="0" textBoxHeight="20" skewFactor="1"/>
  <SLIDER name="new slider" id="f6706e5237419417" memberName="slider5"
          virtualName="" explicitFocusOrder="0" pos="120 72 15 200" min="0"
          max="127" int="0" style="LinearVertical" textBoxPos="NoTextBox"
          textBoxEditable="0" textBoxWidth="0" textBoxHeight="20" skewFactor="1"/>
  <SLIDER name="new slider" id="70c97ed7aec875e7" memberName="slider6"
          virtualName="" explicitFocusOrder="0" pos="88 72 15 200" min="0"
          max="127" int="0" style="LinearVertical" textBoxPos="NoTextBox"
          textBoxEditable="0" textBoxWidth="0" textBoxHeight="20" skewFactor="1"/>
  <SLIDER name="new slider" id="4007e7b4aeb1e2e9" memberName="slider7"
          virtualName="" explicitFocusOrder="0" pos="104 72 15 200" min="0"
          max="127" int="0" style="LinearVertical" textBoxPos="NoTextBox"
          textBoxEditable="0" textBoxWidth="0" textBoxHeight="20" skewFactor="1"/>
  <SLIDER name="new slider" id="9eeaf6c9120eff18" memberName="slider8"
          virtualName="" explicitFocusOrder="0" pos="136 72 15 200" min="0"
          max="127" int="0" style="LinearVertical" textBoxPos="NoTextBox"
          textBoxEditable="0" textBoxWidth="0" textBoxHeight="20" skewFactor="1"/>
  <SLIDER name="new slider" id="37a78729e4bf4bea" memberName="slider9"
          virtualName="" explicitFocusOrder="0" pos="192 72 15 200" min="0"
          max="127" int="0" style="LinearVertical" textBoxPos="NoTextBox"
          textBoxEditable="0" textBoxWidth="0" textBoxHeight="20" skewFactor="1"/>
  <SLIDER name="new slider" id="86eadd5a7dcff461" memberName="slider10"
          virtualName="" explicitFocusOrder="0" pos="160 72 15 200" min="0"
          max="127" int="0" style="LinearVertical" textBoxPos="NoTextBox"
          textBoxEditable="0" textBoxWidth="0" textBoxHeight="20" skewFactor="1"/>
  <SLIDER name="new slider" id="a3dd1ae9bb711f8e" memberName="slider11"
          virtualName="" explicitFocusOrder="0" pos="176 72 15 200" min="0"
          max="127" int="0" style="LinearVertical" textBoxPos="NoTextBox"
          textBoxEditable="0" textBoxWidth="0" textBoxHeight="20" skewFactor="1"/>
  <SLIDER name="new slider" id="fa3a7a031e1775d3" memberName="slider12"
          virtualName="" explicitFocusOrder="0" pos="208 72 15 200" min="0"
          max="127" int="0" style="LinearVertical" textBoxPos="NoTextBox"
          textBoxEditable="0" textBoxWidth="0" textBoxHeight="20" skewFactor="1"/>
  <SLIDER name="new slider" id="2ca01a378ce7c7cf" memberName="slider13"
          virtualName="" explicitFocusOrder="0" pos="264 72 15 200" min="0"
          max="127" int="0" style="LinearVertical" textBoxPos="NoTextBox"
          textBoxEditable="0" textBoxWidth="0" textBoxHeight="20" skewFactor="1"/>
  <SLIDER name="new slider" id="5d0b87ba2e763d14" memberName="slider14"
          virtualName="" explicitFocusOrder="0" pos="232 72 15 200" min="0"
          max="127" int="0" style="LinearVertical" textBoxPos="NoTextBox"
          textBoxEditable="0" textBoxWidth="0" textBoxHeight="20" skewFactor="1"/>
  <SLIDER name="new slider" id="32bf29c8f48923e0" memberName="slider15"
          virtualName="" explicitFocusOrder="0" pos="248 72 15 200" min="0"
          max="127" int="0" style="LinearVertical" textBoxPos="NoTextBox"
          textBoxEditable="0" textBoxWidth="0" textBoxHeight="20" skewFactor="1"/>
  <SLIDER name="new slider" id="c8f1ca06c0d507e7" memberName="slider16"
          virtualName="" explicitFocusOrder="0" pos="280 72 15 200" min="0"
          max="127" int="0" style="LinearVertical" textBoxPos="NoTextBox"
          textBoxEditable="0" textBoxWidth="0" textBoxHeight="20" skewFactor="1"/>
  <LABEL name="new label" id="546511be0f798420" memberName="label2" virtualName=""
         explicitFocusOrder="0" pos="24 24 208 24" edTextCol="ff000000"
         edBkgCol="0" labelText="CSL 5.0 MIDI Faders" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="24" bold="0" italic="0" justification="33"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif
