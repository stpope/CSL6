///
///  MIDIIO.h -- MIDI IO using PortMIDI
///
///	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
///

#ifndef CSL_MIDIIO_H
#define CSL_MIDIIO_H

#include "CSL_Types.h"
#include "CGestalt.h"

#include "math.h"
#include "portmidi.h"
#include "porttime.h"

// copied from 240_test.c. need to sort out where it belongs.

#define OUTPUT_BUFFER_SIZE 0
#define DRIVER_INFO NULL
#define TIME_PROC Pt_Time
#define TIME_INFO NULL
#define TIME_START Pt_Start(1, 0, 0) /* timer started w/millisecond accuracy */
#define MIDI_THRU NULL

// PmDeviceInfo data structure taken from portmidi.h
//
//  typedef struct {
//		int structVersion; 
//		const char *interf; /* underlying MIDI API, e.g. MMSystem or DirectX */
//		const char *name;   /* device name, e.g. USB MidiSport 1x1 */
//		int input;			/* true iff input is available */
//		int output;			/* true iff output is available */
//		int opened;			/* used by generic PortMidi code to do error checking on arguments */		
//	} PmDeviceInfo;

// PmEvent data structure taken from portmidi.h
//  typedef long PmMessage;
//	typedef struct {
//		PmMessage      message;
//		PmTimestamp    timestamp;
//	} PmEvent;

namespace csl {
	
///	CSL_MIDIMessageType

typedef enum {
	kNone			= 0,
	kNoteOff		= 8,
	kNoteOn			= 9,
	kPolyTouch		= 10,
	kControlChange	= 11,
	kProgramChange  = 12,
	kAftertouch		= 13,
	kPitchWheel		= 14,
	kSysEX			= 15
} CSL_MIDIMessageType;

///	CSL_MIDIMessage

class CSL_MIDIMessage {

public:
	CSL_MIDIMessageType message;
	unsigned channel;	// 0-indexed, so from 0 to 15 
	unsigned data1;
	unsigned data2;
	long time;
	
	CSL_MIDIMessage();
//	CSL_MIDIMessage(CSL_MIDIMessageType, ch, d1, d2 );
}; 

///	copy_CSL_MIDIMessage -- copies CSL_MIDIMessage

void copy_CSL_MIDIMessage(CSL_MIDIMessage* source, CSL_MIDIMessage* dest );

///	CSL_MIDIMessageToPmEvent -- converts CSL_MIDIMessage to PmEvent

void CSL_MIDIMessageToPmEvent(CSL_MIDIMessage* cslMIDI, PmEvent* event );

///	PmEventToCSL_MIDIMessage -- converts PmEvent to CSL_MIDIMessage

void PmEventToCSL_MIDIMessage( PmEvent* event, CSL_MIDIMessage* cslMIDI );

///	Message_ChannelToStatus -- converts from message and channel to status byte

unsigned Message_ChannelToStatus(CSL_MIDIMessageType message, unsigned channel );

///
///	MIDIIO class
///

class MIDIIO {
public:	
	MIDIIO();
	virtual ~MIDIIO();
	
	virtual void open() { };		///< can't open the abstract class
	bool is_open();					///< true if MIDI stream is opened. 
	void close();					///< closing MIDI stream
	void dump_device_info();		///< printing device info for all devices.
	void dump_count_devices();		///< printing total number of devices available

									///< thin wrapper for PortMidi functions. made available for flexibility.
	int count_devices();
	int get_default_input_id();
	int get_default_output_id();	
	const PmDeviceInfo* get_device_info(int deviceID );
	
protected:
	PmStream * mMIDIStream;			///< opened stream
	PmDeviceID mDeviceID;			///< device ID which will/is opened.

					///< static to keep track of Pm_Initialize() / Pm_Terminate()
	static bool mIsInitialized; 
	static unsigned mNumInstantiated;
	static bool mIsPortTimeStarted;
	
					///< status indicators
	bool mIsInput;
	bool mIsOutput;
	bool mIsOpen;

	void handle_error(PmError err);
};

///
///	MIDIIn class
///

class MIDIIn : public MIDIIO {
public:
	MIDIIn();	
	
	unsigned buffer_size();
	void set_buffer_size(unsigned bufferSize );
	void open();				///< method for opening the default stream.
	void open(int deviceID);
	bool poll();
	void read();				///< generic read method. gets MIDI message and store it in mBuffer
	void read_interpret();		///< read method and sets internal flag for which message was received
	
	PmEvent buffer() { return mBuffer[0]; }
	void dump_buffer();

	CSL_MIDIMessage message() {	return mMsg; }
	void dump_CSL_MIDIMessage();
	
	bool is_NoteOn_received();
	bool is_NoteOff_received();
	bool is_PolyTouch_received();
	bool is_ControlChange_received();
	bool is_ProgramChange_received();
	bool is_Aftertouch_received();
	bool is_PitchWheel_received();
	bool is_SysEX_received();
	
	unsigned get_note();
	unsigned get_velocity();
	unsigned get_PolyAftertouch();
	unsigned get_ControlChange_function();
	unsigned get_ControlChange_value();
	unsigned get_ProgramNumber();
	unsigned get_Aftertouch();
	unsigned get_PitchWheel();
	float get_frequency();
	float get_velocity_float();  ///< has range of [0.0 1.0] mapped to [0 127]
	
	void setFilter();
	void filter_active_sensing(bool flag );
	void filter_sysex(bool flag );
	void filter_clock_msg(bool flag );
	

protected:
						/// fundamental members
	long	 mBufferSize;
	long	 mFilterFlag;
	
	CSL_MIDIMessage mMsg;
	
	PmEvent mBuffer[1]; ///< buffer which gets filled by read()
	PmError mLength;	///< length 

};

///
///	MIDIOut class
///

class MIDIOut : public MIDIIO {
public:
	MIDIOut();
	~MIDIOut();

	unsigned buffer_size();							
	void set_buffer_size(unsigned bufferSize ); // TODO: should not be called after opening.
	long latency();
	void set_latency(long latency );			// TODO: should not be called after opening.
	
	void open();
	void open(int deviceID );
	void set_message(CSL_MIDIMessage msg, long when ); 
	void write();
	void write(CSL_MIDIMessage* msg, long length );  ///< thin wrapper for Pm_Write
	void write_short(CSL_MIDIMessage msg );
	void write_SysEX(long when, unsigned char *msg );
	
				/// convenience method for each MIDI messages
				/// writes directly and doesn't use member mMsg for temporal storage.
	void write_NoteOn(unsigned channel, unsigned pitch, unsigned velocity );		///< MIDINote#, [0, 127]
	void write_NoteOn(unsigned channel, float frequency, float amplitude );		///< [Hz], [0.0 1.0];
	void write_NoteOff(unsigned channel, unsigned pitch, unsigned velocity );		///< MIDINote#, [0, 127]
	void write_NoteOff(unsigned channel, float frequency, float amplitude );		///< [Hz], [0.0 1.0];
	void write_PolyTouch(unsigned channel, unsigned pitch, unsigned amount );
	void write_ControlChange(unsigned channel, unsigned function, unsigned value );
	void write_ProgramChange(unsigned channel, unsigned programNum );
	void write_Aftertouch(unsigned channel, unsigned amount );						///< [0, 127]
	void write_PitchWheel(unsigned channel, unsigned amount );						///< [0, 16384] 

protected:
	long		mBufferSize;
	long		mLatency;
	
	CSL_MIDIMessage mMsg;
};

}   // csl namespace

#endif
