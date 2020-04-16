/*
  ==============================================================================

  This is an automatically generated file created by the Jucer!

  Creation date:  17 Sep 2009 3:29:44 pm

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Jucer version: 1.12

  ------------------------------------------------------------------------------

  The Jucer is part of the JUCE library - "Jules' Utility Class Extensions"
  Copyright 2004-6 by Raw Material Software ltd.

  ==============================================================================
*/

#ifndef __JUCER_HEADER_CSLCOMPONENT_CSLTESTCOMPONENT_AC44DB4F__
#define __JUCER_HEADER_CSLCOMPONENT_CSLTESTCOMPONENT_AC44DB4F__

//[Headers]     -- You can add your own extra header files here --

/// This is the JUCE wrapper for the CSL demos
///
///	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
///
/// It loads a set of test code files for CSL tests and displays a menu of the tests.
///
///		USAGE: CSL_Juce [suite#] [test#]
///
/// The main() function reads argv[1] as the suite selector (from the above list, 1-based)
/// and argv[2] as the test within the suite, e.g., use "2 10" to select the 10th test in the
/// srcTestList suite ("Vector IFFT" -- found at the bottom of Test_Sources.cpp).
/// This is useful for debugging; set up the GUI to run the test you're working on.
///
/// The demo also writes the last used test to ~/.cslrc so that it starts up with the same demo.
///

#include "CSL_Includes.h"	// CSL core
#include "JuceHeader.h"
//#include "src/juce_appframework/audio/devices/juce_AudioDeviceManager.h"
#include "JCSL_Widgets.h"	// my custom widgets (VU meter and oscilloscope)

// Support code: CSL player thread

// a thread class that takes a function

typedef void* (*ThreadFunc)(void*);		// Thread function signature

// Support classes

/// GThread takes a CSL test function and runs it

class GThread : public juce::Thread {
public:
	GThread(ThreadFunc afcn) : Thread("CSL Thread"), fcn(afcn) { };
	void * (*fcn)(void *);

	void run() {
		(*fcn)(NULL);				// call my function
		csl::sleepMsec(100);		// wait a bit for it to stop
	};
};

/// LThread loops a CThread

class CSLComponent;						// forward decl

class LThread : public juce::Thread {
public:
	LThread(GThread * athr, CSLComponent * acomp, bool looper)
			: Thread("CSL Thread"),
			  thr(athr), comp(acomp), loop(looper) { };
	GThread * thr;
	CSLComponent * comp;
	bool loop;

	void run();
};

//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Jucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class CSLComponent  : public juce::Component,
                      public juce::AudioIODeviceCallback,
                      public juce::Timer,
                      public juce::Button::Listener,
                      public juce::ComboBox::Listener,
                      public juce::Slider::Listener
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
	void audioDeviceAboutToStart (juce::AudioIODevice* device);
	void audioDeviceStopped();
	void setComboLabels(unsigned which);
	void timerCallback();
	void recordOnOff();
	void startStop();

    //[/UserMethods]

    void paint (juce::Graphics& g);
    void resized();
    void buttonClicked (juce::Button* buttonThatWasClicked);
    void comboBoxChanged (juce::ComboBox* comboBoxThatHasChanged);
    void sliderValueChanged (juce::Slider* sliderThatWasMoved);


    //==============================================================================
    juce_UseDebuggingNewOperator

//private:
    //[UserVariables]   -- You can add your own custom variables in this section.

	juce::AudioDeviceManager mAudioDeviceManager;		// JUCE audio
	csl::Buffer outBuffer;						// main CSL buffer for output
	GThread * playThread;						// test play and looper threads
	LThread * loopThread;
	int whichTests;								// test selector
	float amplValue;							// amp scale
	bool loop, recrding, playing, displayMode;	// state flags

    //[/UserVariables]

    //==============================================================================
    juce::TextButton* playButton;
    juce::TextButton* quitButton;
    juce::ComboBox* testCombo;
    AudioWaveformDisplay* oscilloscopeL;
    juce::Label* label;
    juce::TextButton* prefsButton;
    juce::Label* cpuLabel;
    AudioWaveformDisplay* oscilloscopeR;
    VUMeter* VUMeterL;
    VUMeter* VUMeterR;
    juce::Slider* scaleSlider;
    juce::Slider* amplitudeSlider;
    juce::ToggleButton* loopButton;
    juce::ComboBox* familyCombo;
    juce::ToggleButton* recordButton;

    //==============================================================================
    // (prevent copy constructor and operator= being generated..)
    CSLComponent (const CSLComponent&);
    const CSLComponent& operator= (const CSLComponent&);
};


#endif   // __JUCER_HEADER_CSLCOMPONENT_CSLTESTCOMPONENT_AC44DB4F__
