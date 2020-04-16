//
//  JackIO.cpp -- DAC IO using JACK
//	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
//

#include "JackIO.h"

using namespace csl;

// Callback function

int JackCallback (jack_nframes_t nframes, void *arg) {
	JackIO * jack = (JackIO * ) arg;
	jack_default_audio_sample_t * in, * out;
	IO_Status tStatus = jack->mStatus;

	jack_transport_state_t ts = jack_transport_query(jack->mClient, NULL);
	if (ts == JackTransportRolling) {
		in = (jack_default_audio_sample_t *) jack_port_get_buffer (jack->mInput_port, nframes);
		out = (jack_default_audio_sample_t *) jack_port_get_buffer (jack->mOutput_port, nframes);
//						// copy in to out
//		memcpy (out, in, sizeof (jack_default_audio_sample_t) * nframes);
						// setup CSL buffer for I/O
		Buffer * outBuffer = & (jack->mOutputBuffer);
		unsigned numOutChannels = outBuffer->mNumChannels;
		outBuffer->mNumFrames = nframes;
		jack->pullInput(* outBuffer, out, true);
	} else if (ts == JackTransportStopped) {
		if (tStatus == kIORunning)
			tStatus = kIOExit;
	}
	return 0;      
}

void jack_shutdown (void *arg) {
	logMsg ("Jack shutdown");
}

// JackIO Class

JackIO::JackIO() {

}

JackIO::JackIO(unsigned s_rate, unsigned b_size, 
			int in_device, int out_device, 
			unsigned in_chans, unsigned out_chans)
		: IO(s_rate, b_size, in_device, out_device, in_chans, out_chans) {
		
}

JackIO::~JackIO() {

}

//	kIONew,
//	kIOInit,
//	kIOOpen,
//	kIORunning,
//	kIOClosed,
//	kIOExit


///< open/close start/stop methods

void JackIO::open() throw(CException) { 
	jack_options_t options = JackNullOption;
	jack_status_t status;
	const char ** ports;
								// this set-up code is taken from the Jack simple_client example
	char * server_name = "CSL_SERVER";
	char * client_name = "CSL_CLIENT";
	options |= (jack_options_t) JackServerName;

	mClient = jack_client_open (client_name, options, &status, server_name);
	if (mClient == NULL) {
		logMsg (kLogError, "jack_client_open() failed, " "status = 0x%2.0x", status);
		if (status & JackServerFailed) {
			logMsg (kLogError, "Unable to connect to JACK server");
		}
	}
	if (status & JackServerStarted) {
		logMsg("\tJACK server started");
	}
	if (status & JackNameNotUnique) {
		client_name = jack_get_client_name(mClient);
		logMsg (kLogError, "unique name `%s' assigned", client_name);
	}

	jack_set_process_callback (mClient, JackCallback, 0);
	jack_on_shutdown (mClient, jack_shutdown, 0);

	logMsg ("engine sample rate: %g", jack_get_sample_rate (mClient));
	mInput_port = jack_port_register (mClient, "input", JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);
	mOutput_port = jack_port_register (mClient, "output", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
	if ((mInput_port == NULL) || (mOutput_port == NULL)) {
		logMsg (kLogError, "no more JACK ports available");
		exit (1);
	}
	if (jack_activate (mClient)) {
		logMsg (kLogError, "cannot activate client");
		exit (1);
	}
	ports = jack_get_ports (mClient, NULL, NULL, JackPortIsPhysical|JackPortIsOutput);
	if (ports == NULL) {
		logMsg (kLogError, "no physical capture ports");
		exit (1);
	}
	if (jack_connect (mClient, ports[0], jack_port_name (mInput_port))) {
		logMsg (kLogError, "cannot connect input ports");
	}
	free (ports);	
	ports = jack_get_ports (mClient, NULL, NULL, JackPortIsPhysical|JackPortIsInput);
	if (ports == NULL) {
		logMsg (kLogError, "no physical playback ports");
		exit (1);
	}
	if (jack_connect (mClient, jack_port_name (mOutput_port), ports[0])) {
		logMsg (kLogError, "cannot connect output ports");
	}
	free (ports);
}

void JackIO::close() throw(CException) {
	jack_client_close (mClient);
};

void JackIO::start() throw(CException) {			///< start my timer thread
}

void JackIO::stop() throw(CException) {			///< stop the timer thread
}

