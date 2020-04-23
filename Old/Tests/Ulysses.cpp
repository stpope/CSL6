//
//  Ulysses.cpp -- CSL functions for driving the Ulysses sound files from the OSC-based gesture sensors.
//	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
//
// This is included as an example of using the various gesture sensors to drive real-time CSL programs.
// It is a test from a piece, and sounds pretty nasty.

// CSL headers

#include "Test_Support.cpp"		// include all of CSL core and the test support functions
#include "OSC_cpp.h"
#include "GestureSensors.h"

using namespace csl;

// Prototypes for "glue" code

extern "C" void init_OSC_addr_space();
extern "C" void setup_OSC_instr_library(Instrument ** library, unsigned num_instruments);
extern "C" void main_OSC_loop(int port);

extern IO * gIO;

#define MATRIX_SOCKET 57123

Mixer mix(2);				// Main stereo output mixer
FileDescriptor sockfd;		// OSC socket file descriptor

// Layer thread functions (see below)

void load_Ulysses_sounds();
void * play_crackle(void * ignored);
void * play_wood(void * ignored);
void * play_nfile(void * ignored);
void * play_sns(void * ignored);
void * map_input(void * ignored);

///////////////////////// MAIN //////////////////////////

#define USE_REVERB		// Warning: the (unoptimized) reverberator takes about 55% of the CPU on a 1.25 GHZ G4 PowerBook

int main(int argc, char **argv) {
	READ_CSL_OPTS;										// load the standard CSL options: 
	logMsg("OSC Matrix server running...");
	
	Matrix mat(MATRIX_SOCKET, "/Matrix");				// create the matrix
	
	load_Ulysses_sounds();								// load sounds and cues
			
	Thread * cThread = ThreadPthread::MakeThread();		// create background threads
	Thread * wThread = ThreadPthread::MakeThread();		// crackle, wood, nfile, sns
	Thread * nThread = ThreadPthread::MakeThread();
	Thread * sThread = ThreadPthread::MakeThread();
	Thread * mThread = ThreadPthread::MakeThread();		// thread to map input values to the controls

	gIO = new CAIO;					// Create up Port- or Core-Audio output object
#ifdef USE_REVERB
	Stereoverb verb(mix);			// add some stereo reverb to the global mixer
	verb.set_wet_level(0.1);		
	verb.set_room_size(0.97);
	gIO->set_root(verb);				// plug the reverb in as the IO client
#else
	gIO->set_root(mix);				// plug the mixer in as the IO client
#endif
	gIO->open();
	
	gIO->start();					// start the io
	mat.start();					// start the matrix reader
	
									// fork the looping threads
	cThread->create_thread(play_crackle, NULL);
	wThread->create_thread(play_wood, NULL);
	nThread->create_thread(play_nfile, NULL);
	sThread->create_thread(play_sns, NULL);
	mThread->create_thread(map_input, NULL);

	MainLoop();						// Run the main OSC read loop function (which never returns)
} 

////// Sound file loading and playback loop functions

vector <SoundCue *> crackle;			// the sound cue lists
vector <SoundCue *> snap;
vector <SoundCue *> nfile;
SoundFile * snsSnd;

////// Layer thread functions: endless loops playing sound cues under Matrix control

void * play_crackle(void * ignored) {
	SoundCue * voice = crackle[random() % crackle.size()];
	voice->set_to_end();
	StaticVariable pos(0);
	Panner pan( * voice, pos); 
	mix.add_input(pan);					// plug the panner into the mixer
	while (true) {						// loop playing streams
		voice = crackle[random() % crackle.size()];	// get a sound cue to play
		voice->set_to_end();
		pan.set_input( * voice);
		pos.set_value(f_rand() * 2.0 - 1.0);
		voice->trigger();					// now trigger the sample
		sleep_sec(voice->duration() / 44100);
	}
}

void * play_wood(void * ignored) {
	SoundCue * voice = snap[random() % snap.size()];
	voice->set_to_end();
	StaticVariable pos(0);
	Panner pan( * voice, pos); 
	mix.add_input(pan);					// plug the panner into the mixer
	sleep_sec(f_rand() * 8.0);
	while (true) {						// loop playing streams
		voice = snap[random() % snap.size()];	// get a sound cue to play
		voice->set_to_end();
		pan.set_input( * voice);
		pos.set_value(f_rand() * 2.0 - 1.0);
		voice->trigger();					// now trigger the sample
		sleep_sec((voice->duration() / 44100) + (f_rand() * 12.0));
	}
}

void * play_nfile(void * ignored) {
	SoundCue * voice = nfile[random() % nfile.size()];
	voice->set_to_end();
	StaticVariable pos(0);
	Panner pan( * voice, pos); 
	mix.add_input(pan);					// plug the panner into the mixer
	sleep_sec(f_rand() * 8.0);
	while (true) {						// loop playing streams
		voice = nfile[random() % nfile.size()];	// get a sound cue to play
		voice->set_to_end();
		pan.set_input( * voice);
		pos.set_value(f_rand() * 2.0 - 1.0);
		voice->trigger();					// now trigger the sample
		sleep_sec((voice->duration() / 44100) + 3.0 + (f_rand() * 20.0));
	}
}

void * play_sns(void * ignored) {
	SoundCue * voice = new SoundCue("", snsSnd, 0, 1);
	voice->set_to_end();
	StaticVariable pos(0);
	MulOp scaled( * voice, 0.1);		// scale the output
	Panner pan(scaled, pos); 
	mix.add_input(pan);					// plug the panner into the mixer
	sleep_sec(f_rand() * 8.0);
	while (true) {						// loop playing streams
		voice->_start = (int) (f_rand() * (float) snsSnd->duration() * 0.75);
		voice->_stop = voice->_start + (int) ((1.0 + (f_rand() * 4.0)) * 44100);
		voice->set_to_end();
		pos.set_value(f_rand() * 2.0 - 1.0);
		voice->trigger();					// now trigger the sample
		sleep_sec((voice->duration() / 44100) + 6.0 + (f_rand() * 20.0));
	}
}

// function to take the input values and map them onto controls for the other threads

void * map_input(void * ignored) {
	while (true) {						// loop 
		compute_reduction();
	//	dump_reduced_matrix();
		sleep_sec(0.5);
	}
}

// Utility to add cue lists

#define add_snd_cue(sf, sl, sta, sto)   sl.push_back(new SoundCue("", sf, (int)(sta * 441), (int)(sto * 441)));

#define WOOD_FILENAME "../../Data/wood.aiff"
#define NFILE_FILENAME "../../Data/nfile.aiff"
#define SNS_FILENAME "../../Data/sns.aiff"

SoundFile * woodSnd;			// the sound files
SoundFile * nfileSnd;

// Load the sound cue lists taken from the Ulysses SC 1 code

void load_Ulysses_sounds() {
						// load the sample files
	woodSnd = new SoundFile(WOOD_FILENAME);
	woodSnd->open_for_read();
	woodSnd->read_buffer_from_file(woodSnd->duration());
	nfileSnd = new SoundFile(NFILE_FILENAME);
	nfileSnd->open_for_read();
	nfileSnd->read_buffer_from_file(nfileSnd->duration());
	snsSnd = new SoundFile(SNS_FILENAME);
	snsSnd->open_for_read();
	snsSnd->read_buffer_from_file(snsSnd->duration());
				// Add the cue data from Ulysses (times are in centi-sec)
	add_snd_cue(woodSnd, snap, 000, 159);   add_snd_cue(woodSnd, snap, 211, 355);   add_snd_cue(woodSnd, snap, 355, 531); 
	add_snd_cue(woodSnd, snap, 531, 690);   add_snd_cue(woodSnd, snap, 690, 828);   add_snd_cue(woodSnd, snap, 828, 974); 
	add_snd_cue(woodSnd, snap, 1090, 1190); add_snd_cue(woodSnd, snap, 1320, 1380); add_snd_cue(woodSnd, snap, 1466, 1540); 
	add_snd_cue(woodSnd, snap, 1590, 1640); add_snd_cue(woodSnd, snap, 1724, 1784); add_snd_cue(woodSnd, snap, 1794, 1865); 
	add_snd_cue(woodSnd, snap, 1872, 1940); add_snd_cue(woodSnd, snap, 2008, 2072); add_snd_cue(woodSnd, snap, 2156, 2280); 
	add_snd_cue(woodSnd, snap, 2362, 2408); add_snd_cue(woodSnd, snap, 2940, 3018); add_snd_cue(woodSnd, snap, 3145, 3218);
	add_snd_cue(woodSnd, snap, 3218, 3262); add_snd_cue(woodSnd, snap, 3262, 3372); add_snd_cue(woodSnd, snap, 3492, 3562);

	add_snd_cue(woodSnd, crackle, 148, 227);   add_snd_cue(woodSnd, crackle, 882, 1090);  add_snd_cue(woodSnd, crackle, 1224, 1284); 
	add_snd_cue(woodSnd, crackle, 2576, 2774); add_snd_cue(woodSnd, crackle, 3003, 3070); add_snd_cue(woodSnd, crackle, 3316, 3450);

	add_snd_cue(nfileSnd, nfile, 000, 252);   add_snd_cue(nfileSnd, nfile, 260, 420);   add_snd_cue(nfileSnd, nfile, 544, 690); 
	add_snd_cue(nfileSnd, nfile, 759, 914);   add_snd_cue(nfileSnd, nfile, 932, 1089);  add_snd_cue(nfileSnd, nfile, 1090, 1237); 
	add_snd_cue(nfileSnd, nfile, 1238, 1358); add_snd_cue(nfileSnd, nfile, 1358, 1621); add_snd_cue(nfileSnd, nfile, 1621, 1826); 
	add_snd_cue(nfileSnd, nfile, 1826, 2034); add_snd_cue(nfileSnd, nfile, 2034, 2191); add_snd_cue(nfileSnd, nfile, 2191, 2389); 
	add_snd_cue(nfileSnd, nfile, 2389, 2626); add_snd_cue(nfileSnd, nfile, 2626, 2794); add_snd_cue(nfileSnd, nfile, 2794, 3010); 
	add_snd_cue(nfileSnd, nfile, 3010, 3284); add_snd_cue(nfileSnd, nfile, 3284, 3550); add_snd_cue(nfileSnd, nfile, 3550, 3785);
	logMsg("Loaded source files %d snap, %d crackle, %d boing", snap.size(), crackle.size(), nfile.size());
}

