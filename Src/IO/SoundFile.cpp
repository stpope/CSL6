//
//  Abst_SoundFile.cpp -- CSL's abstract sound file class
//	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
//

#include "SoundFile.h"

#ifdef USE_TAGS
#include <taglib/fileref.h>			// libTag includes
#include <taglib/tag.h>
#endif

#ifdef CSL_WINDOWS
#define strcasestr(x, y)   strstr(x, y)
#endif

using namespace csl;

SoundFileMetadata::SoundFileMetadata() { }

void SoundFileMetadata::dump() {
	if (mTitle.empty()) {
		logMsg("No metadata");
		return;
	}
	int secs = mLength % 60;						// make a mins:secs display for the duration
	int mins = (mLength - secs) / 60;
	char dur[CSL_WORD_LEN];
	if (secs < 10)
		sprintf(dur, "%d:0%d", mins, secs);
	else
		sprintf(dur, "%d:%d", mins, secs);
	logMsg("Tags: \"%s\" - %s - %s - %d - %s min", 
			mTitle.c_str(), mArtist.c_str(), mAlbum.c_str(), mYear, dur);
	logMsg("		trk %d - %s - %d kbps - %d Hz - %d ch", 
			mTrack, mGenre.c_str(), mBitRate, mSampleRate, mChannels);
//	if ( ! mComment.empty()) 
//		logMsg("		Comment: %s", mComment.c_str());
}


/// Sound file name extensions we recognize - ToDo: should this be in a config file?

const char * gSndFileExts[] = { "mp3", "wav", "aiff", "aif", "mp4", "m4p", "m4b", "m4a", "aac", "flac", 0 };

///< Answer whether the given name looks like a snd file

bool Abst_SoundFile::isSndfileName(const char * path) {
	const char * dot = strrchr(path, '.');			// find file name ext
	if ( ! dot)
		return false;
	dot++;
	if ( ! dot)
		return false;
	for (unsigned i = 0; gSndFileExts[i]; i++) {
		if(strcasestr(dot, gSndFileExts[i]))
			return true;
	}
	return false;
}

///< Answer the snd file type

SoundFileFormat Abst_SoundFile::sndfileNameType(const char * path) {
	const char * lastDot = strrchr(path, '.');		// find the last dot in the name
	if ( ! lastDot)
		return kSoundFileFormatOther;
	lastDot++;
	if ( ! lastDot)
		return kSoundFileFormatOther;
	if (strcasestr(lastDot, "wav"))				// guess the file type from the file name
		return kSoundFileFormatWAV;
	if (strcasestr(lastDot, "mp3"))
		return kSoundFileFormatMP3;
	if (strcasestr(lastDot, "aif"))
		return kSoundFileFormatAIFF;
	if (strcasestr(lastDot, "aiff"))
		return kSoundFileFormatAIFF;
	if (strcasestr(lastDot, "m4a"))
		return kSoundFileFormatMP4;
	if (strcasestr(lastDot, "mp4"))
		return kSoundFileFormatMP4;
	if (strcasestr(lastDot, "m4b"))
		return kSoundFileFormatMP4;
	if (strcasestr(lastDot, "aac"))
		return kSoundFileFormatAAC;
	if (strcasestr(lastDot, "flac"))
		return kSoundFileFormatFLAC;
	if (strcasestr(lastDot, "caf"))
		return kSoundFileFormatCAF;
	if (strcasestr(lastDot, "ogg"))
		return kSoundFileFormatOGG;
	if (strcasestr(lastDot, "shn"))
		return kSoundFileFormatSHN;
	if (strcasestr(lastDot, "snd"))
		return kSoundFileFormatSND;
	return kSoundFileFormatOther;
}

// Answer the MIME type based on the file name

const char * Abst_SoundFile::mimeType(const char * path) {
	SoundFileFormat fmt = Abst_SoundFile::sndfileNameType(path);
	switch (fmt) {
		case kSoundFileFormatAIFF: return "audio/x-aiff";
		case kSoundFileFormatMP3:  return "audio/mpeg";
		case kSoundFileFormatMP4:  return "audio/mp4a-latm";
		case kSoundFileFormatWAV:  return "audio/x-wav";
		default:				   return "audio/basic";
	}
}

// Abst_SoundFile Constructors

Abst_SoundFile::Abst_SoundFile(string tpath, int tstart, int tstop) : 
			WavetableOscillator(), Writeable(), Seekable(), 
			mProperties(0),
			mPath(string(tpath)), 
			mMode(kSoundFileClosed),
			mFormat(kSoundFileFormatOther),
			mIsValid(false), 
			mIsLooping(false),
			mStart(tstart), 
			mStop(tstop),
			mRate(1.0),
			mNumFrames(0),
			mBytesPerSample(0),
			mBase(0) { /* no-op */}

///< Copy constructor -- shares sample buffer

Abst_SoundFile::Abst_SoundFile(Abst_SoundFile & otherSndFile) :
			WavetableOscillator(), Writeable(), Seekable(), 
			mProperties(otherSndFile.mProperties),
			mMode(otherSndFile.mode()), 
			mIsValid(otherSndFile.isValid()), 
			mIsLooping(otherSndFile.isLooping()),
			mStart(otherSndFile.startFrame()), 
			mStop(otherSndFile.stopFrame()),
			mRate(otherSndFile.playbackRate()),
			mBase(otherSndFile.mBase) {
	this->setWaveform(otherSndFile.mWavetable);
	mCurrentFrame = 0;
	if ( ! otherSndFile.isCached())
		logMsg(kLogError, "Cannot copy uncached sound file \"%s\"", mPath.c_str());
	setPath(otherSndFile.path());
}

// Clean up data allocated

Abst_SoundFile::~Abst_SoundFile() {
//	freeBuffer();		this is done by the WaveTableOsc destructor
	if (mProperties) 
		delete mProperties;
}

void Abst_SoundFile::setPath(string tpath) {
	mPath = tpath;
}

unsigned Abst_SoundFile::channels() const {
	return mIsValid ? mNumChannels : 0;
}

float Abst_SoundFile::durationInSecs() {
	return mIsValid ? ((float) (mStop - mStart) / (float) mFrameRate) : 0.0;
}

void Abst_SoundFile::freeBuffer() {
	if (mWavetable.mAreBuffersAllocated)			// if no buffer allocated
		mWavetable.freeBuffers();
}

// check if the buffer's big enough

void Abst_SoundFile::checkBuffer(unsigned numFrames) {
	if ( ! mWavetable.mAreBuffersAllocated) {				// if no sample read buffer allocated
		mWavetable.freeBuffers();
		mWavetable.setSize(mNumChannels, numFrames);
		mWavetable.allocateBuffers();
	} else if (mWavetable.mNumFrames < numFrames) {			// if asking for more samples than fit in the buffer
		logMsg("Reallocating sound file buffers (%d)", numFrames * mNumChannels);	
		mWavetable.freeBuffers();
		mWavetable.setSize(mNumChannels, numFrames);
		mWavetable.allocateBuffers();
	}
//#ifdef CSL_USE_SRConv
//	if ( ! mSRConvBuffer.mAreBuffersAllocated) {					// if no SRC buffer allocated
//		mSRConvBuffer.setSize(1, CGestalt::maxBufferFrames() * mNumChannels);
//		mSRConvBuffer.allocateBuffers();
//	}
//#endif
}

void Abst_SoundFile::checkBuffer(unsigned numChans, unsigned numFrames) {
	if ( ! mWavetable.mAreBuffersAllocated) {				// if no sample read buffer allocated
		mNumChannels = numChans;
		mWavetable.setSize(mNumChannels, numFrames);
		mWavetable.allocateBuffers();
							// if asking for more samples than fit in the buffer
	} else if ((mWavetable.mNumFrames < numFrames) || (mNumChannels != numChans)) {
		logMsg("Reallocating sound file buffers (%d)", numFrames * mNumChannels);	
		mNumChannels = numChans;
		mWavetable.freeBuffers();
		mWavetable.setSize(mNumChannels, numFrames);
		mWavetable.allocateBuffers();
	}
}

// average all the channels to mono

void Abst_SoundFile::mergeToMono() {
	if ( ! mWavetable.mAreBuffersAllocated) {
		logMsg("Abst_SoundFile::mergeToMono - buffers not allocated");	
		return;
	}
	if (mNumChannels == 1) {
		logMsg("Abst_SoundFile::mergeToMono - sound file already mono");	
		return;
	}
	Buffer newBuf(1, mWavetable.mNumFrames);	// create a new mono buffer
	newBuf.allocateBuffers();
												// sum channels
	unsigned numCh = mNumChannels;
	for (unsigned i = 0; i < mWavetable.mNumFrames; i++) {
		sample sum = 0.0f;
		for (unsigned j = 0; j < numCh; j++)
			sum += (mWavetable.buffer(j))[i];
												// store the average
		newBuf.setBuffer(0, i, sum / numCh);
	}											// now overwrite my own mWavetable
	mWavetable.copyFrom(newBuf);
	newBuf.mDidIAllocateBuffers = false;
	mWavetable.mDidIAllocateBuffers = true;
	mNumChannels = 1;
}

// Abst_SoundFile::convertRate - perform sample-rate conversion

void Abst_SoundFile::convertRate(int fromRate, int toRate) {
#ifndef USE_SRC
	return;
#else
	mWavetable.convertRate(fromRate, toRate);
	mFrameRate = toRate;
	mNumFrames = mWavetable.mNumFrames;
#endif
}

// ~~~~~~~ Accessors ~~~~~~~~

void Abst_SoundFile::setStart(int val) { 
	mStart = val;
	if (mStart < 0) 
		mStart = 0;
	if ((unsigned) mStart >= duration()) 
		mStart = (int) duration();
	if (mIsValid)
		seekTo(mStart);
}

void Abst_SoundFile::setStartSec(float val) {
	setStart((int) (val * mFrameRate));
}

void Abst_SoundFile::setStartRatio(float val) {
	setStart((int) (val * duration()));
}

void Abst_SoundFile::setStop(int val) { 
	mStop = val;
	if (mStop < 0) 
		mStop = 0;
	int du = (int) duration();
	if (mStop > du) 
		mStop = du;
}

void Abst_SoundFile::setStopSec(float val) {
	setStop((int) (val * mFrameRate));
}

void Abst_SoundFile::setStopRatio(float val) {
	setStop((int) (val * duration()));
}

void Abst_SoundFile::setBase(int val) {
	mBase = val;
}

// Rate and transposition

void Abst_SoundFile::setRate(UnitGenerator & frequency) { 
	if ( ! mInputs[CSL_RATE])
		this->addInput(CSL_RATE, frequency);
	else
		mInputs[CSL_RATE]->mUGen = & frequency;
}

void Abst_SoundFile::setRate(float frequency) { 
	mRate = frequency;
#ifdef CSL_DEBUG
	logMsg("FrequencyAmount::set scale input value");
#endif
}

bool Abst_SoundFile::isActive() {
	if ( ! mIsValid)
		return false;
	return (mCurrentFrame < (unsigned) mStop);
}

bool Abst_SoundFile::isCached() { 
	return (mWavetable.mNumFrames == duration()); 
}

///< answer if file has X samples in RAM

bool Abst_SoundFile::isCached(unsigned samps) {	
//	logMsg("isCached : %d > %d + %d", mWavetable.mNumFrames, mCurrentFrame, samps);
	return (mWavetable.mNumFrames >= (mCurrentFrame + samps));
}

// trigger the file to start

void Abst_SoundFile::trigger() {
	if (mStart > 0) {
		mCurrentFrame = mStart;
		seekTo(mStart);
	} else {
		mCurrentFrame = 0;
		seekTo(0);
	}
}

// set the pointer to the end of the file

void Abst_SoundFile::setToEnd() {
	mCurrentFrame = mStop;
}

// Read the ID3 tags. Returns true if able to read them from the file

bool Abst_SoundFile::readTags() throw (CException) {
#ifdef USE_TAGS
	if ( ! mProperties) 
		mProperties = new SoundFileMetadata;
	TagLib::FileRef tfil(mPath.c_str());
	if ( ! tfil.isNull() && tfil.tag()) {
		TagLib::Tag *tag = tfil.tag();
		mProperties->mTitle		= string(tag->title().toCString());
		mProperties->mArtist	= string(tag->artist().toCString());
		mProperties->mAlbum		= string(tag->album().toCString());
		mProperties->mYear		= tag->year();
		mProperties->mComment	= string(tag->comment().toCString());
		mProperties->mTrack		= tag->track();
		mProperties->mGenre		= string(tag->genre().toCString());
	}
	if( ! tfil.isNull() && tfil.audioProperties()) {
		TagLib::AudioProperties *properties = tfil.audioProperties();
		mProperties->mBitRate	= properties->bitrate();
		mProperties->mSampleRate= properties->sampleRate();
		mProperties->mChannels	= properties->channels();
		mProperties->mLength	= properties->length();
	}
	return !tfil.isNull();
#else
	return false;
#endif
}

// log snd file props

void Abst_SoundFile::dump() {
	const char * nam = path().c_str();
	if (strlen(nam) > 50)
		logMsg("SndFile \"%s\"\n\t\t%d Hz, %d ch, %5.3f sec", 
				nam, frameRate(), channels(), durationInSecs());
	else
		logMsg("SndFile \"%s\" - %d Hz, %d ch, %5.3f sec", 
				path().c_str(), frameRate(), channels(), durationInSecs());
}

////////////// next_buffer -- the work is done here //////////

void Abst_SoundFile::nextBuffer(Buffer &outputBuffer) throw(CException) {
	unsigned numFrames = outputBuffer.mNumFrames;
	unsigned currentFrame = mCurrentFrame;
	DECLARE_SCALABLE_CONTROLS;						// declare the scale/offset buffers and values

	if (currentFrame >= (unsigned) mStop) {			// if done
		outputBuffer.zeroBuffers();
		return;
	}
	if (currentFrame + numFrames >= (unsigned) mStop) {		// if final window
		numFrames = mStop - currentFrame;
		outputBuffer.zeroBuffers();
	}
	if ( ! this->isCached(numFrames)) {				// if not playing from cache buffer
		this->readBufferFromFile(numFrames);		// read from file
	}

	LOAD_SCALABLE_CONTROLS;							// load the scaleC and offsetC from the constant or dynamic value

	if (mRate == 1) {												// if playing at normal rate
		if (scalePort->isFixed() && offsetPort->isFixed()			// if fixed scale/offset, use memcpy
					&& (scaleValue == 1) && (offsetValue == 0)) {
			unsigned numBytes = numFrames * sizeof(sample);			// buffer copy loop
			for (unsigned i = 0; i < outputBuffer.mNumChannels; i++) {
				int which = csl_min(i, (mNumChannels - 1));
				SampleBuffer sndPtr = mWavetable.buffer(which) + currentFrame;
				SampleBuffer outPtr = outputBuffer.buffer(i);
				memcpy(outPtr, sndPtr, numBytes);					// here's the memcpy
			}
		} else {													// else loop applying scale/offset
			sample samp;
			for (unsigned i = 0; i < outputBuffer.mNumChannels; i++) {
				SampleBuffer buffer = outputBuffer.buffer(i);	// get pointer to the selected output channel
				SampleBuffer dPtr = mWavetable.buffer(csl_min(i, (mNumChannels - 1))) + currentFrame;
				for (unsigned j = 0; j < numFrames; j++) {			// here's the sample loop
					samp = (*dPtr++ * scaleValue) + offsetValue; 	// get and scale the file sample
					*buffer++ = samp;
					UPDATE_SCALABLE_CONTROLS;						// update the dynamic scale/offset
				}
				scalePort->resetPtr();
				offsetPort->resetPtr();
			}
		}
	} else {										// if mRate != 1.0, 
													// use wavetable interpolation
		this->setFrequency(mRate);
		for (unsigned i = 0; i < mNumChannels; i++)
			WavetableOscillator::nextBuffer(outputBuffer, i);	
	}
	currentFrame += numFrames;								// increment buf ptr
	if ((currentFrame >= (unsigned) mStop) && mIsLooping)	// if we are past the end of the file...
		currentFrame = 0;							// this will click, have to call nextBuffer() recursively here
	mCurrentFrame = currentFrame;					// store back to member
	return;
}


////////////////////////////////////////////////////////////////////////////////////////////
//
// Sound Cue implementation
//

// Generic constructors

SoundCue::SoundCue() {
	mFile = NULL;
	mCurrent = 0;
}

SoundCue::SoundCue(string name, Abst_SoundFile *file, int start, int stop) : 
			mName(name), 
			mFile(file), 
			mStart(start), 
			mStop(stop) { 
	mCurrent = mStart;
	mReadRate = (UnitGenerator*) 0;
}

SoundCue::~SoundCue() { }

// Read an instance's data from a file
// The format is that used in the SeSpSp layer index file:
// 	 name 	   start 	stop
//	b5b.r21e.aiff 13230080 15876096		-- NB: these are in FRAMES

void SoundCue::readFrom (FILE *input) {
	char cmName[128];
	unsigned start, stop;

	if(fscanf(input, "%s %u %u\n", cmName, &start, &stop) == 3) {
	    mName = cmName;
	    mStart = start;
	    mStop = stop;
	    mCurrent = mStart;
	}
}

// Pretty-print the receiver

void SoundCue::dump() {
	logMsg("\tSC: \"%s\" %d - %d (%.3f)\n", 
			mName.c_str(), mStart, mStop, 
			((float)(mStop - mStart) / (float)(mFile->frameRate() * mFile->channels())));
}

// Copy samples from the file's sample buffer to the output
// I assume that the # of channels in the output is the same as in my file, i.e.,
// To play a mono file, "wrap" it in a Deinterleaver

void SoundCue::nextBuffer(Buffer &outputBuffer) throw(CException) {
	unsigned numFrames = outputBuffer.mNumFrames;
	unsigned toCopy = 0, toZero;
	SampleBuffer out;
	
	for (unsigned h = 0; h < outputBuffer.mNumChannels; h++) {
		out = outputBuffer.buffer(h);
		if (mCurrent >= mStop) {			// if done
			for (unsigned i = 0; i < numFrames; i++) 
				*out++ = 0.0;
			continue;
		}	
		SampleBuffer samps = (mFile->mWavetable.buffer(0)) + (mCurrent * mFile->channels()) + h;
		if ( ! mFile->isValid()) {		// if no file data
			fprintf(stderr, "\tCannot play uncached sound file \"%s\"n", mName.c_str());
			for (unsigned i = 0; i < numFrames; i++) 
				*out++ = 0.0;
			continue;
		}
		toCopy = numFrames;		// figure out how many samples to copy
		toZero = 0;	
		if ((mCurrent + numFrames) >= (unsigned) mStop) {
			toCopy = mStop - mCurrent;
			toZero = numFrames - toCopy;
		//	fprintf(stderr, "\tSample \"%s\" finished\n", mName);
		}						// Now do the copy loops
		for (unsigned i = 0; i < toCopy; i++) 
			*out++ = *samps++;
		for (unsigned i = 0; i < toZero; i++) 
			*out++ = 0.0;
	}
	mCurrent += toCopy;			// increment the receiver's sample pointer
	return;
}

// Utility functions

bool SoundCue::isActive() { 
	if (mFile == 0)
		return (false);
	return (mCurrent < mStop);
}

void SoundCue::setToEnd(void) {
	mCurrent = mStop;
}

void SoundCue::trigger(void) {
	mCurrent = mStart;
	mFloatCurrent = 0.0;
}

#ifdef USE_SNDFILEBUFFER

////////////////////////////////////////////////////////////////////////////////////////////
//
// SoundFileBuffer implementation
//

#ifdef USE_JSND
	#include "SoundFileJ.h"
	#define SoundFile JSoundFile			// JUCE snd file class
#endif

#ifdef USE_LSND
	#include "SoundFileL.h"
	#define SoundFile LSoundFile			// LibSndFile snd file class
#endif

#ifdef USE_CASND
	#include "SoundFileCA.h"
	#define SoundFile CASoundFile			// CoreAudio snd file class
#endif

SoundFileBuffer::SoundFileBuffer(string path, unsigned numFrames) 
		: Buffer(CGestalt::numOutChannels(), numFrames) {
	mFile = (Abst_SoundFile * ) (new SoundFile(path));
	mFile->openForRead(false);
	mFile->readBufferFromFile(CGestalt::sndFileFrames());
	mNumChannels = mFile->channels();
	mNumFrames = mFile->duration();
	mNumAlloc = csl_min(CGestalt::sndFileFrames(), mNumFrames);
	mMonoBufferByteSize = mNumFrames * mNumChannels * sizeof(sample);
	mAreBuffersAllocated = true;
	mDidIAllocateBuffers = false;
	
	mBuffers = new SampleBuffer[mNumChannels];	// reserve space for buffers
	for (unsigned i = 0; i < mNumChannels; i++)
		setBuffer(i, mFile->buffer(i));
}

///< Copy constructor -- shares sample buffer

SoundFileBuffer::SoundFileBuffer(Abst_SoundFile & otherSndFile) {
	// setBuffer
}

SoundFileBuffer::~SoundFileBuffer() { }


/// answer a samp ptr, testing that it's in RAM

sample * SoundFileBuffer::samplePtrFor(unsigned channel, unsigned offset){
	if ((mFile->base() <= offset) && (mFile->base() + mFile->cacheSize() > offset))
		return(mFile->mWavetable.buffer(channel) + (offset - mFile->base()));
	if (mFile) {
		mFile->seekTo(offset);
		mFile->readBufferFromFile(mFile->mWavetable.mNumFrames);
		return(mFile->mWavetable.buffer(channel));
	}
	return NULL;
}

/// answer a samp ptr tested for extent (offset + maxFrame)

sample * SoundFileBuffer::samplePtrFor(unsigned channel, unsigned offset, unsigned maxFrame){
	if ((mFile->base() <= offset) && ((mFile->base() + mNumAlloc) > (offset + maxFrame)))
		return (mFile->mWavetable.buffer(channel) + (offset - mFile->base()));
	if (mFile) {
		mFile->seekTo(offset);
		mFile->readBufferFromFile(mFile->mWavetable.mNumFrames);
		return(mFile->mWavetable.buffer(channel));
	}
	return NULL;
}

#endif

#ifdef UNDEFINED

//class SampleFile : public Abst_SoundFile {
//public:
//	SampleFile();							/// Constructor
//	SampleFile(string name, Abst_SoundFile *file = 0, int start = 1, int stop = -1);
//	~SampleFile();
//							/// Data members
//	unsigned mMIDIKey;			// sample's MIDI key #
//	double mFrequency;			// sample's actual frequency
//	double mMinRatio;			// min transp.
//	double mMaxRatio;			// max transp. (often 1.0)
//	
//	double ratioForKey(int desiredMIDI);
//	double ratioForPitch(int desiredMIDI);

SampleFile::SampleFile() {

}

SampleFile::SampleFile(string tpath, unsigned MIDIKey, double frequency, double minRatio, double maxRatio) {

}

SampleFile::~SampleFile() {

}

#endif
