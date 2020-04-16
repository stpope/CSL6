//
//  SoundFileCA.cpp -- sound file class using CoreAudio
//
//	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
//

#include "SoundFileCA.h"

// JUCE types

typedef signed char                 int8;
typedef unsigned char               uint8;
typedef signed short                int16;
typedef unsigned short              uint16;
typedef signed int                  int32;
typedef unsigned int                uint32;

#define CONVERT_16_BIT(unsV, sinV, bigE)				\
	if (bigE)											\
		unsV = ((unsV << 8) | (unsV >> 8));				\
	sinV = unsV & 0x7fff;								\
	if (unsV & 0x8000)									\
		sinV += 0x8000

#define CONVERT_24_BIT(res, ptr, bigE)					\
	if (bigE)											\
		res = (((int) ptr[2]) << 16)					\
			| (((uint32) (uint8) ptr[1]) << 8)			\
			| ((uint32) (uint8) ptr[0]);				\
	else												\
		res = (((int) ptr[0]) << 16)					\
			| (((uint32) (uint8) ptr[1]) << 8)			\
			| ((uint32) (uint8) ptr[2])

using namespace csl;

//
// CASoundFile implementation
//

CASoundFile::CASoundFile(string tpath, int tstart, int tstop, bool doRead) 
		: Abst_SoundFile(tpath, tstart, tstop),
		  mURL(0) {
					
	if (tpath.size() > 0)				// set up the file URL
		mURL = CFURLCreateWithFileSystemPath (NULL, 
				(CFStringCreateWithCString (NULL, tpath.c_str(), kCFStringEncodingMacRoman)), 
			kCFURLPOSIXPathStyle, false);
	if ( ! doRead)
		return;
	try {
		openForRead(doRead);						// read and cache whole file
		setToEnd();
	} catch (CException & e) {
//		logMsg(kLogError, "File open exception caught: %s", e.mMessage.c_str());
		return;
	}
}

CASoundFile::CASoundFile(CASoundFile & otherSndFile)
		: Abst_SoundFile(otherSndFile),
		  mURL(0) { }

CASoundFile::CASoundFile(CFURLRef path, bool doRead)
		: Abst_SoundFile(""),
		  mURL(path) {
	char theName[CSL_NAME_LEN];
	CFStringGetCString (CFURLGetString(path), theName, CSL_NAME_LEN, kCFStringEncodingMacRoman);
	mPath = string(theName);
	if ( ! doRead)
		return;
	try {
		openForRead(doRead);						// read and cache whole file
		setToEnd();
	} catch (CException & e) {
//		logMsg(kLogError, "File open exception caught: %s", e.mMessage.c_str());
		return;
	}
}

CASoundFile::~CASoundFile() { /* no-op */ }

// Accessors

SoundFileFormat CASoundFile::format() {
	return kSoundFileFormatAIFF;			// ??
}

// set up the receiver's variables based on the file

void CASoundFile::initFromSndfile() {
	mIsValid = (mFrameRate != 0.0);
	if ( ! mIsValid) {
//		logMsg(kLogError, "Cannot open sound file \"%s\"", mPath.c_str());
		return;
	}
	if (mStart > 0)
		seekTo(mStart, kPositionStart);
	else
		mStart = 0;
	if (mStop < 0)
		mStop = mNumFrames;
}

// CASoundFile::openForRead()

void CASoundFile::openForRead(bool load) throw (CException) {
	OSStatus err = noErr;	
	mMode = kSoundFileRead;
	UInt32 propertySize, fileSize;

	if (mURL == 0)								// create URL is opened with a file name
		mURL = CFURLCreateFromFileSystemRepresentation(NULL, 
								(const UInt8*) mPath.c_str(), mPath.size(), false);
												// open audio file
	AudioFileOpenURL(mURL, 0x01, kAudioFileCAFType, &mSoundID);
	
	err = AudioFileGetPropertyInfo(mSoundID,	// AudioFileID inAudioFile,
							  kAudioFilePropertyAudioDataPacketCount, // AudioFilePropertyID inPropertyID,
							  &propertySize,	// UInt32 *outDataSize,
							  NULL);			// UInt32 *isWritable
	if (err) { 
		printf("\nCASoundFile::openForRead: AudioFileGetPropertyInfo Error = %x\nFile: \"%s\"\n\n", 
				(int) err, mPath.c_str()); 
		return;
	}
	err = AudioFileGetProperty(mSoundID, kAudioFilePropertyAudioDataPacketCount, &propertySize, &fileSize);
	if (err) { 
		printf("\nCASoundFile::openForRead: AudioFileGetProperty Error = %x\nFile: \"%s\"\n\n", 
				(int) err, mPath.c_str()); 
		return;
	}
	AudioStreamBasicDescription fileFormat;
	propertySize = sizeof(AudioStreamBasicDescription);
	err = AudioFileGetProperty(mSoundID, kAudioFilePropertyDataFormat, &propertySize, &fileFormat);
	if (err) { 
		printf("\nCASoundFile::openForRead: AudioFileGetProperty2 Error = %x\nFile: \"%s\"\n\n", 
				(int) err, mPath.c_str()); 
		return;
	}
//	logMsg("Format = 0x%x", fileFormat.mFormatID);
//	logMsg("Flags = 0x%x", fileFormat.mFormatFlags);
	mFrameRate = (unsigned) fileFormat.mSampleRate;
	mNumChannels = (unsigned) fileFormat.mChannelsPerFrame;
	mNumFrames = (unsigned) fileSize;
	mBigEndian = fileFormat.mFormatFlags & kAudioFormatFlagIsBigEndian;
	mBytesPerSample = (unsigned) fileFormat.mBitsPerChannel / 8;
	if (mBytesPerSample == 0) {
		logMsg(kLogError, "Sound file \"%s\" has mBitsPerChannel = 0\n", mPath.c_str());
		mBytesPerSample = 2;
	}
	
	this->initFromSndfile();
	if ( ! mIsValid) {
		logMsg(kLogError, "Cannot open sound file \"%s\"\n", mPath.c_str());
		return;
	}
		
	if (load && (mNumFrames <= CGestalt::maxSndFileFrames())) {		// read file if size < global max
//		logMsg("Open/read sound file \"%s\" %d frames %g sec %d channels", 
//				mPath.c_str(), duration(), durationInSecs(), channels());
		this->readBufferFromFile(mNumFrames);				//////// read entire file ////////
		mCurrentFrame = mStart;
	}
}

void CASoundFile::openForWrite(SoundFileFormat tformat, unsigned tchannels, 
		unsigned rate, unsigned bitDepth) throw (CException) {
	logMsg("Error: CASoundFile::openForWrite unsupported");			
	// no-op
}

void CASoundFile::close() {
	freeBuffer();
}

// read some samples from the file into the temp buffer

void CASoundFile::readBufferFromFile(unsigned numFrames) {
	OSStatus err = noErr;	
	SampleBuffer sampleBufferPtr;
	unsigned currentFrame = mCurrentFrame;
	unsigned myChannels = mNumChannels;
	UInt32 numBytes, fileSize;

	this->checkBuffer(numFrames);			// check my buffer, allocate if necessary
											// if we are at the end of the file and not looping
	if ((currentFrame >= (unsigned) mStop) && !mIsLooping) {
		for (unsigned i = 0; i < mNumChannels; i++) {
			sampleBufferPtr = mWavetable.buffer(i);
			memset(sampleBufferPtr, 0, numFrames * sizeof(sample));
		}
		return;
	}
	fileSize = numFrames;
	numBytes = numFrames * myChannels * mBytesPerSample;
	unsigned char * soundBuffer = (unsigned char *) malloc(numBytes);	// create a temp buffer to read into
	
//		OSStatus AudioFileReadPackets (
//		   AudioFileID                  inAudioFile,
//		   Boolean                      inUseCache,
//		   UInt32                       *outNumBytes,
//		   AudioStreamPacketDescription *outPacketDescriptions,
//		   SInt64                       inStartingPacket,
//		   UInt32                       *ioNumPackets,
//		   void                         *outBuffer
//		);
															// read the samples
	err = AudioFileReadPackets(mSoundID, false, &numBytes, NULL, currentFrame, &fileSize, soundBuffer);
	if (err) { 
		printf("\nCASoundFile::openForRead: AudioFileReadPackets Error = %x\nFile: \"%s\"\n\n", 
				(int) err, mPath.c_str()); 
		return;
	}
	if (fileSize != numFrames) {
		printf("\nCASoundFile::openForRead: read too few samples, %d instead of %d\n", 
				(int) fileSize, numFrames); 
	}
															// now process the short buffer just read in
	unsigned short * tBfr = (unsigned short *) soundBuffer;
	unsigned char * cBfr = soundBuffer;
	unsigned short uVal;
	int iVal;
	short sVal;
//	printf("\nCASoundFile::readBufferFromFile: %d ch %d bps\n\n", myChannels, mBytesPerSample); 
	if (myChannels == 1) {									// copy mono data
		sample * sampPtr = mWavetable.buffer(0);			// this code is lifted from JUCE (AiffAudioFormat.cpp)
		if (mBytesPerSample == 2) {							// 16-bit samples
			sample scale = 1.0f / 32867.0f;
			for (unsigned j = 0; j < fileSize; j++) {		// copy/scale sample data
				uVal = *tBfr++;
				CONVERT_16_BIT(uVal, sVal, mBigEndian);
				*sampPtr++ = (sample) sVal * scale;
			}
		} else if (mBytesPerSample == 3) {					// 24-bit samples
			sample scale = (sample) (1.0 / 8388608.0);
			for (unsigned j = 0; j < fileSize; j++) {		// copy/scale sample data
				CONVERT_24_BIT(iVal, cBfr, mBigEndian);
				*sampPtr++ = (sample) iVal * scale;
				cBfr += 3;
			}
		}
	} else if (myChannels == 2) {							// multichannel files: copy/scale, then deinterleave
		sample * sampBuf = (sample *) malloc(numFrames * myChannels * sizeof(sample));
		sample * sampPtr = sampBuf;
		if (mBytesPerSample == 2) {							// 16-bit samples
			sample scale = 1.0f / 32867.0f;					// copy/scale sample data
			for (unsigned j = 0; j < (fileSize * myChannels); j++) {
				uVal = *tBfr++;
				CONVERT_16_BIT(uVal, sVal, mBigEndian);
				*sampPtr++ = (sample) sVal * scale;
			}
		} else if (mBytesPerSample == 3) {					// 24-bit samples
			sample scale = (sample) (1.0 / 8388608.0);
			for (unsigned j = 0; j < (fileSize * myChannels); j++) {	// copy/scale sample data
				CONVERT_24_BIT(iVal, cBfr, mBigEndian);
				*sampPtr++ = (sample) iVal * scale;
				cBfr += 3;
			}
		}
		Interleaver interl;									// multichannel deinterleaver
		interl.deinterleave(mWavetable, sampBuf, fileSize, myChannels);
		free(sampBuf);
	}														// else? ToDo: handle other # channels?
	free(soundBuffer);										// free the temp buffer
	currentFrame += numFrames;

															// if we are past the end of the file...
	if (currentFrame > (unsigned) mStop) {
		unsigned numFramesRemaining = currentFrame - mStop;
		unsigned numFramesRead = numFrames - numFramesRemaining;
//		SampleBuffer tempBufferPtr = sampleBufferPtr + (numFramesRead * myChannels);
		if (mIsLooping) {									// loop back to start of file
			while (numFramesRead < numFrames) {
				currentFrame = seekTo(0, kPositionStart);
						// ToDo: handle loop here			// call JUCE read function
//				mAFReader->read(mWavetable.mBuffers, myChannels, mCurrentFrame, numFrames, false);
				currentFrame += numFramesRead;
			}
		} else {
			unsigned bytesToClear = numFramesRemaining * sizeof(sample);
			for (unsigned i = 0; i < mNumChannels; i++) {
				sampleBufferPtr = mWavetable.buffer(i);
				memset(sampleBufferPtr, 0, bytesToClear);
			}
		}
	}
	return;
}

// Seek

unsigned CASoundFile::seekTo(int position, SeekPosition whence) throw(CException) {
	if (this->isCached())
		return mCurrentFrame;
	switch (whence) {
		case kPositionStart: 
			mCurrentFrame = position; 
			break;
		case kPositionCurrent: 
			mCurrentFrame = mCurrentFrame - position; 
			break;
		case kPositionEnd: 
			mCurrentFrame = duration() - position; 
			break;
		default: 
			mCurrentFrame = SEEK_CUR;
			logMsg("Error: Invalid position seek flag. Used kPositionCurrent.");			
			break;
	}
	return mCurrentFrame;
}

// write a CSL buffer to the interleaved output file

void CASoundFile::writeBuffer(Buffer &inputBuffer) throw(CException) {
	logMsg("Error: CASoundFile::writeBuffer unsupported");			
	// no-op
}

// log snd file props

void CASoundFile::dump() {
	const char * nam = path().c_str();
	if (strlen(nam) > 50)
		logMsg("SndFile \"%s\"\n\t\t%d Hz, %d ch, %5.3f sec %s", 
				nam, frameRate(), channels(), durationInSecs(),
				(mBigEndian ? "BigE" : "LittleE"));
	else
		logMsg("SndFile \"%s\" - %d Hz, %d ch, %5.3f sec %s", 
				nam, frameRate(), channels(), durationInSecs(),
				(mBigEndian ? "BigE" : "LittleE"));
}

