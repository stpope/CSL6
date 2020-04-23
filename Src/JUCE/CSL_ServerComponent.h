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

#ifndef __JUCER_HEADER_CSLSERVERCOMPONENT_CSLSERVERCOMPONENT_EB9904E6__
#define __JUCER_HEADER_CSLSERVERCOMPONENT_CSLSERVERCOMPONENT_EB9904E6__

//[Headers]     -- You can add your own extra header files here --

#include "CSL_Includes.h"	// CSL core
#include <AdditiveInstrument.h>
#include <BasicFMInstrument.h>
#include "SndFileInstrument.h"
#include "OSC_support.h"

#include "juce.h"			// JUCE core
#include "src/juce_appframework/audio/devices/juce_AudioDeviceManager.h"

//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Jucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class CSLServerComponent  : public Component,
                            public Timer,
                            public AudioIODeviceCallback,
                            public ButtonListener,
                            public SliderListener
{
public:
    //==============================================================================
    CSLServerComponent ();
    ~CSLServerComponent();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.

 	void audioDeviceIOCallback (const float** inputChannelData,
							int totalNumInputChannels,
							float** outputChannelData,
							int totalNumOutputChannels,
							int numSamples);
	void audioDeviceAboutToStart (AudioIODevice* device);
	void audioDeviceStopped();
	void timerCallback();
	void recordOnOff();

    //[/UserMethods]

    void paint (Graphics& g);
    void resized();
    void buttonClicked (Button* buttonThatWasClicked);
    void sliderValueChanged (Slider* sliderThatWasMoved);

    // Binary resources:
    static const char* create_logo_sm_gif;
    static const int create_logo_sm_gifSize;

    //==============================================================================
    juce_UseDebuggingNewOperator

private:
    //[UserVariables]   -- You can add your own custom variables in this section.

	AudioDeviceManager mAudioDeviceManager;		// JUCE audio
	csl::Buffer outBuffer;						// main CSL buffer
	bool recrding, playing;						// state flags
	float amplValue;							// amp scale

    //[/UserVariables]

    //==============================================================================
    TextButton* playButton;
    TextButton* stopButton;
    Label* label;
    TextButton* prefsButton;
    ToggleButton* RecordButton;
    TextButton* quitButton;
    Label* cpuLabel;
    Slider* amplSlider;
    Image* internalCachedImage1;

    //==============================================================================
    // (prevent copy constructor and operator= being generated..)
    CSLServerComponent (const CSLServerComponent&);
    const CSLServerComponent& operator= (const CSLServerComponent&);
};


#endif   // __JUCER_HEADER_CSLSERVERCOMPONENT_CSLSERVERCOMPONENT_EB9904E6__
