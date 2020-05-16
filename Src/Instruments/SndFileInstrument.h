///
/// SndFileInstrument.h -- Sound file player instrument class
///	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
///
/// This instrument implements sound file playback with a basic
/// A/R envelope and panning; it provides 4 parameter accessors
/// for use by OSC.
///
/// Accessors
///		"am", set_amplitude_f
///		"ra", set_rate_f
///		"po", set_position_f
///		"fi", set_file_f
///		"st", set_start_f
///		"en", set_stop_f
///		"at", set_attack_f
///		"de", set_decay_f
///
/// OSC note formats (6 or 14 arguments):
///			ampl, pos
///			ampl, pos, rate
///			ampl, pos, start, stop
///			ampl, pos, rate, start, stop
///			ampl, pos, start, stop, attack, decay
///			ampl, pos, rate, start, stop, attack, decay

#ifndef CSL_SndFile_Instrument_H
#define CSL_SndFile_Instrument_H

#include "Instrument.h"
#include "SoundFile.h"

namespace csl  {

///
/// Sound file player instruments w & w/out envelopes, playback-rate and inset positions
///

/// SndFileInstrument0 is the simple version
	
class SndFileInstrument0 : public Instrument {
public:
	SndFileInstrument0(string path);
	SndFileInstrument0(string folder, string path);
	~SndFileInstrument0();
	
						/// Plug functions
	void setParameter(unsigned selector, int argc, void **argv, const char *types);
						/// play note
	void play();
	void playOSC(int argc, void **argv, const char *types);
	void playNote(float ampl = 1, float pos = 0);
	void playMIDI(int chan, int key, int vel);

						/// These are the UGens of the DSP graph (i.e., the sndfile player instrument)
	BufferStream mPlayer;				///< sndfile (buffer) player
	Panner mPanner;						///< stereo panner
protected:
	void initialize(string path);
	SoundFile * mFile;					///< the sound file I read from
};

#ifdef STILL_TO_DO

class SndFileInstrument1 : public SndFileInstrument0 {
public:
	SndFileInstrument1(string path, int start = -1, int stop = -1);
	SndFileInstrument1(string folder, string path, int start = -1, int stop = -1);
	~SndFileInstrument1();
	
	/// Plug functions
	void setParameter(unsigned selector, int argc, void **argv, const char *types);
	/// play note
	void play();
	void playOSC(int argc, void **argv, const char *types);
	void playNote(float ampl = 1, float rate = 1, float pos = 0, int start = -1, int stop = -1,
				  float attack = 0.0, float decay = 0.0);
	void playMIDI(int chan, int key, int vel);
	
	/// These are the UGens of the DSP graph (i.e., the sndfile player instrument)
	AR mEnvelope;						///< AR envelope
	StaticVariable mRate;				///< plugs playback rate (ignored for now)
	int mStart, mStop;					///< start/stop sample indices
protected:
	void initialize(string path);
};

//class SampleFile : public SoundFile {
//	SampleFile();							/// Constructor
//	SampleFile(string path, unsigned MIDIKey, double frequency, double minRatio, double maxRatio);
//	~SampleFile();
//							/// Data members
//	unsigned mMIDIKey;			// sample's MIDI key #
//	double mFrequency;			// sample's actual frequency
//	double mMinRatio;			// min transp.
//	double mMaxRatio;			// max transp. (often 1.0)
//	
//	double ratioForKey(int desiredMIDI);			// answer rate ratios for the given MIDI or Hz transpositions
//	double ratioForPitch(int desiredMIDI);

///
/// Sample bank player instrument (untested)
///

class SampleBankInstrument : public SndFileInstrument1 {
public:
	SampleBankInstrument();
	~SampleBankInstrument();

	void addSample(SoundFile & sndFile, unsigned midi = 0, double freq = 0.0, double minRatio = 0.75);

	void play();		
	void playOSC(int argc, void **argv, const char *types);		
	void playNote(float ampl = 1, float rate = 1, float pos = 0, int start = -1, int stop = -1, 
				float attack = 0.0, float decay = 0.0);

protected:
//	SampleBank mBank;					///< sample bank map

};
	
#endif // STILL_TO_DO

}

#endif
