//
//	Test_Ambisonic.cpp -- C main functions for the CSL4 Ambisonic Encoding, Rotating & Decoding Demo.
//	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
//

#include <stdio.h>
//#include "Test_Support.cpp"		// include all of CSL core and the test support functions
#include "CSL_Includes.h"		// include all of CSL core
#include "PAIO.h"
#include "Ambisonic.h"
#include "AmbisonicUtilities.h"

using namespace csl;

//
// Main
//

int main(int argc, const char * argv[]) {
//	READ_CSL_OPTS;
	printf("\n********************************************\n");
	printf("\nAmbisonic Encoding, Rotating & Decoding Demo\n");

	printf("\n...Making sources... \n");
	
	// set up the DAC
	int inchannels = 2;
	int outchannels = 16;
	int samplerate = 44100;
	int blocksize = 64;
	
	Pa_Initialize();
	const PaDeviceInfo* indevinfo = Pa_GetDeviceInfo(Pa_GetDefaultInputDevice());
	const PaDeviceInfo* outdevinfo = Pa_GetDeviceInfo(Pa_GetDefaultOutputDevice());
	inchannels = inchannels > indevinfo->maxInputChannels ? indevinfo->maxInputChannels : inchannels; 
	outchannels = outchannels > outdevinfo->maxOutputChannels ? outdevinfo->maxOutputChannels : outchannels; 
	
	printf("\nUsing device: ");
	printf(outdevinfo->name);
	printf(" i:%d ", inchannels);
	printf(" o:%d ", outchannels);
	printf("\n");
	if (inchannels < 2 || outchannels < 2) { printf("ERROR, less than 2 channel soundcard!"); return 0; }

	PAIO * gIO = new PAIO(samplerate, blocksize, 
						Pa_GetDefaultInputDevice(), Pa_GetDefaultOutputDevice(), 
						inchannels, outchannels);
	
			// source 1 - birdy noise
	double fmfcenter1 = 2000;		// centre frequency of the FM modulator
	double pulseRate1 = 2.5;		// pulses of sound per second
	double fmaScale1 = 0.2;		// half the loudness of the final sound
	
			// FM Modulator Modulator
	Sine fmfm1;
	fmfm1.setFrequency(pulseRate1 * 0.01);
	fmfm1.setOffset(fmfcenter1*2.0);
	fmfm1.setScale(fmfcenter1 * 0.25);

			// FM Modulator
	Sawtooth fmf1;		
	fmf1.setFrequency(pulseRate1 * 16);				
	fmf1.setOffset(fmfm1);		//fmfcenter1*2.0	
	fmf1.setScale(-fmfcenter1);			

			// Carrier Amplitude Envelope
	Sawtooth fma1;
	fma1.setFrequency(pulseRate1);
	fma1.setOffset(fmaScale1);
	fma1.setScale(-fmaScale1);

			// FM Carrier
	Sine source1;
	source1.setFrequency(fmf1);				// set the carrier's frequency
	source1.setScale(fma1);					// scale the carrier's output by the amplitude envelope	

	printf("...Making encoder 1... \n");
	SpatialSource sp_src1(source1, 90.0);		// src * 90 degrees
	AmbisonicEncoder encoder1(sp_src1, 2);

			// source 2 - zippy noise
	double fmfcenter2 = fmfcenter1 / 8.0;		// centre frequency of the FM modulator
	double pulseRate2 = pulseRate1 / 4.0;		// pulses of sound per second
	double fmaScale2 = fmaScale1;		// half the loudness of the final sound

			// FM Modulator Modulator
	Sawtooth fmfm2;
	fmfm2.setFrequency(pulseRate2);
	fmfm2.setOffset(pulseRate2 * 1.0);
	fmfm2.setScale(16.0);

			// FM Modulator
	Sine fmf2;		
	fmf2.setFrequency(fmfm2);				
	fmf2.setOffset(fmfcenter2*2.0);			
	fmf2.setScale(-fmfcenter2);			

			// Carrier Amplitude Envelope
	Sawtooth fma2;
	fma2.setFrequency(pulseRate2);
	fma2.setOffset(fmaScale2);
	fma2.setScale(-fmaScale2);

			// FM Carrier
	Sawtooth source2;
	source2.setFrequency(fmf2);				// set the carrier's frequency
	source2.setScale(fma2);					// scale the carrier's output by the amplitude envelope	

	printf("...Making encoder 2... \n");
	SpatialSource sp_src2(source2, 90.0);		// src * 90 degrees
	AmbisonicEncoder encoder2(sp_src2, 2);
	
	printf("...Making mixer... \n");
	AmbisonicMixer ambiMix(2);
	ambiMix.addInput(encoder1);
	ambiMix.addInput(encoder2);
	
	printf("...Making rotator... \n");
	AmbisonicRotator rotator(ambiMix);
	rotator.setRotate(0.f);
	
	printf("...Making speaker layout... \n");
													// default speaker layout matches the layout used in MAT's 4th Floor lab
	SpeakerLayout * speakerlayout = SpeakerLayout::defaultSpeakerLayout();		
	//	speakerlayout.limitSpeakers(outchannels);		// if there aren't enough channels on the DAC, ignore the extra speakers (HACK)
	
	printf("...Making decoder... \n");
	AmbisonicDecoder decoder(rotator, speakerlayout);
	
	printf("...Setting IO root ...");
	gIO->setRoot(decoder);						// make some sound
	
	printf("...Opening IO...");
	gIO->open();
	
	printf("...Starting IO...\n");
	gIO->start();	
	
//	encoder2.setAzimuth(0.0);			// in front
//	encoder2.setAzimuth(CSL_PI / 8.0);	// 22.5' right
//	encoder2.setAzimuth(CSL_PI / 4.0);	// 45' right
//	encoder2.setAzimuth(CSL_PI / 2.0);	// right
//	encoder2.setElevation(0.0);			// on horizontal plane
//	encoder2.setElevation(CSL_PI / 8.0);	// 22.5' up
//	encoder2.setElevation(CSL_PI / 4.0);	// 45' up
//	encoder2.setElevation(CSL_PI / 2.0);	// up
	
	int i=0;		// go into the infinite loop ommmmm
	while(true) {
					// move the sources around
		sp_src1.setAzimuth		(fmod(sp_src1.azimuth()	+ 0.001, CSL_TWOPI));		
		sp_src2.setElevation	(fmod(sp_src2.elevation()	+ 0.001, CSL_TWOPI));
		
		if ( i-- == 0 ) { 
			i = 1000; // comment every n iterations
			printf("set azimuth   of birdy noise = %f\n", sp_src1.azimuth()); 
			printf("set elevation of zippy noise = %f\n", sp_src2.elevation()); 
		}
		sleepUsec(1000);	// sleep for n microseconds
	}
	
	logMsg("CSL done.");
	gIO->stop();							// stop the driver and close down
	gIO->close();	

	return 0;								// exit
}
