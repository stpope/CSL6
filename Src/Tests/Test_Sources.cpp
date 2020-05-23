//
//	Test_Sources.cpp -- C main functions for CSL noise and other source tests.
//	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
//
// This program simply reads the run_tests() function (at the bottom of this file)
//  and executes a list of basic CSL source tests
// There are other source tests in Test_Oscillators.cpp and Test_Envelopes.cpp
//

#ifndef USE_JUCE
#define USE_TEST_MAIN			// use the main() function in test_support.h
#include "Test_Support.cpp"		// include all of CSL core and the test support functions
#else
#include "Test_Support.h"
#endif

/////////////////////// Here are the actual unit tests ////////////////////

/// Noise tests, WhiteNoise & PinkNoise - using Scalable to protect our ears

void testNoises() {
	WhiteNoise whitey(0.5);		// amplitude, offset
//	logMsg("dumping quiet white noise...");
//	dumpTest(whitey);
	logMsg("playing quiet white noise...");
	runTest(whitey);
	logMsg("quiet white noise done.");

	PinkNoise pinky(42, 0.8);	// seed, amplitude, offset
//	logMsg("dumping quiet pink noise with seed...");
//	dumpTest(pinky);
	logMsg("playing quiet pink noise with seed...");
	runTest(pinky);
	logMsg("quiet pink noise with seed done.");
}

/// Plucked string simulation

void testString() {
	KarplusString pluck(fRandM(80, 800));			// rand freq range
	pluck.setScale(0.6);							// loud
	pluck.trigger();								// srart it
	logMsg("playing plucked string...");
	runTest(pluck, 5);
	logMsg("done.");
}

/// Test a chorus of strings -- this demo plays an endless loop of string arpeggii.
/// For each arpeggio, it picks a starting pitch and pitch step, a starting position
/// and position increment, and a duration; the inner loop then creates notes and sleeps.

void testStringChorus() {
	unsigned numStrings = 64;						// # strings
	Mixer mix(2);									// stereo mix
	UGenVector strings;								// vector of strings
	UGenVector pans;								// vector of panners
	
	for (int i = 0; i < numStrings; i++) {			// loop to create strings/panners
		KarplusString * plk = new KarplusString();	// create string sources
		plk->setScale(40.0 / (float) numStrings);	// make them loud
		strings.push_back(plk);						// add strings to the vector of strings
		Panner * pan = new Panner(*plk);			// create stereo panners on the strings
		pans.push_back(pan);						// add panners to the vector
		mix.addInput(*pan);							// add panners to the mixer
	}
	theIO->setRoot(mix);							// send mix to IO
	logMsg("playing 64-string chorus...");
	unsigned cnt = 0;								// string cnt
	
	while(1) {										// loop for string arpeggio phrases
		int numN = iRandM(3, 10);					// pick # notes to play
		int pit1 = iRandM(30, 84);					// pick start pitch (in MIDI)
		int pitX = iRandM(1, 4);					// pick pitch step
		if (coin())									// pick step up or down
			pitX = 0 - pitX;
		float pos1 = fRandZ();						// pick starting pos (0 - 1)
		if (pos1 == 0)
			pos1 = 0.1;
		pos1 = sqrtf(pos1);							// sqrt of starting pos (moves it away from 0)
		if (coin())									// pick pos L or R
			pos1 = 0.0f - pos1;
		float posX = (pos1 > 0) 					// calc pan step
					? ((0.0f - ((pos1 + 1.0f) / (float)numN))) 
					: ((1.0f - pos1) / (float)numN);
		float dela = fRandM(0.08, 0.25);			// calc delay
		logMsg("n: %d  p: %d/%d  \tx: %5.2f/%5.2f  \td:%5.2f", numN, pit1, pitX, pos1, posX, dela);
													// note loop
		for (int i = 0; i < numN; i++) {			// loop to create string gliss
//			logMsg("\t\ts: %d  p: %d  x: %5.2f", cnt, pit1, pos1);
													// set freq to MIDI pitch
			((KarplusString *)strings[cnt])->setFrequency(keyToFreq(pit1));
			pit1 += pitX;
													// set pan
			((Panner *)pans[cnt])->setPosition(pos1);
			pos1 += posX;
													// trigger
			((KarplusString *)strings[cnt])->trigger();
			if (sleepSec(dela))						// sleep
				goto done;							// exit if sleep was interrupted
			cnt++;									// pick next string
			if (cnt == numStrings)					// should I check ifthe string is still active?
				cnt = 0;							// reset string counter
		}
		sleepSec(fRandM(0.02, 0.2));				// sleep a bit between sets
	}
done:
	logMsg("done.");
	for (int i = 0; i < numStrings; i++) {			// loop to create strings/panners
		delete strings[i];
		delete pans[i];
	}
}

///////////////// SoundFile tests ////////

/// Test the sound file player - mono, stereo input files

void testMonoFilePlayer() {
//	SoundFile sfile(CGestalt::dataFolder(), "Piano_A5_mf_mono.aiff");	// open a piano note file
	SoundFile sfile(CGestalt::dataFolder() + "MKG1a1b.aiff");
//	SoundFile sfile(CGestalt::dataFolder(), "Piano_A5_mf.caf");			// play a piano note
//	SoundFile sfile(CGestalt::dataFolder(), "splash_mono.aiff");

	sfile.dump();														// print snd file info

//	float * left = sfile.mWavetable.buffer(0);						    // dump the first few samples
//	for (unsigned j = 0; j < 1000; j += 4)								// 0 to 1000 by 4
//		printf("\t\t%5.3f\n", left[j]);

	logMsg("Playing sound file...");
	sfile.trigger();
	runTest(sfile);
//	runTest(pan);
	logMsg("done.");
}

void testStereoFilePlayer() {
	SoundFile sfile(CGestalt::dataFolder() + "piano-tones.aiff");		// load a piano note
	sfile.dump();														// print snd file info

//	float * left = sfile.mSampleBuffer.buffer(0);					    // dump the first few samples
//	float * rite = sfile.mSampleBuffer.buffer(1);
//	for (unsigned j = 0; j < 1000; j++)
//		printf("\t\t%5.3f : %5.3f\n", *left++, *rite++);

	logMsg("Playing sound file...");
	sfile.trigger();
	runTest(sfile);
	logMsg("done.");
}

#ifdef USE_MP3			// uses libsndfile for now

/// Test the MP3 file reader

void testMP3FilePlayer() {
	MP3File sfile(CGestalt::dataFolder(), "Piano_B4_096.mp3", true);	// convert and play an MP3 file
//	MP3File sfile("/Users/stp/Code/FMAK/music/Rock/Hard/LZ/02 - The Rain Song.mp3", true);
	fi.log();							// print snd file info
	logMsg("Playing sound file...");
	fi.trigger();
	runTest(sfile);
	logMsg("done.");
}

#endif

/// Test the sound file player with rate shift

void testSoundFileTranspose() {
	SoundFile * sfile = new SoundFile(CGestalt::dataFolder() + "whistle_mono.aiff");
	sfile->dump();							// print snd file info
	Panner pan(*sfile, 0.0);				// a panner
	logMsg("playing sound file...");
	float rate = 0.8;
	theIO->setRoot(pan);					// make some sound
	for (int i = 0; i < 4; i++) {			// loop to play transpositions
		sfile->setRate(rate);
		sfile->trigger();
		sleepSec(1.4);						// sleep for the desired duration
		sfile->setToEnd();
		rate += 0.15;
	}
	theIO->clearRoot();						// make some silence
	logMsg("done.");
	delete sfile;
}

/// Test the WaveShaper

void testWaveShaper() {
	WaveShaper wvs(110, 0);					// wave-shape 1 = clipping
	ADSR adsr(3.0, 1, 1, 0.7, 1);
	wvs.setScale(adsr);
    MulOp mul(wvs, 0.4);                    // using a MulOp with a constant
	logMsg("Playing WaveShaper 1");
	adsr.trigger();
	runTest(mul, 3);
	logMsg("done.");
    
	WaveShaper wv2(110, 2);					// wave-shape 1 = clipping
	ADSR ads2(3.0, 1, 1, 0.7, 1);
	wv2.setScale(ads2);
    MulOp mu2(wv2, 0.4);                    // using a MulOp with a constant
	logMsg("Playing WaveShaper 2");
	ads2.trigger();
	runTest(mu2, 3);
	logMsg("done.");
    
	WaveShaper wv3(110, 1);					// wave-shape 1 = clipping
	ADSR ads3(3.0, 1, 1, 0.7, 1);
	wv3.setScale(ads3);
    MulOp mu3(wv3, 0.4);                    // using a MulOp with a constant
	logMsg("Playing WaveShaper 3");
	ads3.trigger();
	runTest(mu3, 3);
	logMsg("done.");
}

///////////////// Instrument tests ////////

/// Test basic FM instrument

#include <BasicFMInstrument.h>

void testFMInstrument() {
	FMInstrument * vox = new FMInstrument;
	logMsg("Playing simple fm instrument...");
	float dur = 6.0;
	float m_freq = 160.0;
	float * dPtr = & dur;
	float * fPtr = & m_freq;
	vox->setParameter(set_duration_f, 1, (void **) &dPtr, "f");
	vox->setParameter(set_index_f, 1, (void **) &fPtr, "f");
	vox->setParameter(set_m_freq_f, 1, (void **) &fPtr, "f");
	vox->play();
	runTest(*vox, dur);
	logMsg("done.");
	delete vox;
}

/// Test SOS instrument - play 3 different timbres

void testSOSInstrument() {
	AdditiveInstrument * vox1 = new AdditiveInstrument(48, 1.75f);
	AdditiveInstrument * vox2 = new AdditiveInstrument(48, 1.75f);
	AdditiveInstrument * vox3 = new AdditiveInstrument(48, 1.75f);
	logMsg("Playing 3 SOS instruments...");
    vox1->playMIDI(1.0f, 1, 30, 80);        // MIDI msg: dur, chan, key, vel
	runTest(*vox1, 1.0f);
	vox2->playMIDI(1.0f, 1, 30, 80);
	runTest(*vox2, 1.0f);
	vox3->playMIDI(1.0f, 1, 30, 80);
	runTest(*vox3, 1.0f);
	vox1->playMIDI(1.0f, 1, 30, 80);
	runTest(*vox1, 1.0f);
	vox2->playMIDI(1.0f, 1, 30, 80);
	runTest(*vox2, 1.0f);
	vox3->playMIDI(1.0f, 1, 30, 80);
	runTest(*vox3, 1.0f);
	logMsg("done.");
	delete vox1;
	delete vox2;
	delete vox3;
}

void testFancyFMInstrument() {
	FancyFMInstrument * vox = new FancyFMInstrument;
	logMsg("Playing fancy fm instrument...");
	float dur = 6.0;
	float * dPtr = & dur;
    float cfreq = 220.0;
    float mfreq = 260.0;
    float indF = 560.0;
    float * cPtr = & cfreq;
    float * mPtr = & mfreq;
    float * iPtr = & indF;
	vox->setParameter(set_duration_f, 1, (void **) &dPtr, "f");
    vox->setParameter(set_index_f, 1, (void **) &iPtr, "f");
    vox->setParameter(set_c_freq_f, 1, (void **) &cPtr, "f");
    vox->setParameter(set_m_freq_f, 1, (void **) &mPtr, "f");
	vox->play();
	runTest(*vox, dur);
	logMsg("done.");
	delete vox;
}

/// Test SoundFile instrument

#include <SndFileInstrument.h>

void testSndFileInstrument() {
	SndFileInstrument0 vox(CGestalt::dataFolder(), "round.aiff");
	logMsg("Playing sound file instrument...");
	vox.play();
	runTest(vox);
	logMsg("done.");
}

// Add a sound file to the given UGenVector and Mixer, add a panner on it to the panner UGenVector

void addSndtoBank(const char * nam, UGenVector &snds, UGenVector &pans, Mixer &mix) {
	SoundFile * sfile = new SoundFile(CGestalt::dataFolder() + nam);
	snds.push_back(sfile);						// add strings to the vector of strings
	Panner * pan = new Panner(*sfile);			// create stereo panners on the strings
	pans.push_back(pan);						// add panners to the vector
	mix.addInput(*pan);							// add panners to the mixer
}

// Create a bank of 64 sample players using the above function
// run a loop playing stuttering sound samlpes

void testSndFileBank() {
	unsigned numSounds = 64;				// # sound files
	Mixer mix(2);							// stereo mix
	UGenVector snds;						// vector of sound files
	UGenVector pans;						// vector of panners
	
	for (int i = 0; i < numSounds / 4; i++) {	// loop to set up snd file bank
		addSndtoBank("Columbia-44_1.aiff", snds, pans, mix);
		addSndtoBank("MKG1a1b.aiff", snds, pans, mix);
		addSndtoBank("round.aiff", snds, pans, mix);
		addSndtoBank("sns.aiff", snds, pans, mix);
	}
	Stereoverb reverb(mix);					// stereo reverb
	reverb.setRoomSize(0.98);				// long reverb time
	theIO->setRoot(reverb);					// send mix to IO
	logMsg("playing sound file forest...");
	
	while (1) {								// loop for snd files
		int numN = iRandM(3, 8);			// pick # notes to play
		int which = iRandM(0, 4);			// pick which snd
		int base = which * numSounds / 4;
											// pick start point
		SoundFile * snd = ((SoundFile *)snds[base]);
		int startp = iRandV(snd->duration());
		float dela = fRandM(0.05, 0.15);	// calc delay
		int dur = (int)(fRandM(dela, dela * 4.0f) * (float)CGestalt::frameRate());	// calc dur

		logMsg("n: %d  s: %d  fr: %d  \tdu: %d", numN, which, startp, dur);
											// note loop
		for (int i = 0; i < numN; i++) {	// loop to create snd file "stutter"
			snd = ((SoundFile *)snds[which + numN]);
			snd->setStart(startp);
			snd->setStop(startp + dur);
											// set pan to rand
			((Panner *)pans[which + numN])->setPosition(fRand1());
			snd->trigger();					// trigger
			if (sleepSec(dela))				// sleep
				goto done;					// exit if sleep was interrupted
		}
	}
done:
	logMsg("done.");
	for (int i = 0; i < numSounds; i++) {	// loop to create strings/panners
		delete snds[i];
		delete pans[i];
	}
}

// Play a grain cloud

//#ifndef CSL_WINDOWS

#include <Granulator.h>

void testGrainCloud() {
	GrainCloud cloud;						// grain cloud
	GrainPlayer player(& cloud);			// grain player
											// open and read in a file for granulation
	SoundFile sndFile(CGestalt::dataFolder() + "MKG1a1b.aiff");
	sndFile.dump();

	cloud.mSamples = sndFile.mWavetable.buffer(0);
	cloud.numSamples = sndFile.duration();
	cloud.mRateBase = 1.0f;					// set the grain cloud parameters
	cloud.mRateRange = 0.8f;
	cloud.mOffsetBase = 0.5f;
	cloud.mOffsetRange = 0.5f;
	cloud.mDurationBase = 0.15f;
	cloud.mDurationRange = 0.12f;
	cloud.mDensityBase = 16.0f;
	cloud.mDensityRange = 10.0f;
	cloud.mWidthBase = 0.0f;
	cloud.mWidthRange = 1.0f;
	cloud.mVolumeBase = 8.0f;
	cloud.mVolumeRange = 20.5f;
	cloud.mEnvelopeBase = 0.5f;
	cloud.mEnvelopeRange = 0.49f;
	logMsg("playing Granular cloud.");
	cloud.startThreads();					// start the grain create/reap threads
	runTest(player, 15);
	logMsg("done.");
	cloud.isPlaying = false;
	sleepSec(0.5);
}

//#endif

///////////////// IFFT tests ////////

#define prt_ifft(b, m, p)                        \
    { vox.binValueMagPhase(b, &m, &p);           \
    logMsg("\tBn %d = %.4f @ %.4f", b, m, p); }

void test_ifft() {
	IFFT vox(CGestalt::blockSize() /* * 2 */ );		// use default IFFT parameters
	vox.setBinMagPhase(2, 0.25, 0);				    // set a few harmonics (not in-phase)
	vox.setBinMagPhase(4, 0.08, 0);
	vox.setBinMagPhase(6, 0.04, 0);
	vox.setBinMagPhase(8, 0.02, 0);
//	vox.setBinMagPhase(5, 0.1, 0);				    // 5th bin = 440 Hz?
    float mag, phs;
//    for (int i = 0; i < 10; i++)
//        prt_ifft(i, mag, phs)
	logMsg("playing IFFT...");
	runTest(vox);
	logMsg("IFFT done.");
}

/// do vector cross-fade the long way

void test_vector_ifft() {
	float dur = 8.0f;
	IFFT vox1, vox2;							    // create ifft players
	
	vox1.setBinMagPhase(4, 0.25, 0);			    // set different harmonics
	vox1.setBinMagPhase(6, 0.25, 0);
	LineSegment env1(dur, 0.5, 0.0001);
	MulOp mul1(vox1, env1);						    // use a MulOp

	vox2.setBinMagPhase(5, 0.25, 0);			    // for the 2nd IFFT
	vox2.setBinMagPhase(9, 0.25, 0);
	LineSegment env2(dur, 0.0001, 0.5);
	MulOp mul2(vox2, env2);

	AddOp add(mul1, mul2);						    // sum the MulOps
    ADSR adsr(dur, 0.02, 0.04, 0.4, 1);
    MulOp summ(add, adsr);
    adsr.trigger();
    
	logMsg("playing IFFT crossfade...");
	runTest(summ, dur);
	logMsg("IFFT crossfade done.");
}

//////// RUN_TESTS Function ////////

#ifndef USE_JUCE

void runTests() {
//	testNoises();
//	testString();
//	testMonoSoundFilePlayer();
//	testStereoSoundFilePlayer();
//	testMP3FilePlayer();
//	testSoundFileTranspose();
//	testFMInstrument();
//	testSndFileInstrument();
//	testFancyFMInstrument();
	test_ifft();
//	test_vector_ifft();
}

#else

// test list for Juce GUI

testStruct srcTestList[] = {
	"Noise tests",				testNoises,				"Test noise generators",
	"Plucked string",			testString,				"Waves of string arpeggii, stereo with reverb",
	"Random string melodies",	testStringChorus,		"Many random string arpeggii",
	"Mono snd file player",		testMonoFilePlayer,		"Test playing a sound file",
	"Stereo snd file player",	testStereoFilePlayer,	"Play a stereo sound file",
#ifdef USE_MP3	
	"MP3 Snd file player",		testMP3FilePlayer,		"Play an MP3 file",
#endif
	"Snd file transpose",		testSoundFileTranspose,	"Demonstrate transposing a sound file",
	"Sample file bank",			testSndFileBank,		"Play a large sample bank from sound files",
	"FM instrument",			testFMInstrument,		"Play the basic FM instrument",
	"Fancy FM instrument",		testFancyFMInstrument,	"FM note with attack chiff and vibrato",
	"SumOfSines instrument",	testSOSInstrument,		"Demonstrate the SumOfSines instrument",
	"Snd file instrument",		testSndFileInstrument,	"Test the sound file instrument",
	"WaveShaping synthesis",	testWaveShaper,			"Play 2 wave-shaper notes with envelopes",
    "IFFT synthesis",           test_ifft,              "Make a sound with IFFT synthesis",
    "Vector IFFT",              test_vector_ifft,       "Vector synthesis with 2 IFFTs",
	"Soundfile granulation",	testGrainCloud,			"Random sound file granulation example",
	NULL,						NULL,					NULL
};

#endif
