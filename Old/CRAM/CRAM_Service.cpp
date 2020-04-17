//
// CRAM_Service.cpp -- CREATE Real-time Applications Manager (CRAM) service wrapper code for using CSL as a CRAM-managed service
// See the copyright and acknowledgment of authors in the file COPYRIGHT
//
//  The CSLService class is the wrapper used to manage CSL programs from CRAM.
//  This file has the main() function that starts any of a number of different CSL services under the control of CRAM.
//
//	Usage:
//  CSL_Server service_name listener_port emergency_port CSL_function [-a] [-o UDP_IO-port] [-i RemoteFrameStream-in-IP/port]
// e.g.,
//  CSL_Server CSL31 12345 12355 bells				-- play bells sending sound out to the local server
//  CSL_Server CSL31 12345 12355 voices -o 33123	-- play voices listening for client requests on port 33123
//  CSL_Server CSL31 12345 12355 mixer -i 128.111.92.51 12445 -i 128.111.92.60 12445  -- mix stereo sample streams from the two given servers
//

#include "CRAM_Service.h"
#include <time.h>
#include <signal.h>
#include <iostream>
#include "CSL_All.h"		// include all of CSL core
#include "PAIO.h"
#include "UDP_IO.h"

#ifndef CSL_WINDOWS
#include "CAIO.h"
#include <unistd.h>			// for usleep
#endif

using namespace cram;
using namespace csl;
using namespace std;

IO * gIO = NULL;			// the global IO guy
vector <char *> hosts;		// lists of remote inputs for the mixer
vector <unsigned> ports;

// top-level C functions for the CSL players

extern "C" void * fm_bells(void * ptr);
extern "C" void * swept_noise(void * ptr);
extern "C" void * phonemes(void * ptr);
extern "C" void * mixer(void * ptr);

// The constructor just sets up the CRAM service

CSLService :: CSLService(char * name, unsigned short lport, unsigned short eport, char * which) 
			: Server(name, lport, eport) {
	mLog.logMsg("[CSL Server created]");
	mWhich = (char *) malloc(strlen(which) + 1);
	strcpy(mWhich, which);
	mAppStatus = CRAM_INIT;
	if ( ! strcmp(mWhich, "bells")) {
		mOptions.push_back(make_pair (string("density"), string("10")));
		mOptions.push_back(make_pair (string("reverb"), string("0.97")));
	} else if ( ! strcmp(mWhich, "noise")) {
		mOptions.push_back(make_pair (string("lo_frequency"), string("0.2")));
		mOptions.push_back(make_pair (string("hi_frequency"), string("0.35")));
	} else if ( ! strcmp(mWhich, "voices")) {
		mOptions.push_back(make_pair (string("period"), string("10")));
	}
}

CSLService :: ~CSLService() {
	free(mWhich);
}

// Class methods

// Start is the method that starts CSL playing

void CSLService :: start(void) {
	mLog.logMsg("[Starting CSL server %s]", mName);
	mAppStatus = CRAM_ON;
	int err;
					// Start up one of the player threads, depending on the value of the selector
	if ( ! strcmp(mWhich, "bells")) {
		err = pthread_create( & mThread, NULL, fm_bells, (void *) & mLog);
	} else if ( ! strcmp(mWhich, "noise")) {
		err = pthread_create( & mThread, NULL, swept_noise, (void *) & mLog);
	} else if ( ! strcmp(mWhich, "voices")) {
		err = pthread_create( & mThread, NULL, phonemes, (void *) & mLog);
	} else if ( ! strcmp(mWhich, "mixer")) {
		err = pthread_create( & mThread, NULL, mixer, (void *) & mLog);
	} else {
		err = -999;
	}
	if (err != 0 ) {
		mLog.logMsg(cram::kLogError, "Error starting CSL thread %d\n", err);
		mAppStatus = CRAM_FAILED;
	}
}

// Stop and exit

void CSLService :: stop(void) {
	mLog.logMsg(kLogImportant, "[Stopping service %s]", mName);
	gIO->clear_root();				// shut down the output
	gIO->stop();
	gIO->close();	
	mAppStatus = CRAM_OFF;
	pthread_kill(mThread, SIGINT);  // kill the CSL play thread 
	Server::stop();					// kill the main listener threads, causing main() to exit
}

void CSLService :: suspend(void) {		// suspend method
	mLog.logMsg("[Suspend service %s]", mName);
	mAppStatus = CRAM_SUSPENDED;
	gIO->stop();
}

void CSLService :: resume(void) {		// resume method
	mLog.logMsg("[Resums service %s]", mName);
	mAppStatus = CRAM_ON;
	gIO->start();
}

// Set CSL-specific options

void CSLService :: set_opt(const char * nam, const char * val) {
	int i_val;
	
	mLog.logMsg("[Configure %s = %s]", nam, val);
	if ( ! strcmp(nam, "period")) {
		i_val = atoi(val);
		if (i_val < 10)		i_val = 10;
		if (i_val > 600)	i_val = 600;
		return;
	}
	Program :: set_opt(nam, val);				// else hand it up to the superclass
}

//// MAIN ////

CSLService * server;

// Usage:
//		CSL_Server service_name listener_port emergency_port CSL_function [-a] [-o out-port] [-i in-ip in-port] ... [-i in-ip in-port]

int main(int argc, char * argv[]) {
	bool autoPlay = false;
	
	if (argc < 5) {
		printf("Usage: %s service_name listener_port emergency_port program-selector\n", argv[0]);
		exit(0);
	}					// argv 2 and 3 have to be the CRAM listener ports
	int lport = atoi(argv[2]);
	int eport = atoi(argv[3]);
	char * which = argv[4];
						// all remaining arguments are of the form [-o out-port] [-i in-ip in-port]
	if (argc > 5) {
		for (int i = 5; i < argc; i++) {
			if ( ! strcmp(argv[i], "-o")) {
				int outPort = atoi(argv[++i]);
				gIO = new UDP_IO;		// Socket-based IO
				gIO->open(outPort);
			} else if ( ! strcmp(argv[i], "-i")) {
				hosts.push_back(argv[++i]);
				ports.push_back(atoi(argv[++i]));
			} else if ( ! strcmp(argv[i], "-a")) {
				autoPlay = true;
			}
		}
	}							// Create the server instance
	if (gIO == NULL) {			// output port not specified
		gIO = new PAIO;			// PortAudio IO
		gIO->open(); 
	}
	server = new CSLService(argv[1], lport, eport, which);
	if (autoPlay) {				// if auto-play, start service
		server->start();
		while(true) 
			sleep_sec(1);
	}
								// start the listener and emergency threads
	if (server->start_threads() != 0) {
		printf("\t[%s: FATAL: Application start-up failed]\n", argv[1]);
	} else {					// suspend until emergency thread terminates
		printf("\t[%s: Server running]\n", argv[1]);
		pthread_join(server->mEThread, NULL);
		printf("\t[%s: Server terminated]\n", argv[1]);
	}
	delete server;
	printf("\t[%s: Server exiting]\n", argv[1]);
}

//// CSL Server DSP graph code

// Random FM bells panning L/R with loads of reverb

extern "C" void * fm_bells(void * ptr) {
	Logger * mLog = (Logger *) ptr;
	float frq;
	mLog->logMsg("\tplaying FM bells...");
										// Create the DSP graph
	ADSR a_env(1, 0.01, 0.1, 0.1, 0.6);		// amplitude env = std ADSR
	a_env.scale_values(0.25);
	ADSR i_env(1, 0.001, 0.1, 0.5, 0.5);	// index env
	Sine vox, mod(110);						// declare 2 oscillators
	DynamicVariable var(110, i_env);		// multiply index envelope by mod freq
	MulOp i_mul(mod, var);					// scale the modulator by its envelope
	AddOp adder(i_mul, 220.0);				// add in the modulation
	vox.set_frequency(adder);				// set the carrier's frequency
	MulOp a_mul(vox, a_env);				// scale the carrier's output by the amplitude envelope
	Sine pos(0.25);							// LFO panning
	Panner pan(a_mul, pos);
//	Stereoverb verb(pan);					// add some stereo reverb
//	gIO->set_root(verb);					// plug the graph into the output
	gIO->set_root(pan);					// plug the graph into the output
	gIO->start();							// start the IO
	while(true) {							// endless loop playing bells
		frq = 500 + (frandom() * 2000);		// pick a new base freq
		mod.set_frequency(frq * 1.414);		// plug the frequency into the graph
		var.set_value(frq);
		adder.set_operand(frq);
		a_env.trigger();					// trigger the envelopes
		i_env.trigger();	
		sleep_sec(1 + (frandom() * 1));		// sleep a few sec
	}										// ...never reached
}

// Swept noise instrument

extern "C" void * swept_noise(void * ptr) {
	PinkNoise wnoise;							// pink noise source
	StaticVariable bw(150);
							// lo-freq noise
	RandEnv center(0.2, 400.0, 600.0);			// frequency, amplitude, offset
	Butter filt(wnoise, wnoise.sample_rate(), Butter::BW_BAND_PASS, center, bw);
	filt.set_bandwidth(150.0);	
	RandEnv ampl(0.15, 0.2, 0.2);				// frequency, amplitude, offset
	MulOp mult(filt, ampl);
	RandEnv pos(0.1);			
	Panner pan(mult, pos);
							// hi-freq noise
	RandEnv center2(0.35, 1500.0, 8000.0);		// frequency, amplitude, offset
	Butter filt2(wnoise, wnoise.sample_rate(), Butter::BW_BAND_PASS, center2, bw);
	filt2.set_bandwidth(300.0);	
	RandEnv ampl2(0.2, 0.2, 0.2);				// frequency, amplitude, offset
	MulOp mult2(filt2, ampl2);
	RandEnv pos2(0.1);			
	Panner pan2(mult2, pos2);
							// mix the panners and send to stereo reverb
	AddOp add(pan, pan2);						// summer
//	Stereoverb verb(add);						// composite splitter/stereo-reverb/joiner class
//	verb.set_room_size(0.98);					// very large room
//	gIO->set_root(verb);						// plug the graph into the output
	gIO->set_root(add);						// plug the graph into the output
	gIO->start();								// start the IO
	while(true) {								// endless loop
		sleep_sec(10);
	}
}

// Mixer instrument that takes a list of remote stream references

extern "C" void * mixer(void * ptr) {
	Logger * mLog = (Logger *) ptr;
	Mixer mix(2);				// stereo mixer
								// loop to add all of the specified remote inputs to the mixer
	for (unsigned i = 0; i < hosts.size(); i++)
		mix.add_input( * (new RemoteFrameStream(hosts[i], ports[i], 
										CSL_DEFAULT_RESPONSE_PORT + (i * 4), 
										2, CSL_RFS_BUFFER_SIZE)));
	mLog->logMsg("\tplaying CSL mixer...");
	Stereoverb verb(mix);					// add some stereo reverb
	verb.set_wet_level(0.5);
	verb.set_room_size(0.983);
	gIO->start();							// start the IO
	gIO->set_root(verb);					// start the mixer/reverb
	while(true) {							// endless loop
		sleep_sec(10);
	}
}

// Support for speaker streams

#define LL_FILENAME "/usr/local/CSL/Data/SpeakerDB.txt"
#define SS_FILENAME "/usr/local/CSL/Data/SpeakerDB.aiff"

vector <SoundCue *> gSpeakers;			// global list of sound cues
SoundFile * gSndFile;					// global sound file with speakers

void load_cues(void) {
	FILE * inp;
	char c_name[32], line[128];
	unsigned start, stop;
						// load the large sample file
	gSndFile = new SoundFile(SS_FILENAME);
	gSndFile->open_for_read();
	gSndFile->read_buffer_from_file(gSndFile->duration());
						// now load the sound cue data
	inp = fopen(LL_FILENAME, "r");
	if (inp == NULL) {
		printf("\tError opening file %s\n", LL_FILENAME);
		return;
	}
	while (!feof(inp)) {
		fgets(line, 128, inp);
		sscanf(line, "%s %u %u\n", c_name, &start, &stop);
		gSpeakers.push_back(new SoundCue(c_name, gSndFile, start, stop));
	//	printf("\tSC: %s %d - %d\n", c_name, start, stop);
	}
	fclose(inp);
}

// Phoneme stream instrument -- Play phrases at random positions every so often

extern "C" void * phonemes(void * ptr) {
	load_cues();
	SoundCue * voice = gSpeakers[random() % gSpeakers.size()];
	voice->set_to_end();
	StaticVariable pos(0);

	MulOp mult( * voice, 0.1);
	Panner pan(mult, pos);
//	Stereoverb verb(pan);					// add some stereo reverb
//	verb.set_wet_level(0.5);
//	verb.set_room_size(0.985);
//	gIO->set_root(verb);					// plug the graph into the output
	gIO->set_root(pan);						// plug the graph into the output
	gIO->start();							// start the IO

	while(true) {							// get a speaker to play
		voice = gSpeakers[random() % gSpeakers.size()];
		voice->set_to_end();
		pan.set_input( * voice);
		pos.set_value(frandom() * 2 - 1);
		voice->trigger();					// now trigger the sample
		sleep_sec(10 + (frandom() * 15));
	}
}

// Logging functions

void csl::logMsg(LogLevel level, char * format, ...) {
	va_list args;
	cram::LogLevel cLev;
	switch (level) {
		case kLogInfo: cLev = cram::kLogInfo; break;
		case kLogWarning: cLev = cram::kLogWarning; break;
		case kLogError: cLev = cram::kLogError; break;
		case kLogFatal: cLev = cram::kLogFatal; break;		
	}
	va_start(args, format);
	server->mLog.vlogMsg(cLev, format, args);
	va_end(args);
}

void csl::logMsg(char * format, ...) {
	va_list args;
	va_start(args, format);
	server->mLog.vlogMsg(cram::kLogInfo, format, args);
	va_end(args);
}

