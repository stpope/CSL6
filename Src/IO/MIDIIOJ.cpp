//
//  MIDIIOJ.cpp -- MIDI IO for CSL using JUCE
//
//	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
//

#include "MIDIIOJ.h"

extern juce::AudioDeviceManager * gAudioDeviceManager;	// global JUCE audio device mgr

using namespace csl;

//	CSL_MIDIMessage

CMIDIMessage::CMIDIMessage() 
	: message(kNone),			// init to no event
	  channel(0), data1(0), data2(0), time(0) { }

bool CMIDIMessage::isNoteOn()			{ return (command == kNoteOn); }
bool CMIDIMessage::isNoteOff()			{ return (command == kNoteOff); }
bool CMIDIMessage::isNoteOnOff()		{ return ((command == kNoteOff) || (command == kNoteOn)); }

bool CMIDIMessage::isPolyTouch()		{ return (command == kPolyTouch); }
bool CMIDIMessage::isControlChange()	{ return (command == kControlChange); }
bool CMIDIMessage::isProgramChange()	{ return (command == kProgramChange); }
bool CMIDIMessage::isAftertouch()		{ return (command == kAftertouch); }
bool CMIDIMessage::isPitchWheel()		{ return (command == kPitchWheel); }
bool CMIDIMessage::isSysEX()			{ return (command == kSysEX); }

unsigned CMIDIMessage::getCommand()					{ return (unsigned) command; }
unsigned CMIDIMessage::getNote()					{ return (unsigned) data1; }
unsigned CMIDIMessage::getVelocity()				{ return (unsigned) data2; }
unsigned CMIDIMessage::getPolyAftertouch()			{ return (unsigned) data2; }
unsigned CMIDIMessage::getControlFunction()			{ return (unsigned) data1; }
unsigned CMIDIMessage::getControlValue()			{ return (unsigned) data2; }
unsigned CMIDIMessage::getProgramNumber()			{ return (unsigned) data1; }
unsigned CMIDIMessage::getAftertouch()				{ return (unsigned) data1; }
unsigned CMIDIMessage::getPitchWheel()				{ return ((unsigned) (data2 << 7) + (unsigned) data1 ); }
float    CMIDIMessage::getFrequency()				{ return keyToFreq(data1); }
float    CMIDIMessage::getVelocityFloat()			{ return ((float) data2 / 127.0f); }

#pragma mark MIDIIO

//    MIDIIO

// static var. definition

bool MIDIIO::mIsInitialized = false; 

// Made available for flexibility

int MIDIIO::countDevices() { 
	juce::StringArray midiDevs = juce::MidiInput::getDevices();
	return (int) midiDevs.size();
}

// Made available for flexibility

void MIDIIO::timerCallback() {
    juce::Array<juce::MidiDeviceInfo> inDevices = juce::MidiInput::getAvailableDevices();
    for (unsigned i = 0; i < inDevices.size(); i++) {
        mInNames.add(inDevices[i].name);
        mInDevices.add(new MidiDeviceListEntry(inDevices[i]));
    }
    juce::Array<juce::MidiDeviceInfo> outDevices = juce::MidiOutput::getAvailableDevices();
    for (unsigned i = 0; i < outDevices.size(); i++) {
        mOutNames.add(outDevices[i].name);
        mOutDevices.add(new MidiDeviceListEntry(outDevices[i]));
    }
    open(0);
    stopTimer();
    printf("\nFound %d MIDI ins and %d MIDI outs\n", inDevices.size(), outDevices.size());
}

void MIDIIO::dumpDevices() {
	unsigned len = mInDevices.size();
	logMsg("\tMIDI in devices");
	for (unsigned i = 0; i < len; i++)
		logMsg("		%d = %s", i, mInNames[i].toUTF8());
	logMsg("	Def: %d", juce::MidiInput::getDefaultDeviceIndex());
	len = mOutDevices.size();
	logMsg("\n\tMIDI out devices");
	for (unsigned i = 0; i < len; i++)
        logMsg("		%d = %s", i, mOutNames[i].toUTF8());
	logMsg("	Def: %d\n", juce::MidiOutput::getDefaultDeviceIndex());
}

// Constructors

MIDIIO::MIDIIO() {
	if( ! mIsInitialized ) {
		mIsInitialized = true;
	}
	mIsOpen = false;
	mMsg.message = kNone;
    startTimer(50);
    logMsg("Start MIDI I/O");
}

MIDIIO::~MIDIIO() {
	mIsInitialized = false;
}

void MIDIIO::open() {
	open(juce::MidiInput::getDefaultDeviceIndex());
}

bool MIDIIO::isOpen() {
	return mIsOpen;
}

void MIDIIO::close() {
	mIsOpen = false;
	this->clear();
}

// clear MIDI stream

void MIDIIO::clear() {
	mMsg.message = kNone;
	mBuffer.clear();
}

//	copy_MIDIMessage -- copies csl::CMIDIMessage <--> juce::MidiMessage

void MIDIIO::copyMessage(CMIDIMessage& source, CMIDIMessage& dest) {
	dest.message = source.message;
	dest.command = source.command;
	dest.channel = source.channel;
	dest.data1 = source.data1;
	dest.data2 = source.data2;
	dest.time = source.time;
}

void MIDIIO::copyMessage(CMIDIMessage& source, juce::MidiMessage* dest) {
	dest = new juce::MidiMessage((source.command | source.channel),
                 source.data1, source.data2, source.time);
}

#define CCOPY_MSG	dest.channel = source.getChannel();		\
					dest.data1 = source.getNoteNumber();	\
					dest.data2 = source.getVelocity()

void MIDIIO::copyMessage(const juce::MidiMessage& source, CMIDIMessage& dest) {
	const unsigned char * data = (unsigned char *) source.getRawData();
	unsigned char cmd = (data[0] & 0xf0) >> 4;
	dest.command = cmd;
	switch (cmd) {
			case kNoteOn:
				if (source.getVelocity() > 0) {
					dest.message = kNoteOn;
					CCOPY_MSG;
					break;
				} 
			case kNoteOff:
				dest.message = kNoteOff;
				CCOPY_MSG;
				break;
			case kPolyTouch:
				dest.message = kPolyTouch;
				CCOPY_MSG;				
			case kControlChange:
				dest.message = kControlChange;
				dest.channel = source.getChannel();	
				dest.data1 = source.getControllerNumber();
				dest.data2 = source.getControllerValue();
				break;
			case kProgramChange:
				dest.message = kProgramChange;
				CCOPY_MSG;
				mMsg.data2 = 0;
			break;
			case kAftertouch:
				dest.message = kAftertouch;
				CCOPY_MSG;
				break;
			case kPitchWheel:
				dest.message = kPitchWheel;
				CCOPY_MSG;
				dest.data2 = 0;
				break;
			case kSysEX:
				dest.message = kSysEX;
				dest.channel = 0;
				dest.data1 = 0;
				dest.data2 = 0;
				break;
			default:
				logMsg(kLogError, "Unknown MIDI input: stat=%d chan=%d value1=%d value2=%d", 
						cmd, source.getChannel(), source.getNoteNumber(), source.getVelocity());
				break;
	}
	dest.time = (Timestamp) source.getTimeStamp();
}

void MIDIIO::dumpBuffer() {
	unsigned cmd, voice, pitch = 0, vel;
//	PmMessage msg;
//	msg = mBuffer[0].message;
//	cmd = Pm_MessageStatus(msg) & 0xF0;
//	voice = Pm_MessageStatus(msg) & 0x0F;
//	pitch = Pm_MessageData1(msg);
//	vel = Pm_MessageData2(msg);
//	printf("\tGot message: time %.3f, cmd: %x midich: %d data1: %d data2: %d\n", 
//		(float) mBuffer[0].timestamp / 1000.0f, cmd, voice, pitch, vel); 		
	
}

// MIDIIO error handler

void MIDIIO::handleError(CException * err) {
//	Pm_Terminate();
	logMsg(kLogError, "An error occured in MIDIIO: %s", err->what());
}

juce::ReferenceCountedObjectPtr<MidiDeviceListEntry> MIDIIO::findDevice (juce::MidiDeviceInfo device, bool isInputDevice) {
    const juce::ReferenceCountedArray<MidiDeviceListEntry>& midiDevices = isInputDevice ? mInDevices : mOutDevices;
    for (auto& d : midiDevices)
        if (d->deviceInfo == device)
            return d;
    return nullptr;
}

#pragma mark MIDIIn

//	MIDIIn

MIDIIn::MIDIIn() : MIDIIO() {	
	mFilterFlag = 0;
	mDeviceID = 0;
	mDevice = 0;
	mBufferSize = 100;
}

unsigned MIDIIn::bufferSize() { 
	return mBufferSize; 
}

void MIDIIn::setBufferSize(unsigned bufferSize) { 
	mBufferSize = bufferSize; 
}

// open and register me as the MidiInputCallback

void MIDIIn::open(int deviceID) {
	mDeviceID = deviceID;
    juce::MidiDeviceInfo def_ind = mInDevices[deviceID].get()->deviceInfo;
    juce::String descr = def_ind.identifier;
    mDevice = (juce::MidiInput::openDevice(descr, this));
	if ( ! mDevice) {
		logMsg(kLogError, "Cannot open midi in %d", mDeviceID);
		return;
	}
    mDevice->start();
	logMsg("Open midi input %s", mDevice->getName().toUTF8());
	if (gAudioDeviceManager)
		gAudioDeviceManager->setMidiInputEnabled(mDevice->getIdentifier(), true);
	mIsOpen = true;
}

///< start MIDI stream

void MIDIIn::start() {
	mStartTime = juce::Time::getMillisecondCounter() * 0.001;
	if (mDevice)
		mDevice->start();
    mKeyboardState.addListener (this);
}

///< stop MIDI stream

void MIDIIn::stop() {
	if (mDevice)
		mDevice->stop();
    mKeyboardState.removeListener (this);
}

void MIDIIn::handleIncomingMidiMessage (juce::MidiInput* /*source*/, const juce::MidiMessage& message) {
                    // This is called on the MIDI thread
    const juce::ScopedLock sl (midiMonitorLock);
    incomingMessages.add(message);
    triggerAsyncUpdate();
}

void MIDIIn::handleNoteOn (juce::MidiKeyboardState*, int midiChannel, int midiNoteNumber, float velocity) {
    juce::MidiMessage m (juce::MidiMessage::noteOn (midiChannel, midiNoteNumber, velocity));
    m.setTimeStamp ((juce::Time::getMillisecondCounterHiRes() * 0.001) - mStartTime);
    logMsg("\tMIDI note on  C %d K %d V %d", midiChannel, midiNoteNumber, (int) (velocity * 127.0f));
    copyMessage(m, mMsg);               // copy the message
    this->changed((void *) &m);
}

void MIDIIn::handleNoteOff (juce::MidiKeyboardState*, int midiChannel, int midiNoteNumber, float velocity) {
    juce::MidiMessage m (juce::MidiMessage::noteOff (midiChannel, midiNoteNumber, velocity));
    m.setTimeStamp ((juce::Time::getMillisecondCounterHiRes() * 0.001) - mStartTime);
    logMsg("\tMIDI note off C %d K %d V %d", midiChannel, midiNoteNumber, (int) (velocity * 127.0f));
    copyMessage(m, mMsg);               // copy the message
    this->changed((void *) &m);
}

void MIDIIn::handleAsyncUpdate() {              // This is called on the message loop
    juce::Array<juce::MidiMessage> messages;
    {
        const juce::ScopedLock sl (midiMonitorLock);
        messages.swapWith (incomingMessages);
    }
    juce::String messageText;
    for (auto& m : messages) {                    // "Note on E6 Velocity 25 Channel 1"
        auto data = m.getRawData();
        unsigned cmd = (data[0] & 0xf0) >> 4;
        unsigned ch = (data[0] & 0x0f) + 1;
        switch (cmd) {
            case kNoteOn: {
                unsigned nt = data[1] & 0x7f;
                unsigned vl = data[2] & 0x7f;
                float vf = ((float)vl) / 127.0;
                handleNoteOn(0, ch, nt, vf); }
                break;
            case kNoteOff: {
                unsigned nt = data[1] & 0x7f;
                unsigned vl = data[2] & 0x7f;
                float vf = ((float)vl) / 127.0;
                handleNoteOff(0, ch, nt, vf); }
                break;
          default:
                messageText << "\t" << m.getDescription() << "\n";
        }
    }
    logMsgNN(messageText.toRawUTF8());
}

// quick poll

bool MIDIIn::poll() {
	return (mMsg.message != kNone);
}

// step to next event or reset flag

void MIDIIn::nextEvent() {
	if (mBuffer.isEmpty()) {				// if empty, clear
		mMsg.message = kNone;
	} else {								//else take first even from Q
		int t0 = mBuffer.getFirstEventTime();
		juce::MidiBuffer::Iterator it(mBuffer);
		it.getNextEvent(*mJMsg, t0);
		copyMessage(*mJMsg, mMsg);			// copy the message
		mBuffer.clear(t0, 1);				// remove it from the buffer
	}
}

void MIDIIn::dumpMessage() {
	printf("\tMIDIIn  ");
	switch(mMsg.message) {
		case kNone:
			printf("kNone");
			break;
		case kNoteOff:
			printf("kNoteOff");
			break;
		case kNoteOn:
			printf("kNoteOn");
			break;
		case kPolyTouch:
			printf("kPolyTouch");
			break;
		case kControlChange:
			printf("ControlChange");
			break;
		case kProgramChange:
			printf("kProgramChange");
			break;
		case kAftertouch:
			printf("kAftertouch");
			break;
		case kPitchWheel:
			printf("kPitchWheel");
			break;
		case kSysEX:
			printf("kSysEX");
			break;
		default:
			printf("unknown");
	}
	printf("  \tch: %2d  d1: %2d  d2: %2d  t: %5.3f\n", mMsg.channel, mMsg.data1, mMsg.data2, mMsg.time);
}

// evaluate answers the midi command

int MIDIIn::evaluate(void * arg) {
	CMIDIMessage * msg = (CMIDIMessage *) arg;
	return msg->message; 
}

//	MIDIOut

MIDIOut::MIDIOut() : MIDIIO(), mOut(0) {
	mDeviceID = 0; // Pm_GetDefaultOutputDeviceID();
	mBufferSize = 0;
	mLatency	= 0;	// do I need to set this to > 0 value for synchronization?
	juce::MidiOutput::getDevices();
	juce::StringArray midiDevs = juce::MidiInput::getDevices();

	 * mOut;
}

MIDIOut::~MIDIOut() {
}

void MIDIOut::open(int deviceID) {
	mDeviceID = deviceID;
	mOut = (juce::MidiOutput::openDevice(deviceID)).get();
}

void MIDIOut::write(CMIDIMessage & msg) {
	copyMessage(msg, mJMsg);
	mOut->sendMessageNow (*mJMsg);
	delete mJMsg;
}

void MIDIOut::writeSysEX(long when, unsigned char *msg ) {
}

void MIDIOut::writeNoteOn(unsigned channel, unsigned pitch, unsigned velocity ) {
}

void MIDIOut::writeNoteOn(unsigned channel, float frequency, float amplitude ) {
}

void MIDIOut::writeNoteOff(unsigned channel, unsigned pitch, unsigned velocity ) {
}

void MIDIOut::writeNoteOff(unsigned channel, float frequency, float amplitude ) {
}

void MIDIOut::writePolyTouch(unsigned channel, unsigned pitch, unsigned amount ) {
}

void MIDIOut::writeControlChange(unsigned channel, unsigned function, unsigned value ) {
}

void MIDIOut::writeProgramChange(unsigned channel, unsigned programNum ) {
}

void MIDIOut::writeAftertouch(unsigned channel, unsigned amount ) {
}

void MIDIOut::writePitchWheel(unsigned channel, unsigned amount ) {
}


// init a Midi player from a file name

void MIDIPlayer::init(juce::String namS) {
	juce::File fil(namS);
	mFile.readFrom(*(fil.createInputStream()));
	mNumTrax = mFile.getNumTracks();
	mTrak = (juce::MidiMessageSequence *) mFile.getTrack(0);
	mIsOn = false;
	logMsg("MIDIPlayer %d trax", mNumTrax);
						// dump all track events
	for (int i = 0; i < mNumTrax; i++) {
		mTrak = (juce::MidiMessageSequence *) mFile.getTrack(i);
		logMsg("		track %d : %d evts", i, mTrak->getNumEvents());
	}
	int tix = mFile.getTimeFormat();
	juce::MidiMessageSequence tempi;
	mFile.findAllTempoEvents(tempi);
	if (tempi.getNumEvents() == 0) {
		mTempoScale = 120.0f;
	} else {
		juce::MidiMessageSequence::MidiEventHolder * t1 = tempi.getEventPointer(0);
		juce::MidiMessage msg = t1->message;				// and its event
		double sex = msg.getTempoSecondsPerQuarterNote();
		mTempoScale = sex / tix;
	}
}

// MIDIPlayer constructor loads MIDI file

MIDIPlayer::MIDIPlayer(string nam, InstrumentLibrary * lib) {
	juce::String namS(nam.c_str());
	init(namS);
	mLibrary = lib;
}

// MIDIPlayer constructor loads MIDI file

MIDIPlayer::MIDIPlayer(string folder, string nam, InstrumentLibrary * lib) {
	string abs(folder);
	abs += nam;
	juce::String namS(abs.c_str());
	init(namS);
	mLibrary = lib;
}

// start method reads tracks

void MIDIPlayer::start(int index) {
	if (index < 0)
		mTrak = this->mergeTrax();
	else
		mTrak = (juce::MidiMessageSequence *) mFile.getTrack(index);					// get track
	mTrak->deleteSysExMessages();					// clean up
	mTrak->updateMatchedPairs();					// match note-on-off
	float tim0 = (float) mTrak->getStartTime() * mTempoScale;	// time 0
	float timX = (float) mTrak->getEndTime() * mTempoScale;		// time x
	float timN = tim0;								// time now
	float startT = fTimeNow();
	unsigned numEvts = mTrak->getNumEvents();		// # notes
	mIsOn = true;
	float pos;										// stereo position
	float * pPtr = & pos;
	logMsg("MIDIPlayer playing %d evts  %5.2f sec.", numEvts, timX - tim0);

	while (mIsOn && (timN < timX)) {				// schedule loop
		int ind = mTrak->getNextIndexAtTime((double) (timN / mTempoScale));	// get ind of next event
		float timE = mTrak->getEventTime(ind) * mTempoScale;	// time of event
		juce::MidiMessageSequence::MidiEventHolder * evt 	// get the event holder
					= mTrak->getEventPointer(ind);
		if ( ! evt) continue;
        juce::MidiMessage msg = evt->message;				// and its event
		copyMessage(msg, mMsg);						// make a CMIDIMessage
		sleepSec(timE - timN);						// sleep till then
		
		if (mMsg.isNoteOn()) {						// handle note-on events
			float timD = mTrak->getTimeOfMatchingKeyUp(ind);
			if (timD != 0)
				timD = (timD * mTempoScale) - timE; // get note duration
			else {
//				logMsg("		NoteOff %d not found", ind);
				timD = timX - timE;
			}
			if (mLibrary) {							// get the instrument to play on
				int which = 0;
				InstrumentVector iv = (* mLibrary)[mMsg.channel];
				if (iv.empty()) {
					logMsg("MIDIPlayer empty iv");
				}
				Instrument * inst = iv[which];		// get the vector for this channel
				if ( ! inst) {
					logMsg("MIDIPlayer empty in");
				}
				while (inst->isActive()) {			// if we're already playing
					which++;
					if (which == iv.size()) {		// look for a free instrument
//						iv.push_back(new Instrument(*inst));
						logMsg("MIDIPlayer out of voices in instr %d", mMsg.channel);
						goto next;					// goto next note instead of voice-stealing
					}
					inst = iv[which];
				}				
				pos = fRand1();						// pick a random position
				inst->setParameter(set_position_f, 1, (void **) &pPtr, "f");
													// play the MIDI note
//				logMsg("NoteOn %5.2f - %5.2f  \t%2d.%2d  %2d  %2d", 
//						timE, timD, mMsg.command, mMsg.channel, mMsg.data1, mMsg.data2);
				inst->playMIDI(timD, mMsg.channel, mMsg.data1, mMsg.data2);
			}
//		} else if ( ! mMsg.isNoteOff()) {
//			logMsg("Msg %5.2f  %d - %d %d %d", timV * mTempoScale, 
//					mMsg.message, mMsg.channel, mMsg.data1, mMsg.data2);
		}
next:
		mTrak->deleteEvent(ind, mMsg.isNoteOn());	// delete event from list
		timN = timE;								// update clock
	}
}

// set stop flag

void MIDIPlayer::stop() {
	mIsOn = false;
}

// merge all the file's tracks into 1

juce::MidiMessageSequence * MIDIPlayer::mergeTrax() {
	juce::MidiMessageSequence * tr = new juce::MidiMessageSequence;
	for (int i = 0; i < mNumTrax; i++)
		tr->addSequence(*mFile.getTrack(i), 0, 0, mFile.getLastTimestamp());
	return tr;
}
