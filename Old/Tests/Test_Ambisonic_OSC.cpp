//
//	Test_Ambisonic_OSC.cpp -- C main functions for the CSL4 Ambisonic Encoding, Rotating & Decoding Demo.
//	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
//

#include <stdio.h>
#include "Test_Support.cpp"		// include all of CSL core and the test support functions
#include "OSC_support.cpp"			// include CSL OpenSoundControl support functions

#include "HOA_ALL.h"

// put all the global OSC stuff in a namespace

namespace ambiOSC {

// OSC callback for positioning an encoder (float azimuth, float elevation, float radius (not yet implemented))

void encoderOSC(void * user_data, int arglen, const void * vargs, OSCTimeTag when, NetworkReturnAddressPtr ra) {
	char * error;
	
	HOA_Encoder *ambiPtr = (HOA_Encoder *)user_data;
	
	// what kind of OSC types to expect (in this case, ',fff' for 3 floats)
	char * typeString = (char *) vargs; 	
	
	// the rest of the OSC mess age (i.e. the actual OSC values)
	void * otherArgs = (void *) OSCDataAfterAlignedString(typeString, typeString+arglen, &error);
	
	// verify that
	if (strcmp(typeString, ",fff")) {
		logMsg(kLogError, "ERROR: Invalid type string in OSC message, expected \",fff\" got \"%s\"", typeString);
	
		return;
	}
	//logMsg("OSC set azimuth, elevation of %u to %f, %f", sizeof(ambiPtr), ((float *) otherArgs)[0], ((float *) otherArgs)[1]);
	
	ambiPtr->setAzimuth(((float *) otherArgs)[0]);
	ambiPtr->setElevation(((float *) otherArgs)[1]);
	
	fflush(stdout);
}

// OSC callback for controlling a rotator (float rotate, float tumble, float tilt (not yet implemented))

void rotatorOSC(void * user_data, int arglen, const void * vargs, OSCTimeTag when, NetworkReturnAddressPtr ra) {
	char * error;
	
	HOA_Rotator *ambiPtr = (HOA_Rotator *)user_data;
	
	// what kind of OSC types to expect (in this case, ',fff' for 3 floats)
	char * typeString = (char *) vargs; 	
	
	// the rest of the OSC mess age (i.e. the actual OSC values)
	void * otherArgs = (void *) OSCDataAfterAlignedString(typeString, typeString+arglen, &error);
	
	// verify that
	if (strcmp(typeString, ",ff")) {
		logMsg(kLogError, "ERROR: Invalid type string in OSC message, expected \",ff\" got \"%s\"", typeString);
	//	fflush(stdout);
		return;
	}
	//logMsg("OSC set rotation, tumble & tilt of %s to %f, %f, %f", ambiPtr, ((float *) otherArgs)[0], ((float *) otherArgs)[1], ((float *) otherArgs)[2]);
	
	ambiPtr->setRotate(((float *) otherArgs)[0]);
	ambiPtr->setTumble(((float *) otherArgs)[1]);
	//ambiPtr->setTilt(((float *) otherArgs)[2]);	// not yet implemented
}
	

} // end of namespace ambiOSC

int main (int argc, const char * argv[]) {
	READ_CSL_OPTS;
	
	printf("********************************************\n");
	printf("Ambisonic Encoding, Rotating & Decoding Demo\n");

	const PaDeviceInfo* indevinfo = Pa_GetDeviceInfo(Pa_GetDefaultInputDeviceID());
	const PaDeviceInfo* outdevinfo = Pa_GetDeviceInfo(Pa_GetDefaultOutputDeviceID());
	printf("\nUsing device: ");
	printf(outdevinfo->name);
	printf(" i:%d ", indevinfo->maxInputChannels);
	printf(" o:%d ", outdevinfo->maxOutputChannels);
	printf("\n");

	gIO = new PAIO(44100, 64, Pa_GetDefaultInputDeviceID(), Pa_GetDefaultOutputDeviceID(), 16, 16);

	printf("\n...Making sources... \n");
	
	// source 1
							// The DSP graph is taken from the FM test example
	Sine source1, mod1(400);					// declare 2 oscillators
	Sawtooth a_env1(3, 0.5);
	a_env1.set_offset(0.5);
	Sawtooth i_env1(3.1, 400);	
	mod1.set_scale(i_env1);				// scale the modulator by its envelope
	mod1.set_offset(400);					// add in the base freq
	source1.set_frequency(mod1);				// set the carrier's frequency
	source1.set_scale(a_env1);					// scale the carrier's output by the amplitude envelope	
	
	printf("...Making encoder 1... \n");
	HOA_Encoder encoder1(source1, 1, CSL_PI/2.0, 0.0);
	
	// source 2
							// The DSP graph is taken from the FM test example
	Sine source2, mod2(378);					// declare 2 oscillators
	Sawtooth a_env2(9, 0.5);
	a_env2.set_offset(0.5);
	Sawtooth i_env2(8.1, 500);	
	mod2.set_scale(i_env2);				// scale the modulator by its envelope
	mod2.set_offset(400);					// add in the base freq
	source2.set_frequency(mod2);				// set the carrier's frequency
	source2.set_scale(a_env2);					// scale the carrier's output by the amplitude envelope	
	
	printf("...Making encoder 2... \n");
	HOA_Encoder encoder2(source2, 1, CSL_PI, 0.0);
	
	printf("...Making mixer... \n");
	HOA_Mixer ambiMix(encoder1);
	ambiMix.addInput(encoder2);
	
	printf("...Making rotator... \n");
	HOA_Rotator rotator(ambiMix);
	rotator.setRotate(0.f);
	
	printf("...Making speaker layout... \n");
	HOA_SpeakerLayout tetrahedralLayout; 
	// /cube.dat   /tetrahedron.dat  /mat4.dat	/reallyLargeLayout.dat /test9.dat
	
	printf("...Making decoder... \n");
	HOA_Decoder decoder(rotator, tetrahedralLayout, HOA_PROJECTION); // HOA_PSEUDOINVERSE, HOA_PROJECTION, HOA_INPHASE, HOA_BASIC, HOA_MAXRE
	
	printf("...Setting IO root ...");
	gIO->set_root(decoder);						// make some sound
	
	printf("...Initialising OSC...");
	int udp_port = 57574; //54321;
	init_OSC_addr_space();	// Set up OSC address space root
	
	// announce new OSC methods to control the sources & rotator
	OSCNewMethod("source1", OSCTopLevelContainer, ambiOSC::encoderOSC, &encoder1, &QueryResponseInfo);
	OSCNewMethod("source2", OSCTopLevelContainer, ambiOSC::encoderOSC, &encoder2, &QueryResponseInfo);
	OSCNewMethod("rotator", OSCTopLevelContainer, ambiOSC::rotatorOSC, &rotator, &QueryResponseInfo);
	
	printf("...Opening IO...");
	gIO->open();
	
	printf("...Starting IO...\n");
	gIO->start();	
	
	// start the OSC callbacks	
	main_OSC_loop(udp_port);	
	
	while(true) {
		sleep_sec(1);	// sleep for 1000ms
	}
	
	logMsg("CSL done.");
	gIO->stop();							// stop the driver and close down
	gIO->close();	

	return 0;								// exit
}
