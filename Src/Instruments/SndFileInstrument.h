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
/// Sound file player instrument
///

class SndFileInstrument : public Instrument {
public:
	SndFileInstrument(string path, int start = -1, int stop = -1);
	SndFileInstrument(string folder, string path, int start = -1, int stop = -1);
	~SndFileInstrument();

								/// Plug functions
	void setParameter(unsigned selector, int argc, void **argv, const char *types);
								/// play note
	void play();		
	void playOSC(int argc, void **argv, const char *types);		
	void playNote(float ampl = 1, float rate = 1, float pos = 0, int start = -1, int stop = -1, 
				float attack = 0.0, float decay = 0.0);
	void playMIDI(int chan, int key, int vel);	

								///< These are the UGens of the DSP graph (i.e., the sndfile player instrument)
	SoundFile mPlayer;					///< sample player
	AR mEnvelope;						///< AR envelope
	Panner mPanner;						///< stereo panner
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
/// Sample bank player instrument
///

class SampleBankInstrument : public SndFileInstrument {
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

}

#endif
