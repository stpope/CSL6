//
//  MIDIIOP.cpp -- MIDI IO using PortMIDI
//
//	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
//

#include "MIDIIO.h"

//#define DEBUG_MIDIIO_READ
//#define DEBUG_MIDIIO_READ_INTERPRET

using namespace csl;

/******************************************************************************
	CSL_MIDIMessage
******************************************************************************/

CSL_MIDIMessage::CSL_MIDIMessage() : message(kNone), channel(0), data1(0), data2(0), time(0) { }

/******************************************************************************
	copy_CSL_MIDIMessage
		copies CSL_MIDIMessage
******************************************************************************/

void csl::copy_CSL_MIDIMessage(CSL_MIDIMessage* source, CSL_MIDIMessage* dest ) {
	dest->message = source->message;
	dest->channel = source->channel;
	dest->data1 = source->data1;
	dest->data2 = source->data2;
	dest->time = source->time;
}

/******************************************************************************
	CSL_MIDIMessageToPmEvent
		converts CSL_MIDIMessage to PmEvent
******************************************************************************/
void csl::CSL_MIDIMessageToPmEvent(CSL_MIDIMessage* cslMIDI, PmEvent* event ) {

	event->message = Pm_Message(Message_ChannelToStatus(cslMIDI->message, cslMIDI->channel ), 
								 cslMIDI->data1,
								 cslMIDI->data2 );
	event->timestamp = cslMIDI->time;
}

/******************************************************************************
	PmEventToCSL_MIDIMessage
		converts PmEvent to CSL_MIDIMessage
******************************************************************************/
void csl::PmEventToCSL_MIDIMessage( PmEvent* event, CSL_MIDIMessage* cslMIDI ) {
	
	cslMIDI->message = (CSL_MIDIMessageType)
		((unsigned) ((Pm_MessageStatus(event->message) & 0xF0) >> 4) );
	cslMIDI->channel = Pm_MessageStatus(event->message) & 0x0F;
	cslMIDI->data1   = Pm_MessageData1(event->message);
	cslMIDI->data2   = Pm_MessageData2(event->message);
	cslMIDI->time    = event->timestamp; 
}


/******************************************************************************
Message_ChannelTovoid
converts from message and channel to void byte
******************************************************************************/
unsigned csl::Message_ChannelToStatus(CSL_MIDIMessageType message, unsigned channel ) {
	
	return (((unsigned)message << 4) + channel );	
}

/******************************************************************************
	MIDIIO
******************************************************************************/

// static var. definition

bool MIDIIO::mIsInitialized = false; 
bool MIDIIO::mIsPortTimeStarted = false;
unsigned MIDIIO::mNumInstantiated = 0;

// Constructors

MIDIIO::MIDIIO() {
	if( ! mIsInitialized ) {
		Pm_Initialize();		
		mIsInitialized = true;
	}
	mNumInstantiated++;
	mIsOpen = false; 
	mIsPortTimeStarted = false;		
}

MIDIIO::~MIDIIO() {
	mNumInstantiated--;
	if(mNumInstantiated == 0) {
		Pt_Stop();
		mIsPortTimeStarted = false;
		Pm_Terminate(); 
		mIsInitialized = false;
	}
	
}

bool MIDIIO::is_open() { 
	return mIsOpen; 
}

void MIDIIO::close() {
	PmError err;
	err = Pm_Close(mMIDIStream );
	if(err != pmNoError )
		handle_error(err);
}

void MIDIIO::dump_device_info() {
	int i;
	char dev_nam[128], * nl;
	
    printf("\nPortMidi devices:\n");
    for (i = 0; i < Pm_CountDevices(); i++) {
        const PmDeviceInfo *info = Pm_GetDeviceInfo(i);
		if ((nl = index(info->name, '\n')) == NULL)
			strcpy(dev_nam, info->name);
		else
			strncpy(dev_nam, info->name, (nl - info->name));
        printf("\tDeviceID %d: %s, %s", i, info->interf, dev_nam);
		if (info->input)	printf(" (input)");
		if (info->output)   printf(" (output)");
		printf("\n");
    }
	printf("\n");
}

void MIDIIO::dump_count_devices() {
	printf("PortMidi device count = %d\n", Pm_CountDevices());
}

// thin wrappers for PortMidi functions. Made available for flexibility

int MIDIIO::count_devices() { 
	return (int) Pm_CountDevices();
}

int MIDIIO::get_default_input_id() { 
	return (int) Pm_GetDefaultInputDeviceID();
}

int MIDIIO::get_default_output_id() {	
	return (int) Pm_GetDefaultOutputDeviceID();
}

const PmDeviceInfo* MIDIIO::get_device_info(int deviceID ) {
	return Pm_GetDeviceInfo((PmDeviceID) deviceID );
}

// PortMIDI error handler

void MIDIIO::handle_error(PmError err) {
	Pm_Terminate();
	logMsg(kLogError, "An error occured while using PortMIDI, Error# %d, %s",err, Pm_GetErrorText(err));
}

/******************************************************************************
	MIDIIn
******************************************************************************/

MIDIIn::MIDIIn() : MIDIIO() {	
	mIsInput = true;
	mIsOutput = false;
	mFilterFlag = 0;
	
	mDeviceID = Pm_GetDefaultInputDeviceID();
	mBufferSize = 100;
}

unsigned MIDIIn::buffer_size() { 
	return mBufferSize; 
}

void MIDIIn::set_buffer_size(unsigned bufferSize) { 
	mBufferSize = bufferSize; 
}

void MIDIIn::open() {
					// checking if device with mDeviceID is an input. 
	const PmDeviceInfo* info = get_device_info(mDeviceID );
	if( ! info->input ) {
		return handle_error(pmInvalidDeviceId );
	}
	if( ! mIsPortTimeStarted ) 
		TIME_START;		// porttime start.
	mIsPortTimeStarted = true;

	PmError err;
//	Pm_OpenInput(PortMidiStream**, PmDeviceID, void*, long int, PmTimestamp (*)(void*), void*)
	err = Pm_OpenInput(&mMIDIStream, mDeviceID, DRIVER_INFO, mBufferSize, 
						(PmTimeProcPtr) TIME_PROC, TIME_INFO );
	if(err != pmNoError ) 
		return handle_error(err);
	mFilterFlag = mFilterFlag | PM_FILT_ACTIVE;	// portmidi filters active sensing by default.
}

void MIDIIn::open(int deviceID) {
	mDeviceID = deviceID;
	open();
}


bool MIDIIn::poll() {
	PmError err; 
	err = Pm_Poll(mMIDIStream );
	if(err == FALSE )
		return false;		
	if(err == TRUE ) 
		return true;
	if(err != pmNoError ) {
		handle_error(err);
		return false;		// need better error handling....
	}
	return false;
}

void MIDIIn::read() {
	mLength = Pm_Read(mMIDIStream, mBuffer, 1);
#ifdef DEBUG_MIDIIO_READ
	unsigned cmd, voice, pitch = 0, vel;
	PmMessage msg;

	if (mLength > 0) {
		msg = mBuffer[0].message;
		cmd = Pm_Messagevoid(msg) & 0xF0;
		voice = Pm_Messagevoid(msg) & 0x0F;
		pitch = Pm_MessageData1(msg);
		vel = Pm_MessageData2(msg);
		printf("\tGot message: time %.3f, cmd: %x midich: %d data1: %d data2: %d\n", (float) mBuffer[0].timestamp / 1000.0f, cmd, voice, pitch, vel); 		
	}
#endif
	if(mLength == 0 ) {
		logMsg("in MIDIIn::read(). MIDI message not read");
		return;
	} else
		if(mLength < 0 ) 
			return handle_error(mLength );
}

void MIDIIn::read_interpret() {
	read();
	unsigned cmd, voice, pitch = 0, vel;
	PmMessage msg;
	msg = mBuffer[0].message;
	cmd = (Pm_MessageStatus(msg) & 0xF0) >> 4;
	voice = Pm_MessageStatus(msg) & 0x0F;
	pitch = Pm_MessageData1(msg);
	vel = Pm_MessageData2(msg);
	switch(cmd) {
			case kNoteOn:
				if(vel > 0) {
					mMsg.message = kNoteOn;
					mMsg.channel = voice; 
					mMsg.data1 = pitch;
					mMsg.data2 = vel;
					break;
				} 
			case kNoteOff:
				mMsg.message = kNoteOff;
				mMsg.channel = voice;
				mMsg.data1 = pitch;
				mMsg.data2 = vel;
				break;
			case kPolyTouch:
				mMsg.message = kPolyTouch;
				mMsg.channel = voice;
				mMsg.data1 = pitch;
				mMsg.data2 = vel;				
			case kControlChange:
				mMsg.message = kControlChange;
				mMsg.channel = voice;
				mMsg.data1 = pitch;
				mMsg.data2 = vel;
				break;
			case kProgramChange:
				mMsg.message = kProgramChange;
				mMsg.channel = voice;
				mMsg.data1 = pitch;
				mMsg.data2 = 0;
				break;
			case kAftertouch:
				mMsg.message = kAftertouch;
				mMsg.channel = voice;
				mMsg.data1 = pitch;
				mMsg.data2 = 0;
				break;
			case kPitchWheel:
				mMsg.message = kPitchWheel;
				mMsg.channel = voice;
				mMsg.data1 = pitch;
				mMsg.data2 = vel;
				break;
			case kSysEX:
				mMsg.message = kSysEX;
				mMsg.channel = 0;
				mMsg.data1 = 0;
				mMsg.data2 = 0;
				break;
			default:
				printf("Unknown input: void=%d chan=%d value1=%d value2=%d", cmd, voice, pitch, vel);
				break;
	}
	
#ifdef DEBUG_MIDIIO_READ_INTERPRET
	printf("\tGot message: time %.3f, cmd: %x midich: %d data1: %d data2: %d\n", (float) mBuffer[0].timestamp / 1000.0f, cmd, mMsg.channel, mMsg.data1, mMsg.data2); 		
#endif
}

void MIDIIn::dump_buffer() {
	unsigned cmd, voice, pitch = 0, vel;
	PmMessage msg;
	msg = mBuffer[0].message;
	cmd = Pm_MessageStatus(msg) & 0xF0;
	voice = Pm_MessageStatus(msg) & 0x0F;
	pitch = Pm_MessageData1(msg);
	vel = Pm_MessageData2(msg);
	printf("\tGot message: time %.3f, cmd: %x midich: %d data1: %d data2: %d\n", (float) mBuffer[0].timestamp / 1000.0f, cmd, voice, pitch, vel); 		
	
}
void MIDIIn::dump_CSL_MIDIMessage() {
	printf("MIDIIn message received\n");
	switch(mMsg.message) {
		case kNone:
			printf("\t message: kNone");
			break;
		case kNoteOff:
			printf("\t message: kNoteOff");
			break;
		case kNoteOn:
			printf("\t message: kNoteOn");
			break;
		case kPolyTouch:
			printf("\t message: kPolyTouch");
			break;
		case kControlChange:
			printf("\t message: kControlChange");
			break;
		case kProgramChange:
			printf("\t message: kProgramChange");
			break;
		case kAftertouch:
			printf("\t message: kAftertouch");
			break;
		case kPitchWheel:
			printf("\t message: kPitchWheel");
			break;
		case kSysEX:
			printf("\t message: kSysEX");
			break;
		default:
			printf("\t message: unknown");
	}
	printf(" channel: %d data1: %d data2: %d time: %d\n", mMsg.channel, mMsg.data1, mMsg.data2, mMsg.time);
}


bool MIDIIn::is_NoteOn_received() { 
	return (mMsg.message == kNoteOn) ? true : false; 
}

bool MIDIIn::is_NoteOff_received() { 
	return (mMsg.message == kNoteOff) ? true : false; 
}

bool MIDIIn::is_PolyTouch_received() { 
	return (mMsg.message == kPolyTouch) ? true : false; 
}

bool MIDIIn::is_ControlChange_received() { 
	return (mMsg.message == kControlChange) ? true : false; 
}

bool MIDIIn::is_ProgramChange_received() { 
	return (mMsg.message == kProgramChange) ? true : false; 
}

bool MIDIIn::is_Aftertouch_received() { 
	return (mMsg.message == kAftertouch) ? true : false; 
}

bool MIDIIn::is_PitchWheel_received()  { 
	return (mMsg.message == kPitchWheel) ? true : false;
}

bool MIDIIn::is_SysEX_received() { 
	return (mMsg.message == kSysEX) ? true :false; 
}

unsigned MIDIIn::get_note()					{ return mMsg.data1; }
unsigned MIDIIn::get_velocity()				{ return mMsg.data2; }
unsigned MIDIIn::get_PolyAftertouch()			{ return mMsg.data2; }
unsigned MIDIIn::get_ControlChange_function() { return mMsg.data1; }
unsigned MIDIIn::get_ControlChange_value()	{ return mMsg.data2; }
unsigned MIDIIn::get_ProgramNumber()			{ return mMsg.data1; }
unsigned MIDIIn::get_Aftertouch()				{ return mMsg.data1; }
unsigned MIDIIn::get_PitchWheel()				{ return ((mMsg.data2 << 7) + mMsg.data1 ); }
float    MIDIIn::get_frequency()				{ return keyToFreq(mMsg.data1 ); }
float    MIDIIn::get_velocity_float()			{ return ((float) mMsg.data2 / 127.0f); }

void MIDIIn::setFilter() {
	PmError err;
	err = Pm_SetFilter(mMIDIStream, PM_FILT_ACTIVE | PM_FILT_CLOCK);
	if(err != pmNoError )
		handle_error(err);
}

void MIDIIn::filter_active_sensing(bool flag ) {
	if( ! (mFilterFlag & PM_FILT_ACTIVE) && flag ) { // set filter
		mFilterFlag = mFilterFlag | PM_FILT_ACTIVE;
		PmError err;
		err = Pm_SetFilter(mMIDIStream, mFilterFlag );
		if(err != pmNoError )
			return handle_error(err);
		logMsg("active sensing will be filtered, mFilterFlag = %d", mFilterFlag);
		return;
	}
	if((mFilterFlag & PM_FILT_ACTIVE) && !flag) {  // un-set the filter

		mFilterFlag = mFilterFlag ^ PM_FILT_ACTIVE; // xoring to flip just the unwanted bit.
		PmError err;
		err = Pm_SetFilter(mMIDIStream, mFilterFlag );
		if(err != pmNoError )
			return handle_error(err);
		logMsg("active sensing will not be filtered, mFilterFlag = %d", mFilterFlag);
		return;		
	}
	// do nothing
	logMsg("active sensing filter state not changed, mFilterFlag = %d", mFilterFlag);
}


void MIDIIn::filter_sysex(bool flag ) {
	if( ! (mFilterFlag & PM_FILT_SYSEX) && flag ) { // set filter
		mFilterFlag = mFilterFlag | PM_FILT_SYSEX;
		PmError err;
		err = Pm_SetFilter(mMIDIStream, mFilterFlag );
		if(err != pmNoError )
			return handle_error(err);
		logMsg("sysex will be filtered, mFilterFlag = %d", mFilterFlag);
		return;
	}
	if((mFilterFlag & PM_FILT_SYSEX) && !flag) {  // un-set the filter
		
		mFilterFlag = mFilterFlag ^ PM_FILT_SYSEX; // xoring to flip just the unwanted bit.
		PmError err;
		err = Pm_SetFilter(mMIDIStream, mFilterFlag );
		if(err != pmNoError )
			return handle_error(err);
		logMsg("sysex will not be filtered, mFilterFlag = %d", mFilterFlag);
		return;		
	}
	// do nothing
	logMsg("sysex filter state not changed, mFilterFlag = %d", mFilterFlag);
}


void MIDIIn::filter_clock_msg(bool flag ) {
	if( ! (mFilterFlag & PM_FILT_CLOCK) && flag ) { // set filter
		
		mFilterFlag = mFilterFlag | PM_FILT_CLOCK;
		PmError err;
		err = Pm_SetFilter(mMIDIStream, mFilterFlag );
		if(err != pmNoError )
			return handle_error(err);
		logMsg("clock message will be filtered, mFilterFlag = %d", mFilterFlag);
		return;
	}
	if((mFilterFlag & PM_FILT_CLOCK) && !flag) {  // un-set the filter
		
		mFilterFlag = mFilterFlag ^ PM_FILT_CLOCK; // xoring to flip just the unwanted bit.
		PmError err;
		err = Pm_SetFilter(mMIDIStream, mFilterFlag );
		if(err != pmNoError )
			return handle_error(err);
		logMsg("clock message will not be filtered, mFilterFlag = %d", mFilterFlag);
		return;		
	}
	// do nothing
	logMsg("clock message filter state not changed, mFilterFlag = %d", mFilterFlag);
}


/******************************************************************************
	MIDIOut
******************************************************************************/

MIDIOut::MIDIOut() : MIDIIO() {
	mIsInput	= false;
	mIsOutput   = true;
	mDeviceID = Pm_GetDefaultOutputDeviceID();
	mBufferSize = 0;
	mLatency	= 0;	// do I need to set this to > 0 value for synchronization?
}

MIDIOut::~MIDIOut() {
}

unsigned MIDIOut::buffer_size() {	return mBufferSize; }
void MIDIOut::set_buffer_size(unsigned bufferSize ) { mBufferSize = bufferSize; }
long MIDIOut::latency () {	return mLatency; }
void MIDIOut::set_latency(long latency ) { mLatency = latency; }

void MIDIOut::open() {
		// checking if device with mDeviceID is an output. 
	const PmDeviceInfo* info = get_device_info(mDeviceID );
	if( ! info->output ) {
		return handle_error(pmInvalidDeviceId );
	}
	
	if( ! mIsPortTimeStarted ) 
		TIME_START; 
	mIsPortTimeStarted = false;
	
	PmError err;
	err = Pm_OpenOutput(&mMIDIStream, mDeviceID, DRIVER_INFO, mBufferSize,
						 (PmTimeProcPtr) TIME_PROC, TIME_INFO, mLatency );
	if(err != pmNoError ) 
		return handle_error(err );
}

void MIDIOut::open(int deviceID ) {
	mDeviceID = deviceID;
	open();
}

void MIDIOut::set_message(CSL_MIDIMessage msg, long when ) {
	copy_CSL_MIDIMessage(&msg, &mMsg );
}

void MIDIOut::write() {
	PmEvent event;
	csl::CSL_MIDIMessageToPmEvent(&mMsg, &event);
	
	long eventLength = 1;   // does it need to be longer?? 
	PmError err;
	err = Pm_Write(mMIDIStream, &event, eventLength);
	if(err != pmNoError )
		return handle_error(err );
}

void MIDIOut::write(CSL_MIDIMessage* msg, long length ) {
	int i;
	for(i = 0; i < length; i++ ) {
		copy_CSL_MIDIMessage(msg, &mMsg );
		write();
	}
}

void MIDIOut::write_short(CSL_MIDIMessage msg ) {
	PmError err;
	err = Pm_WriteShort(mMIDIStream, 
				   msg.time, 
				   Pm_Message(((unsigned)msg.message << 4) + msg.channel, 
							   msg.data1,
							   msg.data2 )
				   );
	if(err != pmNoError )
		return handle_error(err );
}

void MIDIOut::write_SysEX(long when, unsigned char *msg ) {
	PmError err;
	err = Pm_WriteSysEx(mMIDIStream, when, msg); 
	if(err != pmNoError )
		return handle_error(err );
}

void MIDIOut::write_NoteOn(unsigned channel, unsigned pitch, unsigned velocity ) {
	PmEvent event;
	event.timestamp = TIME_PROC();
	event.message = Pm_Message(Message_ChannelToStatus(kNoteOn, channel), 
								pitch,
								velocity );
	PmError err;
	err = Pm_Write(mMIDIStream, &event, 1);
	if(err != pmNoError )
		return handle_error(err );
}

void MIDIOut::write_NoteOn(unsigned channel, float frequency, float amplitude ) {
	PmEvent event;
	event.timestamp = TIME_PROC();
	event.message = Pm_Message(Message_ChannelToStatus(kNoteOn, channel), 
								freqToKey(frequency),
								(int)ceil(amplitude * 127.0f) );
	PmError err;
	err = Pm_Write(mMIDIStream, &event, 1);
	if(err != pmNoError )
		return handle_error(err );
}

void MIDIOut::write_NoteOff(unsigned channel, unsigned pitch, unsigned velocity ) {
	PmEvent event;
	event.timestamp = TIME_PROC();
	event.message = Pm_Message(Message_ChannelToStatus(kNoteOff, channel), 
								pitch,
								velocity );
	PmError err;
	err = Pm_Write(mMIDIStream, &event, 1);
	if(err != pmNoError )
		return handle_error(err );
}

void MIDIOut::write_NoteOff(unsigned channel, float frequency, float amplitude ) {
	PmEvent event;
	event.timestamp = TIME_PROC();
	event.message = Pm_Message(Message_ChannelToStatus(kNoteOff, channel), 
								freqToKey(frequency),
								(int)ceil(amplitude * 127.0f) );
	PmError err;
	err = Pm_Write(mMIDIStream, &event, 1);
	if(err != pmNoError )
		return handle_error(err );
}

void MIDIOut::write_PolyTouch(unsigned channel, unsigned pitch, unsigned amount ) {
	PmEvent event;
	event.timestamp = TIME_PROC();
	event.message = Pm_Message(Message_ChannelToStatus(kPolyTouch, channel), 
								pitch,
								amount );
	PmError err;
	err = Pm_Write(mMIDIStream, &event, 1);
	if(err != pmNoError )
		return handle_error(err );
}

void MIDIOut::write_ControlChange(unsigned channel, unsigned function, unsigned value ) {
	PmEvent event;
	event.timestamp = TIME_PROC();
	event.message = Pm_Message(Message_ChannelToStatus(kControlChange, channel), 
								function,
								value );
	PmError err;
	err = Pm_Write(mMIDIStream, &event, 1);
	if(err != pmNoError )
		return handle_error(err );
}

void MIDIOut::write_ProgramChange(unsigned channel, unsigned programNum ) {
	PmEvent event;
	event.timestamp = TIME_PROC();
	event.message = Pm_Message(Message_ChannelToStatus(kProgramChange, channel), 
								programNum,
								0 );
	PmError err;
	err = Pm_Write(mMIDIStream, &event, 1);
	if(err != pmNoError )
		return handle_error(err );
}

void MIDIOut::write_Aftertouch(unsigned channel, unsigned amount ) {
	PmEvent event;
	event.timestamp = TIME_PROC();
	event.message = Pm_Message(Message_ChannelToStatus(kAftertouch, channel), 
								amount,
								0 );
	PmError err;
	err = Pm_Write(mMIDIStream, &event, 1);
	if(err != pmNoError )
		return handle_error(err );
}

void MIDIOut::write_PitchWheel(unsigned channel, unsigned amount ) {
	PmEvent event;
	event.timestamp = TIME_PROC();
	event.message = Pm_Message(Message_ChannelToStatus(kPitchWheel, channel), 
								amount,
								0 );
	PmError err;
	err = Pm_Write(mMIDIStream, &event, 1);
	if(err != pmNoError )
		return handle_error(err );
}
