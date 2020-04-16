//
//  SHARC.h -- SHARC (Sandell Harmonic Archive) timbre database sample classes
//	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
//
// SHARC is a collection of analyses of instrument (string, woodwind, and brass)
// tones taken from the McGill Univ. Master Sample CDs by Gregory Sandell.
//
// The top-level object, a SHARCLibrary, holds onto a collection is SHARCInstrument objects,
// and each of these has a number of SHARCSpectrum instances for the individual sample spectra.
//
// A SHARC spectrum is simply a list of partials, each of which has frequency, amplitude, and phase.
// The spectrum also has the actual pitch of the sampled note, the note's name, its MIDI pitch, and other useful data.
//
// A SHARC instrument is a spectrum collection.
// Each "instrument" represents a collection of spectra derived from samples of an orchestral instrument.
//
// The SHARCLibrary is the top-level library (instrument collection) class
// The top-level object, a SHARCLibrary, holds onto a collection is SHARCInstrument objects,
// and each of these has a number of SHARCSpectrum instances for the individual sample spectra.
//
// SHARC is a collection of analyses of instrument (string, woodwind, and brass) tones.
// The top-level directory has a subdirectory for each instrument.
// In each instrument directory, there is a table of contents file, and some number of spectrum files.
//
// SHARC CONTENTS files have a number of lines, each of which describes a single spectrum; the
// format of these lines is as follows:
//
//	 * Column 1: The note name string -- "c4" = middle C.
//	 * Column 2: The MIDI note number (where c4 = 48)
//	 * Column 3: Number of harmonics in the file
//	 * Column 4: The maximum absolute value of the sample segment.
//	 * Column 5: The nominal fundamental frequency for the pitch.
//	 * Column 6: The actual fundamental frequency.
//	 * Column 7: Volume number of the MUMS CDs from which this note comes
//	 * Column 8: MUMS track number
//	 * Column 9: MUMS index number
//	 * Column 10: Total duration (in seconds) of the performed note.
//	 * Column 11: The point in time from which the analysis was taken.
//	 * Column 12: the Spectral centroid in hertz
//
// Example CONTENTS file line
//	a#3  46  43  6367  233.082  231.496  2  8  1  3.080  2.124 1316.400
//
// Each spectrum file has num_partials lines, each of which is simply a magnitude (in dB relative to the loudest
// partial) and a phase value. The frequency of the partial is simply the base frequency of the sample times
// the partial's row number.
//
// Example spectrum file excerpt
//   0.00000   -1.17861
// -40.89460   -2.59737
//  -8.82998   -2.07608
// -47.88580   -1.99008
// -10.16250   -0.29849
//
// This structure is exactly mapped by the C++ implementation.
//
// Note: The implementation has UNIX-specific code in it.
//

#ifndef INCLUDE_SHARC_H
#define INCLUDE_SHARC_H

#include "CSL_Core.h"
#include "Oscillator.h"

#define MAX_PARTIALS 128
#define MAX_SPECTRA 64
#define MAX_INSTRUMENTS 40

namespace csl {

///
/// SHARC spectrum class
///

class SHARCSpectrum {

public:
	char * _note_name;
	unsigned _midi_key;
	float _nom_pitch;
	float _actual_pitch;
	unsigned _max_amp;
	unsigned _num_partials;
	Partial ** _partials;

	SHARCSpectrum(char * folder, char * name, unsigned m_key, float n_pitch, float a_pitch,
					unsigned m_amp, unsigned n_partials);
	~SHARCSpectrum();
	
	bool read_from_file(char * folder, char * name);
	unsigned count_partials();
	void dump_example();
};

///
/// SHARC instrument class
///

class SHARCInstrument {

public:				// Data members
	char * _name;
	unsigned _num_spectra;
	SHARCSpectrum ** _spectra;
					// Constructor
	SHARCInstrument(char * folder, char * name);
	~SHARCInstrument();
					// Accessing
	char * * spectrum_names();
	unsigned * spectrum_keys();
	float * spectrum_frequencies();
	SHARCSpectrum * spectrum_named(char * name);
	SHARCSpectrum * spectrum_with_key(unsigned key);
	SHARCSpectrum * spectrum_with_frequency(float freq);
					// For debugging
	unsigned count_spectra();
	unsigned count_partials();
	void dump_example();

private:
					// Load all the samples described in the given CONTENTS file
	bool read_from_TOC(char * folder, char * name);
};

///
/// SHARC library class
///

class SHARCLibrary {

public:				// Data members
	unsigned _num_instruments;
	SHARCInstrument ** _instruments;
					// Constructor
	SHARCLibrary();
	SHARCLibrary(char * name);
	~SHARCLibrary();
					// Accessing
	char * * instrument_names();
	SHARCInstrument * instrument_named(const char * name);
	SHARCSpectrum * spectrum_named(const char * inst, char * spect);
	void dump();
					// For debugging
	void dump_stats();
	void dump_example();
	
					// statics
	static SHARCLibrary* sSHARCLib;			///< The protected single instance of the HRTF Database
	static void loadDefault();
	static SHARCLibrary * library();
	static SHARCInstrument * instrument(char * instr);
	static SHARCInstrument * instrument(unsigned instr);
	static SHARCSpectrum * spectrum(const char * instr, char * note);
	static SHARCSpectrum * spectrum(const char * instr, unsigned note);
	static SHARCSpectrum * spectrum(unsigned instr, unsigned note);

private:
	bool read_from_directory(char * name);
};

}

#endif
