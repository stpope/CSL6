#include "VBAP.h"
#include "string.h"
#include "SoundFile.h"
#include "ThreadedFrameStream.h"
#include "PME.h"
#include "DLine.h"
#include "BinaryOp.h"
#include "Oscillator.h"
#include "InOut.h"
#include <time.h>

#include "Test_support.cpp"		// include all of CSL core and the test support functions


//#ifndef _NO_NAMESPACE
//using namespace std;
//using namespace math;
//#define STD std
//#else
//#define STD
//#endif
//
//#ifndef _NO_EXCEPTION
//#  define TRYBEGIN()	try {
//#  define CATCHERROR()	} catch (const STD::exception& e) { \
//	cerr << "Error: " << e.what() << endl; }
//#else
//#  define TRYBEGIN()
//#  define CATCHERROR()
//#endif
//

using namespace csl;

// Cmd-line options:
//  -r srate -b blksiz -o noch -i nich -v verbosity -l logperiod -p outport -u
//
// MAIN -- sweeps source across arc
//
// Added options
//		"-S val" = set source azimuth in degrees
//		"-L val" = set loudspeaker azimuth in degrees
//

#define SOURCE 49
#define SPEAKER 50

//fMatrix LI(3,3);
 
float max_mu = 0.0;
float min_mu = 100.0;
float max_e = 0.0;
float min_e = 100.0;
float max_error = 0.0;
float min_error = 100.0;

extern unsigned exit_count;


int main (int argc, const char * argv[]) {
	
	unsigned value;						// variable used for arg parsing
	int sc = SOURCE;					// float values for freq and dur
	int sp = SPEAKER;
	char layout_file[100];
	
	READ_CSL_OPTS;										// parse the standard CSL options: 
	if (find_option(argc, argv, 'f', &value) > 0) {		// "-f filename" = open speaker layout file
		strcpy(layout_file, argv[value]);
		printf("Layout file: %s\n", layout_file);
	}
	
	
	SoundFile fi("/Users/dmccoy/Code/thesis/Demo.aif");
	fi.open_for_read();
	fi.set_is_looping(true);
//	SoundFile fi2("/Users/doug/Code/EMA/trumpet.aif");
//	fi2.open_for_read();
//	fi2.set_is_looping(true);
//	printf("\tFile \"%s\" SR: %d #CH: %d #FR: %d\n", fi._path, fi.rate(), fi.channels(), fi.duration());
//	printf("\tFile \"%s\" SR: %d #CH: %d #FR: %d\n", fi2._path, fi2.rate(), fi2.channels(), fi2.duration());
//	fflush(stdout);

	ThreadedReader tsf(1);
	tsf.set_input(fi);
	tsf.set_threshold( 2 * CGestalt::block_size() );
//	ThreadedReader tsf2(1);
//	tsf2.set_input(fi2);
//	tsf2.set_threshold( 2 * CGestalt::block_size() );

//	Sine vox(440.0, 0.8);

//	FanOut fan(tsf, 2);
//	DLine delay(vox, 1.0);
//	delay.init_delay_time(0.1);
//	AddOp mix(fan, delay);
	
	
	VBAP v;

	v.read_speaker_layout_from_file( layout_file );
	 
	v.find_triples();
	
	v.dump_speakers();
	v.dump_triples();
	
//	Sine vox(200.0, 0.7);
//	vox.set_num_channels(1);
	
	Source s(tsf, 0., 0., 0.5);			// sawtooth at sc degrees
//	CSource s2(vox);			// sawtooth at sc degrees

//	CVector R, V;
//	R.set(0.0001, 0.5, 0.0);
//	V.set(0.0001, 0.003, 0.0001);
//	
//	double r = R.r();
//	double th = R.theta();
//	double el = R.ele();
//
//	s2.set_position('s', 180., 0.);
	v.add_source(&s);
//	v.add_source(&s2);
	
	PMESource ps(s);
//	PMESource ps2(s2);
	
	PME pme("128.111.221.5", 54321);
	pme.add_pme_source(ps);
//	pme.add_pme_source(ps2);

	
//	CGestalt :: set_block_size(128);
//	gIO = new PAIO(paNoDevice, Pa_GetDefaultOutputDeviceID(), 0, v.channels());		// sets up IO with correct num channels
	gIO = new PAIO(Pa_GetDefaultInputDeviceID(), Pa_GetDefaultOutputDeviceID(), 5, 8);		// sets up IO with correct num channels
//	gIO = new PAIO(paNoDevice, Pa_GetDefaultOutputDeviceID(), 0, 1);		// sets up IO with correct num channels
	
	InOut temp(kNoProc, 2);
//	temp.setInChan(8);

	
	logMsg("CSL playing VBAP...");
//	gIO->set_root(v);						// make some sound
	gIO->set_root(temp);						// make some sound
	gIO->open();
	gIO->start();							// start the driver callbacks

//	Orbit orb;
//	orb.calculate_orbital_params(R, V);
//	orb.dump();
////	exit(0);
//	cout << "R:\t" << R.len() << "\t" << R.theta() << "\t" << R.ele() << endl;
	
//	int c = 100 * 40;
//	while (c--){
//		CVector Rnew;
//		orb.calculate_new_position_in_orbit();
//		orb.calculate_absolute_position(Rnew);
//		
//		
//	  	s.set_position('s', Rnew.theta(), Rnew.ele(), Rnew.r());
//		if (c%20 == 0)
//			cout << "Rnew:\t" << Rnew.len() << "\t" << Rnew.theta() << "\t" << Rnew.ele() << endl;
////		cout << "Which triple:\t" << s._which_triple << endl;
//		sleep_sec(.01);
//	}
	
//	float d = 50.005;
//	float time = 5.;
//	float time_interval = 1./100.;
//	
//
//	for (float dist=-d; dist<d; dist+=d/time*time_interval){
//		delay.set_target_delay_time(fabsf(dist/330.0) );
//		sleep_sec(time_interval);
//	}
	
//	pme.manage_sources();

	sleep_sec(500);
	logMsg("CSL done.");
	gIO->stop();							// stop the driver and close down
	gIO->close();	
	
	return 0;								// exit
	
	
}
