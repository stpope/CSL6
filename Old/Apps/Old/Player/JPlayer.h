/*
  ==============================================================================

  This is an automatically generated file created by the Jucer!

  Creation date:  28 Nov 2007 12:36:42 pm

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Jucer version: 1.11

  ------------------------------------------------------------------------------

  The Jucer is part of the JUCE library - "Jules' Utility Class Extensions"
  Copyright 2004-6 by Raw Material Software ltd.

  ==============================================================================
*/

#ifndef __JUCER_HEADER_CSLCOMPONENT_JPLAYER_C5C219DA__
#define __JUCER_HEADER_CSLCOMPONENT_JPLAYER_C5C219DA__

//[Headers]     -- You can add your own extra header files here --

#include "juce.h"
#include "CSL_Includes.h"	// include all of CSL core
#include "JCSL_Widgets.h"	// JUCE widgets for audio


class SndfileTableComponent : public Component,
                              public TableListBoxModel
{
public:
    TableListBox* table;    // the table component itself
    int numRows;            // The number of rows of data we've got

SndfileTableComponent () {
        addAndMakeVisible (table = new TableListBox (T("demo table"), this));
        table->setColour (ListBox::outlineColourId, Colours::grey);
        table->setOutlineThickness (1);
		table->getHeader()->addColumn(T("File name"), 1, 100);
		table->getHeader()->addColumn(T("S Rate"), 2, 100);
		table->getHeader()->addColumn(T("n Chans"), 3, 100);
		table->getHeader()->addColumn(T("Dur"), 4, 100);
										   
        table->getHeader()->setSortColumnId (1, true);

}




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
                      public SliderListener,
                      public ButtonListener
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
					// put up a file dialog to select an AIFF file
	void loadFile();

    //[/UserMethods]

    void paint (Graphics& g);
    void resized();
    void sliderValueChanged (Slider* sliderThatWasMoved);
    void buttonClicked (Button* buttonThatWasClicked);


    //==============================================================================
    juce_UseDebuggingNewOperator

private:
    //[UserVariables]   -- You can add your own custom variables in this section.

	AudioDeviceManager mAudioDeviceManager;
	bool isPlaying;							// whether I'm on or off
    DirectoryContentsList* directoryList;
	TableListBoxModel * fileList;

    //[/UserVariables]

    //==============================================================================
    Label* label;
    AudioWaveformDisplay* oscilloscope;
    VUMeter* VUMeterL;
    VUMeter* VUMeterR;
    Slider* levelSlider;
    FileTreeComponent* directoryTree;
    TableListBox* fileList;
    TextButton* scopeButton;

    //==============================================================================
    // (prevent copy constructor and operator= being generated..)
    CSLComponent (const CSLComponent&);
    const CSLComponent& operator= (const CSLComponent&);
};


#endif   // __JUCER_HEADER_CSLCOMPONENT_JPLAYER_C5C219DA__
