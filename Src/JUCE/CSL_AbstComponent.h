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

#ifndef __JUCER_HEADER_CSLABCOMPONENT_CSLTESTCOMPONENT_AC44DB4F__
#define __JUCER_HEADER_CSLABCOMPONENT_CSLTESTCOMPONENT_AC44DB4F__

//[Headers]     -- You can add your own extra header files here --

/// This is the CSL_AbstComponent JUCE wrapper for the CSL demos
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

class CSLAbstComponent;						// forward decl

class LThread : public juce::Thread {
public:
	LThread(GThread * athr, CSLAbstComponent * acomp, bool looper)
			: Thread("CSL Thread"),
			  thr(athr), comp(acomp), loop(looper) { };
	GThread * thr;
	CSLAbstComponent * comp;
	bool loop;

	void run();
};

//[/Headers]

//==============================================================================
/**
                                                                    //[Comments]
    CSLAbstComponent
                                                                    //[/Comments]
*/
class CSLAbstComponent : public juce::Component,
                      public juce::AudioIODeviceCallback,
                      public juce::Timer {
public:
    CSLAbstComponent ();
    ~CSLAbstComponent();

    //[UserMethods]     -- You can add your own custom methods in this section.

	virtual void audioDeviceIOCallback (const float** inputChannelData,
							int totalNumInputChannels,
							float** outputChannelData,
							int totalNumOutputChannels,
							int numSamples);
	virtual void audioDeviceAboutToStart (juce::AudioIODevice* device);
	virtual void audioDeviceStopped();

	virtual void recordOnOff();
	virtual void startStop(csl::VoidFcnPtrN * fcn);

	virtual void timerCallback();

    //[/UserMethods]

    juce_UseDebuggingNewOperator

//private:
    //[UserVariables]   -- You can add your own custom variables in this section.

	juce::AudioDeviceManager mAudioDeviceManager;		// JUCE audio
	csl::Buffer outBuffer;						// main CSL buffer
	GThread * playThread;						// test play and looper threads
	LThread * loopThread;
	float amplValue;							// amp scale
	bool loop, recrding, playing, displayMode, changed;	// state flags

    //[/UserVariables]

    // (prevent copy constructor and operator= being generated..)
    CSLAbstComponent (const CSLAbstComponent&);
    const CSLAbstComponent& operator= (const CSLAbstComponent&);
};

// Signal component has the widgets of the JUCE demo

class CSLSignalComponent : public CSLAbstComponent,
                        public juce::Button::Listener,
                        public juce::ComboBox::Listener,
                        public juce::Slider::Listener {
public:
    CSLSignalComponent();
    ~CSLSignalComponent();
	
	virtual void resized();
    virtual void buttonClicked (juce::Button* buttonThatWasClicked);
    virtual void comboBoxChanged (juce::ComboBox* comboBoxThatHasChanged);
    virtual void sliderValueChanged (juce::Slider* sliderThatWasMoved);

	virtual void audioDeviceIOCallback (const float** inputChannelData,
							int totalNumInputChannels,
							float** outputChannelData,
							int totalNumOutputChannels,
							int numSamples);

    juce::TextButton* playButton;			///< Buttons, sliders and meters
    juce::TextButton* quitButton;
    juce::TextButton* prefsButton;
    juce::Label* cpuLabel;
    VUMeter* VUMeterL;
    VUMeter* VUMeterR;
    juce::Slider* amplitudeSlider;
    juce::ToggleButton* loopButton;
    juce::ToggleButton* recordButton;
};

#endif   // __JUCER_HEADER_CSLCOMPONENT_CSLTESTCOMPONENT_AC44DB4F__
