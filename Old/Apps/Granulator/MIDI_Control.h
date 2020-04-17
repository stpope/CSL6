//
//  MIDI_Control.h -- Controls granular synthesis app with MIDI sliders
//	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
//

#ifndef CSL_GRANMIDI_H
#define CSL_GRANMIDI_H

#include "juce.h"
#include <map>

class CSLComponent;		// forward declaration
class RangeSlider;

typedef std::map<unsigned, RangeSlider *> SliderMap;

//
// MIDIControllerMapper class
//	currently hard-wired to send MIDI controller messages to a CSLComponent (the granulator GUI)
//

class MIDIControllerMapper : public MidiInputCallback {
public:
	MIDIControllerMapper(CSLComponent * component, int deviceIndex, int baseChan);
	~MIDIControllerMapper();
					// callback method sets sliders in component
	void handleIncomingMidiMessage(MidiInput * source, const MidiMessage & message);

					// the granulator component I control
	CSLComponent * comp;
	MidiInput * theIn;
	SliderMap sliderMap;
	
};

#endif
