
# CSL 6.0 README

![alt text](https://github.com/stpope/CSL6/blob/master/Doc/CSL6_Logo_tiny.jpg "CSL Logo")

This directory contains the source code and documentation for the CREATE Signal 
Library (CSL, pronounced "sizzle," previously called the CREATE Oscillator or CO). 
CSL is a cross-platform C++ library for digital audio signal synthesis, analysis, 
spatialization and interactive sound/music application development.
CSL was developed at the Center for Research in Electronic Art Technology 
(CREATE) and the Graduate Program in Media Arts and Technology (MAT) at the 
University of California, Santa Barbara (UCSB) starting in the late 1990s.

This is release 6.0, April, 2020.

The home page for CSL is http://FASTLabInc.com/CSL. See the screen shots,
documentation downloads and links to screencast demos there.

CSL is known to work on Mac OSX, Linux, iOS, Android, RaspberryPi and
MS-Windows (work-in-progress; see below). 
To get started with CSL, build the demo app (screen shot below)
and use the combo boxes at the bottom-left to select among the tests, whose source code
is in the Src/Tests folder. For the Mac, use the XCode project in the 
Builds/MacOSX folder; for Linux, use the makefile in the Builds/LinuxMakefile folder.
There are pre-compiled versions of the demo for MacOS and Linux in the ZIP file https://github.com/stpope/CSL6/blob/master/Demo.zip

### Links for the impatient

Basic CSL overview paper with code examples: http://www.fastlabinc.com/CSL/CSL_Intro.pdf

Presentation slides for the really impatient: http://www.fastlabinc.com/CSL/CSL_Overview.pdf

CSL Demo App quick start cheat sheet: https://github.com/stpope/CSL6/blob/master/Doc/6.0DemoQuickStart.pdf

## About CSL

CSL is a C++ class library for sound/music applications; to use it, you write and compile 
C++ programs like the ones in the Src/Tests directory. These programs will use the CSL 
class library, and may read input files or respond to in-coming MIDI or OSC 
commands. CSL apps can run stand-alone as servers, or have interactive GUIs, or be 
plug-ins to out-board signal processing tools. CSL is frequently used together 
with JUCE, a comprehensive C++ class library for multimedia and GUI programming, 
but it can also be used for stand-alone "head-less" apps without JUCE, or with 
other GUI frameworks such as Qt.

![alt text](https://github.com/stpope/CSL6/blob/master/Doc/6.0DemoScreen2.jpg "CSL Demo screen shot")

The target users for CSL are C++ programmers (familiar with the 
development tools of their platforms, e.g., Xcode on the Mac, Eclipse on Linux, or 
Visual Studio on MS-Windows) who also know some software sound synthesis language 
such as Csound or SuperCollider.
It also helps to have used the JUCE library, or at least to have downloaded it and 
compiled and run its own demo app.

The core classes of CSL implement a traditional computer music programming model (like
a Music-N-family language or a modular synthesizer), with objects that represent 
buffers and streams of audio samples, and unit generator objects that represent audio 
sources and processors (e.g., many kinds of oscillators, filters and spatial panners)
connected together into graphs that produce complex dynamic sounds.
The signal processing graphs (patches) can be connected to several kinds of output
for real-time processing or writing to files, and can be controlled via scripts or MIDI or OSC.
Most typical sound synthesis and processing objects are provided, as shown in the CSL class diagram below.

![alt text](http://fastlabinc.com/CSL/classes.jpg)

CSL grew out of code examples developed to teach a sequence of six graduate courses in digital audio programming, 
the MAT 240 series taught at UCSB (and elsewhere) for over 10 years. 
Students in these courses contributed many ideas and much concrete
code to CSL (see the doc). The course materials for these courses are all available on-line at the link, http://heaveneverywhere.com/TheBigMATBook

## CSL Coding Start-up

The best way to get started is to (1) read some of the PDF papers in the Doc folder, and 
(2) look at the Doxygen-generated API documentation in Doc/html.zip

You can unzip this file to get the full HTML doc; you might also want to study the files,

	Src/Kernel/CSL_Types.h (note the system defaults here)
	Src/Kernel/CSL_Core.h (the kernel classes are here)
	Src/Sources/SimpleSines.{h,cpp} (a tutorial for writing unit generators)
	Src/Tests/TestSources.cpp (or any of the other test sources in that folder)

To compile the sources, you will need to create the links in the Src/Includes folder; 
to do this, open a UNIX shell (terminal, or DOS prompt) and execute the commands,

1. change to the Includes folder

	cd ~/Code/CSL6/Src/Includes

2. make symbolic links from the include files to this folder

	./remake (or use the .bat file for MS-Windows)

Some of the code assumes that the CSL package is installed in ~/Code/CSL6; there are default settings 
in Src/Kernel/CSL_Types.h that have to be changed if you put it somewhere else.

Note that the release contains a number of files that have not been ported to the 
newest framework; these are generally in subdirectories called "Old" and may well 
work with a little bit of tweaking; all the really unstable code has been removed.
There are also a number of demos that are turned off by default, i.e., they are not presented in the demo menus.
Look at the files in the Src/Tests folder for details.

## Platform Requirements (tested on these platforms)

	MacOS: OSX 10.13 or newer with Xcode
	Linux: Ubuntu 19 (or other recent release of Linux) with X11-libs, ALSA, Freetype, etc.
		X86 (PC) and ARM (RaspberryPi) platforms supported
	iOS: recent releases and toolchains
	Android: recent releases and toolchains
	MS-Windows: see note below
	All: JUCE 6.0

CSL has run on MS_Windows in previous versions using tool-chains with standards-compliant C++ compilers
such as Cygwin or CodeBlocks. JUCE's current tools support CodeBlocks on Windows, but we have not succeeded in
making this work.
The source tree includes a CodeBlocks project that compiles and links, but produces a non-executable excutable.
The included VisualStudio19 project starts to compile, but fails to parse the thread function signatures.
Another option would be using the GCC toolchain in Cygwin. There are unfortunately several places where CSL
uses C++ idioms that the Microsoft compiler rejects (see the file Doc/Bugs.txt). 
We'd love some help from a Windows development expert here...

Linking with the CSL Libraries

To link CSL programs, you'll need the following packages installed on your machine:

Either,

 	JUCE: Cross-platform everything -- http://www.juce.com

OR these or similar libraries:

 	Cross-platform audio I/O: PortAudio (http://www.portaudio.com) V19 or rtaudio;
 	Cross-platform MIDI I/O: PortMIDI (http://www-2.cs.cmu.edu/~music/portmusic);
 	Cross-platform sound file I/O: libsndfile 
 		(http://www.zip.com.au/~erikd/libsndfile) V1.04 or newer; 
 	OpenSoundControl support: liblo (http://liblo.sourceforge.net);
	Fast Fourier Transform: FFTW (http://www.fftw.org) package V3.X; and
	Graphics and GUI support such as Qt.

If you use FFTW (rather than FFTReal), it needs to be compiled in the way described 
in Sources/Spectral.h.

## CSL JUCE demo

Building CSL

 	Use the JUCE projucer with the file CSL6.jucer to generate the build projects.

 	On a Mac: Builds/MacOSX/CSL6Demo.xcodeproj
 	On MS-Windows: Builds/MS_Windows
 	On Linux: Builds/LinuxMakefiles; make

The standard way of running interactive CSL programs is using the JUCE GUI. 
Take a look at the projects for Mac/Linux/Windows; we create a window with a couple 
of VU meters and oscilloscopes and combo boxes to select a test suite and specific test 
to run. There's a demo GUI cheat sheet in the Doc folder.

Here's the JUCE demo GUI's Menu List

Oscillator Tests  -  Test_Oscillators.cpp

               Sweep test	Test a sine with swept freq and volume swell
             Simple sines	Test some simple sine oscilators
       Standard waveforms	Demonstrate the standard wave forms
              Scaled sine	Play a scaled-quiet sine wave
    Wavetable interpolation	Show truncated/interpolated wave tables
              AM/FM sines	Play an AM and FM sine wave
         Dump AM/FM sines	Dump the graph of the AM/FM sine
        SumOfSines cached	Play a sum-of-sines additive oscillator
    SumOfSines non-cached	Play an uncached inharmonic sum-of-sines
         SumOfSines build	Build up a harmonic series on a sum-of-sines
           SumOfSines 1/f	Play a 1/f spectrum sum-of-sines
      Wavetable from file	Play a wave table from a sound file
                SHARC SOS	Load/print the SHARC timbre database, play example
             Vector SHARC	Show vector cross-fade of SHARC spectra

Source Tests  -  Test_Sources.cpp

              Noise tests	Test noise generators
           Plucked string	Waves of string arpeggii, stereo with reverb
     Mono Snd file player	Test playing a sound file
    Stereo Snd file player	Play a stereo sound file
       Snd file transpose	Demonstrate transposing a sound file
         Sample file bank	Play a large sample bank from sound files
            FM instrument	Play the basic FM instrument
      Fancy FM instrument	FM note with attack chiff and vibrato
    SumOfSines instrument	Demonstrate the SumOfSines instrument
      Snd file instrument	Test the sound file instrument
           IFFT synthesis	Make a sound with IFFT synthesis
              Vector IFFT	Vector synthesis with 2 IFFTs
    Soundfile granulation	Random sound file granulation example

Envelope Tests  -  Test_Envelopes.cpp

           Glissando test	Demonstrate a glissando function
      Swell on aqmplitude	Make an amplitude swell
       Frequency envelope	Play a note with a frequency envelope
                  AR sine	Play an AR (attack/release) amplitude envelope
          AM/FM envelopes	Test AM and FM envelopes
                   ADSR 2	Play an ADSR (attack/decay/sustain/release)
                  ADSR FM	Dual-envelope FM example
       Rand Freq envelope	Play a random-walk frequency envelope
         50 Rand F/A envs	Test 50 random frequency envelope players
                 Fancy FM	Play a fancy FM note
         Complex envelope	Play a note with a complex amplitude envelope
          Many random SOS	Layer many SumOfSines instruments with envelopes

Effect Tests  -  Test_Effects.cpp

                  Clipper	Demonstrate the signal clipper
               FIR filter	Play an FIR band-pass filter
              All filters	Test different filter types
        Filtered snd file	Dynamic BPF on a voice track
           Dynamic filter	Play a dynamic BP filter on noise
     Many dynamic filters	Many dynamic filtered-noise instruments
                   Reverb	Show mono reverb on impulses
              Stereo-verb	Listen to the stereo reverb
                Multi-tap	Play a multi-tap delay line
           Block up-sizer	Test the block resizer on up-sizing
         Block down-sizer	Test the block resizer on down-sizing
        Sample-avg filter	Demo in-line sample-average-filter class

Panner Tests  -  Test_Panners.cpp

            Stereo panner	Demonstrate the stero panner
                    Mixer	Mixer with 4 sine inputs (slow sum-of-sines)
            Panning mixer	Play a panning stereo mixer
     Bigger panning mixer	Test a mixer with many inputs
                 Osc bank	Mix a bank of oscillators
       HRTF horiz circles	Test the HRTF-based binaural panner
       HRTF axial circles	Play a HRTF-panner with axial circles
      HRTF median circles	Play a HRTF-panner with median circles
               Ambisonics	Test the Ambisonic-based spatial panner

Control Tests  -  Test_Control.cpp

               Dump ports	Dump list of MIDI ports to stdout
               Dump input	Dump MIDI input from default device
               MIDI notes	Play MIDI notes (reads MIDI kbd)
              MIDI output	Test sending MIDI output
            MIDI listener	Start the MIDI listener object
         MIDI file player	Play a MIDI file on an instrument library
        OSC client/server	OSC client/server on a library
               OSC server	Start OSC server on a library

Audio Tests  -  Test_Audio.cpp

         Dump audio ports	Dump list of audio ports to stdout
            Echo audio in	Play the microphone in
               Audio echo	Add echo to the live input
             Input panner	Stereo panner on the live input
           Input listener	Demonstrate recording input listener

The source code for all these tests is in the Src/Tests directory (and in a "Test" 
file group in the IDE); it's a good way to learn CSL to run the JUCE demo in an 
XCode/Eclipse/VisualStudio debugger and set breakpoints in the test functions 
you're interested in while using the GUI.

## CSL main() functions

Note that there are several versions of the main() function; if you're not using
one of the prepared project files, try compiling the library (most of the sources)
with the file Beep_main.cpp as the main. Alternatively, take a look at the bottom 
of Test_Oscillators.cpp (or Test_Sources.cpp) and select a couple of tests to run.

Test targets

	Beep_Test -- simplest FM beep
	CSL_Test -- basic test suite; uses Test_main.cpp
	OSC_Test -- Several; OSC tests, see OSC_main.cpp
	MIDI_Test -- Reads MIDI in
	CSL Client_Server -- compiles 2 executables for remote c/s streaming

## Source Code Organization

The subdirectories of CSL are reflected in the project file categories:

	Tests - Test/demo main() driver functions
	Kernel - Buffers, UnitGenerators and all the other core classes
	Sources - Oscillators, noise, envelopes, PhysMod
	Processors - Operators, filters, mixers, panners
	IO - IO drivers and LAN streaming
	Utilities - Thread and buffer support classes
	Instruments - OSC/MIDI instrument wrappers
	Spatializers - Panners and spatializers
	Doc - published papers, Doxygen doc, etc.
	Data - Test sounds, HRTF data, etc.

Reading the source

	Set tabs to 4 spaces so comments line up.
	Use a syntax-coloring editor, if available.
	Note the naming conventions.

Coding Conventions

Note that CSL uses old-school exception signatures, so it's most compatible with the C++ 11 standard. You'll get warnings about them from newer compilers. See the file Doc/Bugs.txt for details.

Naming

	Class, member, and method names are written in "camelCase" as in "UnitGenerator."
	Data members (instance variables) are written with initial "m" followed by embedded caps as in "mOffset."
	Enumeration constants are written with initial "k" followed by embedded caps as in "kDone."

Privacy

	In general data members are protected and have accessor functions where appropriate. 
	The one exception is Buffer which is considered a record class and has public members.

CORE CSL Types & Classes

	sample(float), SampleBuffer, SampleBufferVector, 
	SampleComplex, SampleComplexVector
	PortMap, UGenVector, UGenMap, IODeviceVector, Timestamp, VOIDFCNPTR
	Buffer, UnitGenerator, Controllable, Scalable, Effect
	FanOut, Splitter, Joiner, Interleaver, IO
	Window, Envelope, Oscillator, Filter,Mixer, Panner

Constants

	CSL_PI, CSL_TWOPI, CSL_PIHALF, CSL_SQRT_TWO, CSL_SPEED_OF_SOUND, 
	CSL_EXP_PER_DB, CSL_SAMPS_PER_METER, CSL_DEGS_PER_RAD

## Compile-time options 

These are not set in the header files so that different apps can share the source tree.

SoundFile type (enable one):

	USE_JSND - use the JUCE-based sound file class (requires only JUCE)
	USE_LSND - use libSndFile-based sound file class (supports many file types)
	USE_CASND - use the CoreAudio version (used on iOS)

FFT implementation (enable one):

	USE_FFTW - use FFTW 3 (faster but complicated to build)
	USE_FFTREAL - use FFTReal (smaller and simpler)
	USE_KISSFFT	- use KISS FFT (smaller, untested)
	
Thread implementation

	USE_JTHREADS - use the JUCE thread classes (otherwise use pthreads)

MIDI API

	USE_JMIDI - use JUCE's MIDI I/O

OSC API

	USE_LOSC - use LibLo for OSC (assumes liblio 0.26 is installed)

Main function to use

	USE_JUCE - use a JUCE component for the main() function (otherwise test_main())

## Documentation

See the PDF and HTML files in the Doc/ directory. (RTFM)

See the file COPYRIGHT for the list of authors and UC copy-left.

stp et al. - January, 1998 - November, 2012 - April, 2020

Contact: stephen _at_ heaveneverywhere _dot_ com
