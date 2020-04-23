//
//  OscillatorBL.cpp -- Band-limited oscillators -- these can use on-demand sine summation, or store their wavetables 
//		(the latter choice might be dangerous with dynamic frequency control, of course)
//	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
//

#include "OscillatorBL.h"
#include <math.h>

using namespace csl;

SquareBL::SquareBL() : SumOfSines() {
}

SquareBL::SquareBL(float frequency) : SumOfSines(true, frequency) {
	this->setFrequency(frequency);
}

SquareBL::SquareBL(float frequency, float phase) : SumOfSines(true, frequency, phase) {
	this->setFrequency(frequency);
}

// Recompute the partials list whenever you change the frequency


void SquareBL::nextWaveInto(sample * dest, unsigned count, bool oneHz) {
// void SquareBL::nextBuffer(Buffer & outputBuffer, unsigned outBufNum) throw (CException) {
	unsigned max_harm = ((float) mFrameRate / (mInputs[CSL_FREQUENCY]->nextValue() * 2.0f ));
	if (mPartials.size() > max_harm)
		mPartials.erase(mPartials.begin() + (max_harm - 1), mPartials.end());
	else if (mPartials.size() < max_harm)
		for (unsigned i = mPartials.size(); i < max_harm; i += 2)
			this->addPartial((float)i, 0.5f / i);
	SumOfSines::nextWaveInto(dest, count, oneHz);
}

// If you start cacheing, then compute a new band-limited wavetable
/*
status SquareBL::nextWaveInto(sample * dest, unsigned count, bool oneHz) {
//	float incr = CSL_TWOPI / (count * mFrameRate);
	float incr = CSL_TWOPI / (float) count;
	sample frequency = mInputs[CSL_FREQUENCY]->nextValue();
	sample * out_ptr = dest;
	sample value;
	float max_harm;
	
	max_harm = floor(((float) mFrameRate) / (frequency * 2.0f ));
	for (unsigned i = 0; i < count; i++) {		// sample loop
		value = 0;
		for (float harm = 1.0; harm <= max_harm ; harm += 2)
			value +=  sin(harm * phase)  / harm ;
//		*out_ptr++ = (value * scale) + offset;
		*out_ptr++ = value;
		phase += incr;
	}
	while (phase > CSL_TWOPI)
		phase -= CSL_TWOPI;
}

ImpulseBL::ImpulseBL() : CompOrCacheOscillator() { }

unsigned ImpulseBL::next_buffer(sample * buffer, unsigned num_frames) {
	float* fp = (float*) buffer;
	float twopi = (CSL_PI * 2.0);

	_frequency->next_buffer(_frequency_buffer, num_frames);
	_phase->next_buffer(_phase_buffer, num_frames);
// _duty->next_buffer(_duty_buffer, num_frames);

	float rateRecip = 1.0/_rate;
	float scale = 0.025/CSL_PI; //4.0 / (CSL_PI );

	// generate buffer_size 32-bit floats
	for (unsigned i = 0; i < num_frames; i++) {
		float nextFloat = 1.0;
		unsigned num_of_max_harmonic = (unsigned) floor(_rate / (_frequency_buffer[i] * 2)  );

		for (unsigned which_harmonic = 1; which_harmonic <= num_of_max_harmonic; which_harmonic++) {
//		for (int which_harmonic = 0; which_harmonic <10; which_harmonic++) {

			//float gibbs = cos((which_harmonic-1)*CSL_PI/2/num_of_max_harmonic);
			nextFloat +=   sin(which_harmonic *  _phase_accumulator);// * gibbs * gibbs ;
		}
		(*fp++) = scale * nextFloat;	// set all the channels equal to this float

		static float max_amp = 0;
		max_amp = (max_amp > scale * nextFloat) ? max_amp : scale * nextFloat;
		// twopi * the normalized frequency + phase
		_phase_accumulator += twopi * (_frequency_buffer[i] * rateRecip) + _phase_buffer[i];

		// wraparound after 2pi radians
		while (_phase_accumulator > twopi)
			_phase_accumulator -= twopi;
	}
	return num_frames;
}

TriangleBL::TriangleBL() : CompOrCacheOscillator() { }

unsigned TriangleBL::next_buffer(sample * buffer, unsigned num_frames) {
	float* fp = (float*) buffer;
	float twopi = (CSL_PI * 2.0);

	_frequency->next_buffer(_frequency_buffer, num_frames);
	_phase->next_buffer(_phase_buffer, num_frames);
// _duty->next_buffer(_duty_buffer, num_frames);

	float rateRecip = 1.0/_rate;
	float scale = 8.0 / (CSL_PI * CSL_PI);
			// generate buffer_size 32-bit floats
	for (unsigned i = 0; i < num_frames; i++) {
		float nextFloat = 0.0;
		unsigned num_of_max_harmonic = (unsigned) (_rate / (_frequency_buffer[i]  * 2) );
		for (unsigned which_harmonic = 1; which_harmonic <= num_of_max_harmonic; which_harmonic += 2) {
//		for (int which_harmonic = 0; which_harmonic <10; which_harmonic++) {
			nextFloat +=   sin(which_harmonic * _phase_accumulator ) / (which_harmonic * which_harmonic );
		}
		(*fp++) = scale * nextFloat;	// set all the channels equal to this float
		static float max_amp = 0;
		max_amp = (max_amp > scale * nextFloat) ? max_amp : scale * nextFloat;
				// twopi * the normalized frequency + phase
		_phase_accumulator += twopi * (_frequency_buffer[i] * rateRecip) + _phase_buffer[i];
				// wraparound after 2pi radians
		while (_phase_accumulator > twopi)
				_phase_accumulator -= twopi;
	}
	return num_frames;
}

*/
