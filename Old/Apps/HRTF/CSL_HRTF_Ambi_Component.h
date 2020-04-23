/*
  ==============================================================================

  This is an automatically generated file created by the Jucer!

  Creation date:  28 Feb 2010 8:49:58 pm

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Jucer version: 1.12

  ------------------------------------------------------------------------------

  The Jucer is part of the JUCE library - "Jules' Utility Class Extensions"
  Copyright 2004-6 by Raw Material Software ltd.

  ==============================================================================
*/

#ifndef __JUCER_HEADER_CSLCOMPONENT_CSLHRTFAMBICOMPONENT_3340DB__
#define __JUCER_HEADER_CSLCOMPONENT_CSLHRTFAMBICOMPONENT_3340DB__

//[Headers]     -- You can add your own extra header files here --

#include "juce.h"
#include "CSL_Includes.h"	// include all of CSL core
#include "JCSL_Widgets.h"

#define SLEEP_MSEC 100

// a thread class that takes a function

typedef void* (*ThreadFunc)(void*);		/// Thread function signature
class CSLComponent;

// Support classes

/// HThread takes a CSL test function and runs it

class HThread : public Thread {
public:								/// create with fcn ptr
	HThread(ThreadFunc afcn) : Thread(T("CSL Thread")), fcn(afcn) { };
	void * (*fcn)(void *);			///< my function

	void run() {					///< run the fcn
		(*fcn)(NULL);				///< it might sleep, which loops monitoring the gStop flag
	};
};

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
                      public ComboBoxListener,
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
	void audioDeviceAboutToStart (AudioIODevice* device) { };
	void audioDeviceStopped() { };
	void stopStart();

    //[/UserMethods]

    void paint (Graphics& g);
    void resized();
    void buttonClicked (Button* buttonThatWasClicked);
    void comboBoxChanged (ComboBox* comboBoxThatHasChanged);
    void sliderValueChanged (Slider* sliderThatWasMoved);


    //==============================================================================
    juce_UseDebuggingNewOperator

//private:
    //[UserVariables]   -- You can add your own custom variables in this section.

	AudioDeviceManager mAudioDeviceManager;		// JUCE audio
	csl::Buffer outBuffer;						// main CSL buffer
	HThread * playThread;						// test play thread
	float amplValue;							// amp scale
	bool playing;								// state flag

    //[/UserVariables]

    //==============================================================================
    TextButton* playButton;
    TextButton* quitButton;
    ComboBox* sampleCombo;
    Label* label;
    TextButton* prefsButton;
    Label* cpuLabel;
    Slider* amplitudeSlider;
    ComboBox* headCombo;
    Label* label2;
    Slider* aPosSlider;
    Slider* ePosSlider;
    Slider* dPosSlider;
    Label* label3;
    Label* label4;
    Slider* aSpeedSlider;
    Slider* eSpeedSlider;
    Slider* dSpeedSlider;
    Slider* aRandSlider;
    Slider* eRandSlider;
    Slider* dRandSlider;
    Label* label5;
    Label* label6;
    Label* label7;
    Slider* durSlider;
    Label* label8;
    Label* label9;
    Label* label11;
    Label* label12;
    TextButton* resetButton;
    VUMeter* VUMeterL;
    VUMeter* VUMeterR;
    Label* label13;
    TextEditor* dataFolder;
    ComboBox* methodCombo;
    Label* label14;

    //==============================================================================
    // (prevent copy constructor and operator= being generated..)
    CSLComponent (const CSLComponent&);
    const CSLComponent& operator= (const CSLComponent&);
};


#endif   // __JUCER_HEADER_CSLCOMPONENT_CSLHRTFAMBICOMPONENT_3340DB__
