///
///  CSL_SWIG.h -- the SWIG specification file for the core classes of CSL version 4.0
///	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
///

%include exception.i 
%include "std_vector.i"
%include "std_list.i"

%module CSL

%{
	#include <stdexcept>
	#include <vector>
	#include <list>
	#include <map>
	#include <string>

	#include "CSL_Types.h"		// CSL type definitions and central macros, Observer classes
	#include "CSL_Exceptions.h"	// CSL exception hierarchy
	#include "CGestalt.h"		// System constants class
	#include "CSL_Core.h"
	#include "RingBuffer.h"		/// Utility circular buffer
	#include "Variable.h"		/// Static/dynamic variables

	#include "Interleaver.h"		/// Sample stream interleaving
	#include "BlockResizer.h"
	#include "CPoint.h"
	#include "ThreadUtilities.h"

	#include "BinaryOp.h"		/// Binary operators
	#include "Envelope.h"		/// Breakpoint envelopes
	#include "Oscillator.h"		/// All the oscillator classes
	#include "Noise.h"
	#include "Window.h"
	#include "Spectral.h"
	#include "KarplusString.h"

	#include "InOut.h"
	#include "Filters.h"
	#include "DelayLine.h"
	#include "Mixer.h"
	#include "Convolver.h"
	#include "FIR.h"

	#include "PAIO.h"
	#include "FileIO.h"
	#include "SoundFile.h"

	using namespace csl;
%}

////////////////////////////////

	%include "CSL_Types.h"		// CSL type definitions and central macros, Observer classes
	%include "CSL_Exceptions.h"	// CSL exception hierarchy
	%include "CGestalt.h"		// System constants class
	%include "CSL_Core.h"
	%include "RingBuffer.h"		/// Utility circular buffer
	%include "Variable.h"		/// Static/dynamic variables

	%include "Interleaver.h"		/// Sample stream interleaving
	%include "BlockResizer.h"
	%include "CPoint.h"
	%include "ThreadUtilities.h"

	%include "BinaryOp.h"		/// Binary operators
	%include "Envelope.h"		/// Breakpoint envelopes
	%include "Oscillator.h"		/// All the oscillator classes
	%include "Noise.h"
	%include "Window.h"
	%include "Spectral.h"
	%include "KarplusString.h"

	%include "InOut.h"
	%include "Filters.h"
	%include "DelayLine.h"
	%include "Mixer.h"
	%include "Convolver.h"
	%include "FIR.h"

	%include "PAIO.h"
	%include "FileIO.h"
	%include "SoundFile.h"

/////////////////////////////
