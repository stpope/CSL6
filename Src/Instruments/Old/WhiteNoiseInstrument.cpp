//
// WhiteNoiseInstrument.cpp -- Simple WhiteNoise example instrument class.
//	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
// 
// Implementation of the filtered WhiteNoise example as an instrument class.
// This example shows how to create an instrument class from a DSP graph and set up its accessors for use with OSC.

#include "WhiteNoiseInstrument.h"

using namespace csl;

// The constructor initializes the DSP graph's UGens

WhiteNoiseInstrument::WhiteNoiseInstrument() :		// initializers for the UGens
		_a_env(3, 0.1, 0.1, 0.5, 1),		// set up a standard ADSR (3-sec duration)
		_sig(),								// init the white noise generator
		_lpfilter(_sig, _sig.sample_rate(),Butter::BW_LOW_PASS, 1000),	// and filter the output

		_a_mul(_lpfilter, _a_env)			// scale the filter's output by the amplitude envelope
{
	_name = (char *) malloc(CSL_NAME_LEN);
	strcpy(_name, "Basic_WhiteNoise");
	_graph = & _a_mul;						// store the root of the graph as the inst var _graph
											// now set up the null-terminated list of envelopes
	_envelopes = (Envelope **) malloc (2 * sizeof(void *));
	_envelopes[0] = & _a_env;
	_envelopes[1] = 0;
}

// The destructor frees the stuff we allocated

WhiteNoiseInstrument::~BasicWhiteNoiseInstrument() {
	free(_name);		free(_envelopes);
}

// Plug function for use by OSC setter methods

void WhiteNoiseInstrument::set_parameter(unsigned selector, void * d) {
	switch (selector) {						// switch on which parameter is being set
		case set_duration_f:
			_a_env.set_duration(* (float *)d); break;
		case set_amplitude_f:
			_a_env.scale_values(* (float *)d); break;
		case set_c_freq_f:
			_lpfilter.set_cutoff_frequency(* (float *)d); break;	
		case set_attack_f:
			_a_env.set_attack(* (float *)d); break;
		case set_decay_f:
			_a_env.set_decay(* (float *)d); break;
		case set_sustain_f:
			_a_env.set_sustain(* (float *)d); break;
		case set_release_f:
			_a_env.set_release(* (float *)d); break;
		default:
			logMsg(kLogError, "Unknown selector in BasicWhiteNoiseInstrument set_parameter selector: %d\n", selector);
	 }
}

// Answer the number of and the list of accessors

unsigned BasicWhiteNoiseInstrument::num_accessors() { return 13; };

// Fill in the list of accessor objects

void WhiteNoiseInstrument::get_accessors(Accessor ** list) {
	unsigned int n = 0;
	list[n++] = new Accessor("du", set_duration_f, CSL_FLOAT_TYPE);
	list[n++] = new Accessor("am", set_amplitude_f, CSL_FLOAT_TYPE);
	list[n++] = new Accessor("cf", set_c_freq_f, CSL_FLOAT_TYPE);
	list[n++] = new Accessor("aa", set_attack_f, CSL_FLOAT_TYPE);
	list[n++] = new Accessor("ad", set_decay_f, CSL_FLOAT_TYPE);
	list[n++] = new Accessor("as", set_sustain_f, CSL_FLOAT_TYPE);
	list[n++] = new Accessor("ar", set_release_f, CSL_FLOAT_TYPE);
}

// Play a note with a given arg list
// Formats (5 or 13 arguments):
// 	dur, ampl, c_fr, m_fr, ind
// 	dur, ampl, c_fr, m_fr, ind, att, dec, sus, rel, i_att, i_dec, i_sus, i_rel

void WhiteNoiseInstrument::play_osc_note(char * types, void * args, char * endOfArgs) {
	float ** fargs = (float **) args;
	unsigned nargs;
	if (strcmp(types, ",fff") == 0)
		nargs = 3;
	else if (strcmp(types, ",fffffff") == 0)
		nargs = 7;
	else {
		logMsg(kLogError, "Invalid type string in OSC message, expected \",i\" got \"%s\"\n", types);
		return;
	}
	printf("\tWhiteNoiseInstr: PN: %g %g %g %g %g\n", *fargs[0], *fargs[1], *fargs[2]);
	_a_env.set_duration(*fargs[0]);
	_a_env.scale_values(*fargs[1]);
	_lpfilter.set_cutoff_frequency(*fargs[2]);
	if (nargs == 13) {
		_a_env.set_attack(*fargs[3]);
		_a_env.set_decay(*fargs[4]);
		_a_env.set_sustain(*fargs[5]);
		_a_env.set_release(*fargs[6]);
	}
	_a_env.reset();			// now trigger the envelopes!
}
