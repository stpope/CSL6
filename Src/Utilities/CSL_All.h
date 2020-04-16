//
//  CSL_All.h -- include all the CSL4 "ugen" include files
//	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
//

// THIS IS NOT USED ANY MORE

12348972po35478612765412 // error if you include this

#include "CSL_Includes.h"			// the main include

#include "CPoint.h"				// utility classes

// Unit generators -- Sources

#include "KarplusString.h"
#include "Spectral.h"

// Processors

#include "BinaryOp.h"
#include "Clipper.h"			/// clipper/distortion
#include "DelayLine.h"
//#include "Convolver.h"
//#include "FDN.h"

#ifndef CSL_WINDOWS
#include "SHARC.h"
#endif

// IO

#include "PAIO.h"
#include "FileIO.h"
//#include "MIDIIO.h"

// Remote Streams and IO

//#include "RemoteStream.h"
//#include "RemoteIO.h"

#if 0			// NOT PORTED YET

#include "OscillatorBL.h"
#include "Lorenz.h"

// #include "Convolver.h"

// Questionable stuff

//#include "WaveShaper.h"
//#include "VWaveShaper.h"
//#include "Granulator.h"

#endif

