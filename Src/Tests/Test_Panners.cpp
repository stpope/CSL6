//
//	Test_Panners.cpp -- C main functions for the basic CSL panner and mixer tests.
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

#define USE_CONVOLVER

#ifdef USE_CONVOLVER
#include "Convolver.h"				// FFT-based convolver
#include "Convolver2.h"				// FFT-based convolver
#endif

#ifndef CSL_WINDOWS
#include <unistd.h>
#endif

/////////////////////// Here are the actual unit tests ////////////////////

/// Pan a sine to stereo

void testPan() {
	Osc vox(440);							// src sine 
	Osc lfo(0.2, 1, 0, CSL_PI);				// position LFO
	vox.setScale(0.3);
	Panner pan(vox, lfo);					// panner
	logMsg("playing a panning sine...");
	runTest(pan, 8);
	logMsg("done.\n");
}

/// Pan a sine to "N" channels

void testN2MPan() {
	Osc vox(440);
	Osc lfoX(0.2, 1, 0, CSL_PI);
	Osc lfoY(0.2, 1, 0, 0);
	vox.setScale(0.3);
			//	NtoMPanner(UnitGenerator & i, UnitGenerator & pX, UnitGenerator & pY, float a, unsigned in_c, unsigned out_c, float spr);
	NtoMPanner pan(vox, lfoX, lfoY, 1, 1, 2, 2);					// a panner
	logMsg("playing N-channel panning sine...");
	runTest(pan, 5);
	logMsg("done.\n");
}

/// Test mixer -- really slow additive synthesis

void testSineMixer() {
	Osc vox1(431, 0.3);				// create 4 scaled sine waves
	Osc vox2(540, 0.1);
	Osc vox3(890, 0.03);
	Osc vox4(1280, 0.01);
//	AR vol(5, 1, 1);				// amplitude envelope
//	Mixer mix(2, vol);				// stereo mixer with envelope
	Mixer mix(2);					// create a stereo mixer
	mix.addInput(vox1);				// add the sines to the mixer
	mix.addInput(vox2);
	mix.addInput(vox3);
	mix.addInput(vox4);
	logMsg("playing mix of 4 sines...");
	runTest(mix);
	logMsg("mix done.\n");
}

/// Pan and mix 2 sines

void testPanMix() {
	Osc vox1, vox2;							// sources oscs
	RandEnvelope env1(0.5, 80, 180);		// random freq envelopes
	RandEnvelope env2(0.5, 80, 180);		// (frq, amp, offset, step)
	vox1.setFrequency(env1);				// set osc freq and scale
	vox1.setScale(0.3);
	vox2.setFrequency(env2);
	vox2.setScale(0.3);
	Osc lfo1(0.3), lfo2(0.5, 1, 0, CSL_PI);// position LFOs (out of phase)
	Panner pan1(vox1, lfo1);				// 2 panners
	Panner pan2(vox2, lfo2);
	Mixer mix(2);							// stereo mixer
	mix.addInput(pan1);
	mix.addInput(pan2);
	logMsg("playing mix of panning sins...");
#ifndef CSL_WINDOWS
	srand(getpid());						// seed the rand generator -- UNIX SPECIFIC CODE HERE
#endif
	runTest(mix, 15);
	logMsg("done.\n");
}

/// Pan and mix many sines

void testBigPanMix() {
	int num = 64;							// # of layers
	float scale = 3.0f / (float) num;		// ampl scale
	Mixer mix(2);							// stereo mixer
	for (int i = 0; i < num; i++) {			// loop to add a panning, LFO-controlled osc to the mix
		Osc * vox = new Osc();
		RandEnvelope * env = new RandEnvelope(0.5, 80, 180);
		vox->setFrequency(*env);
		vox->setScale(scale);
		Osc * lfo = new Osc(fRandM(0.5, 0.9), 1, 0, fRandM(0, CSL_PI));
		Panner * pan = new Panner(*vox, *lfo);
		mix.addInput(*pan);
	}
	logMsg("playing mix of %d panning sines (1 minute)...", num);
#ifndef CSL_WINDOWS
	srand(getpid());						// seed the rand generator -- UNIX SPECIFIC CODE HERE
#endif
	runTest(mix, 60);
	logMsg("done.\n");
	mix.deleteInputs();						// clean up
}

/// Make a bank or 50 sines with random walk panners and glissandi

void testOscBank() {
	Mixer mix(2);								// stereo mixer
	for (unsigned i = 0; i < 50; i++) {			// RandEnvelope(float frequency, float amplitude, float offset, float step);
		RandEnvelope * pos = new RandEnvelope;
		RandEnvelope * freq = new RandEnvelope(0.8, 200, 300, 20);
//		Sine * vox = new Sine(110.0, 0.01);		// use computed sine
		Osc * vox = new Osc(110.0, 0.01);		// use stored look-up table
		vox->setFrequency(* freq);
		Panner * pan = new Panner(* vox, * pos);
		mix.addInput(pan);
	}
	logMsg("playing mix of 50 panning sines (1 minute)...");
#ifndef CSL_WINDOWS
	srand(getpid());							// seed the rand generator so we get different results -- UNIX SPECIFIC CODE HERE
#endif
	runTest(mix, 60);
	logMsg("done.\n");
	mix.deleteInputs();							// clean up
}

/// swap sound channels with a ChannelMappedBuffer InOut
/// Constructor: InOut(input, inChan, outChan,  [ch-1 ... ch-outChan]);

#include "InOut.h"			/// Copy in-out plug

void testCMapIO() {
	Osc osc(220);								// carrier to be enveloped
	AR a_env(2, 0.6, 1.0);						// AR constructor dur, att, rel
	osc.setScale(a_env);						// amplitude modulate the sine
	Panner pan(osc, 0.0);						// panner
	InOut chanMap(pan, 2, 2, kNoProc, 2, 1);	// swap stereo channels
	logMsg("playing swapped sin...");
	runTest(chanMap);
	logMsg("done.\n");
}

////////// Convolution

#ifdef USE_CONVOLVER_OLD

void testConvolver() {
	JSoundFile fi(CGestalt::dataFolder(), "rim3_L.aiff");
	try {
		fi.openForRead();
	} catch (CException) {
		logMsg(kLogError, "Cannot read sound file...");
		return;
	}
	logMsg("playing sound file...");
	fi.trigger();
	runTest(fi);
	logMsg("sound file player done.\n");
					// Create convolver; FFT size will be block size * 2
	Convolver cv(CGestalt::dataFolder(), "Quadraverb_large_L.aiff");	
	cv.setInput(fi);
	logMsg("playing convolver...");
	fi.trigger();
	runTest(cv);
	fi.trigger();
	runTest(cv);
	fi.close();
	logMsg("convolver done.\n");
}

#define IRLEN (44100 * 4)			// 4 sec. IR

void testConvolver2() {
	WhiteNoise nois;				// amplitude, offset
	AR env(0.05, 0.0001, 0.049);	// AR constructor dur, att, rel
	nois.setScale(env);				// amplitude modulate the sine
	
	logMsg("playing noise burst...");
	env.trigger();
	runTest(nois);
	logMsg("done.\n");
									// make a simple IR with a few echoes
	Buffer buf(1, IRLEN);
	buf.allocateBuffers();
	float * samp = buf.buffer(0);
	for (unsigned i = 0; i < IRLEN; i += 5000)
		samp[i] = 1 / (1 + (sqrt(i) / 5000));
									// Create convolver; FFT size will be block size * 2
	Convolver cv(buf);
	cv.setInput(nois);
	logMsg("playing convolver...");
	env.trigger();
	runTest(cv);
	env.trigger();
	runTest(cv);
	logMsg("convolver done.\n");
}

void testConvolver3() {
	WhiteNoise nois;				// amplitude, offset
	AR env(0.05, 0.0001, 0.049);	// AR constructor dur, att, rel
	nois.setScale(env);				// amplitude modulate the sine
	
	logMsg("playing noise burst...");
	env.trigger();
	runTest(nois);
	logMsg("done.\n");
	Convolver cv(CGestalt::dataFolder(), "3.3s_LargeCathedral_mono.aiff");	
	cv.setInput(nois);
	logMsg("playing convolver...");
	env.trigger();
	runTest(cv);
	env.trigger();
	runTest(nois);
	env.trigger();
	runTest(cv);
	logMsg("convolver done.\n");
}

#endif

//#ifndef CSL_WINDOWS

/// Spatializer with HRTF

#include "SpatialAudio.h"
#include "Binaural.h"

/// Repeat a short test file moving in circles around the horizontal plane

void test_Binaural_horiz() {

    char folder[CSL_NAME_LEN];                       // create HRTF data location
    sprintf(folder, "%sIRCAM_HRTF/512_DB/HRTF_1047.dat", CGestalt::dataFolder().c_str());
    HRTFDatabase::Reload(folder);                    // Load the HRTF data
    HRTFDatabase::Database()->dump();
				// Open a mono soundfile
	SoundFile sndfile(CGestalt::dataFolder() + "splash_mono.aiff");
	sndfile.dump();
				// make the sound "Positionable"
	SpatialSource source(sndfile);
				// Create a spatializer.
	Spatializer panner(kBinaural);
				// Add the sound source to it
	panner.addSource(source);
				// loop to play transpositions
	logMsg("playing HRTF-spatialized rotating sound source (horizontal plane)...");
	theIO->setRoot(panner);							// make some sound

	for (float i = 0.0f; i < 30.0f; i++) {
		source.setPositionP(i * 24.0f, 0.0f, 3.0f);	// rotate in small steps
		source.dump();
		sndfile.trigger();
		sleepSec(0.8);
	}
    sleepSec(0.8);
	theIO->clearRoot();
	logMsg("done.");
}

/// Repeat a short test file moving in circles around the vertical plane at AZ = CSL_PIHALF 
/// (axial plane in line with your ears, easy to localize)

void test_Binaural_vertAxial() {
				// Open a mono soundfile
	SoundFile sndfile(CGestalt::dataFolder() + "guanno_mono.aiff");
	sndfile.dump();

    char folder[CSL_NAME_LEN];                        // create HRTF data location
    strcpy(folder, CGestalt::dataFolder().c_str());    // CSL data folder location
    strcat(folder, "IRCAM_HRTF/512_DB/HRTF_1047.dat");    // HRTF data location
    HRTFDatabase::Reload(folder);                    // Load the HRTF data
				// make the sound "Positionable"
	SpatialSource source(sndfile);
				// Create a spatializer.
	Spatializer panner(kBinaural);
				// Add the sound source to it
	panner.addSource(source);
				// loop to play transpositions
	logMsg("playing HRTF-spatialized rotating sound source (vertical plane)...");
	theIO->setRoot(panner);							// make some sound
	for (int i = 30; i > 6; i--) {
		source.setPosition('s', CSL_PIHALF, (float) (i * 15), 2.0f);	// rotate in small steps
		source.dump();
		sndfile.trigger();
		sleepSec(0.8);
	}
	theIO->clearRoot();
	logMsg("done.");
}

/// Repeat a short test file moving in circles around the vertical plane at AZ = 0 
/// (median plane between your ears, hard to localize)

void test_Binaural_vertMedian() {
				// Open a mono soundfile
	SoundFile sndfile(CGestalt::dataFolder() + "triangle_mono.aiff");
	sndfile.openForRead(true);
	sndfile.dump();
	sndfile.setToEnd();

    char folder[CSL_NAME_LEN];                          // create HRTF data location
    strcpy(folder, CGestalt::dataFolder().c_str());     // CSL data folder location
    strcat(folder, "IRCAM_HRTF/512_DB/HRTF_1047.dat");  // HRTF data location
    HRTFDatabase::Reload(folder);                       // Load the HRTF data
				// make the sound "Positionable"
	SpatialSource source(sndfile);
				// Create a spatializer.
	Spatializer panner(kBinaural);
				// Add the sound source to it
	panner.addSource(source);
				// loop to play transpositions
	logMsg("playing HRTF-spatialized rotating sound source (medial plane)...");
	theIO->setRoot(panner);							// make some sound
	for (int i = 30; i > 6; i--) {
		source.setPosition('s', 0.0f, (float) (i * 15), 2.0f);	// rotate in small steps
		source.dump();
		sndfile.trigger();
		sleepSec(0.8);
	}
	theIO->clearRoot();
	logMsg("done.");
}

/// Spatializer with Ambisonics

void test_Ambi_horiz() {
				// Open a mono soundfile
	SoundFile sndfile(CGestalt::dataFolder() + "triangle_mono.aiff");
	sndfile.dump();
                // make the sound "Positionable"
	SpatialSource source(sndfile);
				// Create a spatializer.
	Spatializer panner(kAmbisonic);
				// Add the sound source to it
	panner.addSource(source);
				// loop to play transpositions
	logMsg("playing Ambisonic-spatialized rotating sound source (horizontal plane)...");
	theIO->setRoot(panner);					// make some sound
	for (float i = 0.0f; i < 30.0f; i++) {
		source.setPositionP(i * 24.0f, 0.0f, 3.0f);	// rotate in small steps
		source.dump();
		sndfile.trigger();
		sleepSec(0.8);
	}
	theIO->clearRoot();
	logMsg("done.");
}

/// Spatializer with simple panners & filters and/or reverb

void test_SimpleP() {
				// Open a mono soundfile
	SoundFile sndfile(CGestalt::dataFolder() + "Piano_A5_mf_mono.aiff");
	sndfile.dump();
				// make the sound "Positionable"
	SpatialSource source(sndfile);
				// Create a "simple" spatializer.
	Spatializer panner(kSimple);
				// Add the sound source to it
	panner.addSource(source);
				// loop to play transpositions
	logMsg("playing simply spatialized rotating sound source...");
	Mixer mix(2);
	mix.addInput(panner);
	theIO->setRoot(mix);					// make some sound
	for (float i = 0.0f; i < 30.0f; i++) {
				// rotate in small steps, getting farther away
		source.setPositionP(i * 24.0f, 0.0f, (2.0f + (i * 0.75f)));
		source.dump();
		sndfile.trigger();
		sleepSec(0.9);
	}
	theIO->clearRoot();
	logMsg("done.");
}

/// Spatializer with VBAP

void test_VBAP_horiz() {
		// Open a mono soundfile
	SoundFile sndfile(CGestalt::dataFolder() + "triangle_mono.aiff");
	sndfile.openForRead(true);
	sndfile.dump();
	sndfile.setToEnd();
		// make the sound "Positionable"
	SpatialSource source(sndfile);
		// Create a spatializer.
	Spatializer panner(kVBAP, SpeakerLayout::defaultSpeakerLayout());
		// Add the sound source to it
	panner.addSource(source);
		// loop to play transpositions
	logMsg("playing VBAP-spatialized rotating sound source (horizontal plane)...");
	theIO->setRoot(panner);							// make some sound
	for (int i = 0; i < 30; i++) {
		source.setPosition('s', (float) (i * 24), 0.0f, 2.0f);	// rotate in small steps
		source.dump();
		sndfile.trigger();
		sleepSec(0.8);
	}
	theIO->clearRoot();
	logMsg("done.");
}

/// NEW (1905) Convolver

//#define IR_FILE "impulse.aiff"
//#define IR_FILE "impulse2.aiff"
//#define IR_FILE "Large3.3sCathedral_mono.aiff"
//#define IR_FILE "5.6s_Gothic Church.aiff"		// stereo IR
#define IR_FILE "7.0s_Cathedral C.aiff"

// 400-tap 800-1200 Hz BPF

float test_IR[400] = {
	0.000115321, 3.6737e-05, 4.1587e-05, 4.60227e-05, 4.98864e-05, 5.29159e-05, 5.49895e-05, 5.58942e-05, 5.55457e-05,
	5.36663e-05, 5.01198e-05, 4.46702e-05, 3.75496e-05, 2.84695e-05, 1.72398e-05, 4.32139e-06, -1.05814e-05, -2.70865e-05,
	-4.5164e-05, -6.44729e-05, -8.47828e-05, -0.000105662, -0.0001268, -0.00014773, -0.000168014, -0.000187043, -0.000204465,
	-0.00021964, -0.000232086, -0.000241318, -0.000246827, -0.000248233, -0.000245146, -0.000237276, -0.000224392, -0.0002064,
	-0.000183281, -0.000155108, -0.000122088, -8.45941e-05, -4.30158e-05, 2.01944e-06, 4.98645e-05, 9.97062e-05, 0.000150644,
	0.000201698, 0.000251824, 0.000299918, 0.000344833, 0.000385437, 0.000420621, 0.000449288, 0.000470436, 0.000483164,
	0.000486657, 0.000480307, 0.000463611, 0.000436301, 0.00039829, 0.000349742, 0.000291022, 0.000222748, 0.000145778,
	6.12089e-05, -2.96601e-05, -0.000125284, -0.000223972, -0.000323836, -0.000422867, -0.000518974, -0.000609991, -0.000693752,
	-0.000768108, -0.000831012, -0.000880526, -0.000914892, -0.000932579, -0.000932317, -0.00091314, -0.000874445, -0.000815971,
	-0.000737887, -0.000640763, -0.000525601, -0.000393824, -0.000247279, -8.82151e-05, 8.07491e-05, 0.000256652, 0.000436239,
	0.000616023, 0.000792325, 0.000961359, 0.00111931, 0.0012624, 0.00138695, 0.00148951, 0.00156691, 0.00161633, 0.0016354,
	0.00162226, 0.00157561, 0.00149478, 0.00137979, 0.00123132, 0.00105081, 0.000840386, 0.000602914, 0.000341936, 6.16378e-05,
	-0.000233194, -0.000537263, -0.000844827, -0.00114981, -0.0014459, -0.00172668, -0.00198573, -0.00221678, -0.00241383,
	-0.00257131, -0.00268415, -0.00274799, -0.00275922, -0.00271515, -0.00261406, -0.00245529, -0.00223935, -0.00196787,
	-0.00164369, -0.00127085, -0.000854537, -0.000401045, 8.22805e-05, 0.000587166, 0.00110451, 0.00162453, 0.00213693,
	0.00263108, 0.00309619, 0.00352153, 0.00389664, 0.0042115, 0.00445684, 0.00462426, 0.00470647, 0.00469748, 0.00459276,
	0.00438943, 0.00408636, 0.00368426, 0.00318581, 0.00259565, 0.00192042, 0.00116873, 0.000351074, -0.000520217, -0.00143115,
	-0.00236624, -0.00330864, -0.00424048, -0.00514303, -0.00599699, -0.00678285, -0.00748112, -0.00807269, -0.00853916,
	-0.00886316, -0.00902861, -0.00902113, -0.00882822, -0.00843959, -0.00784736, -0.00704628, -0.00603386, -0.00481056,
	-0.0033798, -0.00174805, 7.5225e-05, 0.00207758, 0.00424374, 0.0065557, 0.00899298, 0.0115328, 0.0141505, 0.0168196,
	0.0195123, 0.0222, 0.0248534, 0.0274431, 0.0299398, 0.032315, 0.0345414, 0.0365927, 0.0384449, 0.040076, 0.0414664,
	0.0425994, 0.0434614, 0.044042, 0.044334, 0.044334, 0.044042, 0.0434614, 0.0425994, 0.0414664, 0.040076, 0.0384449,
	0.0365927, 0.0345414, 0.032315, 0.0299398, 0.0274431, 0.0248534, 0.0222, 0.0195123, 0.0168196, 0.0141505, 0.0115328,
	0.00899298, 0.0065557, 0.00424374, 0.00207758, 7.5225e-05, -0.00174805, -0.0033798, -0.00481056, -0.00603386, -0.00704628,
	-0.00784736, -0.00843959, -0.00882822, -0.00902113, -0.00902861, -0.00886316, -0.00853916, -0.00807269, -0.00748112,
	-0.00678285, -0.00599699, -0.00514303, -0.00424048, -0.00330864, -0.00236624, -0.00143115, -0.000520217, 0.000351074,
	0.00116873, 0.00192042, 0.00259565, 0.00318581, 0.00368426, 0.00408636, 0.00438943, 0.00459276, 0.00469748, 0.00470647,
	0.00462426, 0.00445684, 0.0042115, 0.00389664, 0.00352153, 0.00309619, 0.00263108, 0.00213693, 0.00162453, 0.00110451,
	0.000587166, 8.22805e-05, -0.000401045, -0.000854537, -0.00127085, -0.00164369, -0.00196787, -0.00223935, -0.00245529,
	-0.00261406, -0.00271515, -0.00275922, -0.00274799, -0.00268415, -0.00257131, -0.00241383, -0.00221678, -0.00198573,
	-0.00172668, -0.0014459, -0.00114981, -0.000844827, -0.000537263, -0.000233194, 6.16378e-05, 0.000341936, 0.000602914,
	0.000840386, 0.00105081, 0.00123132, 0.00137979, 0.00149478, 0.00157561, 0.00162226, 0.0016354, 0.00161633, 0.00156691,
	0.00148951, 0.00138695, 0.0012624, 0.00111931, 0.000961359, 0.000792325, 0.000616023, 0.000436239, 0.000256652, 8.07491e-05,
	-8.82151e-05, -0.000247279, -0.000393824, -0.000525601, -0.000640763, -0.000737887, -0.000815971, -0.000874445, -0.00091314,
	-0.000932317, -0.000932579, -0.000914892, -0.000880526, -0.000831012, -0.000768108, -0.000693752, -0.000609991, -0.000518974,
	-0.000422867, -0.000323836, -0.000223972, -0.000125284, -2.96601e-05, 6.12089e-05, 0.000145778, 0.000222748, 0.000291022,
	0.000349742, 0.00039829, 0.000436301, 0.000463611, 0.000480307, 0.000486657, 0.000483164, 0.000470436, 0.000449288,
	0.000420621, 0.000385437, 0.000344833, 0.000299918, 0.000251824, 0.000201698, 0.000150644, 9.97062e-05, 4.98645e-05,
	2.01944e-06, -4.30158e-05, -8.45941e-05, -0.000122088, -0.000155108, -0.000183281, -0.0002064, -0.000224392, -0.000237276,
	-0.000245146, -0.000248233, -0.000246827, -0.000241318, -0.000232086, -0.00021964, -0.000204465, -0.000187043, -0.000168014,
	-0.00014773, -0.0001268, -0.000105662, -8.47828e-05, -6.44729e-05, -4.5164e-05, -2.70865e-05, -1.05814e-05, 4.32139e-06,
	1.72398e-05, 2.84695e-05, 3.75496e-05, 4.46702e-05, 5.01198e-05, 5.36663e-05, 5.55457e-05, 5.58942e-05, 5.49895e-05,
	5.29159e-05, 4.98864e-05, 4.60227e-05, 4.1587e-05, 3.6737e-05, 0.000115321
};

#define USE_SND_FILES

void test_convolution() {	// Make dual-mono soundfile chain
							// drum rim shot as input
	JSoundFile * sndfile1 = JSoundFile::openSndfile(string(CGestalt::dataFolder() + "rim3_L.aiff"));
	JSoundFile * sndfile2 = JSoundFile::openSndfile(string(CGestalt::dataFolder() + "rim3_L.aiff"));
    MulOp smul1(*sndfile1, 10.0);
    MulOp smul2(*sndfile2, 10.0);
#ifdef USE_SND_FILES		// read IR from a file - long reverb tail
    char infn[CSL_DEF_LEN];
    sprintf(infn, "%s%s", CGestalt::dataFolder().c_str(), IR_FILE);
//    Convolver2(UnitGenerator & in, char * inFName, unsigned chan = 0, unsigned len = 512, bool norm = false);
	Convolver2 conv1(smul1, &infn[0], 0, CGestalt::blockSize() * 2, true);
	Convolver2 conv2(smul2, &infn[0], 1, CGestalt::blockSize() * 2, true);
#else						// use a block of data (above) as the IR - narrow BP filter
	Convolver2 conv1(*sndfile1, test_IR, 400, CGestalt::blockSize() * 2, true);
	Convolver2 conv2(*sndfile2, test_IR, 400, CGestalt::blockSize() * 2, true);
#endif
	Joiner join(conv1, conv2);						// mono-to-stereo joiner
													// loop to play transpositions
	logMsg("playing convolved sound source (5 times)...");
	theIO->setRoot(join);							// make some sound
	for (int i = 0; i < 5; i++) {				    // play a long time...
		sndfile1->trigger();
		sndfile2->trigger();
		sleepSec(6.0);
	}
	theIO->clearRoot();
	logMsg("done.");
	delete sndfile1;
	delete sndfile2;
}

// ------------------------------------

// helper fcn to load data from IR files - loads all the data for now - ToDo: load on-demand

float * load_filter_data(char * fname, unsigned & siz) {
	char * pos = strrchr(fname, '.');
	if ( ! pos) {
		logMsg(kLogError, "Can't parse filter file name %s   ", fname);
		return 0;
	}
	pos++;
	if (strcmp(pos, "wav") == 0) {				// 32-bit float WAV file
		JSoundFile * inFile = JSoundFile::openSndfile(string(fname));
		unsigned frames = inFile->duration();	// # of frames in IR
		if ( ! frames) {
			logMsg(kLogFatal, "Can't find IR file %s", fname);
		}
		inFile->mWavetable.mDidIAllocateBuffers = false;
		SampleBuffer bfr = inFile->buffer(0);
		siz = frames;
		return bfr;
	}
	FILE * inF = fopen(fname, "rb");
	fseek(inF, 0L, SEEK_END);
	unsigned sz = (unsigned) ftell(inF);
	rewind(inF);
	if (strcmp(pos, "ir") == 0) {				// raw data with text header
		unsigned len, rat;
		char lin[CSL_WORD_LEN];
		fgets(lin, CSL_WORD_LEN, inF);			// read/parse the simple header
		int got = sscanf(lin, "%d %d", & len, & rat);
		if (got != 2) {
			logMsg(kLogError, "Can't read filter file header %s   ", fname);
			return 0;
		}
		SampleBuffer bfr;
		SAFE_MALLOC(bfr, sample, len);
		unsigned d0 = (unsigned) ftell(inF);
		fread(bfr, sizeof(float), len, inF);	// read float data
		fclose(inF);
		siz = len;
		return bfr;
	}
	if (strcmp(pos, "bin") == 0) {				// raw data
		unsigned len = sz / sizeof(float);
		SampleBuffer bfr;
		SAFE_MALLOC(bfr, sample, len);
		fread(bfr, sizeof(float), len, inF);	// read float data
		fclose(inF);
		siz = len;
		return bfr;
	}
	logMsg(kLogError, "Can't parse filter file name %s   ", fname);
	return 0;
}

// test of IR file normalization

void test_convolution_file(float * data, unsigned len) {	// Make dual-mono soundfile chain
							// drum rim shot as input
	PinkNoise noise1(20.0f);					// the sound source
	PinkNoise noise2(20.0f);					// the sound source
	Joiner join(noise1, noise2);				// mono-to-stereo joiner
//	Convolver2 conv1(noise1, data, len, CGestalt::blockSize() * 2, true);
//	Convolver2 conv2(noise2, data, len, CGestalt::blockSize() * 2, true);
//	Joiner join(conv1, conv2);					// mono-to-stereo joiner
												// loop to play transpositions
	logMsg("playing convolved sound source...");
	theIO->capture_on(2.2);
	theIO->setRoot(join);							// make some sound
	sleepSec(2.0);
	theIO->clearRoot();
	Buffer * bfr = theIO->get_capture();
	float rms = bfr->rms(0, 0, theIO->mOffset);
	theIO->capture_off();
	logMsg("done: RMS = %g", rms);
}

void test_convolution_files() {
	unsigned len;
	float * data;
	data = load_filter_data("/Content/Code/RoCoCo/App/RoCoCo/Source/IR_data/akg_k240s_44.wav", len);
	test_convolution_file(data, len);
	data = load_filter_data("/Content/Code/RoCoCo/App/RoCoCo/Source/IR_data/BD_DT880_44.wav", len);
	test_convolution_file(data, len);
	data = load_filter_data("/Content/Code/RoCoCo/App/RoCoCo/Source/IR_data/bp250_44.wav", len);
	test_convolution_file(data, len);
	data = load_filter_data("/Content/Code/RoCoCo/App/RoCoCo/Source/IR_data/bp250b_44.wav", len);
	test_convolution_file(data, len);
	data = load_filter_data("/Content/Code/RoCoCo/App/RoCoCo/Source/IR_data/bp1500_44.wav", len);
	test_convolution_file(data, len);
	data = load_filter_data("/Content/Code/RoCoCo/App/RoCoCo/Source/IR_data/bp1500b_44.wav", len);
	test_convolution_file(data, len);
}

//	0.00410895
//	0.0182053
//	0.0191255
//	0.0114013
//	0.101001
//	0.125591

//#endif // CSL_WINDOWS

//////// RUN_TESTS Function ////////

#ifndef USE_JUCE

void runTests() {
//	testPan();
//	testN2MPan();
//	testSineMixer();
//	testPanMix();
//	testConvolver();
//	testConvolver2();
//	testConvolver3();
//	testOscBank();
//	testCMapIO();
	test_Binaural_horiz();
//	test_Binaural_vertAxial();
//	test_Binaural_vertMedian();
}

#else

// test list for Juce GUI

testStruct panTestList[] = {
	"Stereo panner",		testPan,				"Demonstrate the stero panner",
//	"N2M panner",			testN2MPan,	
	"Mixer",				testSineMixer,			"Mixer with 4 sine inputs (slow sum-of-sines)",
	"Panning mixer",		testPanMix,				"Play a panning stereo mixer",
	"Bigger panning mixer",	testBigPanMix,			"Test a mixer with many inputs",
#ifdef USE_CONVOLVER_OLD
	"Test convolver",		testConvolver,			"Test a convolver",
	"Test convolver 2",		testConvolver2,			"Test a convolver",
	"Test convolver 3",		testConvolver3,			"Test a convolver",
#endif
	"Osc bank",				testOscBank,			"Mix a bank of oscillators",
//	"Channel-mapped IO (buggy)",	 testCMapIO,	"Demonstrate channel-mapped IO",
    "HRTF horiz circles (buggy)",    test_Binaural_horiz,       "Test the HRTF-based binaural panner",
    "HRTF axial circles (buggy)",    test_Binaural_vertAxial,   "Play a HRTF-panner with axial circles",
    "HRTF median circles (buggy)",   test_Binaural_vertMedian,  "Play a HRTF-panner with median circles",
	"Ambisonics",			test_Ambi_horiz,		"Test the Ambisonic-based spatial panner",
	"Simple",				test_SimpleP,			"Test the simple spatial panner",
	"VBAP (buggy)",			test_VBAP_horiz,		"Test the VBAP-based spatial panner",
	"Convolver",			test_convolution,		"Test a convolver",
//	"Convolver Norm",		test_convolution_files,	"Test IR normalization",
NULL,						NULL,			NULL
};

#endif

