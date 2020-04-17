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

#ifndef __JUCER_HEADER_CSLCOMPONENT_CSLSIGNALGEN_8E072D41__
#define __JUCER_HEADER_CSLCOMPONENT_CSLSIGNALGEN_8E072D41__

//[Headers]     -- You can add your own extra header files here --

#include "juce.h"

#include "CSL_Includes.h"	// include all of CSL core

#include "JCSL_Widgets.h"

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
                      public SliderListener,
                      public LabelListener
{
public:
    //==============================================================================
    CSLComponent ();
    ~CSLComponent();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.

	float frequency();
	float amplitude();
	void setFreqLabel();
	void setSource(csl::UnitGenerator * src);

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
    void sliderValueChanged (Slider* sliderThatWasMoved);
    void labelTextChanged (Label* labelThatHasChanged);


    //==============================================================================
    juce_UseDebuggingNewOperator

private:
    //[UserVariables]   -- You can add your own custom variables in this section.

	AudioDeviceManager mAudioDeviceManager;
	csl::Buffer outBuffer;
	csl::Thread * playThread;
	csl::UnitGenerator * source;
	csl::SumOfSines * sos;
	bool isPlaying;
	bool useFreq;

    //[/UserVariables]

    //==============================================================================
    TextButton* startButton;
    TextButton* quitButton;
    Label* label;
    TextButton* prefsButton;
    AudioWaveformDisplay* oscilloscope;
    Slider* fineFreq;
    Slider* coarseFreq;
    GroupComponent* groupComponent;
    Label* label2;
    Label* freqLabel;
    ToggleButton* sineButton;
    ToggleButton* squareButton;
    ToggleButton* triangleButton;
    ToggleButton* sawButton;
    ToggleButton* whiteButton;
    ToggleButton* pinkButton;
    Slider* levelSlider;
    Label* levelLabel;
    Slider* hScaleSlider;

    //==============================================================================
    // (prevent copy constructor and operator= being generated..)
    CSLComponent (const CSLComponent&);
    const CSLComponent& operator= (const CSLComponent&);
};


#endif   // __JUCER_HEADER_CSLCOMPONENT_CSLSIGNALGEN_8E072D41__
