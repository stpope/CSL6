//
//  SHARC.cpp -- SHARC timbre database sample classes
//	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
//
#include "SHARC.h"

#include <math.h>
#include <stdlib.h>
#ifndef CSL_WINDOWS
#include <unistd.h>
#endif
#include <sys/stat.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>

using namespace csl;

// SHARCSpectrum implementation
// Populating constructor

SHARCSpectrum::SHARCSpectrum(char * folder, char * name, unsigned m_key,
					float n_pitch, float a_pitch, unsigned m_amp, unsigned n_partials) :
					_midi_key(m_key), _nom_pitch(n_pitch), _actual_pitch(a_pitch),
					_max_amp(m_amp), _num_partials(n_partials) {
	 _note_name = (char *) malloc(strlen(name) + 1);
	 strcpy(_note_name, name);
	_partials = (Partial **) malloc(n_partials * (sizeof(char *)));
	this->read_from_file(folder, name);
}

SHARCSpectrum::~SHARCSpectrum() { 
	free(_note_name);
	free(_partials);
}
	
// File reader

bool SHARCSpectrum::SHARCSpectrum::read_from_file(char * folder, char * name) {
	FILE * inp;
	char cmd[CSL_NAME_LEN];
	char f_name[CSL_NAME_LEN];
	float amp, phas;
	Partial * harm;
	unsigned p_count = 0;
	
	sprintf(cmd, "ls %s/*%s.spect", folder, name);
	inp = popen(cmd, "r");
	if (inp == NULL) {
		printf("\tError reading command %s\n", cmd);
		return false;
	}
	char * retC = fgets(f_name, CSL_NAME_LEN, inp);
	pclose(inp);
	unsigned len = strlen(f_name);
	if ((retC == NULL) || (len < 2)) {
//		printf("\tError finding file %s\n", cmd);
		return false;
	}
	if (f_name[len - 1] == '\n')
		f_name[len - 1] = 0;
	inp = fopen(f_name, "r");
	if (inp == NULL) {
		printf("\tError opening file %s\n", f_name);
		return false;
	}
	while( ! feof(inp)) {
		fscanf(inp, "%f %f\n", &amp, &phas);
		harm = (Partial *) malloc(sizeof(Partial));
		harm->number = p_count + 1;
										// Scale the negative dB value to the range 0-1
		harm->amplitude = (float) pow(10.0, (((double) amp) / 20.0));
		harm->phase = phas;
		_partials[p_count++] = harm;
	}
//	printf("\tRead %d harmonics (exp %d) for spectrum %s\n", p_count, _num_partials, name);
	fclose(inp);
	if (p_count != _num_partials) {
		printf("\tError: number of partials found (%d) not what I expected (%d) for %s in %s\n",  p_count, _num_partials, name, folder);
//		return false;
	}
	return true;
}

unsigned SHARCSpectrum::count_partials() {
	return _num_partials;
}

void SHARCSpectrum::dump_example() {
	Partial * harm;	
	printf("\tFR @ AMP     @ PHA\n");
	for (unsigned i = 0; i < _num_partials; i++) {
		harm = _partials[i];
		printf("\t%d  @ %.5f @ %.5f\n", ((int) harm->number), harm->amplitude, harm->phase);
	}
}

// SHARCInstrument implementation
// Constructor, etc

SHARCInstrument::SHARCInstrument(char * folder, char * name) {
	_num_spectra = 0;
	_spectra = (SHARCSpectrum **) malloc(MAX_SPECTRA * (sizeof(char *)));
	this->read_from_TOC(folder, name);
}

SHARCInstrument::~SHARCInstrument() {
	free(_spectra);
}

// Load all the samples described in the given CONTENTS file

bool SHARCInstrument::read_from_TOC(char * folder, char * name) {
	   FILE * input;
	   float real_freq, nom_freq, dur, start, centroid;
	   unsigned note_num, num_harm, m1, m2, m3, max_amp;
	   char note_name[8], f_name[256];
	   SHARCSpectrum * spect;
	   _name = (char *) malloc(strlen(name) + 1);
	   strcpy(_name, name);
	   _num_spectra = 0;
	   sprintf(f_name, "%s/%s/CONTENTS", folder, name);
								// Open the instrument's contents file
	   input = fopen(f_name, "r");
	   if (input == NULL) {
		   printf("\tError opening file %s\n", f_name);
		   return false;
	}
	   sprintf(f_name, "%s/%s", folder, name);
								// Read through it line by line, loading the spectra described by the lines
	while( ! feof(input)) {
		fscanf(input, "%s %d %d %d %f %f %d %d %d %f %f %f\n",
			note_name, &note_num, &num_harm, &max_amp, &nom_freq, &real_freq,
			&m1, &m2, &m3, &dur, &start, &centroid);
		spect = new SHARCSpectrum(f_name, note_name, note_num, nom_freq, real_freq,
									max_amp, num_harm);
		_spectra[_num_spectra++] = spect;
	}
//	printf("\tRead %d spectra for sample %s\n", _num_spectra, _name);
	fclose(input);
	return true;
}

// Accessing utilities

char * * SHARCInstrument::spectrum_names(){
	char * * names = (char * *) malloc(_num_spectra * (sizeof(char *)));
	
	for (unsigned i = 0; i < _num_spectra; i++) {
		names[i] = _spectra[i]->_note_name;
	}
	return names;
}

unsigned * SHARCInstrument::spectrum_keys() {
	unsigned * names = (unsigned *) malloc(_num_spectra * (sizeof(unsigned)));
	
	for (unsigned i = 0; i < _num_spectra; i++) {
		names[i] = _spectra[i]->_midi_key;
	}
	return names;
}

float * SHARCInstrument::spectrum_frequencies() {
	float * names = (float *) malloc(_num_spectra * (sizeof(float)));
	
	for (unsigned i = 0; i < _num_spectra; i++) {
		names[i] = _spectra[i]->_actual_pitch;
	}
	return names;
}

SHARCSpectrum * SHARCInstrument::spectrum_named(char * name) {
	for (unsigned i = 0; i < _num_spectra; i++) {
		if ( ! strcmp(_spectra[i]->_note_name, name)) {
			return  _spectra[i];
		}
	}
	return (SHARCSpectrum *) 0;
}

SHARCSpectrum * SHARCInstrument::spectrum_with_key(unsigned key) {
	for (unsigned i = 0; i < _num_spectra; i++) {
		if (_spectra[i]->_midi_key == key) {
			return  _spectra[i];
		}
	}
	return (SHARCSpectrum *) 0;
}

SHARCSpectrum * SHARCInstrument::spectrum_with_frequency(float freq) {
	for (unsigned i = 0; i < _num_spectra; i++) {
		if (_spectra[i]->_actual_pitch == freq) {
			return  _spectra[i];
		}
	}
	return (SHARCSpectrum *) 0;
}

// Debugging functions

unsigned SHARCInstrument::count_spectra() {
	return _num_spectra;
}

unsigned SHARCInstrument::count_partials() {
	unsigned part_c;
	SHARCSpectrum * spect;
	part_c = 0;
	for (unsigned i = 0; i < _num_spectra; i++) {
		spect = _spectra[i];
		part_c += spect->count_partials();
	}
	   return part_c;
}

void SHARCInstrument::dump_example() {
	SHARCSpectrum * spect;
	spect = _spectra[2];
	printf("\tSample spectrum for instrument %s sample %s\n", _name, spect->_note_name);
		spect->dump_example();
}

// SHARCLibrary implementation

SHARCLibrary::SHARCLibrary() { }

SHARCLibrary::~SHARCLibrary() { 
	free(_instruments);
}

// Populating constructor

SHARCLibrary::SHARCLibrary(char * name) {
	_num_instruments = 0;
	_instruments = (SHARCInstrument ** ) malloc(MAX_INSTRUMENTS * (sizeof(char *)));
	this->read_from_directory(name);
}

// File reader -- open the named directory and load all subdirectories

bool SHARCLibrary::read_from_directory(char * folder) {
	SHARCInstrument * inst;
	DIR * dir_p;
	struct stat stat_p;
	struct dirent * entry_p;
	char f_name[256];
					// Iterate over the given folder and its sub-folders	
	fprintf(stderr, "Loading SHARC database from folder %s\n", folder);
	dir_p = opendir(folder);
	while(NULL != (entry_p = readdir(dir_p))) {
		if (entry_p->d_name[0] != '.') {			// ignore . and ..
		//	printf("Examining %s \t", entry_p->d_name);
			sprintf(f_name, "%s/%s", folder, entry_p->d_name);
					// Stat the file, check if it's a directory
			if (stat (f_name, &stat_p) < 0) {
				printf("\tError checking file %s\n", f_name);
				return false;
			}
					// If yes, load the instrument in the subdirectory
			if (S_ISDIR(stat_p.st_mode)) {
				inst = new SHARCInstrument(folder, entry_p->d_name);
				_instruments[_num_instruments++] = inst;
			//}  else {
			//	printf("\tFile %s is not a directory\n", f_name);
			}
		}
	}
	closedir(dir_p);
//	printf("Read %d instruments %s\n", _num_instruments, folder);
	return true;
}

// Accessing utilities

char ** SHARCLibrary::instrument_names() {
	char * * names = (char * *) malloc(_num_instruments * (sizeof(char *)));
	
	for (unsigned i = 0; i < _num_instruments; i++) {
		names[i] = _instruments[i]->_name;
	}
	return names;
}

SHARCInstrument * SHARCLibrary::instrument_named(const char * name) {
	for (unsigned i = 0; i < _num_instruments; i++) {
		if ( ! strcmp(_instruments[i]->_name, name)) {
			return  _instruments[i];
		}
	}
	return (SHARCInstrument *) 0;
}

SHARCSpectrum * SHARCLibrary::spectrum_named(const char * inst, char * spect) {
	SHARCInstrument * in = this->instrument_named(inst);
	if (in == NULL) return (SHARCSpectrum *) 0;
	return (in->spectrum_named(spect));
}

// Print out the library sizes

void SHARCLibrary::dump_stats() {
	unsigned inst_c, spect_c, part_c;
	SHARCInstrument * inst;
	inst_c = spect_c = part_c = 0;
	for (unsigned i = 0; i < _num_instruments; i++) {
		inst = _instruments[i];
		inst_c++;
		spect_c += inst->count_spectra();
		part_c += inst->count_partials();
	}
	logMsg("Loaded SHARC library = %d instruments, %d spectra, %d partials\n", inst_c, spect_c, part_c);
}

// Dump an example spectrum

void SHARCLibrary::dump_example() {
	SHARCInstrument * inst;
	inst = _instruments[7];
	inst->dump_example();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////

/// Global SHARC library

SHARCLibrary * SHARCLibrary::sSHARCLib = 0;

void SHARCLibrary::loadDefault() {
	if (sSHARCLib)
		return;
	string folder(CGestalt::dataFolder());
	folder += "SHARC";						// load the SHARC library
	sSHARCLib = new SHARCLibrary((char *)folder.c_str());
	sSHARCLib->dump_stats();
}

// Get the global library ptr

SHARCLibrary * SHARCLibrary::library() {
	if ( ! sSHARCLib)				// initialize lazily
		loadDefault();
	return sSHARCLib;
}
	
// Library and Instrument accessors

SHARCInstrument * SHARCLibrary::instrument(char * instr) {
	if ( ! sSHARCLib)
		loadDefault();
	return sSHARCLib->instrument_named(instr);
}

SHARCInstrument * SHARCLibrary::instrument(unsigned instr) {
	if ( ! sSHARCLib)
		loadDefault();
	return sSHARCLib->instrument_named(sSHARCLib->instrument_names()[instr]);
}

SHARCSpectrum * SHARCLibrary::spectrum(const char * instr, char * note) {
	if ( ! sSHARCLib)
		loadDefault();
	SHARCInstrument * inst = sSHARCLib->instrument_named(instr);
	return inst->spectrum_named(note);
}

SHARCSpectrum * SHARCLibrary::spectrum(const char * instr, unsigned note) {
	if ( ! sSHARCLib)
		loadDefault();
	SHARCInstrument * inst = sSHARCLib->instrument_named(instr);
	return inst->spectrum_with_key(note);
}

SHARCSpectrum * SHARCLibrary::spectrum(unsigned instr, unsigned note) {
	if ( ! sSHARCLib)
		loadDefault();
	SHARCInstrument * inst = sSHARCLib->instrument_named(sSHARCLib->instrument_names()[instr]);
	return inst->spectrum_with_key(note);
}

// dump

void SHARCLibrary::dump() {
	SHARCInstrument * inst;
	char ** i_names, ** s_names;
	unsigned i;
	i_names = instrument_names();
	
	printf("Names of instruments:\n");			// print the list of instruments
	for (i = 0; i < _num_instruments; i++)
		printf("\t%s\n", i_names[i]);
	inst = instrument_named(i_names[7]);
	
	printf("Names of samples in instrument %s:\n", inst->_name);
	s_names = inst->spectrum_names();			// print the notes in a selected instrument
	for (i = 0; i < inst->_num_spectra; i++)
		printf("\t%s\n", s_names[i]);
}
