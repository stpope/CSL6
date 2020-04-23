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

#ifndef __JUCER_HEADER_CSLMIDICOMPONENT_CSLMIDICOMPONENT_22509CB1__
#define __JUCER_HEADER_CSLMIDICOMPONENT_CSLMIDICOMPONENT_22509CB1__

//[Headers]     -- You can add your own extra header files here --
#include "CSL_Includes.h"	// CSL core
#include "juce.h"			// JUCE core
#include "JCSL_Widgets.h"	// my custom widgets (VU meter and oscilloscope)
//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Jucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class CSLMIDIComponent : public Component,
//                      public AudioIODeviceCallback,
                      public Timer,
                      public ButtonListener,
//                      public ComboBoxListener,
//                      public SliderListener,
					  public csl::Observer
{
public:
    //==============================================================================
    CSLMIDIComponent ();
    ~CSLMIDIComponent();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.

	void update(void * arg);			///< MIDI observer update method
	void timerCallback();

    //[/UserMethods]

    void paint (Graphics& g);
    void resized();
    void buttonClicked (Button* buttonThatWasClicked);


    //==============================================================================
    juce_UseDebuggingNewOperator

//private:
    //[UserVariables]   -- You can add your own custom variables in this section.

	csl::MIDIIn * mIn;			///< MIDI input
	char *cLin;					///< message to add
	bool filt;					///< filter note on-offs
	
	AudioDeviceManager mAudioDeviceManager;		// JUCE audio
//	csl::Buffer outBuffer;						// main CSL buffer
//	GThread * playThread;						// test play and looper threads
//	LThread * loopThread;
//	float amplValue;							// amp scale
//	bool loop, recrding, playing, displayMode, changed;	// state flags
	bool changed;	// state flags

    //[/UserVariables]

    //==============================================================================
    TextButton* quitButton;
    Label* label;
    TextEditor* textEditor;
    TextButton* clearButton;
    AudioDeviceSelectorComponent* audioDeviceSelector;
    ToggleButton* filtButton;

    //==============================================================================
    // (prevent copy constructor and operator= being generated..)
    CSLMIDIComponent (const CSLMIDIComponent&);
    const CSLMIDIComponent& operator= (const CSLMIDIComponent&);
};


#endif   // __JUCER_HEADER_CSLMIDICOMPONENT_CSLMIDICOMPONENT_22509CB1__
