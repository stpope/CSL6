//
// Instrument.cpp -- The CSL pluggable instrument class.
//	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
// 

#include "Instrument.h"

using namespace csl;

// Constructors

Instrument::Instrument() : 
			UnitGenerator(), 
			mGraph(0), 
			mName("New"), 
			mUGens(), 
			mEnvelopes(),
			mAccessors() { }

// copy constructor

Instrument::Instrument(Instrument& in) :
			UnitGenerator(in.frameRate(), in.numChannels()), 
			mGraph(in.graph()), 
			mName(in.name()) {
	UGenVector * en = in.envelopes();
	for (unsigned i = 0; i < en->size(); i++)
		mEnvelopes.push_back((*en)[i]);
	AccessorVector av = in.getAccessors();
	for (unsigned i = 0; i < av.size(); i++)
		mAccessors.push_back(av[i]);
	UGenMap * um = in.genMap();
	for (UGenMap::iterator it = um->begin(); it != um->end(); it++)
		mUGens[it->first] = it->second;
}
			
Instrument::~Instrument() { }

// Next_buffer is simply delegated to the receiver's graph

void Instrument::nextBuffer(Buffer & outputBuffer) throw (CException) {
	return (mGraph->nextBuffer(outputBuffer));
}

///< get a UGen from the graph

UnitGenerator * Instrument:: genNamed(string nam) {
	UGenMap::iterator pos = mUGens.find(nam); 
	if (pos == mUGens.end())
		return 0;
	return (pos->second);
}

// Answer whether any of the envelopes are active
// NB: This is a rare use of std::vector iteration (instead of an Accessor* for () loop) at call-back time.

bool Instrument::isActive() {
	if (mEnvelopes.empty()) { 
//		printf(" -- No envs -- "); fflush(stdout);
		return (mGraph->isActive());
	}
	for (UGenVector::iterator pos = mEnvelopes.begin(); pos != mEnvelopes.end(); ++pos) 
		if (((Envelope *) *pos)->isActive()) 
			return TRUE;
	return FALSE;
}

// Playing a note just means triggering all the instrument's envelopes

void Instrument::play() {
//	printf("\tInstr play\n");
	for (UGenVector::iterator pos = mEnvelopes.begin(); pos != mEnvelopes.end(); ++pos) 
		((Envelope *) *pos)->trigger();
}

void Instrument::release() {
	for (UGenVector::iterator pos = mEnvelopes.begin(); pos != mEnvelopes.end(); ++pos) 
		((Envelope *) *pos)->trigger();
}
