/*
  ==============================================================================

  This is an automatically generated file created by the Jucer!

  Creation date:  9 Jan 2009 5:22:37 pm

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Jucer version: 1.11

  ------------------------------------------------------------------------------

  The Jucer is part of the JUCE library - "Jules' Utility Class Extensions"
  Copyright 2004-6 by Raw Material Software ltd.

  ==============================================================================
*/

#ifndef __JUCER_HEADER_CSLCOMPONENT_SPECTRALPROC_D4CD68AC__
#define __JUCER_HEADER_CSLCOMPONENT_SPECTRALPROC_D4CD68AC__

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
                      public Timer,
                      public ButtonListener,
                      public SliderListener,
                      public ComboBoxListener
{
public:
    //==============================================================================
    CSLComponent ();
    ~CSLComponent();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.

	void timerCallback();
	unsigned getFFTLen();
	float getOverlap();

    //[/UserMethods]

    void paint (Graphics& g);
    void resized();
    void buttonClicked (Button* buttonThatWasClicked);
    void sliderValueChanged (Slider* sliderThatWasMoved);
    void comboBoxChanged (ComboBox* comboBoxThatHasChanged);

    //==============================================================================
    juce_UseDebuggingNewOperator

private:
    //[UserVariables]   -- You can add your own custom variables in this section.

	csl::SoundFile * mSndFile;
	csl::Buffer mSpectBuffer;
	csl::SoundFile * mOutFile;
	csl::IO * mIO;

	unsigned mSamples;
	unsigned mChannels;
	unsigned mSRate;
	unsigned totread;			// played samples counter
	unsigned mFFTLen;
	unsigned mNumWins;

    //[/UserVariables]

    //==============================================================================
    TextButton* playButton;
    TextButton* quitButton;
    AudioWaveformDisplay* oscilloscope;
    Label* label;
    AudioSpectrumDisplay* spectrogram;
    Slider* offsetSlider;
    TextButton* loadButton;
    TextButton* analyzeButton;
    TextButton* filterButton;
    TextButton* playSource;
    ComboBox* fftLenCombo;
    ComboBox* winCombo;
    Label* label2;
    Label* label3;
    Label* label4;
    Slider* hiPassSlider;
    Label* label5;
    Slider* loPassSlider;
    Label* label6;
    Slider* thresholdSlider;
    ComboBox* scaleCombo;
    ComboBox* displayCombo;
    Label* label7;
    Label* label8;
    ComboBox* overlapCombo;
    Label* label9;
    Label* label10;
    Slider* timeSlider;
    Label* label11;
    Slider* pitchSlider;
    Label* fileInfo;

    //==============================================================================
    // (prevent copy constructor and operator= being generated..)
    CSLComponent (const CSLComponent&);
    const CSLComponent& operator= (const CSLComponent&);
};


#endif   // __JUCER_HEADER_CSLCOMPONENT_SPECTRALPROC_D4CD68AC__
