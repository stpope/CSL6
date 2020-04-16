///
/// SoundFileL.h -- concrete sound file class using libsndfile
///
///	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
///
/// The MP3 file playback is a mild hack; it first converts each MP3 file to a temp AIFF file and uses 
/// that for playback. There is no MP3 writing (encoding) at present.
///

#ifndef CSL_SoundFileL_H
#define CSL_SoundFileL_H

#include "SoundFile.h"			// abstract class header
#include <sndfile.h>			// libsndfile header file
								// NB: set these as compile-time flags now
#ifndef CSL_WINDOWS
// #define USE_libMAD			// Support MP3 file reading using libMAD
// #define USE_libFAAD			// Support AAC/MP4 file reading using libFAAD
#endif

#include <sndfile.h>			// libsndfile header file
#include <string.h>

// Temp name macro -- also used by other services
// NB: This is UNIX-specific - ToDo: use run-time file sep
// These are no longer used

#if 0

#define MP3_TEMP_NAME(in_path, out_path, temp_dir) {				\
	sprintf(out_path, "%s%s", temp_dir, in_path);					\
	char * lastDot = strrchr(out_path, (int) '.');					\
	if (lastDot) sprintf(lastDot, MP3_TEMP_EXT);					\
	char * pos = out_path;											\
	pos += strlen(temp_dir);										\
	pos = strchr(pos, (int) '/');									\
	while (pos) {													\
		*pos = '_';													\
		pos = strchr(pos, (int) '/');								\
	}}

// AIFF name macro -- also used by other services
// This assumes the in_path ends with ".mp3"

#define AIFF_TEMP_NAME(in_path, out_path) {							\
		strcpy(out_path, in_path);									\
		if (strcasestr(in_path, ".mp3"))							\
			strcpy((out_path + strlen(out_path) - 4), ".aiff"); }

#endif // 0

namespace csl {

// Call-back functions for SF_VIRTUAL_IO struct

//sf_vio_get_filelen lsf_getFileLen(void * vBuffer);
//sf_vio_seek lsf_seek(sf_count_t offset, int whence, void * vBuffer);
//sf_vio_tell lsf_tell(void * vBuffer);
//sf_vio_read lsf_read(void *ptr, sf_count_t count, void * vBuffer);
//sf_vio_write lsf_write(const void *ptr, sf_count_t count, void * vBuffer);

///
/// Here's the sound file reader/writer class; it assumes libSndFile and interleaved sample buffers
///

class LSoundFile : public Abst_SoundFile {
public:							/// Constructor with defaults
	LSoundFile(std::string path, int start = -1, int stop = -1, bool doRead = true, float maxDurInSecs = 0.0);
	LSoundFile(float maxDurInSecs, std::string path);	///< this version sets maxSize and always reads
//	LSoundFile(char * buffer);						///< virtual version uses a sample buffer
	LSoundFile(LSoundFile & otherSndFile);			///< Copy constructor -- shares sample buffer
	~LSoundFile();
													/// Factory methods
	static LSoundFile * openSndfile(string path, int start = -1, int stop = -1, bool doRead = true);
	static LSoundFile * openSndfile(float maxDurInSecs, string path);

	SoundFileFormat format();						///< get format

	virtual void openForRead(bool load = true) throw (CException);	///< open file and get stats
													/// Open a file for write. 
													/// Default values are some common format.
	void openForWrite(SoundFileFormat format = kSoundFileFormatAIFF, 
					unsigned channels = 1, 
					unsigned rate = 44100,
					unsigned bitDepth = 16) throw (CException);
	void openForReadWrite() throw (CException);		///< open r/w
	void close();									///< close file
															/// seek to some position
	unsigned seekTo(int position, SeekPosition whence = kPositionStart) throw(CException);
													/// read a buffer from the file (possibly all of it)
	void readBufferFromFile(unsigned numFrames);

								/// UGen operations
	void nextBuffer(Buffer &outB) throw (CException);				///< copy next buffer from cache
	void writeBuffer(Buffer &inputBuffer) throw (CException);		///< write a buffer of data into the file
	void writeBuffer(Buffer &inB, unsigned fromFrame, unsigned toFrame) throw (CException);
	bool isCached();								///< answer if file has all of its samples in RAM
	bool isCached(unsigned samps);					///< answer if file has X samples in RAM

	SF_INFO * sfInfo() { return mSFInfo; }			///< libsndfile sf-info struct
	SNDFILE * sndFile() { return mSndfile; }		///< libsndfile handle

protected:
	SF_INFO * mSFInfo;								///< libsndfile sf-info struct
	SNDFILE * mSndfile;								///< libsndfile handle
	Interleaver mInterleaver;						///< File IO interleaver/deinterleaver
	float mMaxDurInSecs;							///< max size to read from file. In seconds so it can deal with varying sample rates.
	
//#ifdef CSL_USE_SRConv
//	Buffer mSRConvBuffer;							///< used by the sample rate convertor
//	SRC_STATE * mSRateConv;							///< sample rate convertor (SRC) state struct
//	SRC_DATA mSRateData;							///< SRC call data struct
//	int mSRCReturn;									///< SRC error flag
//#endif

	void initFromSndfile();							///< read SF header
	
//	void checkBuffer(unsigned numFrames);			///< allocate buffer lazily
//	void checkBuffer(unsigned numChans, unsigned numFrames);
};

}

#endif
