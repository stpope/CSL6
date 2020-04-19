//
//	Test_Envelopes.cpp -- C main functions for the basic CSL envelope and control patching tests.
//	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
//
// This program simply reads the run_tests() function (at the bottom of this file)
//  and executes a list of basic CSL tests
//

#ifndef USE_JUCE
#define USE_TEST_MAIN			// use the main() function in test_support.h
#include "Test_Support.cpp"		// include all of CSL core and the test support functions
#else
#include "Test_Support.h"
#endif
#include <unistd.h>

/////////////////////// Here are the actual unit tests ////////////////////

/// Apply a glissando to a sine with a LineSegment

void testGliss() {
	Osc vox;
	LineSegment line(3, 2000, 60, kLine);
	line.dump();
	vox.setFrequency(line);
	vox.setScale(0.3);
	logMsg("playing gliss sin with linear segment...");
	runTest(vox);
	logMsg("done.\n");
	
	LineSegment line2(3, 2000, 60, kExpon);
	line2.dump();
	vox.setFrequency(line2);
	logMsg("playing gliss sin with exponential segment...");
	runTest(vox);
	logMsg("done.\n");
}

/// test an amplitude swell

void testSwell() {
	Osc vox(220);
	LineSegment line(3, 0.0, 0.5);
	vox.setScale(line);
	vox.dump();
	logMsg("playing swell sin with line segment...");
	runTest(vox);
	logMsg("done.\n");
}

/// AM and FM using the dynamic scale and frequency inputs

void testARSin() {
	Osc osc(220);						// carrier to be enveloped
	AR a_env(2, 0.6, 1.0);				// AR constructor dur, att, rel
	osc.setScale(a_env);				// amplitude modulate the sine
	logMsg("AR env dump");
	osc.dump();
	logMsg("playing AR sin...");
	a_env.trigger();
    MulOp mul(osc, 0.4);                // using a MulOp with a constant
	runTest(mul);
	logMsg("done.\n");
}

/// Do the same using the Envelope as an effect

void testARSin2() {
	Osc osc(220);						// carrier to be enveloped
	AR a_env(2, 0.2, 1.0);				// AR constructor dur, att, rel
	a_env.setScale(osc);				// amplitude modulate the sine
	logMsg("playing AR sin 2...");
	a_env.trigger();	
    MulOp mul(a_env, 0.4);              // using a MulOp with a constant
	runTest(mul);
	logMsg("done.\n");
}

/// Test an envelope applied to frequency

void testFrequencyEnv() {
	Osc vox;
	Envelope env(3, 0, 220, 0.7, 280, 1.3, 180, 2.0, 200, 3, 100);
	logMsg("");
	logMsg("BP env dump");
	env.dump();
	logMsg("");
	vox.setFrequency(env);
	vox.setScale(0.3);
	logMsg("playing sin with BP envelope on frequency");
	env.trigger();	
    MulOp mul(vox, 0.4);              // using a MulOp with a constant
	runTest(mul);
	logMsg("done.\n");
}

/// Test an envelope applied to frequency & scale

void testAMFMEnvs() {
	Osc vox;
	Envelope env(3,  0, 100,   0.7, 180,   1.3, 180,   2.0, 200,   3, 140);
	Envelope env2(3,   0.0, 0,   1.0, 1,    2.5, 0,   2.75, 1,    3, 0);
	vox.setFrequency(env);
	vox.setScale(env2);
	logMsg("playing sin with BP envelopes on frequency & scale");
	fflush(stdout);
	env.trigger();	
	env2.trigger();	
    MulOp mul(vox, 0.4);              // using a MulOp with a constant
    runTest(mul);
	logMsg("done.\n");
}


//////////////////////// Envelope tests ////////////////////////////////////////////

/// Demonstrate an ADSR

void testADSR2() {
	float duration = 3.0;	// total dur
	float attack = 0.02;		// att time
	float decay = 0.03;		// dec time
	float sustain = 0.1;		// sust value
	float release = 1.5;		// release time
	ADSR adsr(duration, attack, decay, sustain, release);		// constructor
	Osc vox(220);
	vox.setScale(adsr);
	logMsg("playing sharp ADSR envelope.");
	adsr.trigger();	
	runTest(vox);
	logMsg("done.\n");
	
	ADSR adsr2(duration, 0.5, 0, 1, 0.5);
	vox.setScale(adsr2);
	logMsg("playing gradual ADSR envelope.");
	adsr2.trigger();	
	runTest(vox);
	logMsg("done.\n");
}

/// FM using 2 oscs and 2 ADSRs

#define BASE_FREQ 440.0
//#define BASE_FREQ 8000.0

void testADSR_FM() {
	ADSR a_env(3, 0.1, 0.1, 0.5, 1);			// set up 2 standard ADSRs (3-sec duration)
	ADSR i_env(3, 1.5, 0, 1, 0.4);
				// percussive envelopes
//	ADSR a_env(3, 0.02, 0.1, 0.05, 2);			// set up 2 standard ADSRs (3-sec duration)
//	ADSR i_env(3, 0.0001, 0.1, 0, 0);
	Osc car(BASE_FREQ);							// init the 2 oscillators (fc = fm)
	Osc mod(BASE_FREQ*1.414);
//	i_env.setScale(BASE_FREQ);				// scale the index envelope by the mod. freq
	i_env.setScale(BASE_FREQ);
	mod.setScale(i_env);
	mod.setOffset(BASE_FREQ);
	car.setFrequency(mod);
	car.setScale(a_env);
	i_env.trigger();	
	a_env.trigger();	
	logMsg("playing ADSR FM\n");
//	car.dump();
	runTest(car);
	logMsg("done.\n");
}

/// test the rand env as the freq of a SOS

void testRandFreqEnv() {
	SumOfSines sos(kFrequency, 6,   0.2, 0.2, 0.1, 0.05, 0.02, 0.02);
	sos.createCache();						// make the cached wavetable
	AR a_env(6, 1, 1);						// dur, att, rel
											// RandEnvelope (frq, amp, offset, step)
	RandEnvelope f_env(3, 80, 200, 40);		// freq env = random walk
	sos.setFrequency(f_env);				// set the carrier's frequency
	sos.setScale(a_env);					// multiply index envelope by mod freq
	logMsg("playing random gliss...");
//	f_env.trigger();						// reset the envelopes to time 0
	a_env.trigger();
	runTest(sos, 6);						// run the test to play a note on the root of the DSP graph
	logMsg("done.\n");
}

/// Function to create and answer a RandFreqEnv UGen patch

UnitGenerator * createRandFreqEnvPatch(float dur) {
	Osc * vox = new Osc;						// declare an oscillator
	RandEnvelope * a_env = new RandEnvelope(4, 0.5, 0.5, 0.5);      // ampl env = random (frq, amp, off, step)
	RandEnvelope * f_env = new RandEnvelope(3, 80, 200, 40);        // freq env = random (frq, amp, off, step)
	Osc * lfo = new Osc(fRandM(0.3, 0.6), 1, 0, fRandM(0, CSL_PI)); // LFO with rand frq/phase
	Panner * pan = new Panner(*vox, *lfo);		// stereo panner
	vox->setFrequency(*f_env);					// set the carrier's frequency
	vox->setScale(*a_env);						// multiply index envelope by mod freq
	pan->setScale(0.04);						// scale softer
	return (pan);								// return &pan
}

/// set up 50 RandFreqEnv patches

void test50RandFreqEnv() {
	float duration = 20;						// total dur
	Mixer mix(2);								// stereo mixer
	
	for (unsigned i = 0; i < 50; i++)			// play a mix of RandFreqEnvPatch instruments
		mix.addInput(createRandFreqEnvPatch(duration));
		
	logMsg("playing mix of 50 random gliss oscs...");
#ifndef CSL_WINDOWS
	srand(getpid());							// seed the rand generator -- UNIX SPECIFIC CODE HERE
#endif
    MulOp mul(mix, 0.4);              // using a MulOp with a constant
	runTest(mul, duration);
	logMsg("done.\n");
	mix.deleteInputs();							// clean up
}

/// test the use of scaling

void testEnvScale() {
	Osc vox;									// declare 2 oscillators
	LineSegment line(2, 60, 100);
	vox.setFrequency(line);						// set the carrier's frequency
	ADSR a_env(2, 0.6, 0.1, 0.3, 1);			// amplitude env = std ADSR
	a_env.setScale(vox);						// multiply index envelope by mod freq
	logMsg("playing env as VCA...");
	a_env.trigger();	
	runTest(a_env, 2);							// run the test to play a note on the root of the DSP graph
	logMsg("done.\n");
}

///  FM with vibrato (with AR-envelope), attack chiff (filtered noise with AR-envelope), 
///		and random freq. drift and ampl. swell envelopes

void testFancy_FM() {
	Osc car(BASE_FREQ);							// init the 2 oscillators (fc = fm)
	Osc mod(BASE_FREQ * 1.05);
	ADSR a_env(3, 0.04, 0.2, 0.5, 1);			// set up 2 standard ADSRs (3-sec duration)
	ADSR i_env(3, 1.0, 0, 1, 1.0);
	
	Envelope mVibEnv(kExpon, 3, 0.0, 0.1, 2, 1.0, 3, 0.1);	// vibrato envelope (expon triangle)
	ADSR mChiffEnv(3, 0.01, 0.01, 0.0, 2);		// attack-chiff envelope
   	Osc mVibrato(6);							// sine oscillator for vibrato
	WhiteNoise mChiff;							// attach chiff noise source
	Butter mChFilter(mChiff, BW_BAND_PASS, 4000.f, 200.f);	// and filter
	
	mVibEnv.setScale(8);						// scale vibrato envelope
	mVibrato.setScale(mVibEnv);					// apply vibrato envelope
	mVibrato.setOffset(BASE_FREQ);				// shift vibrato up

	mChiffEnv.setScale(2.0);					// scale chiff envelope
	mChFilter.setScale(mChiffEnv);				// apply chiff envelope
	car.setOffset(mChFilter);					// add in chiff

	i_env.setScale(BASE_FREQ * 2);				// scale mod index envelope
	mod.setScale(i_env);						// scale modulator
	mod.setOffset(mVibrato);					// add in vibrato and base freq
	car.setFrequency(mod);						// apply FM modulation
	car.setScale(a_env);						// apply ampl envelope
	
	Panner mPanner(car, 0.0);					// stereo panner
	Freeverb mReverb(mPanner);					// mono reverb
	mReverb.setRoomSize(0.99);					// longer reverb

	i_env.trigger();							// GO -- trigger envelopes
	a_env.trigger();	
	mVibEnv.trigger();	
	mChiffEnv.trigger();
	
	logMsg("playing Fancy FM\n");
	runTest(mReverb, 4);						// run the test
	runTest(mReverb, 4);						// run the test
	logMsg("done.\n");
}

/// Make an envelope from scratch by adding breakpoints

void testComplexEnvelope() {
	float dur = 5;								// total dur
	SumOfSines sos(kFrequency, 5,   0.4,  0.2,  0.1,  0.05,  0.02);
	sos.createCache();							// make the cached wavetable
	Envelope a_env;								// make an empty envelope
	float tim = 0.0;
	do {										// add a bunch of break points to it
		a_env.addBreakpoint(tim, fRandZ());
		tim += fRandM(0.2, 0.5);				// pick next time
	} while (tim < dur);
	a_env.addBreakpoint(dur, 0);				// add final b-point
	a_env.dump();
	sos.setScale(a_env);						// apply env
	Panner pan(sos, fRand1());					// put sos in a panner
	logMsg("playing mix of 50 random SOS oscs...");
	a_env.trigger();							// trigger env
	runTest(pan, dur);
	logMsg("done.\n");
}

// create a UGen patch for a SOS note with a made-up envelope

UnitGenerator * sosNote(float dur, float frq, float amp) {
	SumOfSines * sos = new SumOfSines(frq);
	for (int i = 0; i < 30; i++)				// add 30 partials per note
		sos->addPartial((float)i, fRandZ(), fRandZ() * CSL_TWOPI);
	sos->createCache();							// make the cached wavetable
	Envelope * a_env = new Envelope;			// make an empty envelope
	float tim = 0.0;
	do {										// add a bunch of break points to it
		a_env->addBreakpoint(tim, fRandZ());
		tim += fRandM(0.3, 0.7);				// pick next time
	} while (tim < dur);
	a_env->addBreakpoint(dur, 0);
	a_env->setScale(amp);						// make it quiet
	sos->setScale(*a_env);						// apply env
	a_env->trigger();	
	Panner * pan = new Panner(*sos, fRand1());
	return (pan);								// return pan
}

void testManyRandSOS() {
	float duration = 20;						// total dur
	Mixer mix(2);								// stereo mixer
	for (unsigned i = 0; i < 100; i++)			// play a mix of SOS instruments
		mix.addInput(sosNote(duration, fRandM(100, 300), 0.012));
	logMsg("playing mix of 100 random-env SOS oscs...");
#ifndef CSL_WINDOWS
	srand(getpid());							// seed the rand generator -- UNIX SPECIFIC CODE HERE
#endif
	runTest(mix, duration);
	logMsg("done.\n");
	mix.deleteInputs();							// clean up
}

//////// RUN_TESTS Function ////////

#ifndef USE_JUCE

void runTests() {
//	testGliss();					// Simple sine demo tests
//	testSwell();
//	testFrequencyEnv();				// Basic waveform tests
//	testARSin();					// Ways of simple scaling
//	testARSin2();					// Ways of simple scaling
//	testAMFMEnvs();
//	testADSR2();					// test truncating and interpolating wavetable osc
//	testADSR_FM();
//	testRandFreqEnv();
//	testEnvScale();
	testFancy_FM();
}

#else

// test list for Juce GUI

testStruct envTestList[] = {
	"Glissando test",		    testGliss,				"Demonstrate a glissando function",
	"Swell on amplitude",	    testSwell,				"Make an amplitude swell",
	"Frequency envelope",	    testFrequencyEnv,		    "Play a note with a frequency envelope",
	"AR sine",				testARSin,				"Play an AR (attack/release) amplitude envelope",
//	"AR sine 2",			    testARSin2,				"",
	"AM/FM envelopes",		testAMFMEnvs,			"Test AM and FM envelopes",
	"ADSR 2",				testADSR2,				"Play an ADSR (attack/decay/sustain/release)",
	"ADSR FM",				testADSR_FM,		    	"Dual-envelope FM example",
	"Rand Freq envelope",   	testRandFreqEnv,		    "Play a random-walk frequency envelope",
	"50 Rand F/A envs",		test50RandFreqEnv,		"Test 50 random frequency envelope players",
//	"Envelope scaling",		testEnvScale,			"",
	"Fancy FM",				testFancy_FM,			"Play a fancy FM note",
	"Complex envelope",		testComplexEnvelope,    	"Play a note with a complex amplitude envelope",
	"Many random SOS",		testManyRandSOS,		    "Layer many SumOfSines instruments with envelopes",
	NULL,					NULL,					NULL
};

#endif
