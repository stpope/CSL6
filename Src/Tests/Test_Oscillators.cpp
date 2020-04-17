//
//	Test_Oscillators.cpp -- C main functions for the basic CSL oscillator source tests.
//	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
//
// This program simply reads the run_tests() function (at the bottom of this file)
//  and executes a list of basic CSL tests
//

#ifdef USE_JUCE
	#include "Test_Support.h"
#else
	#define USE_TEST_MAIN			// use the main() function in test_support.h
	#include "Test_Support.cpp"		// include all of CSL core and the test support functions
#endif

#ifdef USE_JSND
	#include "SoundFileJ.h"
#else
	#include "SoundFileL.h"
#endif

#include "SimpleSines.h"

/////////////////////// Here are the actual unit tests ////////////////////

/// Apply a glissando and swell to a sine oscillator with LineSegments

void testSweep() {
	Osc vox;								    // wave-table oscillator
	LineSegment gliss(3, 40, 5000);		        // freq line (dur val1, val2)
	LineSegment swell(3, 0.000001, 0.5);	    // ampl line
	vox.setFrequency(gliss);				    // apply freq function
	vox.setScale(swell);					    // apply ampl function
//	vox.dump();
	logMsg("playing swept sin with line segment...");
	runTest(vox);						        // play vox for 3 seconds
	logMsg("done.\n");
}

/// Simplest sine wave tests using the tutorial example classes

void testSimpleSines() {
	float freq = 220.0;						// choose a frequency in Hz
	SimpleSine sineOsc(freq);				// create a simple sine oscillator
	sineOsc.dump();
	logMsg("playing simple sin...");
	runTest(sineOsc);						// call the test function that runs this for a few seconds
	logMsg("sin done.");
	SineAsPhased sineOsc2(freq);			// create a sine-as-phased oscillator
	sineOsc2.dump();
	logMsg("playing sin-as-phased...");
	runTest(sineOsc2);						// call the test function that runs this for a few seconds
	logMsg("sin done.");
	SineAsScaled sineOsc3(freq, 0, 0.2, 0);	// sine-as-scaled constructor args: freq, phase, scale, offset
	sineOsc3.dump();
	logMsg("playing sin-as-scaled...");
	runTest(sineOsc3);						// call the test function that runs this for a few seconds
	logMsg("sin done.");
}

/// Test/demonstrate the basic waveform oscillators

void testBasicWaves() {
	float freq = 220.0;						// choose a frequency in Hz
	float ampl = 0.4;						// choose an amplitude scale

	Sine sine(freq, ampl);					// create a computed sine oscillator
	logMsg("playing computed sin...");
//	sine.dump();
	runTest(sine);							// call the test function that runs this for a few seconds
//	dumpTest(sine);							// or just dump the graph
	logMsg("done.\n");
	sleepMsec(250);

	Osc wav(freq, ampl, 0.0, 0.0);			// create a wave-table sine oscillator
	logMsg("playing wavetable sin...");
	runTest(wav);							// call the test function that runs this for a few seconds
	logMsg("done.\n");
	sleepMsec(250);

	Sawtooth saw(freq, ampl);				// create a sawtooth oscillator
	logMsg("playing sawtooth...");
	runTest(saw);							// call the test function that runs this for a few seconds
	logMsg("done.\n");
	sleepMsec(250);

	Square square(freq, ampl);				// create a square oscillator
	logMsg("playing square...");
	runTest(square);						// call the test function that runs this for a few seconds
	logMsg("done.\n");
	sleepMsec(250);
	
//	Impulse impulse(freq, ampl);			// create an impulse oscillator
//	logMsg("playing impulse...");
//	runTest(impulse);						// call the test function that runs this for a few seconds
//	logMsg("done.");
}

/// Scaled sine wave -- 3 methods: with a MulOp and using the Sine's scale input

void testScaledSin() {
	Osc osc(220);
	osc.setScale(0.4);						// simplest: scale the sine directly
	logMsg("playing quiet sin1 (setScale)...");
	runTest(osc);
	logMsg("quiet sin done.");
	sleepMsec(250);
	
	Osc sineOsc2(220);
	MulOp mul(sineOsc2, 0.4);				// using a MulOp with a constant
	logMsg("playing quiet sin2 (MulOp)...");
	mul.dump();
	runTest(mul);
	logMsg("quiet sin done.");
	sleepMsec(250);

	Osc sineOsc3(220);
	StaticVariable var(0.4);				// using a MulOp with a StaticVariable
	MulOp mul2(sineOsc3, var);
	logMsg("playing quiet sin3 (StaticVar)...");
	runTest(mul2);
	logMsg("quiet sin done.");
}

/// Test truncating and interpolating wavetable osc with a small wavetable 
/// (these should sound different because of the truncation error in the first tone)

#define TABLE_SIZE 128

void testWavetableInterpolation() {
	float sineTable[TABLE_SIZE];
	float * ptr = & sineTable[0];
	float incr = CSL_TWOPI / TABLE_SIZE;
	float accum = 0;
	for (unsigned i = 0; i < TABLE_SIZE; i++) {
		*ptr++ = sin(accum);
		accum += incr;
	}
	Buffer oscBuff(1, TABLE_SIZE);
	oscBuff.setBuffer(0, sineTable);
	oscBuff.mAreBuffersAllocated = true;
	Osc wav(oscBuff);
	wav.setScale(0.4);
	logMsg("playing truncating wavetable...");
	runTest(wav);							// call the test function that runs this
	logMsg("done.\n");
	sleepMsec(250);
	wav.setInterpolate(kLinear);			// set the oscillator's interpolation policy
	logMsg("playing interpolating wavetable...");
	runTest(wav);							// re-play test
	logMsg("done.\n");
}

/// AM and FM using the dynamic scale and frequency inputs

void testAMFMSin() {
	Osc osc(220);						// carrier to be AM'ed
	Osc AMmod(6, 0.3, 0.4);				// AM osc: freq, scale, offset
//	mod.setScale(0.2);					// another way to scale the AM osc
//	mod.setOffset(0.8);					// or to offset the AM osc
	osc.setScale(AMmod);				// amplitude modulate the sine
	logMsg("Simple AM/FM sin Examples");
	osc.dump();
	logMsg("playing AM sin...");
	runTest(osc);
	logMsg("AM sin done.\n");

	Osc osc2;							// carrier to be FM'ed
	Osc FMmod(4, 40, 220);				// FM osc: freq, scale, offset
	osc2.setFrequency(FMmod);			// FM using setFrequency()
	osc2.setScale(0.5);					// scale the osc (quieter)
	osc2.dump();
	logMsg("playing FM sin...");
	runTest(osc2);
	logMsg("FM sin done.\n");
}

/// use the dumpTest call to dump the whole graph

void dumpAMFMSin() {
	Osc osc;
	Osc AMmod(6, 0.2, 0.8);				// AM osc: freq, scale, offset
	Osc FMmod(110, 10, 110);			// FM osc: freq, scale, offset
	osc.setScale(AMmod);				// amplitude modulate the sine
	osc.setFrequency(FMmod);			// FM using setFrequency()
	logMsg("Dumping AM/FM sin...");
	osc.dump();
	dumpTest(osc);
	logMsg("Playing AM/FM sin...");
	runTest(osc);
	logMsg("AM/FM sin done.\n");
}

/// Play a sum of sines waveform -- cached harmonic overtone series

void testSumOfSinesCached() {
			// constructor format kFrequency = list of amplitudes for first n overtones
	SumOfSines sos1(kFrequency, 5,   0.4,  0.2,  0.1,  0.05,  0.02);
	sos1.setFrequency(220);
	sos1.setScale(0.2);			// make it quiet
	logMsg("Sum of sines Examples");
	sos1.dump();
	logMsg("playing uncached sum of sines...");
	runTest(sos1);
	logMsg("sum of sines done.\n");
	sleepMsec(250);
	
	sos1.createCache();			// make the cached wavetable
	logMsg("playing cached sum of sines...");
	runTest(sos1);
	logMsg("sum of sines done.\n");
}

/// Play a sum of sines waveform -- 1/f + noise

void testSumOfSines1F() {
								// constructor format # harms, rand noise around 1/f weighting
	SumOfSines sos1(16, 0.5);
	sos1.setFrequency(220);
	sos1.createCache();			// make the cached wavetable
	sos1.setScale(0.2);			// make it quiet
	logMsg("Sum of sines 1/f");
	runTest(sos1);
	logMsg("sum of sines done.\n");
}

// play a loop that adds upper harmonics in steps

void testSumOfSinesSteps() {
	SumOfSines sos2(kFrequency, 3,   0.4,  0.2,  0.1);
	sos2.setFrequency(220);
	sos2.setScale(0.2);			// make it quiet
	logMsg("playing sum of sines loop...");
	for (unsigned i = 3; i < 11; i++) {
		sos2.addPartial(i*2, (0.6 / i));
//		sos2.dump();
		runTest(sos2, 2);
	}
	logMsg("sum of sines done.");
}

/// Now test an uncached inharmonic series
/// constructor format 3 = list of (freq/amp/phase) for (not-necessarily-harmonic) overtones
/// Since it's a non-harmonic overtone series in this example, we can't cache a wavetable

void testSumOfSinesNonCached() {
	SumOfSines vox2(kFreqAmpPhase, 4,   1.0, 0.2, 1.0,   1.08, 0.2, 2.0,   
										1.2, 0.1, 3.0,   1.34, 0.02, 4.0);
	vox2.setFrequency(220);
	vox2.setScale(0.5);			// make it quiet
	
	logMsg("playing uncached sum of sines...");
	runTest(vox2, 5);
	logMsg("sum of sines done.");
}

/// Load an oscillator's wave table from a file -- a single cycle of the vowel "oo" from the word "moon"

void testWaveTableFromFile() {
	SoundFile fi(CGestalt::dataFolder() + "oo_table.aiff");
	fi.openForRead(true);
	logMsg("Loading sound file %s = %d frames @ %d Hz", fi.path().c_str(), fi.duration(), fi.playbackRate());
//	fi.seekTo(0, kPositionStart);
	Buffer oscBuff(1, fi.duration());
	oscBuff.setBuffer(0, fi.mWavetable.buffer(0));
    oscBuff.mAreBuffersAllocated = true;
    oscBuff.mNumAlloc = fi.duration();
    Osc wav(oscBuff);
//    Osc wav;
    wav.setFrequency(440.0);                   // set freq
    wav.setScale(0.25);                        // set ampl
	logMsg("playing wavetable from file...");
	runTest(wav);							// call the test function that runs this for a few seconds
	logMsg("done.");
}

/// Test loading the SHARC samples

#ifndef CSL_WINDOWS

#include <SHARC.h>

void test_SHARC() {								
	SHARCLibrary::loadDefault();				// load the SHARC library
	SHARCLibrary * lib = SHARCLibrary::library();
	lib->dump();
	SHARCInstrument * inst = lib->instrument_named("bassoon");
	SHARCSpectrum * spect = inst->_spectra[4];	// get a spectrum and make a SumOfSines
	SumOfSines sos(* spect);
	sos.setFrequency(110);
	ADSR adsr(2, 0.04, 0.1, 0.6, 1.0);			// make an ADSR
	sos.setScale(adsr);
	logMsg("playing SHARC sum of sines...");
	adsr.trigger();	
	runTest(sos, 2);
	logMsg("done.\n");
}

/// test vector SHARC - load 3 spectra into SOS oscillators and cross-fade

void test_SHARC2() {
	float dur = 6.0f;
	SHARCLibrary::loadDefault();
	SHARCLibrary * lib = SHARCLibrary::library();
												// 3 spectra from the SHARC library
	SHARCSpectrum * spect1 = lib->spectrum("oboe", 60);
	SHARCSpectrum * spect2 = lib->spectrum("tuba", 40);
	SHARCSpectrum * spect3 = lib->spectrum("viola_vibrato", 60);
	SumOfSines sos1(*spect1);					// create 3 SOS oscs
	SumOfSines sos2(*spect2);
	SumOfSines sos3(*spect3);
	sos1.setFrequency(110);						// set freqs
	sos2.setFrequency(220);
	sos3.setFrequency(110);
												// 3 envelopes
	LineSegment env1(dur, 1, 0.0001);			// fade out
	sos1.setScale(env1);
	Triangle env2(dur);							// triangle
	sos2.setScale(env2);
	LineSegment env3(dur, 0.0001, 1);			// fade in
	sos3.setScale(env3);
	Mixer mix(1);								// mix 'em
	mix.addInput(sos1);
	mix.addInput(sos2);
	mix.addInput(sos3);
	Freeverb revb(mix);							// mono reverb
	revb.setRoomSize(0.985);					// longer reverb
	Panner pan(revb);							// stereo panner
	logMsg("playing SHARC vector synth (oboe, tuba, viola)...");
	env1.trigger();								// trigger envelopes
	env2.trigger();	
	env3.trigger();	
	runTest(pan, dur);							// run test fcn
	logMsg("done.\n");
}
#endif

//////// RUN_TESTS Function ////////

#ifndef USE_JUCE

void runTests() {
	testSimpleSines();				// Simple sine demo tests
//	testBasicWaves();				// Basic waveform tests
//	testScaledSin();				// Ways of simple scaling
//	testWavetableInterpolation();	// test truncating and interpolating wavetable osc
//	testAMFMSin();					// Examples of AM and FM
//	dumpAMFMSin();					// Examples of AM and FM
//	testSumOfSinesCached();			// Sum Of Sines tests
//	testSumOfSinesNonCached();		// Sum Of Sines tests
//	testWaveTableFromFile();		// load a wavetable from a sound file
//	testSweep();
}

#else

// test list for Juce GUI

testStruct oscTestList[] = {
	"Sweep/swell test",			testSweep,			        "Test a sine with swept freq and volume swell",
	"Simple sines",				testSimpleSines,	        "Test some simple sine oscilators",
	"Standard waveforms",		testBasicWaves,		        "Demonstrate the standard wave forms",
	"Scaled sine",				testScaledSin,		        "Play a scaled-quiet sine wave",
	"Wavetable interpolation",	testWavetableInterpolation,	"Show truncated/interpolated wave tables",
	"AM/FM sines",				testAMFMSin,				"Play an AM and FM sine wave",
	"Dump AM/FM sines",			dumpAMFMSin,				"Dump the graph of the AM/FM sine",
	"SumOfSines cached",		testSumOfSinesCached,		"Play a sum-of-sines additive oscillator",
	"SumOfSines non-cached",	testSumOfSinesNonCached,	"Play an uncached inharmonic sum-of-sines", 
	"SumOfSines build",			testSumOfSinesSteps,		"Build up a harmonic series on a sum-of-sines",
	"SumOfSines 1/f",			testSumOfSines1F,			"Play a 1/f spectrum sum-of-sines",
	"Wavetable from file",		testWaveTableFromFile,		"Play a wave table from a sound file",
#ifndef CSL_WINDOWS
	"SHARC SOS",				test_SHARC,					"Load/print the SHARC timbre database, play example",
	"Vector SHARC",				test_SHARC2,				"Show vector cross-fade of SHARC spectra",
#endif
	NULL,						NULL,				NULL
};

#endif
