//
//	ProtoAuralizer.cpp --  Test program for Convolver, Geometer, LateReverb, and Spatializer classes
//	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
//
//  Brent Lehman -- 19 February 2004
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "CSL_Core.h"
#include "SoundFile.h"
#include "PAIO.h"
#include "FileIO.h"
#include "Convolver.h"
#include "Geometer.h"
#include "LateReverb.h"
#include "Spatializer.h"

using namespace csl;

int main(int argc, char** argv) {
	int i;
	char* roomfilename;
	int blocksize;
	float A, delta, rho, mix;
	int stringlength;
	Geometer* geometer;
	Spatializer* spatializer;  // One spatializer for each ear
	Spatializer* spatializer2;
	LateReverb* latereverb;
	Convolver* convolver;  // One convolver for each ear
	Convolver* convolver2;
	Joiner* joiner;
	IO * io;
	soundray* soundrays;
	int numrays;
	Buffer* early_ir;  // One early IR for each ear
	int early_ir_length;
	Buffer* early_ir2;
	int early_ir_length2;
	Buffer* late_ir;  // One late IR for each ear
	int late_ir_length;
	Buffer* late_ir2;
	int late_ir_length2;
	Buffer* full_ir;  // One complete IR for each ear
	int full_ir_length;
	Buffer* full_ir2;
	int full_ir_length2;
	sample* ir_samples;  // We need to have a Buffer *and* be able to point to its samples
	sample* ir_samples2;
	short* tempsamples;
	FILE* earlyirfile = fopen("early.raw", "wb");
	FILE* lateirfile = fopen("late.raw", "wb");
	FILE* fullirfile = fopen("full.raw", "wb");

	if (argc < 9) {
		printf("Usage: ProtoAuralizer <soundfile> <roomfile> <kemarpath> <absorption> <transfer> \
			<density> <earlylatemix> <blocksize>\n");
		exit(1);
	}

	// Get input sound
	SoundFile* soundfile = new SoundFile(argv[1]);
	soundfile->open_for_read();
	if (!soundfile->_isValid) {
		printf("Can't open sound file %s\n", argv[1]);
		exit(1);
	}

	// Get room specification
	stringlength = strlen(argv[2]);
	roomfilename = (char*)malloc(stringlength*sizeof(char));
	strcpy(roomfilename, argv[2]);

	// Get late reverb parameters
	A = atof(argv[4]);
	if (A == 0.0) {
		printf("The absorption parameter must be greater than zero.\n");
		exit(1);
	}
	delta = atof(argv[5]);
	rho = atof(argv[6]);
	mix = atof(argv[7]);

	// Get Convolver parameters
	blocksize = atoi(argv[8]);

	if (blocksize == 0) {
		printf("Block size must be a positive integer.\n");
		exit(1);
	}

	CGestalt::setNumOutChannels(2);
	CGestalt::setBlockSize(blocksize);

	// Compute soundrays
	geometer = new Geometer();
	printf("Parsing VRML file.\n");
	geometer->set_everything_using_VRML(argv[2]);
	if (!geometer->fileIsValid) {
		printf("Problem in file %s\n", argv[2]);
		exit(1);
	}
//	geometer->set_source(float x, float y, float z);
//	geometer->set_ear(float x, float y, float z, float direction);

	// Compute early IR
	printf("Computing early reflections.\n");
	soundrays = geometer->find_rays();
	numrays = geometer->number_of_rays();
	spatializer = new Spatializer(argv[3], DUMMY_HEAD);
	spatializer->spatialize_reflections(soundrays, numrays, A);
	spatializer2 = new Spatializer(argv[3], DUMMY_HEAD_INVERTED);
	spatializer2->spatialize_reflections(soundrays, numrays, A);
	early_ir_length = spatializer->get_length();
	early_ir = new Buffer(1, early_ir_length);
	early_ir->allocate_mono_buffers();
	spatializer->get_IR(*early_ir);
	early_ir_length2 = spatializer2->get_length();
	early_ir2 = new Buffer(1, early_ir_length2);
	early_ir2->allocate_mono_buffers();
	spatializer2->get_IR(*early_ir2);

	// Compute late IR
	printf("Computing global reverb.\n");
	for (i=0; (i<early_ir_length) && ((early_ir->_monoBuffers[0])[i]==0.0); i++);
	latereverb = new LateReverb();
	latereverb->set_attributes(A, delta, rho, i/(float)CGestalt::sample_rate());
	late_ir_length = latereverb->get_length();
	late_ir = new Buffer(1, late_ir_length);
	late_ir->allocate_mono_buffers();
	latereverb->get_IR(*late_ir);
	delete latereverb;
	for (i=0; (i<early_ir_length2) && ((early_ir2->_monoBuffers[0])[i]==0.0); i++);
	latereverb = new LateReverb();
	latereverb->set_attributes(A, delta, rho, i/(float)CGestalt::sample_rate());
	late_ir_length2 = latereverb->get_length();
	late_ir2 = new Buffer(1, late_ir_length2);
	late_ir2->allocate_mono_buffers();
	latereverb->get_IR(*late_ir2);

	// Mixdown
	printf("Mixing early and late reverb.\n");
	full_ir_length = (early_ir_length>late_ir_length) ? early_ir_length : late_ir_length;
	full_ir = new Buffer(1, full_ir_length);
	full_ir->allocate_mono_buffers();
	Spatializer::mono_mix_early_late(*early_ir, *late_ir, *full_ir, mix);
/*
	ir_samples = full_ir->mono_buffer(0);
	for (i=0; (i<early_ir_length) && (i<full_ir_length); i++)
		ir_samples[i] = mix * early_ir[i];
	//ir_samples[0] = 1.0; i=1;
	for (; i<full_ir_length; i++)
		ir_samples[i] = 0.0;
	for (i=0; (i<late_ir_length) && (i<full_ir_length); i++)
		ir_samples[i] += (1.0f-mix) * late_ir[i];
*/
	full_ir_length2 = (early_ir_length2>late_ir_length2) ? early_ir_length2 : late_ir_length2;
	full_ir2 = new Buffer(1, full_ir_length2);
	full_ir2->allocate_mono_buffers();
	Spatializer::mono_mix_early_late(*early_ir2, *late_ir2, *full_ir2, mix);
/*
	ir_samples2 = full_ir2->mono_buffer(0);
	for (i=0; (i<early_ir_length2) && (i<full_ir_length2); i++)
		ir_samples2[i] = mix * early_ir2[i];
	for (; i<full_ir_length2; i++)
		ir_samples2[i] = 0.0;
	for (i=0; (i<late_ir_length2) && (i<full_ir_length2); i++)
		ir_samples2[i] += (1.0f-mix) * late_ir2[i];
*/

	printf("IRs are %d and %d samples long.\n", full_ir_length, full_ir_length2);
	tempsamples = (short*)malloc(sizeof(short)*2*
		(full_ir_length>full_ir_length2?full_ir_length:full_ir_length2));

	for (i=0; i<early_ir_length; i++)
		tempsamples[i] = (short)((early_ir->_monoBuffers[0])[i] * 32767.0f);
	for (; i<full_ir_length; i++)
		tempsamples[i] = 0;
	fwrite(tempsamples, sizeof(short), early_ir_length, earlyirfile);

	for (i=0; i<late_ir_length; i++)
		tempsamples[i] = (short)((late_ir->_monoBuffers[0])[i] * 32767.0f);
	for (; i<full_ir_length; i++)
		tempsamples[i] = 0;
	fwrite(tempsamples, sizeof(short), late_ir_length, lateirfile);

	ir_samples = full_ir->_monoBuffers[0];
	ir_samples2 = full_ir2->_monoBuffers[0];
	for (i=0; (i<full_ir_length) && (i<full_ir_length2); i++) {
		tempsamples[i*2] = (short)(ir_samples[i] * 32767.0f);
		tempsamples[i*2+1] = (short)(ir_samples2[i] * 32767.0f);
	}
	if (full_ir_length < full_ir_length2) {
		for (; i<full_ir_length2; i++) {
			tempsamples[i*2] = 0;
			tempsamples[i*2+1] = (short)(ir_samples2[i] * 32767.0f);
		}
		fwrite(tempsamples, sizeof(short), full_ir_length2*2, fullirfile);
	} else {
		for (; i<full_ir_length; i++) {
			tempsamples[i*2] = (short)(ir_samples[i] * 32767.0f);
			tempsamples[i*2+1] = 0;
		}
		fwrite(tempsamples, sizeof(short), full_ir_length*2, fullirfile);
	}
	fclose(earlyirfile);
	fclose(lateirfile);
	fclose(fullirfile);

	FanOut fanout( * soundfile, 2);		// fan out on the snd-file reader

// Instantiate the Convolver and give it the IR
	printf("Instantiating Convolver. blocksize = %d\n", blocksize);
	convolver = new Convolver(blocksize, *full_ir, fanout);
	convolver2 = new Convolver(blocksize, *full_ir2, fanout);

	joiner = new Joiner(*convolver, *convolver2);

	// Instantiate Portaudio stream
	printf("Starting audio.\n");
//	io = new CAIO();
//	io = new PAIO();
	io = new FileIO("aural.aiff");
	io->open();
	io->start();

	//Turn on the Convolver
	printf("Starting Convolver.\n");
	convolver->init_and_start();
	convolver2->init_and_start();

	// Start Portaudio and wait
	io->set_root(*joiner);
	printf("Sleeping for 10 seconds.\n");
	Pa_Sleep(10000);

	// Stop
	printf("Stop.\n");
	io->clear_root();
	io->stop();
	io->close();

	delete convolver;
	delete convolver2;

	return 0;
}
