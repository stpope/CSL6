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

#ifndef __JUCER_HEADER_CSLCOMPONENT_CSLCOMPONENT_53417E52__
#define __JUCER_HEADER_CSLCOMPONENT_CSLCOMPONENT_53417E52__

//[Headers]     -- You can add your own extra header files here --
#include "juce.h"

#include "CSL_Includes.h"	// include all of CSL core

class AudioOutputWaveformDisplay;

//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Jucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class CSLComponent  : public Component,
                      public AudioIODeviceCallback,
                      public ButtonListener,
                      public ComboBoxListener
{
public:
    //==============================================================================
    CSLComponent ();
    ~CSLComponent();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
	void audioDeviceIOCallback (const float** inputChannelData,
							int totalNumInputChannels,
							float** outputChannelData,
							int totalNumOutputChannels,
							int numSamples);
	void audioDeviceAboutToStart (double sampleRate, int numSamplesPerBlock);
	void audioDeviceStopped();
    //[/UserMethods]

    void paint (Graphics& g);
    void resized();
    void buttonClicked (Button* buttonThatWasClicked);
    void comboBoxChanged (ComboBox* comboBoxThatHasChanged);


    //==============================================================================
    juce_UseDebuggingNewOperator

private:
    //[UserVariables]   -- You can add your own custom variables in this section.
	AudioDeviceManager mAudioDeviceManager;
	csl::Buffer outBuffer;
	csl::Thread * playThread;
    //[/UserVariables]

    //==============================================================================
    TextButton* playButton;
    TextButton* quitButton;
    ComboBox* testCombo;
    AudioOutputWaveformDisplay* oscilloscope;
	AudioOutputWaveformDisplay* oscilloscope2;
	Label* label;
    TextButton* prefsButton;
    Label* cpuLabel;
    TextButton* decrButton;
    TextButton* incrButton;

    //==============================================================================
    // (prevent copy constructor and operator= being generated..)
    CSLComponent (const CSLComponent&);
    const CSLComponent& operator= (const CSLComponent&);
};


#endif   // __JUCER_HEADER_CSLCOMPONENT_CSLCOMPONENT_53417E52__
