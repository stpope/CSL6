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

//[Headers] You can add your own extra header files here...

//[/Headers]

#include "SpectralProc.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...

#include "PAIO.h"			/// PortAudio IO

using namespace std;
using namespace csl;

//[/MiscUserDefs]

//==============================================================================
CSLComponent::CSLComponent ()
    : playButton (0),
      quitButton (0),
      oscilloscope (0),
      label (0),
      spectrogram (0),
      offsetSlider (0),
      loadButton (0),
      analyzeButton (0),
      filterButton (0),
      playSource (0),
      fftLenCombo (0),
      winCombo (0),
      label2 (0),
      label3 (0),
      label4 (0),
      hiPassSlider (0),
      label5 (0),
      loPassSlider (0),
      label6 (0),
      thresholdSlider (0),
      scaleCombo (0),
      displayCombo (0),
      label7 (0),
      label8 (0),
      overlapCombo (0),
      label9 (0),
      label10 (0),
      timeSlider (0),
      label11 (0),
      pitchSlider (0),
      fileInfo (0)
{
    addAndMakeVisible (playButton = new TextButton (T("playNote")));
    playButton->setButtonText (T("Play Resynth"));
    playButton->addButtonListener (this);

    addAndMakeVisible (quitButton = new TextButton (T("quitAction")));
    quitButton->setButtonText (T("Quit"));
    quitButton->addButtonListener (this);

    addAndMakeVisible (oscilloscope = new AudioWaveformDisplay());
    oscilloscope->setName (T("new component"));

    addAndMakeVisible (label = new Label (T("CSL test"),
                                          T("CSL Spectral Processing")));
    label->setFont (Font (Font::getDefaultSerifFontName(), 24.0000f, Font::bold));
    label->setJustificationType (Justification::centredLeft);
    label->setEditable (false, false, false);
    label->setColour (Label::textColourId, Colour (0xff2013da));
    label->setColour (TextEditor::textColourId, Colours::black);
    label->setColour (TextEditor::backgroundColourId, Colour (0x0));

    addAndMakeVisible (spectrogram = new AudioSpectrumDisplay());
    spectrogram->setName (T("new component"));

    addAndMakeVisible (offsetSlider = new Slider (T("new slider")));
    offsetSlider->setRange (0, 1, 0);
    offsetSlider->setSliderStyle (Slider::LinearHorizontal);
    offsetSlider->setTextBoxStyle (Slider::NoTextBox, true, 80, 20);
    offsetSlider->addListener (this);

    addAndMakeVisible (loadButton = new TextButton (T("new button")));
    loadButton->setButtonText (T("File"));
    loadButton->addButtonListener (this);

    addAndMakeVisible (analyzeButton = new TextButton (T("new button")));
    analyzeButton->setButtonText (T("Analyze"));
    analyzeButton->addButtonListener (this);

    addAndMakeVisible (filterButton = new TextButton (T("new button")));
    filterButton->setButtonText (T("Apply Filter"));
    filterButton->addButtonListener (this);

    addAndMakeVisible (playSource = new TextButton (T("playSource")));
    playSource->setButtonText (T("Play Source"));
    playSource->addButtonListener (this);

    addAndMakeVisible (fftLenCombo = new ComboBox (T("new combo box")));
    fftLenCombo->setEditableText (true);
    fftLenCombo->setJustificationType (Justification::centredLeft);
    fftLenCombo->setTextWhenNothingSelected (String::empty);
    fftLenCombo->setTextWhenNoChoicesAvailable (T("(no choices)"));
    fftLenCombo->addItem (T("128"), 1);
    fftLenCombo->addItem (T("256"), 2);
    fftLenCombo->addItem (T("512"), 3);
    fftLenCombo->addItem (T("1024"), 4);
    fftLenCombo->addItem (T("2048"), 5);
    fftLenCombo->addItem (T("4096"), 6);
    fftLenCombo->addItem (T("8192"), 7);
    fftLenCombo->addListener (this);

    addAndMakeVisible (winCombo = new ComboBox (T("new combo box")));
    winCombo->setEditableText (false);
    winCombo->setJustificationType (Justification::centredLeft);
    winCombo->setTextWhenNothingSelected (String::empty);
    winCombo->setTextWhenNoChoicesAvailable (T("(no choices)"));
    winCombo->addItem (T("Rect"), 1);
    winCombo->addItem (T("Tri"), 2);
    winCombo->addItem (T("Hanning"), 3);
    winCombo->addItem (T("Hamming"), 4);
    winCombo->addListener (this);

    addAndMakeVisible (label2 = new Label (T("new label"),
                                           T("Window")));
    label2->setFont (Font (15.0000f, Font::plain));
    label2->setJustificationType (Justification::centredLeft);
    label2->setEditable (false, false, false);
    label2->setColour (TextEditor::textColourId, Colours::black);
    label2->setColour (TextEditor::backgroundColourId, Colour (0x0));

    addAndMakeVisible (label3 = new Label (T("new label"),
                                           T("Hi-Pass")));
    label3->setFont (Font (15.0000f, Font::plain));
    label3->setJustificationType (Justification::centredLeft);
    label3->setEditable (false, false, false);
    label3->setColour (TextEditor::textColourId, Colours::black);
    label3->setColour (TextEditor::backgroundColourId, Colour (0x0));

    addAndMakeVisible (label4 = new Label (T("new label"),
                                           T("FFT Len")));
    label4->setFont (Font (15.0000f, Font::plain));
    label4->setJustificationType (Justification::centredLeft);
    label4->setEditable (false, false, false);
    label4->setColour (TextEditor::textColourId, Colours::black);
    label4->setColour (TextEditor::backgroundColourId, Colour (0x0));

    addAndMakeVisible (hiPassSlider = new Slider (T("new slider")));
    hiPassSlider->setRange (0, 10000, 0);
    hiPassSlider->setSliderStyle (Slider::LinearHorizontal);
    hiPassSlider->setTextBoxStyle (Slider::TextBoxLeft, false, 80, 20);
    hiPassSlider->addListener (this);

    addAndMakeVisible (label5 = new Label (T("new label"),
                                           T("Lo-Pass")));
    label5->setFont (Font (15.0000f, Font::plain));
    label5->setJustificationType (Justification::centredLeft);
    label5->setEditable (false, false, false);
    label5->setColour (TextEditor::textColourId, Colours::black);
    label5->setColour (TextEditor::backgroundColourId, Colour (0x0));

    addAndMakeVisible (loPassSlider = new Slider (T("new slider")));
    loPassSlider->setRange (0, 10000, 0);
    loPassSlider->setSliderStyle (Slider::LinearHorizontal);
    loPassSlider->setTextBoxStyle (Slider::TextBoxLeft, false, 80, 20);
    loPassSlider->addListener (this);

    addAndMakeVisible (label6 = new Label (T("new label"),
                                           T("Threshold")));
    label6->setFont (Font (15.0000f, Font::plain));
    label6->setJustificationType (Justification::centredLeft);
    label6->setEditable (false, false, false);
    label6->setColour (TextEditor::textColourId, Colours::black);
    label6->setColour (TextEditor::backgroundColourId, Colour (0x0));

    addAndMakeVisible (thresholdSlider = new Slider (T("new slider")));
    thresholdSlider->setRange (0, 1, 0);
    thresholdSlider->setSliderStyle (Slider::LinearHorizontal);
    thresholdSlider->setTextBoxStyle (Slider::TextBoxLeft, false, 80, 20);
    thresholdSlider->addListener (this);

    addAndMakeVisible (scaleCombo = new ComboBox (T("new combo box")));
    scaleCombo->setEditableText (true);
    scaleCombo->setJustificationType (Justification::centredLeft);
    scaleCombo->setTextWhenNothingSelected (String::empty);
    scaleCombo->setTextWhenNoChoicesAvailable (T("(no choices)"));
    scaleCombo->addItem (T("Lin"), 1);
    scaleCombo->addItem (T("Log"), 2);
    scaleCombo->addListener (this);

    addAndMakeVisible (displayCombo = new ComboBox (T("new combo box")));
    displayCombo->setEditableText (false);
    displayCombo->setJustificationType (Justification::centredLeft);
    displayCombo->setTextWhenNothingSelected (String::empty);
    displayCombo->setTextWhenNoChoicesAvailable (T("(no choices)"));
    displayCombo->addItem (T("Spectrum"), 1);
    displayCombo->addItem (T("Spectrogram"), 2);
    displayCombo->addListener (this);

    addAndMakeVisible (label7 = new Label (T("new label"),
                                           T("Display")));
    label7->setFont (Font (15.0000f, Font::plain));
    label7->setJustificationType (Justification::centredLeft);
    label7->setEditable (false, false, false);
    label7->setColour (TextEditor::textColourId, Colours::black);
    label7->setColour (TextEditor::backgroundColourId, Colour (0x0));

    addAndMakeVisible (label8 = new Label (T("new label"),
                                           T("Scale")));
    label8->setFont (Font (15.0000f, Font::plain));
    label8->setJustificationType (Justification::centredLeft);
    label8->setEditable (false, false, false);
    label8->setColour (TextEditor::textColourId, Colours::black);
    label8->setColour (TextEditor::backgroundColourId, Colour (0x0));

    addAndMakeVisible (overlapCombo = new ComboBox (T("new combo box")));
    overlapCombo->setEditableText (false);
    overlapCombo->setJustificationType (Justification::centredLeft);
    overlapCombo->setTextWhenNothingSelected (String::empty);
    overlapCombo->setTextWhenNoChoicesAvailable (T("(no choices)"));
    overlapCombo->addItem (T("8"), 1);
    overlapCombo->addItem (T("4"), 2);
    overlapCombo->addItem (T("2"), 3);
    overlapCombo->addItem (T("1"), 4);
    overlapCombo->addItem (T("0.5"), 5);
    overlapCombo->addItem (T("0.25"), 6);
    overlapCombo->addListener (this);

    addAndMakeVisible (label9 = new Label (T("new label"),
                                           T("Overlap")));
    label9->setFont (Font (15.0000f, Font::plain));
    label9->setJustificationType (Justification::centredLeft);
    label9->setEditable (false, false, false);
    label9->setColour (TextEditor::textColourId, Colours::black);
    label9->setColour (TextEditor::backgroundColourId, Colour (0x0));

    addAndMakeVisible (label10 = new Label (T("new label"),
                                            T("Time Scale")));
    label10->setFont (Font (15.0000f, Font::plain));
    label10->setJustificationType (Justification::centredLeft);
    label10->setEditable (false, false, false);
    label10->setColour (TextEditor::textColourId, Colours::black);
    label10->setColour (TextEditor::backgroundColourId, Colour (0x0));

    addAndMakeVisible (timeSlider = new Slider (T("new slider")));
    timeSlider->setRange (0.1, 10, 0);
    timeSlider->setSliderStyle (Slider::LinearHorizontal);
    timeSlider->setTextBoxStyle (Slider::TextBoxLeft, false, 80, 20);
    timeSlider->addListener (this);

    addAndMakeVisible (label11 = new Label (T("new label"),
                                            T("Pitch Scale")));
    label11->setFont (Font (15.0000f, Font::plain));
    label11->setJustificationType (Justification::centredLeft);
    label11->setEditable (false, false, false);
    label11->setColour (TextEditor::textColourId, Colours::black);
    label11->setColour (TextEditor::backgroundColourId, Colour (0x0));

    addAndMakeVisible (pitchSlider = new Slider (T("new slider")));
    pitchSlider->setRange (0.1, 10, 0);
    pitchSlider->setSliderStyle (Slider::LinearHorizontal);
    pitchSlider->setTextBoxStyle (Slider::TextBoxLeft, false, 80, 20);
    pitchSlider->addListener (this);

    addAndMakeVisible (fileInfo = new Label (T("new label"),
                                             T("File info")));
    fileInfo->setFont (Font (15.0000f, Font::plain));
    fileInfo->setJustificationType (Justification::centredLeft);
    fileInfo->setEditable (false, false, false);
    fileInfo->setColour (TextEditor::textColourId, Colours::black);
    fileInfo->setColour (TextEditor::backgroundColourId, Colour (0x0));


    //[UserPreSize]
	
	oscilloscope->initialise(0, 0, 0, false);	// channel, rate, window, zeroX
	spectrogram->initialise(0, 0, 0, false);
 
	//[/UserPreSize]

    setSize (800, 600);

    //[Constructor] You can add your own custom stuff here..

	mSndFile = 0;
	mOutFile = 0;
	mIO = 0;

	mSamples = 0;
	mChannels = 0;
	mSRate = 0;
	totread = 0;
	mFFTLen = 0;
	mNumWins = 0;
	 
	fftLenCombo->setSelectedItemIndex(3);
	winCombo->setSelectedItemIndex(2);
	scaleCombo->setSelectedItemIndex(0);
	displayCombo->setSelectedItemIndex(0);
	overlapCombo->setSelectedItemIndex(2);
	
	offsetSlider->setValue(0.0);
	hiPassSlider->setValue(0.0);
	loPassSlider->setValue(10000.0);
	thresholdSlider->setValue(1.0);
	timeSlider->setValue(1.0);
	pitchSlider->setValue(1.0);
	fileInfo->setText("No File", true);
//	oscilloscope->start();
//	spectrogram->start();

    //[/Constructor]
}

CSLComponent::~CSLComponent()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]

    deleteAndZero (playButton);
    deleteAndZero (quitButton);
    deleteAndZero (oscilloscope);
    deleteAndZero (label);
    deleteAndZero (spectrogram);
    deleteAndZero (offsetSlider);
    deleteAndZero (loadButton);
    deleteAndZero (analyzeButton);
    deleteAndZero (filterButton);
    deleteAndZero (playSource);
    deleteAndZero (fftLenCombo);
    deleteAndZero (winCombo);
    deleteAndZero (label2);
    deleteAndZero (label3);
    deleteAndZero (label4);
    deleteAndZero (hiPassSlider);
    deleteAndZero (label5);
    deleteAndZero (loPassSlider);
    deleteAndZero (label6);
    deleteAndZero (thresholdSlider);
    deleteAndZero (scaleCombo);
    deleteAndZero (displayCombo);
    deleteAndZero (label7);
    deleteAndZero (label8);
    deleteAndZero (overlapCombo);
    deleteAndZero (label9);
    deleteAndZero (label10);
    deleteAndZero (timeSlider);
    deleteAndZero (label11);
    deleteAndZero (pitchSlider);
    deleteAndZero (fileInfo);

    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void CSLComponent::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (Colour (0xffebd89b));

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void CSLComponent::resized()
{
    playButton->setBounds (600, 456, 150, 32);
    quitButton->setBounds (704, 8, 88, 24);
    oscilloscope->setBounds (8, 40, 784, 136);
    label->setBounds (8, 568, 240, 24);
    spectrogram->setBounds (8, 240, 784, 208);
    offsetSlider->setBounds (8, 176, 784, 24);
    loadButton->setBounds (8, 8, 48, 24);
    analyzeButton->setBounds (592, 200, 160, 32);
    filterButton->setBounds (424, 456, 152, 32);
    playSource->setBounds (592, 8, 104, 24);
    fftLenCombo->setBounds (72, 204, 88, 24);
    winCombo->setBounds (232, 204, 104, 24);
    label2->setBounds (168, 204, 63, 24);
    label3->setBounds (384, 504, 64, 24);
    label4->setBounds (8, 204, 63, 24);
    hiPassSlider->setBounds (464, 504, 312, 24);
    label5->setBounds (384, 536, 63, 24);
    loPassSlider->setBounds (464, 536, 312, 24);
    label6->setBounds (384, 568, 72, 24);
    thresholdSlider->setBounds (464, 568, 312, 24);
    scaleCombo->setBounds (80, 456, 88, 24);
    displayCombo->setBounds (240, 456, 104, 24);
    label7->setBounds (176, 456, 63, 24);
    label8->setBounds (16, 456, 63, 24);
    overlapCombo->setBounds (408, 204, 104, 24);
    label9->setBounds (344, 204, 63, 24);
    label10->setBounds (8, 504, 72, 24);
    timeSlider->setBounds (88, 504, 288, 24);
    label11->setBounds (8, 536, 72, 24);
    pitchSlider->setBounds (88, 536, 288, 24);
    fileInfo->setBounds (64, 8, 520, 24);
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
		
												////// PLAY //////
		if (mOutFile == 0)
			return;
		mIO->setRoot(*mOutFile);
		mIO->start();

        //[/UserButtonCode_playButton]
    }
    else if (buttonThatWasClicked == quitButton)
    {
        //[UserButtonCode_quitButton] -- add your button handler code here..
		
         JUCEApplication::quit();				////// QUIT //////

       //[/UserButtonCode_quitButton]
    }
    else if (buttonThatWasClicked == loadButton)
    {
        //[UserButtonCode_loadButton] -- add your button handler code here..

												////// LOAD FILE //////
		FileChooser chooser (T("Choose file to open"),File::nonexistent,"*",true);
		File inFile;
		if ( ! chooser.browseForFileToOpen()) {
			fileInfo->setText("No File",true);
			return;
		}
		inFile = chooser.getResult();
		mSpectBuffer.freeBuffers();
		mOutFile = 0;

		try {
			mSndFile = new SoundFile(inFile.getFullPathName().toUTF8());
			mSndFile->openForRead();
		} catch (CException) {
			logMsg(kLogError, "Cannot read sound file...");
			return;
		}
		mSamples = mSndFile->duration();
		mChannels = mSndFile->channels();
		mSRate = mSndFile->frameRate();
		mFFTLen = this->getFFTLen();
		totread = 0;
		if (mSRate != CGestalt::frameRate())
			CGestalt::setFrameRate(mSRate);

		char msg[CSL_NAME_LEN];
		sprintf(msg, "\"%s\" - %d Hz, %d ch, %d fr, %5.2f sec", 
				mSndFile->path().c_str(), mSRate, mChannels, 
				mSamples, mSndFile->durationInSecs());
		printf("\nLoad file: \"%s\" - %d Hz, %d ch, %d fr, %5.2f sec, %d windows\n\n", 
				mSndFile->path().c_str(), mSRate, mChannels, 
				mSamples, mSndFile->durationInSecs(), mSamples / (mFFTLen  ));
		fileInfo->setText(msg,true);
		mFFTLen = this->getFFTLen();
		if (mFFTLen != CGestalt::blockSize())
			CGestalt::setBlockSize(mFFTLen);
		mIO = new PAIO();				// create the PortAudio IO object
		mIO->open();
		float *inPtr[1];
		inPtr[0] = mSndFile->mSampleBuffer.mBuffers[0];
		oscilloscope->audioDeviceIOCallback (0, 1, inPtr, 1, mSamples);
		oscilloscope->repaint();

        //[/UserButtonCode_loadButton]
    }
    else if (buttonThatWasClicked == analyzeButton)
    {
        //[UserButtonCode_analyzeButton] -- add your button handler code here..
		
												////// ANALYZE //////
		if (mSndFile == 0)
			return;
		mFFTLen = this->getFFTLen();
		mNumWins = mSamples / mFFTLen + 1;
		printf("\nAnalyze: %d windows, FFTLen = %d\n", mNumWins, mFFTLen);
		if (mFFTLen != CGestalt::blockSize())
			CGestalt::setBlockSize(mFFTLen);

									// allocate buffers
		mSpectBuffer.setSize(mNumWins, mFFTLen);
		mSpectBuffer.allocateBuffers();

									// setup FFT
		FFT anFFT(*mSndFile, CSL_FFT_REAL, kFFTMeasure, mFFTLen);
		Buffer tempBuffer(1, mFFTLen);
		float *inPtr[1];

									// analysis loop
		for (unsigned i = 0; i < mNumWins; i++) {
			tempBuffer.mBuffers[0] = mSpectBuffer.mBuffers[i];
			anFFT.nextBuffer(tempBuffer);
			inPtr[0] = tempBuffer.mBuffers[0];
			spectrogram->audioDeviceIOCallback (0, 1, inPtr, i, (int) mFFTLen);
		}

        //[/UserButtonCode_analyzeButton]
    }
    else if (buttonThatWasClicked == filterButton)
    {
        //[UserButtonCode_filterButton] -- add your button handler code here..
		
												////// SYNTHESIZE //////
		if (mSndFile == 0)
			return;
		if ( ! mSpectBuffer.mAreBuffersAllocated)
			return;
		printf("\nSynthesize: %d windows, FFTLen = %d\n", mNumWins, mFFTLen);
		IFFT anIFFT;							// create IFFT
		mOutFile = new SoundFile();
		Buffer * outBuffer = & mOutFile->mSampleBuffer;
		outBuffer->setSize(1, mSamples + mFFTLen);	// set up big output buffer
		outBuffer->allocateBuffers();
		Buffer tempBuffer(1, mFFTLen);
//		tempBuffer.allocateBuffers();
									// FFTW half-complex format = r0, r1, r2, ..., rn/2, i(n+1)/2-1, ..., i2, i1
		for (unsigned i = 0; i < mNumWins; i++) {		// synthesis loop
			float * spect = mSpectBuffer.mBuffers[i];
			tempBuffer.mBuffers[0] = & outBuffer->mBuffers[0][i*mFFTLen];
			for (unsigned j = 0; j < mFFTLen / 2; j++) 		// spectral copy loop
				anIFFT.setBin(j, spect[j + 1], spect[mFFTLen - (j + 1)]);
			anIFFT.nextBuffer(tempBuffer);
		}

        //[/UserButtonCode_filterButton]
    }
    else if (buttonThatWasClicked == playSource)
    {
        //[UserButtonCode_playSource] -- add your button handler code here..

												////// PLAY SRC //////
		if (mSndFile == 0)
			return;
		mIO->clearRoot();
		mSndFile->trigger();
		mIO->setRoot(*mSndFile);
		mIO->start();

        //[/UserButtonCode_playSource]
    }

    //[UserbuttonClicked_Post]

//	startTimer(200); // stars the timer to update the slider each 200 ms

    //[/UserbuttonClicked_Post]
}

void CSLComponent::sliderValueChanged (Slider* sliderThatWasMoved)
{
    //[UsersliderValueChanged_Pre]
    //[/UsersliderValueChanged_Pre]

    if (sliderThatWasMoved == offsetSlider)
    {
        //[UserSliderCode_offsetSlider] -- add your slider handling code here..

						// drag in slider seeks in spectrum
		if (mSndFile == 0)							// not if no file loaded
			return;
		if ( ! mSpectBuffer.mAreBuffersAllocated)	// or no analysis
			return;
		if (displayCombo->getSelectedItemIndex())	// or in spectrogram mode
			return;

		float val = (float) offsetSlider->getValue() * (float) (mNumWins - 1);
		spectrogram->setIndicator((unsigned) val);
//		printf("\nSelect window %d\n", (unsigned) val);
		spectrogram->repaint();

        //[/UserSliderCode_offsetSlider]
    }
    else if (sliderThatWasMoved == hiPassSlider)
    {
        //[UserSliderCode_hiPassSlider] -- add your slider handling code here..
        //[/UserSliderCode_hiPassSlider]
    }
    else if (sliderThatWasMoved == loPassSlider)
    {
        //[UserSliderCode_loPassSlider] -- add your slider handling code here..
        //[/UserSliderCode_loPassSlider]
    }
    else if (sliderThatWasMoved == thresholdSlider)
    {
        //[UserSliderCode_thresholdSlider] -- add your slider handling code here..
        //[/UserSliderCode_thresholdSlider]
    }
    else if (sliderThatWasMoved == timeSlider)
    {
        //[UserSliderCode_timeSlider] -- add your slider handling code here..
        //[/UserSliderCode_timeSlider]
    }
    else if (sliderThatWasMoved == pitchSlider)
    {
        //[UserSliderCode_pitchSlider] -- add your slider handling code here..
        //[/UserSliderCode_pitchSlider]
    }

    //[UsersliderValueChanged_Post]
    //[/UsersliderValueChanged_Post]
}

void CSLComponent::comboBoxChanged (ComboBox* comboBoxThatHasChanged)
{
    //[UsercomboBoxChanged_Pre]
    //[/UsercomboBoxChanged_Pre]

    if (comboBoxThatHasChanged == fftLenCombo)
    {
        //[UserComboBoxCode_fftLenCombo] -- add your combo box handling code here..
        //[/UserComboBoxCode_fftLenCombo]
    }
    else if (comboBoxThatHasChanged == winCombo)
    {
        //[UserComboBoxCode_winCombo] -- add your combo box handling code here..
        //[/UserComboBoxCode_winCombo]
    }
    else if (comboBoxThatHasChanged == scaleCombo)
    {
        //[UserComboBoxCode_scaleCombo] -- add your combo box handling code here..
		
		if (scaleCombo->getSelectedItemIndex() == 0) {
			spectrogram->mLogDisplay = false;
			printf("Select linear display (%d)\n", scaleCombo->getSelectedItemIndex());
		} else {
			spectrogram->mLogDisplay = true;
			printf("Select log display\n");
		}
		spectrogram->repaint();

        //[/UserComboBoxCode_scaleCombo]
    }
    else if (comboBoxThatHasChanged == displayCombo)
    {
        //[UserComboBoxCode_displayCombo] -- add your combo box handling code here..
		
		if (displayCombo->getSelectedItemIndex() == 0) {
			spectrogram->mSpectroDisplay = false;
			printf("Select spectral slice display (%d)\n", scaleCombo->getSelectedItemIndex());
		} else {
			spectrogram->mSpectroDisplay = true;
			printf("Select spectrogram display\n");
		}
		spectrogram->repaint();

        //[/UserComboBoxCode_displayCombo]
    }
    else if (comboBoxThatHasChanged == overlapCombo)
    {
        //[UserComboBoxCode_overlapCombo] -- add your combo box handling code here..
        //[/UserComboBoxCode_overlapCombo]
    }

    //[UsercomboBoxChanged_Post]
    //[/UsercomboBoxChanged_Post]
}


//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...

Component* createCSLComponent() {
    return new CSLComponent();
}

// timer callback updates the slider

void CSLComponent::timerCallback() {
//	offsetSlider->setValue(10 * (float) totread / (float) mSamples);
}

unsigned CSLComponent::getFFTLen() {
	int fftTable[] = { 128, 256, 512, 1024, 2048, 4096, 8192 };
	return(fftTable[fftLenCombo->getSelectedItemIndex()]);
}

float CSLComponent::getOverlap() {
	int oTable[] = { 8.0, 4.0, 2.0, 1.0, 0.5, 0.25 };
	return(oTable[overlapCombo->getSelectedItemIndex()]);
}

//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Jucer information section --

    This is where the Jucer puts all of its metadata, so don't change anything in here!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="CSLComponent" componentName=""
                 parentClasses="public Component, public Timer" constructorParams=""
                 variableInitialisers="" snapPixels="8" snapActive="1" snapShown="1"
                 overlayOpacity="0.330000013" fixedSize="1" initialWidth="800"
                 initialHeight="600">
  <BACKGROUND backgroundColour="ffebd89b"/>
  <TEXTBUTTON name="playNote" id="ed1811e776eea99b" memberName="playButton"
              virtualName="" explicitFocusOrder="0" pos="600 456 150 32" buttonText="Play Resynth"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="quitAction" id="dbaf2871fd41de83" memberName="quitButton"
              virtualName="" explicitFocusOrder="0" pos="704 8 88 24" buttonText="Quit"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <GENERICCOMPONENT name="new component" id="d64c351a292a43a4" memberName="oscilloscope"
                    virtualName="" explicitFocusOrder="0" pos="8 40 784 136" class="AudioWaveformDisplay"
                    params=""/>
  <LABEL name="CSL test" id="a9876f115ab3c22e" memberName="label" virtualName=""
         explicitFocusOrder="0" pos="8 568 240 24" textCol="ff2013da"
         edTextCol="ff000000" edBkgCol="0" labelText="CSL Spectral Processing"
         editableSingleClick="0" editableDoubleClick="0" focusDiscardsChanges="0"
         fontname="Default serif font" fontsize="24" bold="1" italic="0"
         justification="33"/>
  <GENERICCOMPONENT name="new component" id="4aa0f216430fecde" memberName="spectrogram"
                    virtualName="" explicitFocusOrder="0" pos="8 240 784 208" class="AudioWaveformDisplay"
                    params=""/>
  <SLIDER name="new slider" id="c62ea84a7afde2e3" memberName="offsetSlider"
          virtualName="" explicitFocusOrder="0" pos="8 176 784 24" min="0"
          max="1" int="0" style="LinearHorizontal" textBoxPos="NoTextBox"
          textBoxEditable="0" textBoxWidth="80" textBoxHeight="20" skewFactor="1"/>
  <TEXTBUTTON name="new button" id="8a98399a2f7021e3" memberName="loadButton"
              virtualName="" explicitFocusOrder="0" pos="8 8 48 24" buttonText="File"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="new button" id="ff32b69787adbf6c" memberName="analyzeButton"
              virtualName="" explicitFocusOrder="0" pos="592 200 160 32" buttonText="Analyze"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="new button" id="3a78edfc8efb61b0" memberName="filterButton"
              virtualName="" explicitFocusOrder="0" pos="424 456 152 32" buttonText="Apply Filter"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="playSource" id="cee4f69fb5f4995e" memberName="playSource"
              virtualName="" explicitFocusOrder="0" pos="592 8 104 24" buttonText="Play Source"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <COMBOBOX name="new combo box" id="fb6a849a3adce0ac" memberName="fftLenCombo"
            virtualName="" explicitFocusOrder="0" pos="72 204 88 24" editable="1"
            layout="33" items="128&#10;256&#10;512&#10;1024&#10;2048&#10;4096&#10;8192" textWhenNonSelected=""
            textWhenNoItems="(no choices)"/>
  <COMBOBOX name="new combo box" id="459ce4827300838b" memberName="winCombo"
            virtualName="" explicitFocusOrder="0" pos="232 204 104 24" editable="0"
            layout="33" items="Rect&#10;Tri&#10;Hanning&#10;Hamming" textWhenNonSelected=""
            textWhenNoItems="(no choices)"/>
  <LABEL name="new label" id="740396597f99f0ff" memberName="label2" virtualName=""
         explicitFocusOrder="0" pos="168 204 63 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Window" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="15"
         bold="0" italic="0" justification="33"/>
  <LABEL name="new label" id="902a00a7ae61bc1e" memberName="label3" virtualName=""
         explicitFocusOrder="0" pos="384 504 64 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Hi-Pass" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="15"
         bold="0" italic="0" justification="33"/>
  <LABEL name="new label" id="f62e67a204527431" memberName="label4" virtualName=""
         explicitFocusOrder="0" pos="8 204 63 24" edTextCol="ff000000"
         edBkgCol="0" labelText="FFT Len" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="15"
         bold="0" italic="0" justification="33"/>
  <SLIDER name="new slider" id="a51dbd1da4384224" memberName="hiPassSlider"
          virtualName="" explicitFocusOrder="0" pos="464 504 312 24" min="0"
          max="10000" int="0" style="LinearHorizontal" textBoxPos="TextBoxLeft"
          textBoxEditable="1" textBoxWidth="80" textBoxHeight="20" skewFactor="1"/>
  <LABEL name="new label" id="16c1b5d4f842ab95" memberName="label5" virtualName=""
         explicitFocusOrder="0" pos="384 536 63 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Lo-Pass" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="15"
         bold="0" italic="0" justification="33"/>
  <SLIDER name="new slider" id="b94ee2e0cd4407fd" memberName="loPassSlider"
          virtualName="" explicitFocusOrder="0" pos="464 536 312 24" min="0"
          max="10000" int="0" style="LinearHorizontal" textBoxPos="TextBoxLeft"
          textBoxEditable="1" textBoxWidth="80" textBoxHeight="20" skewFactor="1"/>
  <LABEL name="new label" id="6278c834e0564462" memberName="label6" virtualName=""
         explicitFocusOrder="0" pos="384 568 72 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Threshold" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="15"
         bold="0" italic="0" justification="33"/>
  <SLIDER name="new slider" id="241faa25d58e045" memberName="thresholdSlider"
          virtualName="" explicitFocusOrder="0" pos="464 568 312 24" min="0"
          max="1" int="0" style="LinearHorizontal" textBoxPos="TextBoxLeft"
          textBoxEditable="1" textBoxWidth="80" textBoxHeight="20" skewFactor="1"/>
  <COMBOBOX name="new combo box" id="81231ff6eb6dbc5b" memberName="scaleCombo"
            virtualName="" explicitFocusOrder="0" pos="80 456 88 24" editable="1"
            layout="33" items="Lin&#10;Log" textWhenNonSelected="" textWhenNoItems="(no choices)"/>
  <COMBOBOX name="new combo box" id="ff0db135c97cc693" memberName="displayCombo"
            virtualName="" explicitFocusOrder="0" pos="240 456 104 24" editable="0"
            layout="33" items="Spectrum&#10;Spectrogram" textWhenNonSelected=""
            textWhenNoItems="(no choices)"/>
  <LABEL name="new label" id="ff70cdfd0e8ee6ac" memberName="label7" virtualName=""
         explicitFocusOrder="0" pos="176 456 63 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Display" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="15"
         bold="0" italic="0" justification="33"/>
  <LABEL name="new label" id="8802d29b86aa6aea" memberName="label8" virtualName=""
         explicitFocusOrder="0" pos="16 456 63 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Scale" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="15"
         bold="0" italic="0" justification="33"/>
  <COMBOBOX name="new combo box" id="98e9ad89310492cc" memberName="overlapCombo"
            virtualName="" explicitFocusOrder="0" pos="408 204 104 24" editable="0"
            layout="33" items="8&#10;4&#10;2&#10;1&#10;0.5&#10;0.25" textWhenNonSelected="" textWhenNoItems="(no choices)"/>
  <LABEL name="new label" id="235b28d85dd079e9" memberName="label9" virtualName=""
         explicitFocusOrder="0" pos="344 204 63 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Overlap" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="15"
         bold="0" italic="0" justification="33"/>
  <LABEL name="new label" id="45edf99a4f46d71f" memberName="label10" virtualName=""
         explicitFocusOrder="0" pos="8 504 72 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Time Scale" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="15"
         bold="0" italic="0" justification="33"/>
  <SLIDER name="new slider" id="a7d8eb7cdc634436" memberName="timeSlider"
          virtualName="" explicitFocusOrder="0" pos="88 504 288 24" min="0.1"
          max="10" int="0" style="LinearHorizontal" textBoxPos="TextBoxLeft"
          textBoxEditable="1" textBoxWidth="80" textBoxHeight="20" skewFactor="1"/>
  <LABEL name="new label" id="6d9b01ae5485de4d" memberName="label11" virtualName=""
         explicitFocusOrder="0" pos="8 536 72 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Pitch Scale" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="15" bold="0" italic="0" justification="33"/>
  <SLIDER name="new slider" id="3c91ea4babc105f4" memberName="pitchSlider"
          virtualName="" explicitFocusOrder="0" pos="88 536 288 24" min="0.1"
          max="10" int="0" style="LinearHorizontal" textBoxPos="TextBoxLeft"
          textBoxEditable="1" textBoxWidth="80" textBoxHeight="20" skewFactor="1"/>
  <LABEL name="new label" id="1a8f7fc3a78b7e8b" memberName="fileInfo"
         virtualName="" explicitFocusOrder="0" pos="64 8 520 24" edTextCol="ff000000"
         edBkgCol="0" labelText="File info" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="15"
         bold="0" italic="0" justification="33"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif
