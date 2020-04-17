//
//	Test_all.cpp -- the simplest test program main function
//	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
//
//		NOTE: This is CSL3 code, and has not been ported to CSL4.
//		See the separate test suites (Test_Oscillators, etc) for the CSL4 tests.
//
// This program simple reads the run_tests() function below and executes a list of basic CSL tests
//
// Note: We don't normally use this file, but copy subsets of the tests from here
// to the Test_main.cpp or some other file for use as main().

#include "Test_Support.cpp"		// include all of CSL core and the test support functions

void run_tests();

////// MAIN //////

int main (int argc, const char * argv[]) {
	logMsg("Running CSL3 tests");
	fflush(stdout);
	READ_CSL_OPTS;					// read the standard CSL options
	gIO->open();
	gIO->start();
	run_tests();
	gIO->stop();
	gIO->close();	
	exit(0);
}

/////////////////////// Here are the actual unit tests ////////////////////

// Simplest sine wave

void test_sin() {
	Sine vox(220);
	logMsg("playing sin...");
	run_test(vox, 3.0);
	logMsg("sin done.");
}

// Scaled sine wave -- 3 methods: with a MulOp and using the Sine's scale input

void test_scaled_sin() {
	Sine vox(220);
	vox.set_scale(0.1);			// simplest: scale the sine directly
	logMsg("playing quiet sin...");
	run_test(vox);
	logMsg("quiet sin done.");

	Sine vox2(220);
	MulOp mul(vox2, 0.1);		// using a MulOp with a constant
	logMsg("playing quiet sin...");
	run_test(mul);
	logMsg("quiet sin done.");
	
	Sine vox3(220);
	StaticVariable var(0.1);		// using a MulOp with a StaticVariable
	MulOp mul2(vox3, var);
	logMsg("playing quiet sin...");
	run_test(mul2);
	logMsg("quiet sin done.");
}

// AM using a MulOp and the dynamic scale

void test_am_sin() {
	Sine vox(220), mod(6);
	MulOp mul(vox, mod);		// AM with MulOp
	logMsg("playing AM sin...");
	run_test(mul);
	logMsg("AM sin done.");
	
	Sine vox2(220), mod2(6);
	vox2.set_scale(mod2);		// AM using set_scale()
	logMsg("playing AM sin...");
	run_test(vox2);
	logMsg("AM sin done.");
}

// Apply various envelopes to a glissando sine with a LineSegment

void test_envelopes() {
	Sine vox;
	LineSegment line(3, 600, 100);
	vox.set_frequency(line);
//	ADSR env(3, 0.1, 0.1, 0.25, 1.5);			// sharp ADSR
//	AR env(3, 1, 1.5);					// gentle AR
	Triangle env(3, 1); 					// 3-sec triangle
	env.dump();
	vox.set_scale(env);
	logMsg("playing gliss sin with envelope...");
	env.trigger();
	run_test(vox);
	logMsg("gliss sin done.");
}

// Sine FM -- verbose and compact methods

void test_fm_sin() {
	Sine vox, mod;
	vox.set_frequency(261);
	mod.set_frequency(261);
	DynamicVariable fm(250, mod);
	StaticVariable offset(261);
	AddOp adder(offset, fm);
	vox.set_frequency(adder);
	StaticVariable vol(0.1);
	MulOp mul(vox, vol);
	logMsg("playing FM sin...");
	run_test(mul);
	logMsg("FM sin done.");
}

// Compact -- only 2 UGens

void test_fm_sin2() {
	Sine vox(261), mod(261);
	mod.set_scale(240);
	mod.set_offset(261);
	vox.set_frequency(mod);
	vox.set_scale(0.1);
	logMsg("playing FM 2 sin...");
	run_test(vox);
	logMsg("FM sin done.");
}

// Glissando sine with a LineSegment

void test_gliss_sin() {
	Sine vox;
	LineSegment line(3, 100, 800);
	vox.set_frequency(line);
	StaticVariable vol(0.1);
	MulOp mul(vox, vol);
	logMsg("playing gliss sin...");
	run_test(mul);
	logMsg("gliss sin done.");
}

// Sawtooth test

void test_saw() {
	Sawtooth vox(220);
	vox.set_scale(0.1);
	logMsg("playing sawtooth...");
	run_test(vox);
	logMsg("sin done.");
}

// Square wave test

void test_square() {
	Square vox(220);
	vox.set_scale(0.1);
	logMsg("playing square wave...");
	run_test(vox);
	logMsg("square done.");
}


// Band-limited Square wave test

void test_squareBL() {
	SquareBL vox(220);
	vox.set_scale(0.1);
	logMsg("playing squareBL wave...");
	run_test(vox);
	logMsg("squareBL done.");
}


// Play a simple wavetable (a sine), then read a buffer from a sound file to use as a wavetable

void test_wavetable() {
	WavetableOscillator vox(220);
	logMsg("playing sine wavetable...");
	run_test(vox);
	logMsg("wavetable done.");
								// now open a test file, read a buffer, and copy that into the wavetable
	SoundFile fi("../../Data/oo_table.aiff");		// a cycle of the vowel "oo" from "moon"
	fi.open_for_read();
	unsigned frames = fi.duration();
	fi.read_buffer_from_file(frames);
	WavetableOscillator vox2(fi._sampleBuffer, frames);		// create a wavetable reader on the sample read in form the file
	vox2.set_frequency(110);
	logMsg("playing file wavetable...");
	run_test(vox2);
	logMsg("wavetable done.");
}

///////////////// IFFT tests /////////////////////////////

void test_ifft() {
	logMsg("playing IFFT...");
	IFFT vox;
	vox.set_bin_mag_phase(1, 0.5, 0);
	vox.set_bin_mag_phase(3, 0.25, 0);
	vox.set_bin_mag_phase(5, 0.05, 0);
	vox.set_bin_mag_phase(9, 0.01, 0);
	run_test(vox);
	logMsg("IFFT done.");
}

void test_long_ifft() {
	logMsg("playing long IFFT...");
	IFFT vox(2048, kFFTMeasure);			// put in an explicit block resizer here
	vox.set_bin_mag_phase(4, 0.5, 0);
	vox.set_bin_mag_phase(12, 0.25, 0);
	vox.set_bin_mag_phase(20, 0.05, 0);
	vox.set_bin_mag_phase(36, 0.01, 0);
	run_test(vox);
	logMsg("long IFFT done.");
}

void test_vector_ifft() {
	logMsg("playing IFFT crossfade...");
	IFFT vox1, vox2;

	vox1.set_bin_mag_phase(2, 0.25, 0);
	vox1.set_bin_mag_phase(4, 0.25, 0);
	vox2.set_bin_mag_phase(6, 0.25, 0);
	vox2.set_bin_mag_phase(8, 0.25, 0);
	LineSegment env1(3, 1, 0);	// fade out
	LineSegment env2(3, 0, 1);	// fade in
	MulOp mul1(vox1, env1);	
	MulOp mul2(vox2, env2);
	AddOp add3(mul1, mul2);	
	run_test(add3);
	logMsg("IFFT crossfade done.");
}

////////// Convolution
#if 0
void test_convolver() {
//	csl::CGestalt::set_block_size(2048);
	Convolver cv("../../Data/Quadraverb_large_L.aiff");
	ThreadedReader tfs(cv);
	SoundFile fi("../../Data/rim3_L.aiff");
	fi.open_for_read();
	logMsg("playing sound file...");
	fi.trigger();
	run_test(fi);
	logMsg("sound file player done.");	
	cv.set_input(fi);
	fi.trigger();
	logMsg("playing convolver...");
//	tfs.set_threshold(2048);
//	tfs.start();
	run_test(cv);
//	tfs.stop();
	fi.close();
	logMsg("convolver done.");
}
#endif

/*****

void test_HRTF() {

	HRTF_Dictionary * dict = new HRTF_Dictionary(1024, "/Users/stp/MAT/240C/Code/IRCAM_HRIR/IRC_1022/COMPENSATED/WAV/IRC_1022_C/CONTENTS");
	HRTF_Convolver * cv = new HRTF_Convolver(dict);
	SampleFile fi("../../rim3_L.aiff");
	SamplePlayer pl(&fi);
	cv->set_input(&pl);
	cv->set_position(15, 15);
	logMsg("playing convolver...");
	pl.trigger();
	run_test(cv, 10);
	logMsg("convolver done.");
	delete(dict);
	delete(cv);
}

*********/

//////////////////////// Envelope tests ////////////////////////////////////////////

void test_envelope() {
	Sine vox(220);
	float duration = 3.0;	// time
	float attack = 0.06;		// time
	float decay = 0.1;		// time
	float sustain = 0.1;		// value
	float release = 1.5;		// time
	ADSR adsr(duration, attack, decay, sustain, release);
	MulOp mul(vox, adsr);
	logMsg("playing sin ADSR envelope.");
	adsr.trigger();	
	run_test(mul);
	logMsg("sin ADSR evelope done.");
	
	Sine vox2(220);
	ADSR adsr2(duration, attack, decay, sustain, release);
	vox2.set_scale(adsr2);
	logMsg("playing sin ADSR envelope.");
//	adsr.dump();
	adsr2.trigger();	
	run_test(vox2);
	logMsg("sin ADSR evelope done.");
}

// Test an envelope applied to frequency

void test_frequency_envelope() {
	Sine vox;
	Envelope env(3, 0, 220, 0.7, 280, 1.3, 180, 2.0, 200, 3, 1000);
	vox.set_frequency(env);
	logMsg("playing sin envelope on frequency");
	env.trigger();	
	run_test(vox);
	logMsg("sin envelope on frequency done.");
}

// Fancy multi-envelope FM example -- verbose version: 2 sines, 2 ADSRs, 1 variable and 3 operators

void test_env_fm_sin() {
	ADSR a_env(3, 0.1, 0.1, 0.3, 1);				// amplitude env= std ADSR
	Envelope i_env(3, 0, 0, 0.1, 2, 0.2, 1, 2.5, 4, 3, 0);	// index env = fancier
	Sine vox, mod;						// declare 2 oscillators
	mod.set_frequency(110);					// set modulator freq
	DynamicVariable var(110, i_env, kOpTimes);		// multiply index envelope by mod freq
	MulOp i_mul(mod, var);					// scale the modulator by its envelope
	AddOp adder(i_mul, 220.0);				// add in the modulation
	vox.set_frequency(adder);					// set the carrier's frequency
	MulOp a_mul(vox, a_env);					// scale the carrier's output by the amplitude envelope
	logMsg("playing fancy FM sin...");
	a_env.trigger();						// reset the envelopes to time 0
	i_env.trigger();	
	run_test(a_mul); 						// run the test to play a note on the root of the DSP graph
	logMsg("FM sin done.");
}

// Compact version of the above -- 2 sines and 2 ADSRs

void test_env_fm_sin2() {
	Sine vox, mod(110);					// declare 2 oscillators
	ADSR a_env(3, 0.1, 0.1, 0.3, 1);				// amplitude env = std ADSR
	Envelope i_env(3, 0, 0, 0.1, 2, 0.2, 1, 2.5, 4, 3, 0);	// index env = fancier
	i_env.scale_values(110);					// multiply index envelope by mod freq
	mod.set_scale(i_env);					// scale the modulator by its envelope
	mod.set_offset(220);					// add in the base freq
	vox.set_frequency(mod);					// set the carrier's frequency
	vox.set_scale(a_env);					// scale the carrier's output by the amplitude envelope
	logMsg("playing fancy FM sin...");
	a_env.trigger();						// reset the envelopes to time 0
	i_env.trigger();	
	run_test(vox); 						// run the test to play a note on the root of the DSP graph
	logMsg("FM sin done.");
}

//////////////////////// Panner/mixer tests ////////////////////////////////////////////

// Test a panning sine wave; once l/r in 3 sec, then with a 2 Hz LFO

void test_panning_sin() {
	Sine vox(330);
	MulOp mul(vox, 0.2);
	LineSegment lin(6, -1, 1);		// L-to-R sweep over 6 sec
	Panner pan(mul, lin);
	logMsg("playing panning sin 1...");
	run_test(pan, 6.0);
	logMsg("panning sin done.");
	Sine pos(0.5);			// LFO panning
	Panner pan2(mul, pos);
	logMsg("playing panning sin 2...");
	run_test(pan2, 5.0);
	logMsg("panning sin done.");
}

// Test mixer -- really slow additive synthesis

void test_mixer_with_sines() {
	Sine vox1(431);				// create 4 scaled sine waves
	MulOp mul1(vox1, 0.3);
	Sine vox2(540);
	MulOp mul2(vox2, 0.1);
	Sine vox3(890);
	MulOp mul3(vox3, 0.3);
	Sine vox4(1280);
	MulOp mul4(vox4, 0.01);
	Mixer mix(2);				// create a stereo mixer
	mix.add_input(mul1);			// add them to the mixer
	mix.add_input(mul2);
	mix.add_input(mul3);
	mix.add_input(mul4);
	logMsg("playing mix of 4 sines...");
	run_test(mix);
	logMsg("mix done.");
}

// Panning mix -- create 4 scaled sine waves with panner LFOs at different frequencies

void test_panning_sine_mix() {
	Sine vox1(431);				// sine osc
	Sine pos1(0.5);				// sine LFO for panner
//	Panner pan1(&vox1, -1.0, 0.1);	// scaling panner
	Panner pan1(vox1, pos1, 0.1);		// scaling panner
	Sine vox2(170);
	Sine pos2(0.3);
//	Panner pan2(&vox2, -0.3, 0.1);
	Panner pan2(vox2, pos2, 0.1);
	Sine vox3(267);
	Sine pos3(0.4);
//	Panner pan3(&vox3, 0.3, 0.1);
	Panner pan3(vox3, pos3, 0.1);
	Sine vox4(224);
	Sine pos4(0.7);
//	Panner pan4(&vox4, 1.0, 0.1);
	Panner pan4(vox4, pos4, 0.1);
	Mixer mix(2);				// create a stereo mixer
	mix.add_input(pan1);			// add the panners to the mixer
	mix.add_input(pan2);
	mix.add_input(pan3);
	mix.add_input(pan4);
	logMsg("playing panning mix of 4 sines...");
	run_test(mix, 9);
	logMsg("mix done.");
}

//////////////////////// Noise and Filter tests ////////////////////////////////////////////

void test_white_noise() {
	WhiteNoise vox;
//	Sine scale(2.0);
//	vox.set_scale(scale);
	logMsg("playing White Noise...");
	run_test(vox);
	logMsg("White Noise done.");
}

void test_pink_noise() {
	PinkNoise vox;
//	Sine scale(2.0);
//	vox.set_scale(scale);
	logMsg("playing Pink Noise...");
	run_test(vox);
	logMsg("Pink Noise done.");
}

void test_bw_bandpass() {
	WhiteNoise wnoise;
	Butter bpfilterW(wnoise, wnoise.sample_rate(), Butter::BW_BAND_PASS, 500, 200);
	logMsg("testing BP filtered white noise (noise first)...");
	bpfilterW.dump ();
	run_test (wnoise);
	logMsg("white noise thru BP 500/200 Butterworth filter");
	run_test (bpfilterW);
	PinkNoise pnoise (22222);
	Butter bpfilterP (pnoise, pnoise.sample_rate(), Butter::BW_BAND_PASS, 500, 200);
	bpfilterP.dump ();
	logMsg("now pink noise...");
	run_test (pnoise);
	logMsg("now pink noise thru BP 500/200 filter...");
	run_test (bpfilterP);
	logMsg("BP filter test done.");
}

void test_filter_sweep() {		
	WhiteNoise wnoise;
	Sine centerSin;
	centerSin.set_frequency(0.5);
	MulOp centerMod(centerSin, 500.0);
	StaticVariable centerOffset(1000);
	AddOp centerSweep(centerMod, 1000.0);
	Sine BWSin;
	BWSin.set_frequency(5);
	MulOp BWMod(BWSin, 50.0);
	AddOp BWSweep(BWMod, 100.0);
	Butter lpfilter(wnoise, wnoise.sample_rate(), Butter::BW_BAND_PASS, centerSweep, BWSweep);
	logMsg("playing filter_sweep...");
	run_test(lpfilter);
	logMsg("filter_sweep done.");
}

void test_moog() {
	WhiteNoise osc;
	osc.set_scale(0.2);
//	Sawtooth osc(400);
//	LineSegment cutoffSweep(5, 2000.0, 200.0);
	Sine cutoffSweep(1);
	cutoffSweep.set_scale(2000);
	cutoffSweep.set_offset(3000);
	Moog lpfilter(osc, osc.sample_rate(), cutoffSweep);
//	Moog lpfilter(osc, osc.sample_rate(), 500.0);
//	lpfilter.set_resonance(cutoffSweep);
	logMsg("playing moog filtered noise...");
	run_test(lpfilter, 5);
	logMsg("moog done.");
}

void test_formant() {
//	WhiteNoise osc;
	Sawtooth osc;
	osc.set_frequency(400);
	Sine sweep;
	sweep.set_frequency(1);
	sweep.set_scale(200);
	sweep.set_offset(600);
//	LineSegment cutoffSweep(3, 2000.0, 200.0);
//	StaticVariable cutoffSweep(2000);
	Formant lpfilter(osc, osc.sample_rate(), sweep, 0.995F );
//	Formant lpfilter(osc, osc.sample_rate(), 600, 0.995F );
//	lpfilter.set_normalize(false);
//	Filter lpfilter(osc);
	logMsg("playing Formant...");
	run_test(lpfilter, 10);
	logMsg("Formant done.");
}

void test_allpass() {
//	WhiteNoise osc;
	Square osc, osc2;
	osc.set_frequency(400);
	osc2.set_frequency(400);

//	LineSegment cutoffSweep(3, 2000.0, 200.0);
	Sine coeff(1);
	coeff.set_scale(0.9);
//	coeff.set_offset(0.5);
	Allpass apfilter(osc, osc.sample_rate(), coeff);
//	Filter lpfilter(&osc);
	AddOp out(apfilter, osc2);
	logMsg("CSL playing Allpass...\n");
	run_test(out,20);
	logMsg("CSL Allpass done.");
}


void test_notch() {
// 	WhiteNoise osc;
	Sine osc(600);
//	LineSegment cutoffSweep(3, 2000.0, 200.0);
	Sine sweep;
	sweep.set_frequency(1);
	sweep.set_scale(550);
	sweep.set_offset(600);
	Notch lpfilter(osc, osc.sample_rate(), sweep, 0.99995F );
	logMsg("playing band-reject (notch) filter...");
	run_test(lpfilter);
	logMsg("notch test done.");
}

// Test the FIR filter on a sample tap data file;
// the file format is: 1st line = num_taps; following lines = tap data (1 float per line)

void test_FIR() {
	PinkNoise noise;
	FIR vox(noise, "../../Data/test.flt");
	logMsg("playing noise...");
	run_test(noise);
	logMsg("noise done.");	
	logMsg("playing FIR filter...");
	run_test(vox);
	logMsg("FIR done.");	
}

// FilterSpecification example -- Simple LPF (2 bands) at 500 Hz with 300 Hz transition band

void test_FIR2() {
	PinkNoise noise;
	double resp[2] = { 1, 0 };				// these are the amplitudes in the 2 freq bands (i.e., lo-pass
	double freq[4] = { 0, 500, 800, 22050 };	// these are the corner freqs of the pass, transition, and stop bands
	double weight[2] = { 10, 20 };			// these are the weights for error (ripple) in the 2 bands
	FilterSpecification fs(64, 2, freq, resp, weight);	// 64 taps (64-step IR), 2 bands
	fs.plan_filter();
	FIR vox(noise, fs);
	logMsg("playing noise...");
	run_test(noise);
	logMsg("noise done.");	
	logMsg("playing FIR filter...");
	run_test(vox);
	logMsg("FIR done.");	
}

//Take stereo input from soundcard and send it to the output (only send left channel to both channel outs)
//Note: will need to modify PAIO setting to take input
void test_InOut() {
	logMsg("playing InOut...");
	InOutFlags f = kLtoM;
	InOut inout(f);
	gIO->set_root(inout);
	gIO->open();
	gIO->start();
	csl::sleep_usec(10000000);	//sleep_usec 10 secs
	gIO->stop();
	gIO->close();
	logMsg("InOut done.");
}

#if 0

//Ouput several random values to the screen
void test_RandomVariable() {
	Buffer b1;
	Buffer b2;
	b1.set_size(csl::CGestalt::num_in_channels(), csl::CGestalt::block_size());
	b1.allocate_mono_buffers();	
	b2.set_size(csl::CGestalt::num_out_channels(), csl::CGestalt::block_size());
	b2.allocate_mono_buffers();	

	b1.zero_buffers();
	b2.zero_buffers();

	//RandomVariable * r = new RandomVariable(kExponential,0,1,0,0);
	//RandomVariable * r = new RandomVariable(kGamma,0,1,1.75,0);
	RandomVariable * r = new RandomVariable(kNormal,0,1,0,0);
	sample t;
	for(int i=0;i<10;i++) {
		t = r->next_sample(b1,b2);
		std::cout << t << endl;
	}
	csl::sleep_usec(10000000);

	//end
	delete r;
	b1.free_mono_buffers();
	b2.free_mono_buffers();
}

//Make a granulator which 
//Function also tests Clipper

void test_Granulator() {
	StaticVariable wave(1);		//not implemented yet
	StaticVariable env(1);		//not implemented yet
	StaticVariable amp(0.2);
	StaticVariable dur(0.07);
	StaticVariable pan(0);
	
	Envelope rate(11,0,30,11,70);
	rate.trigger();
	RandomVariable freq_in1(kNormal, 500, 200, 0, 0);
	Clipper freq(freq_in1, kBoth, 100, 12000);

	Granulator gran(wave, env, rate, amp, freq, dur, pan);

	logMsg("playing granulator...");
	gIO->set_root(gran);
	gIO->open();
	//csl::sleep_usec(2000000);	
	std::cout << "Starting" << endl;
#ifdef RT_IO
	gIO->start();
	csl::sleep_usec(10000000);
#else
	//NOTE: REGULAR START FUNCTION DOESN"T SEEM TO WORK FOR FILEIO
	gIO->start(10);
#endif
	std::cout << "Stopping" << endl;
	gIO->stop();
	std::cout << "Closing" << endl;
	gIO->close();
	std::cout << "Total grains produced 1: " << gran.num_grains << endl;

	logMsg("granulator done.");

	csl::sleep_usec(5000000);
}
#endif

// Basic SHARC test -- play middle c on a trumpet using a SHARC spectrum and a SumOfSines oscillator
#ifndef CSL_WINDOWS

void test_SHARC() {
	SumOfSines vox(65);
	SHARCLibrary * lib;
	SHARCSpectrum * spect;
	lib = new SHARCLibrary("../../sharc");
//	lib->dump_stats();
	spect = lib->spectrum_named("tuba", "c2");
//	spect = lib->spectrum_named("Bach_trumpet", "c4");
//	spect->dump_example();
	
	vox.set_scale(0.1);
	
	vox.add_partials(spect->_num_partials, spect->_partials);
	vox.create_cache();			// cache the waveform (since it's a harmonic overtone spectrum)
	Triangle tri(3, 1); 			
	vox.set_scale(tri);
	logMsg("playing SumOfSines (%d partials)...", spect->_num_partials);
	tri.trigger();
	run_test(vox, 3);			// play a whole note
	logMsg("SumOfSines done.");
}

// Create a cross-fade of 4 SHARC spectra

void test_SHARC2() {
	SumOfSines vox1(196), vox2(196), vox3(196), vox4(196), vox5(196);
	SHARC_Library * lib;
	SHARC_Spectrum * sp1, * sp2, * sp3, * sp4, * sp5;
	lib = new SHARC_Library("../../../Code/sharc");
							// access a spectrum, load it into a SumOfSines, and scale it by a triangle envelope
	sp1 = lib->spectrum_named("altoflute_vibrato", "g3");	
	vox1.add_partials(csl_min(sp1->_num_partials, 20), sp1->_partials);
	vox1.create_cache();			// cache the waveform (since it's a harmonic overtone spectrum)
	Triangle tr1(3, 0.75); 			
	vox1.set_scale(tr1);
							// repeat for 3 other delayed examples
	sp2 = lib->spectrum_named("bass_clarinet", "g3");	
	vox2.add_partials(csl_min(sp2->_num_partials, 20), sp2->_partials);
	vox2.create_cache();			// cache the waveform
	Triangle tr2(3, 0.75); 		
	vox2.set_scale(tr2);
	sp3 = lib->spectrum_named("cello_vibrato", "g3");	
	vox3.add_partials(csl_min(sp3->_num_partials, 20), sp3->_partials);
	vox3.create_cache();			// cache the waveform
	Triangle tr3(3, 0.75); 		
	vox3.set_scale(tr3);
	sp4 = lib->spectrum_named("trombone", "g3");	
	vox4.add_partials(csl_min(sp4->_num_partials, 20), sp4->_partials);
	vox4.create_cache();			// cache the waveform
	Triangle tr4(3, 0.75); 		
	vox4.set_scale(tr4);
	sp5 = lib->spectrum_named("violin_martele", "g3");	
	vox5.add_partials(csl_min(sp4->_num_partials, 20), sp4->_partials);
	vox5.create_cache();			// cache the waveform
	Triangle tr5(3, 0.75); 		
	vox5.set_scale(tr5);

	Mixer mix(2);		// create a stereo mixer
	mix.add_input(vox1);	mix.add_input(vox2); 	
	mix.add_input(vox3); 	mix.add_input(vox4); 	mix.add_input(vox5);
	
	logMsg("playing SumOfSines mix...");	// I don't use run_test() here because I need to trigger the envelopes while it's playing	
	gIO->set_root(mix);			// turn it on
	tr1.trigger();				// trigger the 1st envelope
	usleep(1500000);			// wait 1.5 sec
	tr2.trigger();				// trigger the 2nd envelope
	usleep(1500000);			// wait 1.5 sec
	tr3.trigger();				// trigger the 3rd envelope
	usleep(1500000);			// wait 1.5 sec
	tr4.trigger();				// trigger the 4th envelope
	usleep(1500000);			// wait 1.5 sec
	tr5.trigger();				// trigger the 5th envelope
	usleep(3000000);			// wait 3 seconds
	gIO->clear_root();			// turn it off
	logMsg("SumOfSines done.");
}
#endif

// Test the sound file player

void test_sound_file_player() {
	SoundFile fi("../../Data/round.snd");
	fi.open_for_read();
	logMsg("playing stereo sound file...");
	printf("\tFile \"%s\" SR: %d #CH: %d #FR: %d\n", fi._path, fi.rate(), fi.channels(), fi.duration());
	fflush(stdout);
	run_test(fi);
	logMsg("sound file player done.");

}

// Test basic FM instrument

#include <BasicFMInstrument.h>

void test_basic_fm_instrument() {
	BasicFMInstrument vox;
	logMsg("Playing fm instrument...");
	float dur = 10.0;
	vox.set_parameter(set_duration_f, (void *) &dur);
	vox.play();
	run_test(vox, 10.0);
	logMsg("basic fm done.");
}

void test_logging() {
	logMsg("Testing Logging");
	logMsg(kLogInfo, "Testing Logging Int %i", 10);
	logMsg(kLogInfo, "Testing Logging Float %i %f", 10, 10.01);	
	logMsg(kLogWarning, "Testing Logging String %s", "a string");	
	logMsg(kLogError, "Testing Logging hex %x", 0x0101010);
}

#if 0

/*
 * Test the FDN class using sawtooth as input
 */
#include "FDN.h"
#define NUM_DELAYLINES 3
#define ALPHA 0.99
#define DIFFUSION 0.4
void test_fdn() {
	sample off_diag = ALPHA*(DIFFUSION / (NUM_DELAYLINES - 1));
	sample diag = ALPHA*(1-(off_diag*(NUM_DELAYLINES - 1)));
	Impulse imp;

	//set up parameters to FDN
	unsigned int delayLineLengths[NUM_DELAYLINES] = {781, 873, 1189};
	sample InputGainVector[NUM_DELAYLINES] = {1, 1, 1};
	sample OutputGainVector[NUM_DELAYLINES] = {0.5, 0.5, 0.5};
	sample FeedbackMatrix[NUM_DELAYLINES][NUM_DELAYLINES] = 
		{ {diag, off_diag, off_diag}, {off_diag, diag, off_diag}, {off_diag, off_diag, diag} }; //{ {0.32, 0.23, 0.345}, {0.1, 0.9, 1}, {0.5, 0.4, 0.2} }; 
	sample FeedbackGainVector[NUM_DELAYLINES] = {1, 1, 1};

	FDN myNetwork = FDN(imp, delayLineLengths, NUM_DELAYLINES, InputGainVector, OutputGainVector,
			(sample *)FeedbackMatrix, FeedbackGainVector);


	logMsg("CSL playing FDN...");
    run_test(myNetwork);

	logMsg("CSL done.");

}

/*
 * Chebyschev coefficients evaluation
 * Evaluate Chebyschev coefficients of orders listed in chebyHarmonics
 * Tk(x) is the k-th harmonic
 *
 * T0(x) = 1
 * T1(x) = x
 * T2(x) = 2x^2 - 1
 * T3(x) = 4x^3 - 3x
 * T4(x) = 8x^4 - 8x^2 + 1
 * ...
 */
void EvaluateCheby(sample * chebyCoefficients, int * chebyHarmonics, 
					 sample * chebyWeights, unsigned numHarmonics, unsigned maxHarmonic ) {
	sample * prevT = new sample[maxHarmonic+1];
	sample * currT = new sample[maxHarmonic+1];
	sample * tempT = new sample[maxHarmonic+1];
	sample * tempPtr;

	//Reset coefficient array
	for (unsigned i = 0; i <= maxHarmonic; i++) {
		chebyCoefficients[i] = 0;
		prevT[i] = 0;
		currT[i] = 0;
	}

	//For each Harmonic, calculate Cheby coefficients and store into coefficient
	// array. Use the recursive formula Tk+1(x) = 2xTk(x) - Tk-1(x)
	for (unsigned i = 0; i< numHarmonics; i++) {
		prevT[0] = 1;  //T0
		currT[1] = 1;  //T1
		if (chebyHarmonics[i] == 0) {
			chebyCoefficients[0] += chebyWeights[i] * 1;
		}
		else if (chebyHarmonics[i] == 1) {
			chebyCoefficients[1] += chebyWeights[i] * 1;
		}
		else {
			for (unsigned j = 2; j <= chebyHarmonics[i]; j++) {
				memcpy(tempT+1, currT, maxHarmonic*sizeof(sample));
				tempT[0] = 0;
				//subtract prevT from 2*currT
				for (int k = 0; k <= maxHarmonic; k++) {
					tempT[k] *= 2;
					tempT[k] -= prevT[k];
				}
				tempPtr = prevT;
				prevT = currT;
				currT = tempT;
				tempT = tempPtr;
			}
			for (int k = 0; k <= maxHarmonic; k++) {
				chebyCoefficients[k] += chebyWeights[i] * currT[k];
			}
		}
	}
}

/*
 * Polynomial evaluation
 * Evaluate Polynomial
 */
typedef struct Polynomial {
	unsigned order;
	sample * coeff;
} Polynomial;

sample EvaluatePoly(sample input, Polynomial * f) {
	sample accumulator = f->coeff[0];
	float power = 1;

	for (unsigned i = 1; i <= f->order; i++) {
		power = power*input;
		accumulator += f->coeff[i] * power;
	}
	return accumulator;
}

/*
 * Dummy Waveshaping functions
 * y = 1.5x - 0.5x^3
 * y = x / (1 + |x|)
 * Chebyschev Polynomial
 */
sample simpleWaveShaperFxn(sample input, void * arg) {
	//return (input/(1 + abs(input)));
	//return (1.5f*input - 0.5f * pow(input,3));
	return (EvaluatePoly(input, (Polynomial *)arg));  
}

//#define clipit
sample linearWaveShaperFxn(sample input, void * arg) {
#ifdef clipit
	if (input <= -0.5) {
		return -0.5;
	}
	else if (input >= 0.5) {
		return 0.5;
	}
	else {
		return input;
	}
#else
	return input;
#endif
}

/*
 * Test VWaveShaper class with linear waveshaper
 */
#include "VWaveshaper.h"
#define USE_STATIC_WAVETABLE
void test_waveshaper() {
	Sine vox;					// declare oscillator
	ADSR env = ADSR(5, 0.5, 0.5, 0.5, 0.5); 

#ifndef USE_STATIC_TABLE
	VWaveShaper vox2(vox, NULL, linearWaveShaperFxn);
#else
	VWaveShaper vox2(vox, NULL, linearWaveShaperFxn, 1000);  //Using static table
#endif
	vox.set_frequency(200);			// set the carrier's frequency
	vox.set_scale(env);					// set the carrier's envelop	

	logMsg("CSL playing waveshaper...");

	env.trigger();
    run_test(vox2);

	logMsg("CSL done.");
}

/*
 * Test VWaveShaper class with Chebyschev waveshaping function
 */
#define NUMHARMONICS 4
#define MAXHARMONIC 10
void test_waveshaper2() {
	Sine vox;					// declare oscillator
	ADSR env = ADSR(5, 0.5, 0.5, 0.5, 0.5); 
	int chebyHarmonics[NUMHARMONICS] = {1, 2, 4,  MAXHARMONIC};		 //What harmonic content we want
	sample chebyWeights[NUMHARMONICS] = {1, 0.3, 0.17, 1/10};   //Weight of each harmonic
	sample chebyCoefficients[MAXHARMONIC + 1];	         //Array to store cheby coefficients
	Polynomial f = {MAXHARMONIC, chebyCoefficients};     //Descriptor for cheby coefficients

	EvaluateCheby(chebyCoefficients, chebyHarmonics, chebyWeights, NUMHARMONICS, 
		MAXHARMONIC);  //Produce the summation of Cheby polynomials corresponding to harmonics

#ifndef USE_STATIC_WAVETABLE
	VWaveShaper vox2(vox, (void *)&f, simpleWaveShaperFxn);
#else
	VWaveShaper vox2(vox, (void *)&f, simpleWaveShaperFxn, 1000); //use static table
#endif
	vox.set_frequency(200);					// set the carrier's frequency
	vox.set_scale(env);					// set the carrier's frequency	

	logMsg("CSL playing waveshaper...");

	env.trigger();

    run_test(vox2);
	logMsg("CSL done.");
}

#endif		// busted stuff?

/////////////////////////// This is the list of tests to run ////////////////////////////

void run_tests() {
//	test_sin();				// Basic waveform tests
//	test_scaled_sin();
//	test_am_sin();
//	test_fm_sin();			//Does not work correctly
//	test_fm_sin2();
//	test_gliss_sin();
//	test_saw();
//	test_square();
//	test_squareBL();		//Does not work correctly
//	test_wavetable();
//	test_sum_of_sines();
	
//	test_SHARC();		
 
//	test_ifft();			// IFFT Tests
//	test_long_ifft();		
//	test_vector_ifft();		

//	test_envelope();		// Envelopes and complex patches
//	test_frequency_envelope();
	test_env_fm_sin();

//	test_panning_sin();		// Panners and mixers
//	test_mixer_with_sines();
//	test_panning_sine_mix();

//	test_white_noise();		// Noise and filters
//	test_pink_noise();
//	test_bw_bandpass();
//	test_filter_sweep();	//Does not work correctly
//	test_moog();			
//	test_formant();
//	test_allpass();
//	test_notch();			
//	test_FIR();
//	test_FIR2();

//	test_InOut();
//	test_RandomVariable();
//	test_Granulator();

//	test_sound_file_player();
//	test_remote_server();
//	test_basic_fm_instrument();

//	test_logging();
//	test_convolver();
//	test_waveshaper();
//	test_waveshaper2();
//	test_fdn();

	exit(0);
}
