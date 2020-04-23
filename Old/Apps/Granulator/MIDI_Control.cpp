//
//  MIDI_Control.h -- Controls granular synthesis app with MIDI sliders
//	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
//

#include "MIDI_Control.h"
#include "JGranulator.h"
#include "JCSL_Widgets.h"

// Constructor

MIDIControllerMapper :: MIDIControllerMapper(CSLComponent * component, int deviceIndex, int baseChan) {
	comp = component;
	unsigned i;
	
	StringArray midiDeviceNames = MidiInput::getDevices();
	for(i = 0; i < midiDeviceNames.size(); i++)
		printf("\t\t%d = %s\n", i, midiDeviceNames[i].toRawUTF8());
 
	theIn = MidiInput::openDevice(deviceIndex, this);
	if (theIn == NULL)
		printf("MIDI input open failed...\n");
	theIn->start();
	
	i = baseChan;						// base of MIDI sliders
	sliderMap[i++] = comp->rateSlider;
	sliderMap[i++] = comp->rateSlider;
	sliderMap[i++] = comp->offsetSlider;
	sliderMap[i++] = comp->offsetSlider;
	sliderMap[i++] = comp->durationSlider;
	sliderMap[i++] = comp->durationSlider;
	sliderMap[i++] = comp->densitySlider;
	sliderMap[i++] = comp->densitySlider;
	sliderMap[i++] = comp->widthSlider;
	sliderMap[i++] = comp->widthSlider;
	sliderMap[i++] = comp->volumeSlider;
	sliderMap[i++] = comp->volumeSlider; 
	sliderMap[i++] = comp->envelopeSlider;
	sliderMap[i++] = comp->envelopeSlider;
}

MIDIControllerMapper :: ~MIDIControllerMapper() { }

// input callback

void MIDIControllerMapper :: handleIncomingMidiMessage(MidiInput * source, const MidiMessage & message) {
	int chan, value;
	float fValue;
	RangeSlider * theSlider;
	const MessageManagerLock mmLock;		// since we twiddle the slider values from this method
	
							// filter out all non-control messages
	if ( ! message.isController()) {
		printf("MIDI message ignored\n");
		return;
	}
	chan = message.getControllerNumber();
 	value = message.getControllerValue();
							// switch sends setters to GUI sliders
	theSlider = sliderMap[chan];
	if (theSlider == NULL) {
		printf("MIDI controller ignored\n");
		return;
	}
//	printf("Ctrl: %d [%s] = %d\n", chan, theSlider->getName().toUTF8(), value);
	fValue = theSlider->getMinimum() + (((float) value / 127.0f) * (theSlider->getMaximum() - theSlider->getMinimum()));
	if(chan % 2 == 0)				// even values = set base
		theSlider->setBaseValue(fValue);
	else							// odd values = set range
		theSlider->setRangeValue(fValue);
}
