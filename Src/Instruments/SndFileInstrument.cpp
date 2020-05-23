//
//  SndFileInstrument.cpp -- Sound file player instrument class
//	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
//

#include "SndFileInstrument.h"

using namespace csl;
using namespace std;

// The constructor initializes the DSP graph's UGens

SndFileInstrument0::SndFileInstrument0(string path) :		// initializers
		Instrument(),
		mPlayer(),
		mPanner(mPlayer, 0.0, 1.0),		// stereo panner
		mFile(0) {
	this->initialize(path);
}

SndFileInstrument0::SndFileInstrument0(string folder, string path) :
		Instrument(),
		mPlayer(),
		mPanner(mPlayer, 0.0, 1.0),		// stereo panner
		mFile(0) {
	this->initialize(folder + path);
}

void SndFileInstrument0::initialize(string path) {
	if (path.size() > 0) {				// load the file
		try {
			mFile = new SoundFile(path);
			mFile->openForRead(true);
			mPlayer.setBuffer(mFile->mWavetable);
		} catch (CException & e) {
			logMsg(kLogError, "File open exception caught: %s", e.mMessage.c_str());
			return;
		}
	}
	mPlayer.seekTo(0, kPositionEnd);		// set snd file to end
	mName = path;
	mNumChannels = 2;						// I'm stereo.
	mGraph = & mPanner;						// Store the root of the graph as the inst var mGraph
	mUGens["Panner"] = & mPanner;			// add ugens that can be monitored to the map
	mUGens["Player"] = & mPlayer;
	mEnvelopes.push_back(& mPlayer);		// list envelopes for retrigger
											// set up accessor vector
	mAccessors.push_back(new Accessor("am", set_amplitude_f, CSL_FLOAT_TYPE));
	mAccessors.push_back(new Accessor("po", set_position_f, CSL_FLOAT_TYPE));
	mAccessors.push_back(new Accessor("fi", set_file_f, CSL_STRING_TYPE));
}

// The destructor frees all the UGen pointers

SndFileInstrument0::~SndFileInstrument0() { }

// Plug function for use by OSC setter methods

void SndFileInstrument0::setParameter(unsigned selector, int argc, void **argv, const char *types) {
	float d = * (float *) argv[0];
	if (types[0] == 'i')
		d = (float) (* (int *) argv[0]);
	switch (selector) {
		case set_amplitude_f:
			mPanner.setScale(d); break;
		case set_position_f:
			mPanner.setPosition(d); break;
		case set_file_f:
			if (mFile) delete mFile;
			mFile = new SoundFile((char *) argv[0]);
			mFile->openForRead(true);
			mPlayer.setBuffer(mFile->mWavetable);
			logMsg("Loaded sound file %s", (char *) argv[0]);
			break;
		default:
			logMsg(kLogError, "Unknown selector in SndFileInstrument set_parameter selector: %d", selector);
	}
}

// Play a note with a given arg list
// Formats:
// 	ampl, pos

void SndFileInstrument0::playOSC(int argc, void **argv, const char *types) {
	float ** fargs = (float **) argv;
	int ** iargs = (int **) argv;
	switch(argc) {
		case 2:					// ampl, pos
			if (strcmp(types, "ff") != 0) {
				logMsg(kLogError, "Invalid type string in OSC message, expected \"ff\" got \"%s\"", types);
				return;
			}
			mPanner.setScale(*fargs[0]);
			mPanner.setPosition(*fargs[1]);
//			printf("\tSndFileInstr: PN: %5.3f %5.3f\n", *fargs[0], *fargs[1]);
			break;
		default:
			logMsg(kLogError, "Invalid type string in OSC message: \"%s\"", types);
			return;
	}
	mPlayer.trigger();
}

void SndFileInstrument0::play() {
	mPlayer.trigger();
};

#ifdef SndFileInstrument1 // -----------------------------------------------------------

#pragma mark SndFileInstrument1

// The constructor initializes the DSP graph's UGens

SndFileInstrument1::SndFileInstrument1(string path, int start, int stop) :		// initializers
		SndFileInstrument0(path),
		mEnvelope((mPlayer.stopFrame() - mPlayer.startFrame()) / mPlayer.frameRate(), 0.01, 0.01),
		mRate(1.0) {					// set the player's playback rate
	this->initialize(path);
}

SndFileInstrument1::SndFileInstrument1(string folder, string path, int start, int stop) :		// initializers
		SndFileInstrument0(path),
		mEnvelope((mPlayer.stopFrame() - mPlayer.startFrame()) / mPlayer.frameRate(), 0.01, 0.01),
		mRate(1.0) {						// set the player's playback rate
	this->initialize(folder + path);
}

void SndFileInstrument1::initialize(string path) {
	mName = path;
	mNumChannels = 2;						// I'm stereo.
	mEnvelope.setScale(mPlayer);
	mPanner.setInput(mEnvelope),			// stereo panner
	mGraph = & mPanner;						// Store the root of the graph as the inst var _graph
	if (path.size() > 0) {
		try {
			mPlayer.openForRead();
		} catch (CException & e) {
			logMsg(kLogError, "File open exception caught: %s", e.mMessage.c_str());
			return;
		}
	}
	mUGens["Rate"] = & mRate;				// add ugens that can be monitored to the map
	mUGens["Panner"] = & mPanner;
	mUGens["Player"] = & mPlayer;
	
	//	mEnvelopes.push_back(& mPlayer);		// list envelopes for retrigger
	// set up accessor vector
	mAccessors.push_back(new Accessor("am", set_amplitude_f, CSL_FLOAT_TYPE));
	mAccessors.push_back(new Accessor("ra", set_rate_f, CSL_FLOAT_TYPE));
	mAccessors.push_back(new Accessor("po", set_position_f, CSL_FLOAT_TYPE));
	mAccessors.push_back(new Accessor("fi", set_file_f, CSL_STRING_TYPE));
	mAccessors.push_back(new Accessor("st", set_start_f, CSL_INT_TYPE));
	mAccessors.push_back(new Accessor("en", set_stop_f, CSL_INT_TYPE));
	mAccessors.push_back(new Accessor("at", set_attack_f, CSL_FLOAT_TYPE));
	mAccessors.push_back(new Accessor("de", set_decay_f, CSL_FLOAT_TYPE));
}

// The destructor frees all the UGen pointers

SndFileInstrument1::~SndFileInstrument1() { }

// Plug function for use by OSC setter methods

void SndFileInstrument1::setParameter(unsigned selector, int argc, void **argv, const char *types) {
	float d = * (float *) argv[0];
	if (types[0] == 'i')
		d = (float) (* (int *) argv[0]);
	switch (selector) {
		case set_amplitude_f:
			mPanner.setScale(d); break;
		case set_rate_f:
			mRate.setValue(d); break;
		case set_position_f:
			mPanner.setPosition(d); break;
		case set_file_f:
			mPlayer.setPath((char *) argv[0]);
			mPlayer.openForRead(true);
			mPlayer.setToEnd();
			logMsg("Loaded sound file %s", mPlayer.path().c_str());
			break;
		case set_start_f:
			mPlayer.setStart((int ) d);
			mEnvelope.setDuration(mPlayer.durationInSecs());
			break;
		case set_stop_f:
			mPlayer.setStop((int) d);
			mEnvelope.setDuration(mPlayer.durationInSecs());
			break;
		case set_attack_f:
			mEnvelope.setAttack(d); break;
		case set_decay_f:
			mEnvelope.setRelease(d); break;
		default:
			logMsg(kLogError, "Unknown selector in SndFileInstrument set_parameter selector: %d", selector);
	}
}

// Play a note with a given arg list
// Formats:
// 	ampl, pos
// 	ampl, pos, rate
// 	ampl, pos, start, stop
// 	ampl, pos, rate, start, stop
// 	ampl, pos, start, stop, attack, decay
// 	ampl, pos, rate, start, stop, attack, decay

void SndFileInstrument1::playOSC(int argc, void **argv, const char *types) {
	float ** fargs = (float **) argv;
	int ** iargs = (int **) argv;
	switch(argc) {
		case 2:					// ampl, pos
			if (strcmp(types, "ff") != 0) {
				logMsg(kLogError, "Invalid type string in OSC message, expected \"ff\" got \"%s\"", types);
				return;
			}
			mPanner.setScale(*fargs[0]);
			mPanner.setPosition(*fargs[1]);
			printf("\tSndFileInstr: PN: %5.3f %5.3f\n", fargs[0], fargs[1]);
			break;
		case 3:					// ampl, pos, rate
			if (strcmp(types, "fff") != 0) {
				logMsg(kLogError, "Invalid type string in OSC message, expected \"fff\" got \"%s\"", types);
				return;
			}
			mPanner.setScale(*fargs[0]);
			mPanner.setPosition(*fargs[1]);
			mRate.setValue(*fargs[2]);
			//			printf("\tSndFileInstr: PN: %g %g %g\n", fargs[0], fargs[1], fargs[2]);
			break;
		case 4:					// ampl, pos, start, stop
			if (strcmp(types, "ffii") == 0) {				// start/stop at ints = sample index
				mPanner.setScale(*fargs[0]);
				mPanner.setPosition(*fargs[1]);
				mPlayer.setStart(*iargs[2]);
				mPlayer.setStop(*iargs[3]);
				//				printf("\tSndFileInstr: PN: %g  %g  %d - %d\n", fargs[0], fargs[1], iargs[2], iargs[3]);
				break;
			}
			if (strcmp(types, "ffff") == 0) {				// start/stop at floats = relative
				mPanner.setScale(*fargs[0]);
				mPanner.setPosition(*fargs[1]);
				mPlayer.setStartRatio(*fargs[2]);
				mPlayer.setStopRatio(*fargs[3]);
				//				printf("\tSndFileInstr: PN4: %g  %g\t\t%g - %g\n", fargs[0], fargs[1], fargs[2], fargs[3]);
				break;
			}
			logMsg(kLogError, "Invalid type string in OSC message, expected \"ffii\" got \"%s\"", types);
			return;
		case 5:					// ampl, pos, rate, start, stop
			if (strcmp(types, "fffii") != 0) {
				logMsg(kLogError, "Invalid type string in OSC message, expected \"fffii\" got \"%s\"", types);
				return;
			}
			mPanner.setScale(*fargs[0]);
			mPanner.setPosition(*fargs[1]);
			mRate.setValue(*fargs[2]);
			mPlayer.setStartRatio(*iargs[3]);
			mPlayer.setStopRatio(*iargs[4]);
			//			printf("\tSndFileInstr: PN: %g %g %g %g-%g\n", fargs[0], fargs[1], fargs[2], iargs[3], iargs[4]);
			break;
		case 6:					// ampl, pos, start, stop, attack, decay
			if (strcmp(types, "ffiiff") != 0) {
				logMsg(kLogError, "Invalid type string in OSC message, expected \"ffiiff\" got \"%s\"", types);
				return;
			}
			mPanner.setScale(*fargs[0]);
			mPanner.setPosition(*fargs[1]);
			mPlayer.setStart(*iargs[2]);
			mPlayer.setStop(*iargs[3]);
			mEnvelope.setAttack(*fargs[4]);
			mEnvelope.setRelease(*fargs[5]);
			//			printf("\tSndFileInstr: PN: %g  %g   %g - %g\n", fargs[0], fargs[1], iargs[3], iargs[4]);
			break;
		case 7:					// ampl, pos, rate, start, stop, attack, decay
			if (strcmp(types, "fffiiff") != 0) {
				logMsg(kLogError, "Invalid type string in OSC message, expected \"fffiiff\" got \"%s\"", types);
				return;
			}
			mPanner.setScale(*fargs[0]);
			mPanner.setPosition(*fargs[1]);
			mRate.setValue(*fargs[2]);
			mPlayer.setStart(*iargs[3]);
			mPlayer.setStop(*iargs[4]);
			mEnvelope.setAttack(*fargs[5]);
			mEnvelope.setRelease(*fargs[6]);
			//			printf("\tSndFileInstr: PN: %g %g %d-%d\n", fargs[0], fargs[1], iargs[3], iargs[4]);
			break;
		default:
			logMsg(kLogError, "Invalid type string in OSC message: \"%s\"", types);
			return;
	}
	mEnvelope.setDuration(((float) (mPlayer.stopFrame() - mPlayer.startFrame())) / (float) mPlayer.frameRate());
	mEnvelope.trigger();
	mPlayer.trigger();
}

void SndFileInstrument1::play() {
	//	mEnvelope.setDuration((mPlayer.stopFrame() - mPlayer.startFrame()) / mPlayer.frameRate());
	mEnvelope.trigger();
	mPlayer.trigger();
};

// Play a note via a direct fcn call

void SndFileInstrument1::playNote(float ampl, float rate, float pos, int start, int stop, float attack, float decay) {
	mPanner.setScale(ampl);
	mPanner.setPosition(pos);
	mRate.setValue(rate);
	mPlayer.setStart(start);
	mPlayer.setStop(stop);
	mEnvelope.setDuration((stop - start) / mPlayer.frameRate());
	mEnvelope.setAttack(attack);
	mEnvelope.setRelease(decay);
	mEnvelope.trigger();
	mPlayer.trigger();
}

#endif

