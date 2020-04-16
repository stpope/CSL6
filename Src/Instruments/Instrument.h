///
/// Instrument.h -- The CSL pluggable instrument class.
///	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
/// 
/// Instrument are reflective objects that hold onto a DSP graph (a UnitGen), a name, and a list of "plugs"
/// (Accessors, mapped to MIDI or OSC messages) for external control.
/// Instruments can be registered with MIDI voicers or in the OSC address space.

#ifndef INCLUDE_Instrument_H
#define INCLUDE_Instrument_H

#include "CSL_Includes.h"
#include "Accessor.h"

///
/// Keys for setParameter selector (default parameters for all notes)
///

#define set_duration_f	100			// basic accessors: dur, amp, freq, pos
#define set_amplitude_f	101
#define set_frequency_f	102
#define set_position_f	103			// position as a float or a point
#define set_position_p	104

#define set_attack_f	105			// ADSR envelope
#define set_decay_f		106
#define set_sustain_f	107
#define set_release_f	108

#define set_iattack_f	110			// 2nd ADSR envelope (index, mod)
#define set_idecay_f	111
#define set_isustain_f	112
#define set_irelease_f	113

#define set_index_f		114			// Various frequencies
#define set_c_freq_f	115
#define set_m_freq_f	116

#define set_vib_depth_f	120			// Vibrato, attack-chiff
#define set_chiff_amt_f	121
#define set_chiff_time_f 122

#define set_rate_f		125			// Sampler file, start, stop, rate
#define set_file_f		126
#define set_start_f		127
#define set_stop_f		128

#define set_partial_f	130			// SumOfSines partial and partial list
#define set_partials_f	131

namespace csl  {

///
/// Instrument class (abstract)
///
class Instrument : public UnitGenerator {
public:
					/// Constructors
	Instrument();
	Instrument(Instrument&);							///< copy constructor
	~Instrument();
					/// Accessors
	UnitGenerator * graph() { return mGraph; };			///< my UGen graph
	UGenMap * genMap() { return & mUGens; };			///< the map of ugens in the graph by name
	UGenVector * envelopes() { return & mEnvelopes; };	///< the vector of envelopes to query or trigger

	const string name() { return mName; };				///< answer my name
														/// answer the number of channels
	UnitGenerator * genNamed(string name);				///< get a UGen from the graph

					/// Accessor management
	AccessorVector getAccessors() { return mAccessors; };				///< answer the accessor vector
	unsigned numAccessors() { return mAccessors.size(); };				///< answer the number of accessors
	virtual void setParameter(unsigned selector, int argc, void **argv, const char *types) { };	///< set a named parameter
//	virtual float getParameter(unsigned selector);
	
	virtual void nextBuffer(Buffer & outputBuffer) throw (CException);	///< Sample creation
	
	virtual bool isActive();											///< Envelope query and re-trigger
	virtual void play();		
	virtual void playOSC(int argc, void **argv, const char *types) { };	///< Play a note (subclasses refine)
	virtual void playNote(int argc, void **argv, const char *types) { };	
	virtual void playMIDI(float dur, int chan, int key, int vel) { };	
	virtual void release();	

protected:			/// Caches
	UnitGenerator * mGraph;			///< my UGen graph
	string mName;					///< my name
	UGenMap mUGens;					///< the map of ugens in the graph by name
	UGenVector mEnvelopes;			///< the vector of envelopes to query or trigger
	AccessorVector mAccessors;		///< the accessor vector
};

}

#endif
