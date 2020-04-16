//
//  CSL_Includes.h -- include all the core CSL 5.0 header files
//
//	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
//

// CSL Kernel

#include "CSL_Core.h"			/// The core of CSL 5
//#include "RingBuffer.h"			/// Utility circular buffer
#include "BlockResizer.h"		/// BlockResizer
#include "CPoint.h"				/// 2D & 3D points
//#include "ThreadUtilities.h"	/// CThread utilities
#include "Variable.h"			/// Static/dynamic variables

// Sources

#include "Envelope.h"			/// Breakpoint envelopes
#include "Window.h"				/// Signal processing windows
#include "Oscillator.h"			/// All the oscillator classes
#include "Noise.h"				/// Noise generators
#include "WaveShaper.h"			/// WaveShaper oscillator

// Effects

#include "BinaryOp.h"			/// Binary operators
#include "Filters.h"			/// Canonical-form filters
#include "FIR.h"				/// FIR filters and design
#include "InOut.h"				/// Copy in-out plug
#include "Mixer.h"				/// Mixer, panners
#include "Freeverb.h"			/// Reverberator

// FFT

#ifdef USE_FFTREAL
#include "FFT_Wrapper.h"		/// FFT/IFFT
#include "Spectral.h"			/// FFT/IFFT with FFTW
#endif

#ifdef USE_FFTW
#include "FFT_Wrapper.h"		/// FFT/IFFT
#include "Spectral.h"			/// FFT/IFFT with FFTW
#endif

// Sound files

#ifdef USE_JSND
	#include "SoundFileJ.h"
	#define SoundFile JSoundFile			// JUCE snd file class
#endif

#ifdef USE_LSND
	#include "SoundFileL.h"
	#define SoundFile LSoundFile			// JUCE snd file class
#endif

#ifdef USE_CASND
	#include "SoundFileCA.h"
	#define SoundFile CASoundFile			// JUCE snd file class
#endif

// MIDI

#ifdef USE_JMIDI
	#include "MIDIIOJ.h"
#endif

#ifdef USE_PMIDI
	#include "MIDIIOP.h"
#endif
