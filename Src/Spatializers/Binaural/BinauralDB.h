///
///  BinauralDB.h -- Specification of the HRTF file format and database
///		This is the CSL 5 version that uses the FFT wrapper framework
///
/// Classes
///		HRTF: holds the data that corresponds to an HRTF for a single position
///		as a stereo pair of multi-block arrays of complex spectra.
///		2 channels * 16 blocks * 512-complex-float arrays
///		There is support in the code for taking a subset of the HRIR and changing the block size.
///
///		HRTFDatabase: vector of HRTFs; implemented as a Singleton because it's large.
///		Has a vector of HRTFs and can access them by position -- hrtfAt(CPoint) --
///		or by index -- hrtfAt(unsigned). Create it with a folder/resource name, 
///		it reads "files.txt" for a list of HRIR files to load.
///		This has a number of file/folder/DB IO methods for load/store of bulk HRTF 
///		data sets in IRCAM format and CSL's pre-processed HRTF.dat files.
///
/// The companion file has the classes
///		BinauralPanner: place sources in 3D using block-wise convolution with an HRTF;
///			best heard over headphones .
///		BinauralSourceCache: used for caching previous state of spatial sources.
///
///	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
///	Created by Jorge Castellanos on 7/19/06.
///	Inspired by and partially based on the VST HRTF Plug-in written by Ryan Avery.
///	Rewritten for FFT wrappers and pluggable sound file APIs in 8/09 by STP.
///
/// @todo HRTFs should be equalized, currently they are being loaded and used raw.
/// @todo Interpolation of HRTFs. Currently the closest HRTF is being used.
///
///	CSL HRTF.dat file format
///		Header = a string with the label and geometry list,
///		label is: "HRTF"  name  #-hrtfs  HRIR-len  blk-size  #-blocks
///			as in, HRTF   1047  188      8192      512       16
///
///		geometry is a cr-separated list of 2 integers per line of the azim/elev 
///		in degrees of the points; empty line ends list.
///
///		Header is followed by the per-head, per-spsition, blocked HRTF complex vectors.
///		2-16 blocks/set, each has 257 or 513 complex values for L and R
///			for full-len HRIR, this = 8 * 2 * 513 * 16 = 131 kB / HRTF * 188 = 24.6 MB / DB
///
///		Trailer is "\nHRTF\n"
///		The methods storeToDB() and loadFromDB() implement this format.
///

#ifndef CSL_BINAURALDB_H
#define CSL_BINAURALDB_H

#include "CSL_Core.h"
#include "SpatialPanner.h"
#include "FFT_Wrapper.h"

#ifdef USE_JSND
	#include "SoundFileJ.h"
#endif
#ifdef USE_LSND
	#include "SoundFileL.h"
#endif
#ifdef USE_CASND
	#include "SoundFileCA.h"
#endif 

#ifndef IPHONE								/// Typically CGestalt::blockSize()
	#define HRTF_BLOCK_SIZE 512				/// (or use a block resizer)
	#define HRIR_SIZE 1024					/// reduce this to save space
#else // iPhone
	#define HRTF_BLOCK_SIZE 256				/// run with smaller I/O blocks
	#define HRIR_SIZE 1024					/// reduce this to save space
#endif

#define FLIST_NAME "files.txt"				///< name of file list file
#define DEFAULT_HRTF_FOLDER "IRCAM_HRTF/"	///< where are the HRTFs under the CSL_Data folder?

#ifdef USE_CASND
	#define HRTF_RESOURCE "1047"			///< Default HRTF resource name for iPhone
#else
	#define HRTF_RESOURCE "IRC_1047_R"		///< What's the default HRTF folder under data?
#endif

// complex multiply-accumulate macro; uses cx_r/i marcos

//#define cmac(in1, in2, out)											\
//	cx_r(out) += (cx_r(in1) * cx_r(in2)) - (cx_i(in1) * cx_i(in2));		\
//	cx_i(out) += (cx_r(in1) * cx_i(in2)) + (cx_i(in1) * cx_r(in2))

// this way assumes SampleComplex is sample[2]

#define cmac(in1, in2, out)							\
	out[0] += in1[0] * in2[0] - in1[1] * in2[1];	\
	out[1] += in1[0] * in2[1] + in1[1] * in2[0];

namespace csl {

/// HRTF: holds the data that corresponds to an HRTF for a single position.
/// It has a list of complex buffers for the FFTs of the HRIR,
///		typically 16 blocks of 512 each for an 8k HRTF.
/// This version knows how to parse the IRCAM hrtfs sets
/// Subclasses could load different HRTF sets (e.g., CIPIC).

class HRTF {
public:										/// HRTF constructor allocates memory for the HRTF data and then  
											/// reads the HRIR file and performs the block-wise FFT
	HRTF();									///< generic constructor allocates all data
	HRTF(char * fname, FFT_Wrapper & fft);	///< load an HRIR file and do FFTs
	~HRTF();
	
	void dump();							///< Prints the position that corresponds to this HRTF
	unsigned size();						///< returns the size on bytes of the receiver's storage
	
	CPoint mPosition;						///< The Position at which the HRIR was recorded
	SampleComplexVector *mHrtfL, *mHrtfR;	///< 2 arrays of arrays of mNumFFTBlocks * complex[513]:
											///  the HRTF data in blocks of complex # for stereo
	unsigned mNumFFTBlocks;					///< # of blocks (16)
};

typedef vector <HRTF *> HRTFVector;


/// HRTFDatabase: has a vector of HRTFs and can access them by position -- hrtfAt(CPoint) 
///		-- or by index -- hrtfAt(unsigned).
///	Implemented as a Singleton because it's large (typ 188 HRTFs, 25 MB).
/// Create it with a folder/resource name, it reads "files.txt" for a list of HRIR files to load.
///	This has a number of file/folder/DB IO methods for load/store of bulk HRTF 
///	data sets in IRCAM format and CSL's pre-processed HRTF.dat files.
///

class HRTFDatabase {
public:
	~HRTFDatabase() { };
	
	static HRTFDatabase * Database();		///< accessor for the singleton instance (lazy init)
	static void Destroy();					///< free the instance
	static void Reload(char * folder);		///< load the singleton from the given data folder
	static void convertDB(const char *listname) throw (CException);		///< bulk-convert DBs

	unsigned numHRTFs();					///< Total number of HRTFs loaded into the database.
	unsigned windowSize();					///< The size of the analysis window (in samples).
	unsigned hrtfLength();					///< The length (in samples) of the Transfer Function buffer.
	unsigned hrirLength();					///< The length (in samples) of the impulse responses loaded.
	unsigned numBlocks();					///< The length (in windows) of the impulse responses loaded.

	unsigned hrtfAt(CPoint srcPos);			///< answer the index of the HRTF nearest the given point
	HRTF * hrtfAt(unsigned index);			///< answer an HRTF* by index
	
	void dump(bool verbose = false);		///< Print all the HRTFs in the database.
	unsigned size();						///< returns the size on bytes of the receiver's storage

											///< dump the DB as a single binary file
	void storeToDB(const char *filename,const char *same) throw (CException);
	
protected:
	HRTFDatabase(const char * folder);		///< constructor that loads from a folder (protected)
											/// load a set of HRTFs from a folder or a file list file
	void loadFromFolder (const char *folder) throw (CException);
	void loadFromFile(const char *filename) throw (CException);
	void loadFromDB (const char *dbName) throw (CException);
	
	HRTFVector mHRTFVector;					///< vector of the HRTFs that constitute the database
	
	unsigned mWindowSize;					///< FFT sizes (1024)
	unsigned mHRTFLength;					///< 513
	unsigned mHRIRLength;					///< The length of the Head Related Impulse Response (8192)

	static HRTFDatabase* mDatabase;			///< The protected single instance of the HRTF Database
};

} // end namespace

#endif
