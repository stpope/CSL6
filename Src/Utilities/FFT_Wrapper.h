///
///  FFT_Wrapper.h -- wrapper class for FFTs that hides implementation details
///		This class can be used with FFTW, FFTReal, or other FFT implementations.
///		It assumes real-values float vectors as input to the FFT, and can deliver
///		real or complex results in several formats (complex #s or mag/phase).
///		The IFFT assumes complex input and delivers real-valued output.
///
///		The API is minimal, with a constructor method and the work-horse
///		nextBuffer(Buffer & in, Buffer & out) method doing a single FFT or IFFT
///
///		This file includes both the abstract wrapper, and the 3 standard concrete subclasses.
///		Compile this with one of the API flags defined in the compiler (-DUSE_FFTW), or define it below.
///		There are 3 standard concrete subclasses: 
///			RealFFT (RealFFT code included in CSL), and 
///			FFTW (assumes fftw3f is installed), 
///			KISS FFT, built-in FFT from Laurent de Soras (included, untested).
///
///	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
///

#ifndef CSL_FFT_WRAPPER_H
#define CSL_FFT_WRAPPER_H

				// I use compiler flags for these, so different apps can share the same code base.
//#define USE_FFTW						// use FFTW (faster but complicated to build)
//#define USE_FFTREAL						// use FFTReal (smaller and simpler)
//#define USE_KISSFFT						// use KISS FFT (smaller and simpler)

#include "CSL_Core.h"

#ifdef USE_FFTW
	#include <fftw3.h>
	#define FFT_Wrapper FFTW_Wrapper			// which FFT wrapper class to use?
	#define FFTWF_FLAGS FFTW_MEASURE		// or use FFTW_ESTIMATE; FFTW_PRESERVE_INPUT not necessary
#endif

#ifdef USE_FFTREAL
	#include "FFTReal.h"
	#define FFT_Wrapper FFTR_Wrapper
#endif

#ifdef USE_KISSFFT
	#include <kiss_fft.h>
	#define FFT_Wrapper KISSFFT_Wrapper
#endif

#ifndef FFT_Wrapper
    231450p2378640123764o127i3e6ro72364501o28736r               // error
#endif

namespace csl {

/// real/complex flag (determines results from forward FFT)

typedef enum {			
	CSL_FFT_REAL = 0,
	CSL_FFT_COMPLEX,
	CSL_FFT_MAGPHASE
} CSL_FFTType;

/// forward/reverse flag (determines FFT direction)

typedef enum {			
	CSL_FFT_FORWARD = 0,
	CSL_FFT_INVERSE
} CSL_FFTDir;

///
/// Abstract FFT class can do forward/reverse real/complex I/O FFTs
///

class Abst_FFT_W {
public:							/// Constuctor sets up twiddle factor tables
	Abst_FFT_W(unsigned size, CSL_FFTType type = CSL_FFT_REAL, CSL_FFTDir dir = CSL_FFT_FORWARD) 
				: mSize(size), mCSize((size / 2) + 1), mType(type), mDirection(dir) { };

	virtual ~Abst_FFT_W() { };			///< destructor frees tables

								/// run the transform between in and out
	virtual void nextBuffer(Buffer & in, Buffer & out) throw (CException) = 0;

	unsigned mSize;				///< FFT length
	unsigned mCSize;			///< FFT length / 2 + 1
protected:
	CSL_FFTType mType;			///< real/complex output
	CSL_FFTDir mDirection;		///< forward/reverse
};

//
// FFTW-wrapper concrete subclass
//

#ifdef USE_FFTW

class FFTW_Wrapper : public Abst_FFT_W {
public:
	FFTW_Wrapper(unsigned size, CSL_FFTType type = CSL_FFT_REAL, CSL_FFTDir forward = CSL_FFT_FORWARD);
	~FFTW_Wrapper();
								/// run the transform between in and out
	void nextBuffer(Buffer & in, Buffer & out) throw (CException);

private:		
//	SampleBuffer mInBuf;		///< input sample* ptr
	SampleBuffer mSampBuf;		///< temp sample buf ptr
	fftwf_complex *mSpectBuf;	///< complex spectrum
	fftwf_plan mPlan;			///< FFTW plan object
};

#endif

//
// FFTReal-wrapper concrete subclass
//

#ifdef USE_FFTREAL

class FFTR_Wrapper : public Abst_FFT_W {
public:
	FFTR_Wrapper(unsigned size, CSL_FFTType type = CSL_FFT_REAL, CSL_FFTDir forward = CSL_FFT_FORWARD);
	~FFTR_Wrapper();
								/// run the transform between in and out
	void nextBuffer(Buffer & in, Buffer & out) throw (CException);

private:		
	SampleBuffer mTempBuf;		///< temp sample buf ptr
	FFTReal mFFT;				///< FFTReal object
};

#endif

//
// KISS_FFT-wrapper concrete subclass (untested)
//

#ifdef USE_KISSFFT

class KISSFFT_Wrapper : public Abst_FFT_W {
public:
	KISSFFT_Wrapper(unsigned size, CSL_FFTType type = CSL_FFT_REAL, CSL_FFTDir forward = CSL_FFT_FORWARD);
	~KISSFFT_Wrapper();
								/// run the transform between in and out
	void nextBuffer(Buffer & in, Buffer & out) throw (CException);

private:		
	SampleBuffer mTempBuf;		///< temp sample buf ptr
	kiss_fft_cfg mFFT;			///< KISS FFT config object pointer
	SampleComplexVector inBuf, outBuf;
};

#endif

}

#endif
