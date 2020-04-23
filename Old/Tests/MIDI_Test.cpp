//
//	MIDI_Test_main.cpp -- simple CSL MIDI program 
//	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
//

#include "CSL_Includes.h"   // Basic CSL kernel include
#include "Oscillator.h"		// UGens I use
#include "Envelope.h"
#include "BinaryOp.h"
#include "Variable.h"

#ifndef CSL_WINDOWS
#include "CAIO.h"			// include 3 IO options
#endif
#include "PAIO.h"
#include "MIDIIO.h"

#include <iostream>

#ifndef CSL_WINDOWS
#include <unistd.h>		// for usleep
#else
#include <windows.h>	// for usleep
#endif

using namespace csl;

// fcn prototypes

void initial_test();
void input_read_interpret_test();
void MIDIIO_test();
void input_test();
void output_test();

// MAIN

int main (int argc, const char * argv[]) {

//	initial_test();
//	input_read_interpret_test();
//	MIDIIO_test();
//	input_test();
	output_test();
	return 0;
}

#define FREQ 80

void initial_test() {
	//PAIO io;				// PortAudio IO object
	CAIO io;				// CoreAudio IO object
	
	MIDIIn midiIn;			// create midi in
	midiIn.dump_device_info();
							// prompt for input device
	printf("Open which input device? : ");
	int inDev;
	scanf("%d", &inDev);
	midiIn.open(inDev);						// set up midi in
	midiIn.filter_active_sensing(true);
	midiIn.filter_sysex(true);
	
	// The DSP graph is taken from the FM test example
	Sine vox, mod(FREQ);					// declare 2 oscillators
	ADSR a_env(3, 0.1, 0.1, 0.3, 1);		// amplitude env = std ADSR
	Envelope i_env(3, 0, 0, 0.1, 2, 0.2, 1, 2.5, 4, 3, 0);	// index env
	i_env.scale_values(FREQ);				// multiply index envelope by mod freq
	mod.set_scale(i_env);					// scale the modulator by its envelope
	mod.set_offset(FREQ);					// add in the base freq
	vox.set_frequency(mod);					// set the carrier's frequency
	MulOp mul(a_env, 0.8);
	vox.set_scale(mul);					// scale the carrier's output by the amplitude envelope
	
	logMsg("CSL playing FM...");
	io.set_root(vox);					// make some sound
	io.open();
	io.start();

	int j = 0;
	bool err;
    while (1) {							// loop
        err = midiIn.poll();
        if (err == true) {
            midiIn.read();
			if( j % 2 ) {
				a_env.reset();			// reset the envelopes to time 0
				i_env.reset();
			} else {
				a_env.trigger();		// reset the envelopes to time 0
				i_env.trigger();
			}
			j++;
		}
	}
	logMsg("CSL done.");
	io.stop();							// close down
	io.close();		
	midiIn.close();
}

void input_read_interpret_test() {
	CAIO io;
	MIDIIn midiIn;
	midiIn.dump_device_info();
	printf("Open which input device? : ");
	int inDev;
	scanf("%d", &inDev);
	
	midiIn.open(inDev);
	midiIn.filter_active_sensing(true);
	midiIn.filter_sysex(true);
	
	Sine s;
	ADSR env(3.0, 0.1, 0.1, 0.5, 1.0);	
	Variable amp(0.5);
	MulOp volume(env, amp);
	
	s.set_scale(0.0);
	s.set_frequency(1017.0f);

	io.set_root(s);
	logMsg("CSL testing MIDIIn");
	io.open();
	io.start();
	
	bool err;
	while(1) {
		err = midiIn.poll();
		if(err == true) {
			midiIn.read_interpret();
			if(midiIn.is_NoteOn_received()) {
				s.set_frequency( midiIn.get_frequency() );
//				logMsg("midiIn.get_frequency() = %f", midiIn.get_frequency() );
//				logMsg("midiIn.get_velocity_float() = %f", midiIn.get_velocity_float() );
				s.set_scale( midiIn.get_velocity_float() / 2.0f );
				env.trigger();
			}
			if(midiIn.is_NoteOff_received()) {
				env.release();
			}
		}
	}
}

void MIDIIO_test() {
	MIDIIO midiIO;
	
	cout << "MIDIIO Test.......\n";
	cout << "------------------------------------------------------\n";
	cout << "\ttesting is_open()" << endl;
	bool isopen = midiIO.is_open();
	cout << "\t\tMIDIIO.is_open() = " << isopen << endl << endl; 

	cout << "\ttesting dump_device_info()" << endl;
	midiIO.dump_device_info();  // prints out all of the available MIDI device in stdout

	cout << "\ttesting dump_count_device()" << endl << endl;
	midiIO.dump_count_devices(); // prints out number of available MIDI devices in stdout

	cout << "\ttesting count_device()" << endl;
	int count = midiIO.count_devices();
	cout << "\t\t count_device() = " << count << endl << endl;

	cout << "\ttesting get_default_input_id()" << endl;
	int defaultInID = midiIO.get_default_input_id();
	cout << "\t\tget_fault_input_id() = " << defaultInID << endl << endl;

	cout << "\ttesting get_default_output_id()" << endl;
	int defaultOutID = midiIO.get_default_output_id();
	cout << "\t\tget_fault_output_id() = " << defaultOutID << endl << endl;
	
	cout <<"\ttesting get_device_info(int deviceID)" << endl;
	int i;
	for(i = 0; i < count; i++) {
		PmDeviceInfo* info;
		info = midiIO.get_device_info( i ) ;
		cout << "\t\tdevice[" << i << "] structVersion : " << info->structVersion << endl;
		cout << "\t\tdevice[" << i << "] interf : " << info->interf << endl;
		cout << "\t\tdevice[" << i << "] name : " << info->name << endl;
		cout << "\t\tdevice[" << i << "] input : " << info->input << endl;
		cout << "\t\tdevice[" << i << "] output : " << info->output << endl;
		cout << "\t\tdevice[" << i << "] opened : " << info->opened << endl << endl;
	}
	cout << "------------------------------------------------------\n";
	cout << "MIDIO_test() finished......" << endl;
}

void error_disp(status s, const char* str) {
	switch(s) {
		case cslOk:
			cout << "\t\t" << str << " succeeded" << endl;
			break;
		case cslErr:
			cout << "\t\t" << str << " failed" << endl;
			break;
		default:
			cout << "\t\t" << str << " shoudn't come here...." << endl;
	}
}

void input_test() {
	MIDIIn midiIn;
	status stat; 
	
	cout << "MIDIIn Test.......\n";
	cout << "------------------------------------------------------\n";
	cout << "\ttesting open()" << endl;
	stat = midiIn.open();
	error_disp(stat, "midiIn.open()");
	cout << endl;
	
	cout << "\ttesting close() and re opening." << endl;
	stat = midiIn.close();
	error_disp(stat, "midiIn.close()");
	sleep_sec(0.1); // sleeping a bit for debug... whether this is the cause or not.
	stat = midiIn.open();
	error_disp(stat, "midiIn.open()");
	cout << endl;
	
	cout << "\tclosing midiIn for next test" << endl;
	stat = midiIn.close();
	error_disp(stat, "midiIn.close()");
	
	cout << "\ttesting opening and closing all of the devices available. " << endl;
	int i;
	int count = midiIn.count_devices();
	cout << "\t\tmidiIn.count_device() = " << count << endl;
	for( i = 0; i < count; i++) {
		
		PmDeviceInfo * info;
		info = midiIn.get_device_info(i);
		
		if(info->input == true) {
			stat = midiIn.open( i );
			cout << "\t\tmidiIn.open(" << i << ")";
			error_disp(stat, " "); 
			stat = midiIn.close() ;
			cout << "\t\tmidiIn.close(" << i << ")";
			error_disp(stat, " "); 			
		} else {
			cout << "\t\tDevice[" << i <<"] skipped, not input" << endl;
		}
	}
	cout << "\ttesting read()" << endl;
	midiIn.dump_device_info();
	printf("\t\tOpen which input device? : ");
	int inDev;
	scanf("%d", &inDev);
	
	stat = midiIn.open(inDev);
	if( stat == cslErr ) {
		exit(-1);
	}
	midiIn.filter_active_sensing(true);
	midiIn.filter_sysex(true);

#define NUM_TILL_BREAK 20
	cout << "\t\tready for MIDI input...." << endl;
	unsigned r = 0;	
    bool received = false;
	while (1) {
        received = midiIn.poll();
        if (received == true) {
            midiIn.read();
			midiIn.dump_buffer();
			r++;
			if( r > NUM_TILL_BREAK ) 
				break;
		}
	}
	cout << "\ttesting read_interpret()" << endl;
	r = 0;
	received = false;
	while (1) {
        received = midiIn.poll();
        if (received == true) {
            midiIn.read_interpret();
			midiIn.dump_CSL_MIDIMessage();
			r++;
			if( r > NUM_TILL_BREAK ) 
				break;
		}
	}
	cout << "\ttesting is_##_received() & get_##()" << endl;
	r = 0;
	received = false;
	while (1) {
        received = midiIn.poll();
        if (received == true) {
            midiIn.read_interpret();
			
			if( midiIn.is_NoteOn_received() ) {
				cout << "\t\t NoteOn received\n";
				cout << "\t\t\t get_note(): " << midiIn.get_note() << " get_velocity(): " << midiIn.get_velocity() << endl;
				cout << "\t\t\t get_frequency(): " << midiIn.get_frequency() << " get_velocity_float(): " << midiIn.get_velocity_float() << endl;				
			}
			if( midiIn.is_NoteOff_received() ) {
				cout << "\t\t NoteOff received\n";
				cout << "\t\t\t get_note(): " << midiIn.get_note() << " get_velocity(): " << midiIn.get_velocity() << endl;
			}
			if( midiIn.is_PolyTouch_received() ) {
				cout << "\t\t PolyTouch received\n";
				cout << "\t\t\t get_PolyAftertouch(): " << midiIn.get_PolyAftertouch() << endl;
			}
			if( midiIn.is_ControlChange_received() ) {
				cout << "\t\t ControlChange received\n";
				cout << "\t\t\t get_ControlChange_function(): " << midiIn.get_ControlChange_function() << " get_ControlChange_value(): " << midiIn.get_ControlChange_value() << endl;
			}
			if( midiIn.is_ProgramChange_received() ) {
				cout << "\t\t ProgramChange received\n";
				cout << "\t\t\t get_ProgramNumber(): " << midiIn.get_ProgramNumber() << midiIn.get_velocity() << endl;
			}
			if( midiIn.is_Aftertouch_received() ) {
				cout << "\t\t Aftertouch received\n";
				cout << "\t\t\t get_Aftertouch(): " << midiIn.get_Aftertouch() << endl;
			}
			if( midiIn.is_PitchWheel_received() ) {
				cout << "\t\t PitchWheel received\n";
				cout << "\t\t\t get_PitchWheel(): " << midiIn.get_PitchWheel()  << endl;
			}
			if( midiIn.is_SysEX_received() ) {
				cout << "\t\t SysEX received\n";				
			}
			r++;
			if( r > NUM_TILL_BREAK ) 
				break;
		}
	}
	// TODO: Test filtering... 
	cout << "------------------------------------------------------\n";	
	cout << "input_test() finished...." << endl;
}

void output_test() {
	MIDIOut midiOut;
	status stat;
	
	cout << "MIDIOut Test.......\n";
	cout << "------------------------------------------------------\n";
	cout << "\ttesting open()" << endl;
	stat = midiOut.open();
	error_disp(stat, "midiOut.open()");
	cout << endl;
	
	cout << "\ttesting close() and re opening." << endl;
	stat = midiOut.close();
	error_disp(stat, "midiOut.close()");
	sleep_sec(0.1); // sleeping a bit for debug... whether this is the cause or not.
	stat = midiOut.open();
	error_disp(stat, "midiOut.open()");
	cout << endl;
	
	cout << "\tclosing midiOut for next test" << endl;
	stat = midiOut.close();
	error_disp(stat, "midiOut.close()");
	
	cout << "\ttesting opening and closing all of the devices available. " << endl;
	int i;
	int count = midiOut.count_devices();
	cout << "\t\tmidiIn.count_device() = " << count << endl;
	for( i = 0; i < count; i++) {
		
		PmDeviceInfo * info;
		info = midiOut.get_device_info(i);
		
		if(info->output == true) {
			stat = midiOut.open( i );
			cout << "\t\tmidiIn.open(" << i << ")";
			error_disp(stat, " "); 
			stat = midiOut.close() ;
			cout << "\t\tmidiIn.close(" << i << ")";
			error_disp(stat, " "); 			
		} else {
			cout << "\t\tDevice[" << i <<"] skipped, not output" << endl;
		}
	}
	cout << "\ttesting read()" << endl;
	midiOut.dump_device_info();
	printf("\t\tOpen which output device? : ");
	int inDev;
	scanf("%d", &inDev);
	
	stat = midiOut.open(inDev);
	if( stat == cslErr ) {
		exit(-1);
	}	
}

