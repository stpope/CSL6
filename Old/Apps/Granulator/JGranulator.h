/*
  ==============================================================================

  This is an automatically generated file created by the Jucer!

  Creation date:  23 Nov 2007 2:34:19 pm

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Jucer version: 1.11

  ------------------------------------------------------------------------------

  The Jucer is part of the JUCE library - "Jules' Utility Class Extensions"
  Copyright 2004-6 by Raw Material Software ltd.

  ==============================================================================
*/

#ifndef __JUCER_HEADER_CSLCOMPONENT_JGRANULATOR_3103A2F5__
#define __JUCER_HEADER_CSLCOMPONENT_JGRANULATOR_3103A2F5__

//[Headers]     -- You can add your own extra header files here --

#include "juce.h"
#include "CSL_Includes.h"	// include all of CSL core
#include "Granulator.h"		// Granulator core
#include "MIDI_Control.h"	// MIDI input
#include "JCSL_Widgets.h"	// JUCE widgets for audio

//#define USE_REVERB			// use a Stereoverb - fix me

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
                      public SliderListener
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
	void audioDeviceAboutToStart (AudioIODevice* device);
	void audioDeviceStopped();
					// put up a file dialog to select an AIFF file
	void loadFile();

    //[/UserMethods]

    void paint (Graphics& g);
    void resized();
    void buttonClicked (Button* buttonThatWasClicked);
    void sliderValueChanged (Slider* sliderThatWasMoved);


    //==============================================================================
    juce_UseDebuggingNewOperator

//private:
    //[UserVariables]   -- You can add your own custom variables in this section.

	AudioDeviceManager mAudioDeviceManager;
	csl::Buffer outBuffer;						// CSL buffer I play into
	csl::UnitGenerator * source;					// my output source (may be a reverb)
	csl::GrainPlayer * player;				// my grain player
	csl::GrainCloud * cloud;				// my grain cloud
	bool isPlaying;							// whether I'm on or off
	MIDIControllerMapper * theMapper;

    //[/UserVariables]

    //==============================================================================
    TextButton* startButton;
    TextButton* quitButton;
    Label* label;
    TextButton* fileButton;
    AudioWaveformDisplay* oscilloscope;
    RangeSlider* rateSlider;
    RangeSlider* offsetSlider;
    RangeSlider* densitySlider;
    RangeSlider* durationSlider;
    RangeSlider* widthSlider;
    Label* label2;
    Label* label4;
    Label* label7;
    Label* label8;
    Label* label9;
    RangeSlider* volumeSlider;
    Label* label3;
    VUMeter* VUMeterL;
    VUMeter* VUMeterR;
    RangeSlider* envelopeSlider;
    Slider* reverbSlider;
    Label* label5;
    Label* label6;

    //==============================================================================
    // (prevent copy constructor and operator= being generated..)
    CSLComponent (const CSLComponent&);
    const CSLComponent& operator= (const CSLComponent&);
};


#endif   // __JUCER_HEADER_CSLCOMPONENT_JGRANULATOR_3103A2F5__
