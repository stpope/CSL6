///
/// SoundFile.h -- CSL's abstract sound file class, a sample player UGen, SoundCue & SoundFileBuffer.
/// The concrete subclasses represent sound file APIs, not individual formats.
///
/// Classes: SoundFileMetadata, Abst_SoundFile, SoundCue, SoundFileBuffer (unfinished)
/// We read ID3-style tags from MP4/3, AAC, FLAC, etc. files using TagLib, http://developer.kde.org/~wheeler/taglib.html
///
///	The sound file player is actually a powerful sampler, supporting arbitrary transposition 
/// using wavetable interpolation or a sample rate convertor; Sound files are seekable and also writeable.
///
///	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
///

#ifndef CSL_SoundFile_H
#define CSL_SoundFile_H

#include "CSL_Core.h"			// the core of CSL 5
#include "Oscillator.h"			// the superclass of sound files

#include <string.h>

//#define USE_TAGS

namespace csl {

/// Sound file constants

#ifdef CSL_ENUMS				// use enumerations or integers?

typedef enum {					///< File I/O r/w mode
	kSoundFileClosed,
	kSoundFileRead,
	kSoundFileWrite,	
	kSoundFileReadWrite
} SoundFileMode;

typedef enum {					/// File format
	kSoundFileFormatAIFF,		///< AIFF
	kSoundFileFormatWAV,		///< WAV
	kSoundFileFormatMP3,		///< MP3
	kSoundFileFormatMP4,		///< MP4, M4A
	kSoundFileFormatAAC,		///< AAC
	kSoundFileFormatFLAC,		///< FLAC
	kSoundFileFormatCAF,		///< CoreAudio format
	kSoundFileFormatOGG,		///< Ogg/Vorbis
	kSoundFileFormatSHN,		///< Shorten
	kSoundFileFormatSND, 	   	///< Sun/NeXT SND/AU
	kSoundFileFormatEBICSF,    	///< Extended BIC (Berkeley/IRCAM/CARL) data
	kSoundFileFormatRaw, 	   	///< Raw data
	kSoundFileFormatOther, 	   	///< Other formats
} SoundFileFormat;

#else
	#define kSoundFileRead 0
	#define kSoundFileWrite 1
	#define kSoundFileReadWrite 2
	typedef int SoundFileMode;
	
	#define kSoundFileFormatWAV 0
	#define kSoundFileFormatAIFF 1
	#define kSoundFileFormatSND 2
	#define kSoundFileFormatEBICSF 3
	#define kSoundFileFormatRaw 4
	#define kSoundFileFormatOther 5
	typedef int SoundFileFormat;
#endif

///
/// Class SndFileMetadata holds the ID3 tags of a sound file
///

class SoundFileMetadata {
public:
	std::string mTitle;					///< ID3 tag fields
	std::string mArtist;
	std::string mAlbum;
	unsigned mYear;
	std::string mComment;
	unsigned mTrack;
	std::string mGenre;
	
	unsigned mBitRate;					///< encoding data
	unsigned mSampleRate;
	unsigned mChannels;
	unsigned mLength;

	SoundFileMetadata();
	~SoundFileMetadata() { };
	
	void dump();						///< pretty-print the receiver
};

///
/// Here's the abstract sound file reader/writer class, a sample player UGen.
/// The concrete subclasses represent sound file APIs, not individual formats.
///
///	The sound file player is actually a powerful sampler, supporting arbitrary transposition
/// using wavetable interpolation or a sample rate convertor; Sound files are seekable and also writeable.
///

class Abst_SoundFile : public WavetableOscillator, public Writeable, public Seekable {
public:									/// Constructor. Values not passed default to null.
	Abst_SoundFile(string path, int start = -1, int stop = -1);
	Abst_SoundFile(Abst_SoundFile & otherSndFile);	///< Copy constructor -- shares sample buffer
	~Abst_SoundFile();
	

	static bool isSndfileName(const char * path);			///< Answer whether the given name looks like a snd file
	static SoundFileFormat sndfileNameType(const char * path);	///< Answer the snd file type
	static const char * mimeType(const char * path);				///< Answer the MIME type based on the file name

	///< open file and get stats; read it if "load"
	virtual void openForRead(bool load = true) throw (CException) = 0;
													/// Open a file for writing. 
													/// Default values are some common format.
	virtual void openForWrite(SoundFileFormat format = kSoundFileFormatAIFF, 
					unsigned channels = 1, 
					unsigned rate = 44100,
					unsigned bitDepth = 16) throw (CException) = 0;
													/// seek to some position relative to "whence"
	virtual unsigned seekTo(int position, SeekPosition whence) throw(CException) = 0;
	unsigned seekTo(int position) throw(CException) { return seekTo(position, kPositionStart); }
	
													///< read a buffer from the file (possibly all of it)
	virtual void readBufferFromFile(unsigned numFr) = 0;

													/// UGen operations are implemented here
													/// copy next buffer from file cache
	virtual void nextBuffer(Buffer &outB) throw (CException);
													/// write a buffer of data into the file
	virtual void writeBuffer(Buffer &inB) throw (CException) = 0;
	virtual void writeBuffer(Buffer &inB, unsigned fromFrame, unsigned toFrame)
			throw (CException) = 0;
	virtual SampleBuffer buffer(unsigned bufNum) { return mWavetable.buffer(bufNum); }

										/// accessors
	unsigned channels() const;						///< # chans
	unsigned duration() { return mNumFrames; };		///< number of frames in the sound file
	float durationInSecs();							///< actual duration of the selected portion in sec
//	virtual SoundFileFormat format() = 0;			///< get format
	unsigned sampleSize() { return mBytesPerSample; }	///< get the bytes-per-sample
	SoundFileMode mode() { return mMode; }			///< r/w mode
	unsigned cacheSize() { return mWavetable.mNumAlloc; }		///< size in frames of cached portion
	
	void mergeToMono();								///< average all the channels to mono
	virtual void setToEnd();						///< set to end position
	virtual void trigger();							///< reset to start
	virtual void close() = 0;						///< close file
	virtual void freeBuffer();						///< free the file cache
	void convertRate(int fromRate, int toRate);		///< perform sample-rate conversion

	bool isValid() { return mIsValid; }				///< answer if a valid file/buffer
	bool isActive();								///< answer if currently active
	virtual bool isCached();						///< answer if file is loaded into RAM
	virtual bool isCached(unsigned samps);			///< answer if file has X samples in RAM

	virtual void setPath(string path);				///< set file name path string
	string path() { return mPath; }					///< file name
	virtual void dump();							///< log snd file props
	
	int startFrame() { return mStart; }				///< get/set start frame
	void setStart(int val);
	void setStartSec(float val);
	void setStartRatio(float val);
	int stopFrame() { return mStop; }				///< get/set stop frame
	void setStop(int val);
	void setStopSec(float val);
	void setStopRatio(float val);
	void setBase(int val);
	unsigned int base() { return mBase; };

	double playbackRate() { return mRate; }			///< playback rate (pitch ratio)
	void setRate(UnitGenerator & frequency);		///< set the receiver's playback rate (pitch ratio)
	void setRate(float frequency);

	bool isLooping() { return mIsLooping; }			///< get/set looping state
	void setIsLooping(bool tLooping) { mIsLooping = tLooping; }

	SoundFileMetadata * mProperties;				///< the ID3 tags properties

protected:
	string mPath;									///< file name
	SoundFileMode mMode;							///< r/w mode
	SoundFileFormat mFormat;						///< sf format
	bool mIsValid;									///< is my file valid?
	bool mIsLooping;								///< am i looping start-stop?
	int mStart, mStop;								///< starting/ending frames (or -1 if not used)
	double mRate;									///< sample rate ratio
	unsigned mNumFrames;							///< # sample frames
	unsigned mBytesPerSample;						///< the # of bytes per sample
	unsigned mBase;									///< starting frame in file of buffer
	
	virtual void initFromSndfile() = 0;				///< read SF header
	void checkBuffer(unsigned numFrames);			///< allocate buffer lazily
	void checkBuffer(unsigned numChans, unsigned numFrames);
	bool readTags() throw (CException);				///< read the ID3 or other tags. Returns true if able to read them.
};

///
/// SoundCue -- a pointer to a segment of a sound file
///

class SoundCue : public UnitGenerator {

public:
	SoundCue();							/// Constructor
	SoundCue(string name, Abst_SoundFile *file = 0, int start = 1, int stop = -1);
//	SoundCue(string name, int start = 1, int stop = -1);
	~SoundCue();

	string mName;						///< my name
	Abst_SoundFile * mFile;				///< the file I point into
	int mStart, mStop, mCurrent;		///< the start/stop samples I represent
	UnitGenerator *mReadRate;			///< my playback rate

	void readFrom(FILE *input);			///< for loading file lists
	void dump(void);					///< pretty-print me
						/// UGen operations
	void nextBuffer(Buffer & outputBuffer) throw(CException);	///< copy next buffer from cache

	bool isActive();
	unsigned channels() const { return (mFile->channels()); }
	void setToEnd(void);
	void trigger(void);
	float duration() const { return (float) (mStop - mStart); }	///< duration in frames

protected:
	float mFloatCurrent;				///< current pointer as a float
};

#ifdef USE_SNDFILEBUFFER

///
/// SoundFileBuffer is a buffer that streams a sound file
///

class SoundFileBuffer : public Buffer {
public:									/// Constructor. Values not passed default to null.
	SoundFileBuffer(string path, unsigned numFrames = CSL_mSndFileFrames);
	SoundFileBuffer(Abst_SoundFile & otherSndFile);	///< Copy constructor -- shares sample buffer
	~SoundFileBuffer();
	
											/// answer a samp ptr with offset
	virtual SampleBuffer samplePtrFor(unsigned channel, unsigned offset);
											/// answer a samp ptr tested for extent (offset + maxFrame)
	virtual SampleBuffer samplePtrFor(unsigned channel, unsigned offset, unsigned maxFrame);
	
											/// convenience accessors for sample buffers
	virtual SampleBuffer buffer(unsigned bufNum) { return mBuffers[bufNum]; }
	
											/// Set the buffer pointer (rare; used in joiners)
	virtual void setBuffer(unsigned bufNum, SampleBuffer sPtr) { 
				throw LogicError("Cannot set pointer to file buffer"); };
	virtual void setBuffer(unsigned bufNum, unsigned offset, sample samp) { 
				throw LogicError("Cannot set pointer to file buffer"); };

protected:
//											///< check if the buffer's big enough -- allocate it lazily
//	virtual void checkBuffer(unsigned numFrames);
//	virtual void checkBuffer(unsigned numChans, unsigned numFrames);

	Abst_SoundFile * mFile;					///< the file I point into

};

#endif

#ifdef UNDEFINED						// this was never completed...

class SampleFile : public Abst_SoundFile {
public:
	SampleFile();							/// Constructor
	SampleFile(string name, Abst_SoundFile *file = 0, int start = 1, int stop = -1);
	~SampleFile();
							/// Data members
	unsigned mMIDIKey;			// sample's MIDI key #
	double mFrequency;			// sample's actual frequency
	double mMinRatio;			// min transp.
	double mMaxRatio;			// max transp. (often 1.0)
	
	double ratioForKey(int desiredMIDI);
	double ratioForPitch(int desiredMIDI);
};

#endif

}

#endif
