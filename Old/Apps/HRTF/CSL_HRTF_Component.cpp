/*
  ==============================================================================

  This is an automatically generated file created by the Jucer!

  Creation date:  2 Sep 2009 8:01:06 pm

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Jucer version: 1.12

  ------------------------------------------------------------------------------

  The Jucer is part of the JUCE library - "Jules' Utility Class Extensions"
  Copyright 2004-6 by Raw Material Software ltd.

  ==============================================================================
*/

//[Headers] You can add your own extra header files here...
//[/Headers]

#include "CSL_HRTF_Component.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...

#include "SpatialAudio.h"
#include "Binaural.h"

using namespace csl;

// a few globals

IO * theIO;
Label* gCPULabel;
AudioDeviceManager * gAudioDeviceManager;
SoundFile * gSndfile = 0;
CSLComponent * gComp = 0;
unsigned argCnt;						// globals for argc/v
char **argVals;

//#define WRITE_TO_FILE					// write 20 sec of snd to a file for testing
#ifdef WRITE_TO_FILE
	Buffer * gFileBuffer;
	#define SAMPS_TO_WRITE 819200
	#define OUT_FILE_NAME "/Users/stp/panner_out.aiff"
#endif

float gAzim, gElev, gDist;				// position is global
bool gDirection = true;					// direction of distance movement
#define MAX_DISTANCE 40					// max distance

// fcn to play a single sound (as a separate fcn so we can use a MessageManagerLock)

void playNextSound(SpatialSource & source, int * napTime) {
	const MessageManagerLock mmLock;	// create the lock so we can call the GUI thread from this thread

										// dump the sliders for debugging
//	printf("\t\t%4.2f  %4.2f  %4.2f -- %4.2f  %4.2f  %4.2f -- %4.2f  %4.2f  %4.2f  \t",
//		gComp->aPosSlider->getValue(), gComp->ePosSlider->getValue(), gComp->dPosSlider->getValue(),
//		gComp->aSpeedSlider->getValue(), gComp->eSpeedSlider->getValue(), gComp->dSpeedSlider->getValue(),
//		gComp->aRandSlider->getValue(), gComp->eRandSlider->getValue(), gComp->dRandSlider->getValue());

										// update the global position vars
	gAzim += gComp->aSpeedSlider->getValue();
	gElev += gComp->eSpeedSlider->getValue();
	if (gDirection)
		gDist += gComp->dSpeedSlider->getValue();
	else
		gDist -= gComp->dSpeedSlider->getValue();
	if (fabs(gDist) > MAX_DISTANCE)
		gDirection = ! gDirection;

	source.setPosition('s',				// update src position with the random values
			fRandB(gAzim, gComp->aRandSlider->getValue()),
			fRandB(gElev, gComp->eRandSlider->getValue()),
			fRandB(gDist, gComp->dRandSlider->getValue()));
	source.dump();						// print the position

	theIO->setRoot(*gSndfile);
	gSndfile->trigger();				// start the file playing

	char msg[10];						// print the CPU usage
	float usg = (float) gAudioDeviceManager->getCpuUsage();
	if (isnormal(usg)) {				// catch is-nan
		sprintf(msg, "%5.2f%%", usg * 100.0f);
		String msgS(msg);
		gCPULabel->setText(msgS, true);
	}
										// calculate how long to sleep for in msec
	*napTime = (int) (gComp->durSlider->getValue() * gSndfile->durationInSecs() * 1000.0);
}

// Thread function to run the HRTF spatializer as a loop

void * hrtf_player(void *) {
	int napTime;			// time to sleep
							// make the sound file "Positionable"
	SpatialSource * source = new SpatialSource(*gSndfile);
							// Create a spatializer.
	Spatializer * panner = new Spatializer(kBinaural);
							// Add the sound source to it
	panner->addSource(*source);
//	BlockResizer * blocker = new BlockResizer(*panner, HRTF_BLOCK_SIZE);
							// pass the CSL graph to the IO to make some sound
//	theIO->setRoot(*panner);
	theIO->setRoot(*gSndfile);
							// loop to play transpositions
	logMsg("Playing HRTF-spatialized rotating samples\n");
	while (gComp->playing) {
							// update the sample position from the GUI
		playNextSound(*source, &napTime);
		sleepMsec(napTime);	// sleep a bit (interruptable)
	}
	gComp->playing = false;
	theIO->clearRoot();		// turn off sound
	logMsg("Player stopped.");
	delete panner;
	delete source;
	return 0;
}

//[/MiscUserDefs]

//==============================================================================
CSLComponent::CSLComponent ()
    : playButton (0),
      quitButton (0),
      sampleCombo (0),
      label (0),
      prefsButton (0),
      cpuLabel (0),
      amplitudeSlider (0),
      headCombo (0),
      label2 (0),
      aPosSlider (0),
      ePosSlider (0),
      dPosSlider (0),
      label3 (0),
      label4 (0),
      aSpeedSlider (0),
      eSpeedSlider (0),
      dSpeedSlider (0),
      aRandSlider (0),
      eRandSlider (0),
      dRandSlider (0),
      label5 (0),
      label6 (0),
      label7 (0),
      rateSlider (0),
      durSlider (0),
      label10 (0),
      label8 (0),
      label9 (0),
      label11 (0),
      label12 (0),
      resetButton (0),
      VUMeterL (0),
      VUMeterR (0),
      label13 (0),
      dataFolder (0)
{
    addAndMakeVisible (playButton = new TextButton (T("playNote")));
    playButton->setButtonText (T("Play"));
    playButton->addButtonListener (this);

    addAndMakeVisible (quitButton = new TextButton (T("quitAction")));
    quitButton->setButtonText (T("Quit"));
    quitButton->addButtonListener (this);

    addAndMakeVisible (sampleCombo = new ComboBox (T("test to run")));
    sampleCombo->setEditableText (false);
    sampleCombo->setJustificationType (Justification::centredLeft);
    sampleCombo->setTextWhenNothingSelected (String::empty);
    sampleCombo->setTextWhenNoChoicesAvailable (T("(no choices)"));
    sampleCombo->addItem (T("Guanno"), 1);
    sampleCombo->addItem (T("Piano"), 2);
    sampleCombo->addItem (T("Snare"), 3);
    sampleCombo->addItem (T("Bongo"), 4);
    sampleCombo->addItem (T("Hi-hat"), 5);
    sampleCombo->addItem (T("Splash"), 6);
    sampleCombo->addItem (T("Triangle"), 7);
    sampleCombo->addItem (T("Whistle"), 8);
    sampleCombo->addListener (this);

    addAndMakeVisible (label = new Label (T("CSL test"),
                                          T("CSL HRTF Player")));
    label->setFont (Font (Font::getDefaultSerifFontName(), 28.0000f, Font::bold));
    label->setJustificationType (Justification::centredLeft);
    label->setEditable (false, false, false);
    label->setColour (Label::textColourId, Colour (0xff2013da));
    label->setColour (TextEditor::textColourId, Colours::black);
    label->setColour (TextEditor::backgroundColourId, Colour (0x0));

    addAndMakeVisible (prefsButton = new TextButton (T("new button")));
    prefsButton->setButtonText (T("Prefs"));
    prefsButton->addButtonListener (this);

    addAndMakeVisible (cpuLabel = new Label (T("new label"),
                                             T("0.0%")));
    cpuLabel->setFont (Font (Font::getDefaultSansSerifFontName(), 15.0000f, Font::bold));
    cpuLabel->setJustificationType (Justification::centredRight);
    cpuLabel->setEditable (false, false, false);
    cpuLabel->setColour (TextEditor::textColourId, Colours::black);
    cpuLabel->setColour (TextEditor::backgroundColourId, Colour (0x0));

    addAndMakeVisible (amplitudeSlider = new Slider (T("new slider")));
    amplitudeSlider->setRange (-5, 7, 0);
    amplitudeSlider->setSliderStyle (Slider::LinearHorizontal);
    amplitudeSlider->setTextBoxStyle (Slider::NoTextBox, true, 80, 20);
    amplitudeSlider->addListener (this);

    addAndMakeVisible (headCombo = new ComboBox (T("new combo box")));
    headCombo->setEditableText (false);
    headCombo->setJustificationType (Justification::centredLeft);
    headCombo->setTextWhenNothingSelected (String::empty);
    headCombo->setTextWhenNoChoicesAvailable (T("(no choices)"));
    headCombo->addItem (T("1"), 1);
    headCombo->addItem (T("2"), 2);
    headCombo->addItem (T("3"), 3);
    headCombo->addItem (T("4"), 4);
    headCombo->addItem (T("5"), 5);
    headCombo->addItem (T("6"), 6);
    headCombo->addItem (T("7"), 7);
    headCombo->addItem (T("8"), 8);
    headCombo->addItem (T("9"), 9);
    headCombo->addItem (T("10"), 10);
    headCombo->addItem (T("11"), 11);
    headCombo->addItem (T("12"), 12);
    headCombo->addItem (T("13"), 13);
    headCombo->addItem (T("14"), 14);
    headCombo->addItem (T("15"), 15);
    headCombo->addListener (this);

    addAndMakeVisible (label2 = new Label (T("new label"),
                                           T("HRTF")));
    label2->setFont (Font (15.0000f, Font::plain));
    label2->setJustificationType (Justification::centredLeft);
    label2->setEditable (false, false, false);
    label2->setColour (TextEditor::textColourId, Colours::black);
    label2->setColour (TextEditor::backgroundColourId, Colour (0x0));

    addAndMakeVisible (aPosSlider = new Slider (T("new slider")));
    aPosSlider->setRange (0, 360, 0);
    aPosSlider->setSliderStyle (Slider::LinearVertical);
    aPosSlider->setTextBoxStyle (Slider::TextBoxLeft, true, 0, 0);
    aPosSlider->addListener (this);

    addAndMakeVisible (ePosSlider = new Slider (T("new slider")));
    ePosSlider->setRange (0, 360, 0);
    ePosSlider->setSliderStyle (Slider::LinearVertical);
    ePosSlider->setTextBoxStyle (Slider::TextBoxLeft, true, 0, 0);
    ePosSlider->addListener (this);

    addAndMakeVisible (dPosSlider = new Slider (T("new slider")));
    dPosSlider->setRange (1, 20, 0);
    dPosSlider->setSliderStyle (Slider::LinearVertical);
    dPosSlider->setTextBoxStyle (Slider::TextBoxLeft, true, 0, 0);
    dPosSlider->addListener (this);

    addAndMakeVisible (label3 = new Label (T("new label"),
                                           T("Offset")));
    label3->setFont (Font (15.0000f, Font::plain));
    label3->setJustificationType (Justification::centredLeft);
    label3->setEditable (false, false, false);
    label3->setColour (TextEditor::textColourId, Colours::black);
    label3->setColour (TextEditor::backgroundColourId, Colour (0x0));

    addAndMakeVisible (label4 = new Label (T("new label"),
                                           T("Velocity")));
    label4->setFont (Font (15.0000f, Font::plain));
    label4->setJustificationType (Justification::centredLeft);
    label4->setEditable (false, false, false);
    label4->setColour (TextEditor::textColourId, Colours::black);
    label4->setColour (TextEditor::backgroundColourId, Colour (0x0));

    addAndMakeVisible (aSpeedSlider = new Slider (T("new slider")));
    aSpeedSlider->setRange (-45, 45, 0);
    aSpeedSlider->setSliderStyle (Slider::LinearVertical);
    aSpeedSlider->setTextBoxStyle (Slider::TextBoxLeft, true, 0, 0);
    aSpeedSlider->addListener (this);

    addAndMakeVisible (eSpeedSlider = new Slider (T("new slider")));
    eSpeedSlider->setRange (-45, 45, 0);
    eSpeedSlider->setSliderStyle (Slider::LinearVertical);
    eSpeedSlider->setTextBoxStyle (Slider::TextBoxLeft, true, 0, 0);
    eSpeedSlider->addListener (this);

    addAndMakeVisible (dSpeedSlider = new Slider (T("new slider")));
    dSpeedSlider->setRange (-2, 2, 0);
    dSpeedSlider->setSliderStyle (Slider::LinearVertical);
    dSpeedSlider->setTextBoxStyle (Slider::TextBoxLeft, true, 0, 0);
    dSpeedSlider->addListener (this);

    addAndMakeVisible (aRandSlider = new Slider (T("new slider")));
    aRandSlider->setRange (0, 360, 0);
    aRandSlider->setSliderStyle (Slider::LinearVertical);
    aRandSlider->setTextBoxStyle (Slider::TextBoxLeft, true, 0, 0);
    aRandSlider->addListener (this);

    addAndMakeVisible (eRandSlider = new Slider (T("new slider")));
    eRandSlider->setRange (0, 360, 0);
    eRandSlider->setSliderStyle (Slider::LinearVertical);
    eRandSlider->setTextBoxStyle (Slider::TextBoxLeft, true, 0, 0);
    eRandSlider->addListener (this);

    addAndMakeVisible (dRandSlider = new Slider (T("new slider")));
    dRandSlider->setRange (0, 20, 0);
    dRandSlider->setSliderStyle (Slider::LinearVertical);
    dRandSlider->setTextBoxStyle (Slider::TextBoxLeft, true, 0, 0);
    dRandSlider->addListener (this);

    addAndMakeVisible (label5 = new Label (T("new label"),
                                           T("Randomness")));
    label5->setFont (Font (15.0000f, Font::plain));
    label5->setJustificationType (Justification::centredLeft);
    label5->setEditable (false, false, false);
    label5->setColour (TextEditor::textColourId, Colours::black);
    label5->setColour (TextEditor::backgroundColourId, Colour (0x0));

    addAndMakeVisible (label6 = new Label (T("new label"),
                                           T("Sample")));
    label6->setFont (Font (15.0000f, Font::plain));
    label6->setJustificationType (Justification::centredLeft);
    label6->setEditable (false, false, false);
    label6->setColour (TextEditor::textColourId, Colours::black);
    label6->setColour (TextEditor::backgroundColourId, Colour (0x0));

    addAndMakeVisible (label7 = new Label (T("new label"),
                                           T("Az     El    Dist")));
    label7->setFont (Font (15.0000f, Font::plain));
    label7->setJustificationType (Justification::centredLeft);
    label7->setEditable (false, false, false);
    label7->setColour (TextEditor::textColourId, Colours::black);
    label7->setColour (TextEditor::backgroundColourId, Colour (0x0));

    addAndMakeVisible (rateSlider = new Slider (T("new slider")));
    rateSlider->setRange (0, 10, 0);
    rateSlider->setSliderStyle (Slider::LinearVertical);
    rateSlider->setTextBoxStyle (Slider::TextBoxLeft, true, 0, 0);
    rateSlider->addListener (this);

    addAndMakeVisible (durSlider = new Slider (T("new slider")));
    durSlider->setRange (0.1, 2, 0);
    durSlider->setSliderStyle (Slider::LinearVertical);
    durSlider->setTextBoxStyle (Slider::TextBoxLeft, true, 0, 0);
    durSlider->addListener (this);

    addAndMakeVisible (label10 = new Label (T("new label"),
                                            T("Rate")));
    label10->setFont (Font (15.0000f, Font::plain));
    label10->setJustificationType (Justification::centredLeft);
    label10->setEditable (false, false, false);
    label10->setColour (TextEditor::textColourId, Colours::black);
    label10->setColour (TextEditor::backgroundColourId, Colour (0x0));

    addAndMakeVisible (label8 = new Label (T("new label"),
                                           T("Az     El    Dist")));
    label8->setFont (Font (15.0000f, Font::plain));
    label8->setJustificationType (Justification::centredLeft);
    label8->setEditable (false, false, false);
    label8->setColour (TextEditor::textColourId, Colours::black);
    label8->setColour (TextEditor::backgroundColourId, Colour (0x0));

    addAndMakeVisible (label9 = new Label (T("new label"),
                                           T("Az     El    Dist")));
    label9->setFont (Font (15.0000f, Font::plain));
    label9->setJustificationType (Justification::centredLeft);
    label9->setEditable (false, false, false);
    label9->setColour (TextEditor::textColourId, Colours::black);
    label9->setColour (TextEditor::backgroundColourId, Colour (0x0));

    addAndMakeVisible (label11 = new Label (T("new label"),
                                            T("Duration")));
    label11->setFont (Font (15.0000f, Font::plain));
    label11->setJustificationType (Justification::centredLeft);
    label11->setEditable (false, false, false);
    label11->setColour (TextEditor::textColourId, Colours::black);
    label11->setColour (TextEditor::backgroundColourId, Colour (0x0));

    addAndMakeVisible (label12 = new Label (T("new label"),
                                            T("Volume")));
    label12->setFont (Font (15.0000f, Font::plain));
    label12->setJustificationType (Justification::centredLeft);
    label12->setEditable (false, false, false);
    label12->setColour (TextEditor::textColourId, Colours::black);
    label12->setColour (TextEditor::backgroundColourId, Colour (0x0));

    addAndMakeVisible (resetButton = new TextButton (T("new button")));
    resetButton->setButtonText (T("Reset"));
    resetButton->addButtonListener (this);

    addAndMakeVisible (VUMeterL = new VUMeter());
    VUMeterL->setName (T("new component"));

    addAndMakeVisible (VUMeterR = new VUMeter());
    VUMeterR->setName (T("new component"));

    addAndMakeVisible (label13 = new Label (T("new label"),
                                            T("Data folder")));
    label13->setFont (Font (15.0000f, Font::plain));
    label13->setJustificationType (Justification::centredLeft);
    label13->setEditable (false, false, false);
    label13->setColour (TextEditor::textColourId, Colours::black);
    label13->setColour (TextEditor::backgroundColourId, Colour (0x0));

    addAndMakeVisible (dataFolder = new TextEditor (T("new text editor")));
    dataFolder->setMultiLine (false);
    dataFolder->setReturnKeyStartsNewLine (false);
    dataFolder->setReadOnly (false);
    dataFolder->setScrollbarsShown (false);
    dataFolder->setCaretVisible (true);
    dataFolder->setPopupMenuEnabled (true);
    dataFolder->setText (String::empty);


    //[UserPreSize]
    //[/UserPreSize]

    setSize (608, 424);

    //[Constructor] You can add your own custom stuff here..
	
	CGestalt::setBlockSize(512);			// set to small blocks?
	playing = false;
											// run this once to create the HRTF DB files
//	HRTFDatabase::convertDB("/Users/stp/Code/CSL/CSL_Data/IRCAM_HRTF/files.txt");

							// initialise the device manager with no settings
							// so that it picks a default device to use.
	const String error (mAudioDeviceManager.initialise (0,	/* no input */
													   2,	/* stereo output  */
													   0,	/* no XML defaults */
													   true /* select default device */));
	if (error.isNotEmpty())
		AlertWindow::showMessageBox (AlertWindow::WarningIcon,
									 T("CSL Demo"),
									 T("Couldn't open an output device!\n\n") + error);
	else
		mAudioDeviceManager.addAudioCallback(this);
									// get the audio device
	AudioIODevice* audioIO = mAudioDeviceManager.getCurrentAudioDevice();
									// reset the HW frame rate & block size to the CSL definition											
	AudioDeviceManager::AudioDeviceSetup setup;		
	mAudioDeviceManager.getAudioDeviceSetup(setup);
	setup.bufferSize = CGestalt::blockSize();
	setup.sampleRate = CGestalt::frameRate();
	mAudioDeviceManager.setAudioDeviceSetup(setup,true);
									// set up CSL IO
	theIO = new csl::IO(CGestalt::frameRate(), CGestalt::blockSize(), -1, -1, 
					CGestalt::numInChannels(), CGestalt::numOutChannels());
	theIO->start();

	gCPULabel = cpuLabel;
	gComp = this;
	gAudioDeviceManager = & mAudioDeviceManager;
	playThread = 0;
									// set up all the GUI values
	String folder(CGestalt::dataFolder().c_str());
	dataFolder->setText(folder);

	headCombo->setSelectedId(1);
	sampleCombo->setSelectedId(1);

	amplitudeSlider->setValue(0.0);
	amplValue = 1.0f;

	aPosSlider->setValue(0.0f);		// in degrees
	dPosSlider->setValue(4.0f);		// abs distance
	aSpeedSlider->setValue(15.0f);	// degrees / sound

	durSlider->setValue(1.0f);		// ratio of snd length
	rateSlider->setValue(1.0f);
	rateSlider->setEnabled(false);	// this isn't implemented yet

	VUMeterL->setChannel(0);
	VUMeterR->setChannel(1);

    //[/Constructor]
}

CSLComponent::~CSLComponent()
{
    //[Destructor_pre]. You can add your own custom destruction code here..

	if (playing)
		this->stopStart();

    //[/Destructor_pre]

    deleteAndZero (playButton);
    deleteAndZero (quitButton);
    deleteAndZero (sampleCombo);
    deleteAndZero (label);
    deleteAndZero (prefsButton);
    deleteAndZero (cpuLabel);
    deleteAndZero (amplitudeSlider);
    deleteAndZero (headCombo);
    deleteAndZero (label2);
    deleteAndZero (aPosSlider);
    deleteAndZero (ePosSlider);
    deleteAndZero (dPosSlider);
    deleteAndZero (label3);
    deleteAndZero (label4);
    deleteAndZero (aSpeedSlider);
    deleteAndZero (eSpeedSlider);
    deleteAndZero (dSpeedSlider);
    deleteAndZero (aRandSlider);
    deleteAndZero (eRandSlider);
    deleteAndZero (dRandSlider);
    deleteAndZero (label5);
    deleteAndZero (label6);
    deleteAndZero (label7);
    deleteAndZero (rateSlider);
    deleteAndZero (durSlider);
    deleteAndZero (label10);
    deleteAndZero (label8);
    deleteAndZero (label9);
    deleteAndZero (label11);
    deleteAndZero (label12);
    deleteAndZero (resetButton);
    deleteAndZero (VUMeterL);
    deleteAndZero (VUMeterR);
    deleteAndZero (label13);
    deleteAndZero (dataFolder);

    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void CSLComponent::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (Colour (0xfffffcb2));

    g.setColour (Colour (0xff8c2aa5));
    g.fillRect (158, 251, 92, 2);

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void CSLComponent::resized()
{
    playButton->setBounds (getWidth() - 8 - 184, 48, 184, 40);
    quitButton->setBounds (getWidth() - 8 - 184, 8, 184, 32);
    sampleCombo->setBounds (240, 60, 104, 24);
    label->setBounds (8, getHeight() - 40, 208, 32);
    prefsButton->setBounds (8, 8, 168, 32);
    cpuLabel->setBounds (getWidth() - 72, getHeight() - 32, 66, 24);
    amplitudeSlider->setBounds (224, getHeight() - 32, 304, proportionOfHeight (0.0684f));
    headCombo->setBounds (74, 60, 54, 24);
    label2->setBounds (32, 60, 55, 24);
    aPosSlider->setBounds (16, 160, 40, 184);
    ePosSlider->setBounds (48, 160, 40, 184);
    dPosSlider->setBounds (80, 160, 40, 184);
    label3->setBounds (24, 136, 56, 24);
    label4->setBounds (160, 136, 64, 24);
    aSpeedSlider->setBounds (152, 160, 40, 184);
    eSpeedSlider->setBounds (184, 160, 40, 184);
    dSpeedSlider->setBounds (216, 160, 40, 184);
    aRandSlider->setBounds (288, 160, 40, 184);
    eRandSlider->setBounds (320, 160, 40, 184);
    dRandSlider->setBounds (360, 160, 40, 184);
    label5->setBounds (296, 136, 88, 24);
    label6->setBounds (184, 60, 56, 24);
    label7->setBounds (24, 344, 96, 16);
    rateSlider->setBounds (424, 160, 40, 184);
    durSlider->setBounds (480, 160, 40, 184);
    label10->setBounds (424, 136, 40, 24);
    label8->setBounds (160, 344, 96, 16);
    label9->setBounds (296, 344, 96, 16);
    label11->setBounds (472, 136, 64, 24);
    label12->setBounds (228, getHeight() - 48, 63, 24);
    resetButton->setBounds (184, 8, 168, 32);
    VUMeterL->setBounds (getWidth() - 48, 104, 14, 248);
    VUMeterR->setBounds (getWidth() - 24, 104, 14, 248);
    label13->setBounds (32, 96, 88, 24);
    dataFolder->setBounds (112, 96, 232, 24);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void CSLComponent::buttonClicked (Button* buttonThatWasClicked)
{
    //[UserbuttonClicked_Pre]
    //[/UserbuttonClicked_Pre]

    if (buttonThatWasClicked == playButton)
    {
        //[UserButtonCode_playButton] -- add your button handler code here..

		this->stopStart();

        //[/UserButtonCode_playButton]
    }
    else if (buttonThatWasClicked == quitButton)
    {
        //[UserButtonCode_quitButton] -- add your button handler code here..
		if (playing)
			this->stopStart();
		JUCEApplication::quit();
        //[/UserButtonCode_quitButton]
    }
    else if (buttonThatWasClicked == prefsButton)
    {
        //[UserButtonCode_prefsButton] -- add your button handler code here..

 		AudioDeviceSelectorComponent audioSettingsComp (mAudioDeviceManager,
														CGestalt::numInChannels(), CGestalt::numInChannels(),
														CGestalt::numOutChannels(), CGestalt::numOutChannels(),
														false, false,
														false, false);
					// ...and show it in a DialogWindow...
		audioSettingsComp.setSize (500, 400);
		DialogWindow::showModalDialog (T("Audio Settings"),
									   &audioSettingsComp,
									   this,
									   Colours::azure,
									   true);

        //[/UserButtonCode_prefsButton]
    }
    else if (buttonThatWasClicked == resetButton)
    {
        //[UserButtonCode_resetButton] -- add your button handler code here..
        //[/UserButtonCode_resetButton]
    }

    //[UserbuttonClicked_Post]
    //[/UserbuttonClicked_Post]
}

void CSLComponent::comboBoxChanged (ComboBox* comboBoxThatHasChanged)
{
    //[UsercomboBoxChanged_Pre]
    //[/UsercomboBoxChanged_Pre]

    if (comboBoxThatHasChanged == sampleCombo)
    {
        //[UserComboBoxCode_sampleCombo] -- add your combo box handling code here..

		bool wasPlaying = playing;
		if (playing)
			this->stopStart();

		String data = dataFolder->getText();				// get folder
		string datastr(data.toUTF8());
		CGestalt::setDataFolder(datastr);

		int which = sampleCombo->getSelectedItemIndex();
		char filename[CSL_NAME_LEN];
		strcpy(filename, CGestalt::dataFolder().c_str());	// CSL data folder location
		switch (which) {					// hard-coded list of sound files -- ToDo: read this from a config file
			case 0:	strcat(filename, "guanno_mono.aiff");	break;
#ifdef UCS_CASND
			case 1:	strcat(filename, "Piano_A5_mf_mono.caf");break;		// core-audio
			case 5:	strcat(filename, "splash_mono.caf");	break;
#else
			case 1:	strcat(filename, "Piano_A5_mf_mono.aiff");break;		// aiff
			case 5:	strcat(filename, "splash_mono.aiff");	break;
#endif
			case 2:	strcat(filename, "rim3_L.aiff");		break;
			case 3:	strcat(filename, "bongo_mono.aiff");	break;
			case 4:	strcat(filename, "hihat_mono.aiff");	break;
			case 6:	strcat(filename, "triangle_mono.aiff");	break;
			case 7:	strcat(filename, "whistle_mono.aiff");	break;
		//	case 8:	strcat(filename, "IRC_1028_R");	break;
			default:
				printf("\nError:Cannot select file # %d\n", which);
				return;
		}
//		printf("\nSelect snd file %s\n", filename);
		if (gSndfile){						// delete the soundfile if it's already open
			gSndfile->close();
			delete gSndfile;
			gSndfile = 0;
		}									// open the sound file
		gSndfile = new SoundFile(filename);
		gSndfile->openForRead();
		gSndfile->dump();
		if (gSndfile->channels() > 1)		// make certain it's mono
			gSndfile->mergeToMono();
		if (gSndfile->isValid())			// enable play button if we found a snd file in the default folder
			playButton->setEnabled(true);
		else
			playButton->setEnabled(false);
		if (wasPlaying)
			this->stopStart();

        //[/UserComboBoxCode_sampleCombo]
    }
    else if (comboBoxThatHasChanged == headCombo)
    {
        //[UserComboBoxCode_headCombo] -- add your combo box handling code here..

		bool wasPlaying = playing;
		if (playing)
			this->stopStart();

		String data = dataFolder->getText();				// get folder
		string datastr(data.toUTF8());
		CGestalt::setDataFolder(datastr);

		int which = headCombo->getSelectedItemIndex();
		char folder[CSL_NAME_LEN];
		strcpy(folder, CGestalt::dataFolder().c_str());		// CSL data folder location
		strcat(folder, "IRCAM_HRTF/");		// HRTF data location
#define USE_HRTF_DB_FILES
#ifdef USE_HRTF_DB_FILES
		strcat(folder, "1024.2_DB/");			// HRTF data location
#endif
		switch (which) {					// hard-coded list of HRTF files -- ToDo: read this from a config file
#ifdef USE_HRTF_DB_FILES
			case 0:	strcat(folder, "HRTF_1047.dat");	break;
			case 1:	strcat(folder, "HRTF_1002.dat");	break;
			case 2:	strcat(folder, "HRTF_1009.dat");	break;
			case 3:	strcat(folder, "HRTF_1012.dat");	break;
			case 4:	strcat(folder, "HRTF_1052.dat");	break;
			case 5:	strcat(folder, "HRTF_1022.dat");	break;
			case 6:	strcat(folder, "HRTF_1026.dat");	break;
			case 7:	strcat(folder, "HRTF_1028.dat");	break;
			case 8:	strcat(folder, "HRTF_1033.dat");	break;
			case 9:	strcat(folder, "HRTF_1039.dat");	break;
			case 10:strcat(folder, "HRTF_1043.dat");	break;
			case 11:strcat(folder, "HRTF_1002.dat");	break;
			case 12:strcat(folder, "HRTF_1052.dat");	break;
			case 13:strcat(folder, "HRTF_1056.dat");	break;
			case 14:strcat(folder, "HRTF_1058.dat");	break;
#else
			case 0:	strcat(folder, "IRC_1047_R/");	break;
			case 1:	strcat(folder, "IRC_1006_R/");	break;
			case 2:	strcat(folder, "IRC_1009_R/");	break;
			case 3:	strcat(folder, "IRC_1012_R/");	break;
			case 4:	strcat(folder, "IRC_1052_R/");	break;
			case 5:	strcat(folder, "IRC_1022_R/");	break;
			case 6:	strcat(folder, "IRC_1026_R/");	break;
			case 7:	strcat(folder, "IRC_1028_R/");	break;
			case 8:	strcat(folder, "IRC_1033_R/");	break;
			case 9:	strcat(folder, "IRC_1039_R/");	break;
			case 10:strcat(folder, "IRC_1043_R/");	break;
			case 11:strcat(folder, "IRC_1002_R/");	break;
			case 12:strcat(folder, "IRC_1052_R/");	break;
			case 13:strcat(folder, "IRC_1056_R/");	break;
			case 14:strcat(folder, "IRC_1058_R/");	break;
#endif
			default:
				printf("\nError:Cannot select HRTF # %d\n", which);
				return;
		}
		logMsg("HRTF %d = %s\n", headCombo->getSelectedId(), folder);
		HRTFDatabase::Reload(folder);			// Load the new data
		HRTFDatabase::Database()->dump();

  		if (wasPlaying)
			this->stopStart();

        //[/UserComboBoxCode_headCombo]
    }

    //[UsercomboBoxChanged_Post]
    //[/UsercomboBoxChanged_Post]
}

void CSLComponent::sliderValueChanged (Slider* sliderThatWasMoved)
{
    //[UsersliderValueChanged_Pre]
    //[/UsersliderValueChanged_Pre]

    if (sliderThatWasMoved == amplitudeSlider)
    {
        //[UserSliderCode_amplitudeSlider] -- add your slider handling code here..

		amplValue = pow(2.0, amplitudeSlider->getValue());

        //[/UserSliderCode_amplitudeSlider]
    }
    else if (sliderThatWasMoved == aPosSlider)
    {
        //[UserSliderCode_aPosSlider] -- add your slider handling code here..

		gAzim = gComp->aPosSlider->getValue();

        //[/UserSliderCode_aPosSlider]
    }
    else if (sliderThatWasMoved == ePosSlider)
    {
        //[UserSliderCode_ePosSlider] -- add your slider handling code here..

		gElev = gComp->ePosSlider->getValue();

        //[/UserSliderCode_ePosSlider]
    }
    else if (sliderThatWasMoved == dPosSlider)
    {
        //[UserSliderCode_dPosSlider] -- add your slider handling code here..

		gDist = gComp->dPosSlider->getValue();

        //[/UserSliderCode_dPosSlider]
    }
    else if (sliderThatWasMoved == aSpeedSlider)
    {
        //[UserSliderCode_aSpeedSlider] -- add your slider handling code here..
        //[/UserSliderCode_aSpeedSlider]
    }
    else if (sliderThatWasMoved == eSpeedSlider)
    {
        //[UserSliderCode_eSpeedSlider] -- add your slider handling code here..
        //[/UserSliderCode_eSpeedSlider]
    }
    else if (sliderThatWasMoved == dSpeedSlider)
    {
        //[UserSliderCode_dSpeedSlider] -- add your slider handling code here..
        //[/UserSliderCode_dSpeedSlider]
    }
    else if (sliderThatWasMoved == aRandSlider)
    {
        //[UserSliderCode_aRandSlider] -- add your slider handling code here..
        //[/UserSliderCode_aRandSlider]
    }
    else if (sliderThatWasMoved == eRandSlider)
    {
        //[UserSliderCode_eRandSlider] -- add your slider handling code here..
        //[/UserSliderCode_eRandSlider]
    }
    else if (sliderThatWasMoved == dRandSlider)
    {
        //[UserSliderCode_dRandSlider] -- add your slider handling code here..
        //[/UserSliderCode_dRandSlider]
    }
    else if (sliderThatWasMoved == rateSlider)
    {
        //[UserSliderCode_rateSlider] -- add your slider handling code here..
        //[/UserSliderCode_rateSlider]
    }
    else if (sliderThatWasMoved == durSlider)
    {
        //[UserSliderCode_durSlider] -- add your slider handling code here..
        //[/UserSliderCode_durSlider]
    }

    //[UsersliderValueChanged_Post]
    //[/UsersliderValueChanged_Post]
}

//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...

// Audio callback plays the CSL graph and copies/scales the samples into the JUCE output buffer

void CSLComponent::audioDeviceIOCallback (const float** inputChannelData,
							int totalNumInputChannels,
							float** outputChannelData,
							int totalNumOutputChannels,
							int numSamples) {
	float *sampPtr;
							// put silence in the output buffers
	for (unsigned i = 0; i < totalNumOutputChannels; i++)
		memset(outputChannelData[i], 0, numSamples * sizeof(float));
	if ( ! theIO)
		return;
							// set up a CSL buffer object
	if (theIO->mGraph) {
		outBuffer.setSize(totalNumOutputChannels, numSamples);
		for (unsigned i = 0; i < totalNumOutputChannels; i++)
			outBuffer.setBuffer(i, outputChannelData[i]);

		try {				// Tell the IO to call its graph
			theIO->pullInput(outBuffer);
		} catch (csl::CException e) {
			printf("Error running CSL: graph\n");
		}
							// copy/scale the buffered samples
		if (amplValue != 1.0f) {
			for (unsigned i = 0; i < totalNumOutputChannels; i++) {
				sampPtr = outputChannelData[i];
				for (unsigned j = 0; j < numSamples; j++)
					*sampPtr++ *= amplValue;
		}}
							// pass the audio callback on to the VU meter displays
		VUMeterL->audioDeviceIOCallback (inputChannelData, 0,
				outputChannelData, totalNumOutputChannels, numSamples);
		VUMeterR->audioDeviceIOCallback (inputChannelData, 0,
				outputChannelData, totalNumOutputChannels, numSamples);
#ifdef WRITE_TO_FILE
		if ( ! gFileBuffer)
			return;
		if (gFileBuffer->mNumFrames >= SAMPS_TO_WRITE)
			return;
		sample * sPtr = gFileBuffer->mBuffers[0] + gFileBuffer->mNumFrames;
		memcpy(sPtr, outputChannelData[0], (numSamples * sizeof(sample)));
		sPtr = gFileBuffer->mBuffers[1] + gFileBuffer->mNumFrames;
		memcpy(sPtr, outputChannelData[1], (numSamples * sizeof(sample)));
		gFileBuffer->mNumFrames += numSamples;
#endif
	}
}

// stop or re-start the player thread

void CSLComponent::stopStart () {
	if ( ! playing) {							// if not playing, start
												// get starting positions from sliders
		gAzim = gComp->aPosSlider->getValue();
		gElev = gComp->ePosSlider->getValue();
		gDist = gComp->dPosSlider->getValue();
		playing = true;							// set play flag
		CGestalt::clearStopNow();				// clear stop flag
		playThread = new HThread(hrtf_player);	// thread to call the hrtf_player
		playThread->startThread();				// start player thread
		playButton->setButtonText (T("Stop"));
#ifdef WRITE_TO_FILE							// set up output cache for file
		gFileBuffer = new Buffer(2, SAMPS_TO_WRITE);
		gFileBuffer->allocateBuffers();
		gFileBuffer->mNumFrames = 0;
#endif

	} else {									// if playing, stop
		theIO->clearRoot();						// turn off sound
		CGestalt::setStopNow();					// set flag
		playing = false;						// clear flag
		playThread->stopThread(1000);			// stop player thread
		delete playThread;
		playThread = 0;
		playButton->setButtonText (T("Play"));
#ifdef WRITE_TO_FILE							// store to file
		LSoundFile * ioFile;
		ioFile = new LSoundFile(OUT_FILE_NAME);
		ioFile->openForWrite(kSoundFileFormatAIFF, 2);
		logMsg("Write to file \"%s\"", ioFile->path().c_str());
		ioFile->writeBuffer(*gFileBuffer);
		ioFile->close();
		gFileBuffer->freeBuffers();
		delete gFileBuffer;
		gFileBuffer = 0;
#endif
	}
}

Component* createCSLComponent() {
    return new CSLComponent();
}

//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Jucer information section --

    This is where the Jucer puts all of its metadata, so don't change anything in here!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="CSLComponent" componentName=""
                 parentClasses="public Component, public AudioIODeviceCallback"
                 constructorParams="" variableInitialisers="" snapPixels="8" snapActive="1"
                 snapShown="1" overlayOpacity="0.330000013" fixedSize="1" initialWidth="608"
                 initialHeight="424">
  <BACKGROUND backgroundColour="fffffcb2">
    <RECT pos="158 251 92 2" fill="solid: ff8c2aa5" hasStroke="0"/>
  </BACKGROUND>
  <TEXTBUTTON name="playNote" id="ed1811e776eea99b" memberName="playButton"
              virtualName="" explicitFocusOrder="0" pos="8Rr 48 184 40" buttonText="Play"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="quitAction" id="dbaf2871fd41de83" memberName="quitButton"
              virtualName="" explicitFocusOrder="0" pos="8Rr 8 184 32" buttonText="Quit"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <COMBOBOX name="test to run" id="bd1a5c541fbc8bc7" memberName="sampleCombo"
            virtualName="" explicitFocusOrder="0" pos="240 60 104 24" editable="0"
            layout="33" items="Guanno&#10;Piano&#10;Snare&#10;Bongo&#10;Hi-hat&#10;Splash&#10;Triangle&#10;Whistle"
            textWhenNonSelected="" textWhenNoItems="(no choices)"/>
  <LABEL name="CSL test" id="a9876f115ab3c22e" memberName="label" virtualName=""
         explicitFocusOrder="0" pos="8 40R 208 32" textCol="ff2013da"
         edTextCol="ff000000" edBkgCol="0" labelText="CSL HRTF Player"
         editableSingleClick="0" editableDoubleClick="0" focusDiscardsChanges="0"
         fontname="Default serif font" fontsize="28" bold="1" italic="0"
         justification="33"/>
  <TEXTBUTTON name="new button" id="beeb15a1537fd4f6" memberName="prefsButton"
              virtualName="" explicitFocusOrder="0" pos="8 8 168 32" buttonText="Prefs"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <LABEL name="new label" id="87d3c5b55ea75f76" memberName="cpuLabel"
         virtualName="" explicitFocusOrder="0" pos="72R 32R 66 24" edTextCol="ff000000"
         edBkgCol="0" labelText="0.0%" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default sans-serif font" fontsize="15"
         bold="1" italic="0" justification="34"/>
  <SLIDER name="new slider" id="78c6bd39da739ba6" memberName="amplitudeSlider"
          virtualName="" explicitFocusOrder="0" pos="224 32R 304 6.84%"
          min="-5" max="7" int="0" style="LinearHorizontal" textBoxPos="NoTextBox"
          textBoxEditable="0" textBoxWidth="80" textBoxHeight="20" skewFactor="1"/>
  <COMBOBOX name="new combo box" id="ba2cb1fbf9e3afb9" memberName="headCombo"
            virtualName="" explicitFocusOrder="0" pos="74 60 54 24" editable="0"
            layout="33" items="1&#10;2&#10;3&#10;4&#10;5&#10;6&#10;7&#10;8&#10;9&#10;10&#10;11&#10;12&#10;13&#10;14&#10;15" textWhenNonSelected=""
            textWhenNoItems="(no choices)"/>
  <LABEL name="new label" id="10f4740a72a36c0d" memberName="label2" virtualName=""
         explicitFocusOrder="0" pos="32 60 55 24" edTextCol="ff000000"
         edBkgCol="0" labelText="HRTF" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="15"
         bold="0" italic="0" justification="33"/>
  <SLIDER name="new slider" id="b1957f9874af95b0" memberName="aPosSlider"
          virtualName="" explicitFocusOrder="0" pos="16 160 40 184" min="0"
          max="360" int="0" style="LinearVertical" textBoxPos="TextBoxLeft"
          textBoxEditable="0" textBoxWidth="0" textBoxHeight="0" skewFactor="1"/>
  <SLIDER name="new slider" id="4cb7b9cc0db9c745" memberName="ePosSlider"
          virtualName="" explicitFocusOrder="0" pos="48 160 40 184" min="0"
          max="360" int="0" style="LinearVertical" textBoxPos="TextBoxLeft"
          textBoxEditable="0" textBoxWidth="0" textBoxHeight="0" skewFactor="1"/>
  <SLIDER name="new slider" id="78a225568525a914" memberName="dPosSlider"
          virtualName="" explicitFocusOrder="0" pos="80 160 40 184" min="1"
          max="20" int="0" style="LinearVertical" textBoxPos="TextBoxLeft"
          textBoxEditable="0" textBoxWidth="0" textBoxHeight="0" skewFactor="1"/>
  <LABEL name="new label" id="b8e14a13de1201e6" memberName="label3" virtualName=""
         explicitFocusOrder="0" pos="24 136 56 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Offset" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="15"
         bold="0" italic="0" justification="33"/>
  <LABEL name="new label" id="d3f7c0a55a6bff86" memberName="label4" virtualName=""
         explicitFocusOrder="0" pos="160 136 64 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Velocity" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="15"
         bold="0" italic="0" justification="33"/>
  <SLIDER name="new slider" id="71b98de5337039e0" memberName="aSpeedSlider"
          virtualName="" explicitFocusOrder="0" pos="152 160 40 184" min="-45"
          max="45" int="0" style="LinearVertical" textBoxPos="TextBoxLeft"
          textBoxEditable="0" textBoxWidth="0" textBoxHeight="0" skewFactor="1"/>
  <SLIDER name="new slider" id="6423f02b256cfc0e" memberName="eSpeedSlider"
          virtualName="" explicitFocusOrder="0" pos="184 160 40 184" min="-45"
          max="45" int="0" style="LinearVertical" textBoxPos="TextBoxLeft"
          textBoxEditable="0" textBoxWidth="0" textBoxHeight="0" skewFactor="1"/>
  <SLIDER name="new slider" id="7aea877f9541afca" memberName="dSpeedSlider"
          virtualName="" explicitFocusOrder="0" pos="216 160 40 184" min="-2"
          max="2" int="0" style="LinearVertical" textBoxPos="TextBoxLeft"
          textBoxEditable="0" textBoxWidth="0" textBoxHeight="0" skewFactor="1"/>
  <SLIDER name="new slider" id="2d77b04b650a5f37" memberName="aRandSlider"
          virtualName="" explicitFocusOrder="0" pos="288 160 40 184" min="0"
          max="360" int="0" style="LinearVertical" textBoxPos="TextBoxLeft"
          textBoxEditable="0" textBoxWidth="0" textBoxHeight="0" skewFactor="1"/>
  <SLIDER name="new slider" id="3f119c63a5c4b5f4" memberName="eRandSlider"
          virtualName="" explicitFocusOrder="0" pos="320 160 40 184" min="0"
          max="360" int="0" style="LinearVertical" textBoxPos="TextBoxLeft"
          textBoxEditable="0" textBoxWidth="0" textBoxHeight="0" skewFactor="1"/>
  <SLIDER name="new slider" id="3a2eee3817c682f0" memberName="dRandSlider"
          virtualName="" explicitFocusOrder="0" pos="360 160 40 184" min="0"
          max="20" int="0" style="LinearVertical" textBoxPos="TextBoxLeft"
          textBoxEditable="0" textBoxWidth="0" textBoxHeight="0" skewFactor="1"/>
  <LABEL name="new label" id="2b88c8c813678b81" memberName="label5" virtualName=""
         explicitFocusOrder="0" pos="296 136 88 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Randomness" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="15"
         bold="0" italic="0" justification="33"/>
  <LABEL name="new label" id="4d5acd405eff4509" memberName="label6" virtualName=""
         explicitFocusOrder="0" pos="184 60 56 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Sample" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="15"
         bold="0" italic="0" justification="33"/>
  <LABEL name="new label" id="a0a558c9baa99034" memberName="label7" virtualName=""
         explicitFocusOrder="0" pos="24 344 96 16" edTextCol="ff000000"
         edBkgCol="0" labelText="Az     El    Dist" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="15" bold="0" italic="0" justification="33"/>
  <SLIDER name="new slider" id="4fe5d72e1a808302" memberName="rateSlider"
          virtualName="" explicitFocusOrder="0" pos="424 160 40 184" min="0"
          max="10" int="0" style="LinearVertical" textBoxPos="TextBoxLeft"
          textBoxEditable="0" textBoxWidth="0" textBoxHeight="0" skewFactor="1"/>
  <SLIDER name="new slider" id="147efa85f3a079d6" memberName="durSlider"
          virtualName="" explicitFocusOrder="0" pos="480 160 40 184" min="0.1"
          max="2" int="0" style="LinearVertical" textBoxPos="TextBoxLeft"
          textBoxEditable="0" textBoxWidth="0" textBoxHeight="0" skewFactor="1"/>
  <LABEL name="new label" id="5e1ef5aada689bc4" memberName="label10" virtualName=""
         explicitFocusOrder="0" pos="424 136 40 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Rate" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="15"
         bold="0" italic="0" justification="33"/>
  <LABEL name="new label" id="49a16dd394b9bf43" memberName="label8" virtualName=""
         explicitFocusOrder="0" pos="160 344 96 16" edTextCol="ff000000"
         edBkgCol="0" labelText="Az     El    Dist" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="15" bold="0" italic="0" justification="33"/>
  <LABEL name="new label" id="fcc992a6068a6b8e" memberName="label9" virtualName=""
         explicitFocusOrder="0" pos="296 344 96 16" edTextCol="ff000000"
         edBkgCol="0" labelText="Az     El    Dist" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="15" bold="0" italic="0" justification="33"/>
  <LABEL name="new label" id="2ccf2b0e26757a42" memberName="label11" virtualName=""
         explicitFocusOrder="0" pos="472 136 64 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Duration" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="15"
         bold="0" italic="0" justification="33"/>
  <LABEL name="new label" id="186ed7b048cc0a86" memberName="label12" virtualName=""
         explicitFocusOrder="0" pos="228 48R 63 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Volume" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="15"
         bold="0" italic="0" justification="33"/>
  <TEXTBUTTON name="new button" id="3f6b02723b73cec6" memberName="resetButton"
              virtualName="" explicitFocusOrder="0" pos="184 8 168 32" buttonText="Reset"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <GENERICCOMPONENT name="new component" id="5b6e419157e89447" memberName="VUMeterL"
                    virtualName="" explicitFocusOrder="0" pos="48R 104 14 248" class="VUMeter"
                    params=""/>
  <GENERICCOMPONENT name="new component" id="4e9ceff1d0772e36" memberName="VUMeterR"
                    virtualName="" explicitFocusOrder="0" pos="24R 104 14 248" class="VUMeter"
                    params=""/>
  <LABEL name="new label" id="b82d5ac775e16551" memberName="label13" virtualName=""
         explicitFocusOrder="0" pos="32 96 88 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Data folder" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="15" bold="0" italic="0" justification="33"/>
  <TEXTEDITOR name="new text editor" id="31987e6d7cd91f72" memberName="dataFolder"
              virtualName="" explicitFocusOrder="0" pos="112 96 232 24" initialText=""
              multiline="0" retKeyStartsLine="0" readonly="0" scrollbars="0"
              caret="1" popupmenu="1"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif
