/*
  ==============================================================================

  This is an automatically generated GUI class created by the Projucer!

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Created with Projucer version: 6.0.0

  ------------------------------------------------------------------------------

  The Projucer is part of the JUCE library.
  Copyright (c) 2017 - ROLI Ltd.

  ==============================================================================
*/

#pragma once

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

using namespace juce;

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
                      public Timer,
                      public Button::Listener,
                      public ComboBox::Listener,
                      public Slider::Listener
{
public:
    //==============================================================================
    CSLComponent ();
    ~CSLComponent() override;

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

    bool playing, recrding, loop, displayMode;

    //[/UserMethods]

    void paint (Graphics& g) override;
    void resized() override;
    void buttonClicked (Button* buttonThatWasClicked) override;
    void comboBoxChanged (ComboBox* comboBoxThatHasChanged) override;
    void sliderValueChanged (Slider* sliderThatWasMoved) override;



private:
    //[UserVariables]   -- You can add your own custom variables in this section.

	juce::AudioDeviceManager mAudioDeviceManager;   // JUCE audio
	csl::Buffer outBuffer;						    // main CSL buffer for output
	GThread * playThread;						    // test play and looper threads
	LThread * loopThread;
	int whichTests;								    // test selector
	float amplValue;							    // amp scale

    //[/UserVariables]

    //==============================================================================
    std::unique_ptr<TextButton> playButton;
    std::unique_ptr<TextButton> quitButton;
    std::unique_ptr<ComboBox> testCombo;
    std::unique_ptr<AudioWaveformDisplay> oscilloscopeL;
    std::unique_ptr<Label> label;
    std::unique_ptr<TextButton> prefsButton;
    std::unique_ptr<Label> cpuLabel;
    std::unique_ptr<AudioWaveformDisplay> oscilloscopeR;
    std::unique_ptr<VUMeter> VUMeterL;
    std::unique_ptr<VUMeter> VUMeterR;
    std::unique_ptr<Slider> scaleSlider;
    std::unique_ptr<Slider> amplitudeSlider;
    std::unique_ptr<ToggleButton> loopButton;
    std::unique_ptr<ComboBox> familyCombo;
    std::unique_ptr<ToggleButton> recordButton;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CSLComponent)
};

//[EndFile] You can add extra defines here...
//[/EndFile]

