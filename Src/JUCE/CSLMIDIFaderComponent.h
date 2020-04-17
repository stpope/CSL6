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

#ifndef __JUCER_HEADER_CSLMIDICOMPONENT_CSLMIDIFADERCOMPONENT_8E28AE2__
#define __JUCER_HEADER_CSLMIDICOMPONENT_CSLMIDIFADERCOMPONENT_8E28AE2__

//[Headers]     -- You can add your own extra header files here --
#include "CSL_AbstComponent.h"
//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Jucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class CSLMIDIComponent  : public Component,
                      public Timer,
                      public ButtonListener,
                      public AudioIODeviceCallback,
                      public SliderListener,
					  public csl::Observer {
public:
    //==============================================================================
    CSLMIDIComponent ();
    ~CSLMIDIComponent();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.

 	void update(void * arg);			///< MIDI observer update method

   //[/UserMethods]

    void paint (Graphics& g);
    void resized();
    void buttonClicked (Button* buttonThatWasClicked);
    void sliderValueChanged (Slider* sliderThatWasMoved);

	virtual void timerCallback();
	void startStop();
	virtual void audioDeviceIOCallback (const float** inputChannelData,
							int totalNumInputChannels,
							float** outputChannelData,
							int totalNumOutputChannels,
							int numSamples);
	virtual void audioDeviceAboutToStart (AudioIODevice* device);
	virtual void audioDeviceStopped();

    //==============================================================================
    juce_UseDebuggingNewOperator

//private:
    //[UserVariables]   -- You can add your own custom variables in this section.
	
	csl::MIDIIn * mIn;							///< MIDI input
	char *cLin;									///< message to add
	bool filt;									///< filter note on-offs
	std::vector <Slider *> sliders;				///< list of my 16 sliders
	std::vector <unsigned> values;				///< list of my 16 current values
	std::vector <unsigned> nextVals;			///< list of my 16 update values
	AudioDeviceManager mAudioDeviceManager;		///< JUCE audio
	csl::Buffer outBuffer;						// main CSL buffer
	bool changed, playing;						///< state flags
	unsigned offset;							///< what # is fader 0?
	GThread * playThread;						// test play and looper threads
	
    //[/UserVariables]

    //==============================================================================
    TextButton* quitButton;
    TextButton* selButton;
    TextButton* clearButton;
    AudioDeviceSelectorComponent* audioDeviceSelector;
    Slider* slider;
    Slider* slider2;
    Slider* slider3;
    Slider* slider4;
    Slider* slider5;
    Slider* slider6;
    Slider* slider7;
    Slider* slider8;
    Slider* slider9;
    Slider* slider10;
    Slider* slider11;
    Slider* slider12;
    Slider* slider13;
    Slider* slider14;
    Slider* slider15;
    Slider* slider16;
    Label* label2;

    //==============================================================================
    // (prevent copy constructor and operator= being generated..)
    CSLMIDIComponent (const CSLMIDIComponent&);
    const CSLMIDIComponent& operator= (const CSLMIDIComponent&);
};


#endif   // __JUCER_HEADER_CSLMIDICOMPONENT_CSLMIDIFADERCOMPONENT_8E28AE2__
