///
///	Test_LPC.cpp -- a simple CSL LPC vocoder program -- load a file and display it
///	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
///

#include "Test_Support.cpp"		// include all of CSL core and the test support functions
#include "SoundFile.h"
#include "LPC.h"

/// Cmd-line options (see the handler in Test_Support.cpp):
///		-r samp-rate  -b blk-siz  -o num-out-ch  -i num-in-ch  -v verbosity  -l log-period  -p out-port  -u
///

#define IN_FILE_NAME		"/Volumes/Content/Sound/3-IKnowGod/MKGandhi/1.2a.aif"
#define LPC_FILE_NAME		"lpc-coeff.data"
#define RESIDUAL_FILE_NAME	"lpc-residual.aiff"
#define WINDOW_SIZE		1024
#define LPC_ORDER		20

#define ANALYZE_FILE			// This is the switch for whether to analyze or resynthesize

#ifdef ANALYZE_FILE			// This main() runs the analysis

///
/// MAIN -- Save 2 files for the LPC and residual of the given input file
/// This plays the residual on analysis
///

int main (int argc, const char * argv[]) {
	unsigned winSize = WINDOW_SIZE;					
	csl::CGestalt::setBlockSize(winSize);	// set a smaller block size for some tests

	READ_CSL_OPTS;						// parse the standard CSL options: 
	SoundFile fi(IN_FILE_NAME);			// open sound file for analysis
	try {
		fi.openForRead();
	} catch (CException & e) {
		logMsg(kLogError, "File open exception caught: %s", e.mMessage.c_str());
		return 1;
	}
	float dur = fi.duration() / fi.rate();
	unsigned wins = (fi.duration() / winSize) + 10;
	logMsg("playing sound file \"%s\"\n\t\trate: %d  #ch: %d  dur: %5.3f  wins: %d  size: %d\n", fi.mPath.c_str(), fi.rate(), fi.channels(), dur, wins, winSize);

	LPCAnalyzer lpc(fi,  winSize, winSize, LPC_ORDER);	// LPC analyzer constructor
	lpc.setPreEmphasis(1.0);				// pre-emphasis HPF
	lpc.setBWExpansion(0.992);			// LPC filter BW expansion
	lpc.setUseWindow(FALSE);			// use rect window
	lpc.setFileNames(LPC_FILE_NAME, RESIDUAL_FILE_NAME);
	
	PAIO * theIO = new PAIO;				// PortAudio IO object
	
	logMsg("CSL performing LPC analysis of snd file...");
	theIO->setRoot(lpc);					// make some sound
	theIO->open();
	theIO->start();						// start the driver callbacks

	sleepSec(dur + 0.25);					// sleep for dur plus a bit

	logMsg("CSL done.");
	theIO->stop();						// stop the driver and close down
	theIO->close();	
	
	return 0;							// exit
}

#else	// This main() is for LPC playback using the residual file as the exciter

///
/// MAIN -- Read the given input file and filter it according to the LPC filter coefficients
///

int main (int argc, const char * argv[]) {
	unsigned winSize = WINDOW_SIZE;					
	csl::CGestalt::setBlockSize(winSize);	// set a smaller block size for some tests

	READ_CSL_OPTS;						// parse the standard CSL options: 
	SoundFile fi(RESIDUAL_FILE_NAME);		// open LPC residual file
	try {
		fi.openForRead();
	} catch (Exception & e) {
		logMsg(kLogError, "File open exception caught: %s", e.mMessage.c_str());
		return 1;
	}
	float dur = fi.duration() / fi.rate();
	unsigned wins = (fi.duration() / winSize) + 10;
	logMsg("playing sound file \"%s\"\n\t\trate: %d  #ch: %d  dur: %5.3f  wins: %d  size: %d", fi.mPath.c_str(), fi.rate(), fi.channels(), dur, wins, winSize);

	LPCFilter lpc(fi, LPC_FILE_NAME);		// LPC filter on sound with LPC filter file
	logMsg("LPC win: %d  hop: %d  order: %d  de-emph: %f\n", lpc.windowSize(), lpc.hopSize(), lpc.LPCOrder(), lpc.deEmphasis());

	PAIO * theIO = new PAIO;				// PortAudio IO object
	
	logMsg("CSL performing LPC filtering of snd file...");
	theIO->setRoot(lpc);					// make some sound
	theIO->open();
	theIO->start();						// start the driver callbacks

	sleepSec(dur + 0.25);					// sleep for dur plus a bit

	logMsg("CSL done.");
	theIO->stop();						// stop the driver and close down
	theIO->close();	
	
	return 0;							// exit
}

#endif
