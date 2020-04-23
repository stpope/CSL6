//
//  SoundFileL.cpp -- sound file class using libsndfile
//	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
//

#include "SoundFileL.h"
#include "math.h"
#include "SoundFileMP3.h"

using namespace csl;

#ifdef CSL_WINDOWS
#define strcasestr strstr
#endif

// Factory method

LSoundFile * LSoundFile::openSndfile(string tpath, int tstart, int tstop, bool doRead) {
	const char * fname = tpath.c_str();
#ifdef USE_libMAD
	if (strcasestr(fname, ".mp3")) {							// if an MP3
		return new MP3File(tpath, tstart, tstop);
	}															// if an MP4/AAC
#endif
#ifdef USE_libFAAD
	if ((strcasestr(fname, ".mp4")) || (strcasestr(fname, ".m4a")) 
			|| (strcasestr(fname, ".m4p")) || (strcasestr(fname, ".aac"))) {
		return new MP4File(tpath, tstart, tstop);
	}	
#endif
	return new LSoundFile(tpath, tstart, tstop, doRead);		// else try LibSndfile
}

LSoundFile * LSoundFile::openSndfile(float maxDurInSecs, string tpath) {
	const char * fname = tpath.c_str();
#ifdef USE_libMAD
	if (strcasestr(fname, ".mp3")) {							// if an MP3
		return new MP3File(maxDurInSecs, tpath);
	}															// if an MP4/AAC
#endif
#ifdef USE_libFAAD
	if ((strcasestr(fname, ".mp4")) || (strcasestr(fname, ".m4a")) 
			|| (strcasestr(fname, ".m4p")) || (strcasestr(fname, ".aac"))) {
		return new MP4File(maxDurInSecs, tpath);
	}	
#endif
	return new LSoundFile(maxDurInSecs, tpath);						// else try LibSndfile
}


// LSoundFile Constructors

LSoundFile::LSoundFile(string tpath, int tstart, int tstop, bool doRead, float maxDurInSecs)
			: Abst_SoundFile(tpath, tstart, tstop), 
			  mSFInfo(new SF_INFO), 
			  mSndfile(NULL),
			  mMaxDurInSecs (maxDurInSecs) {
	mSFInfo->format = 0;
	mNumFrames = 0;
#ifdef CSL_USE_SRConv
	mSRateConv = NULL;
#endif
	if ( ! doRead)
		return;
	try {
		openForRead();						// read and cache whole file
		setToEnd();
	} catch (CException & e) {
//		logMsg(kLogError, "File open exception caught: %s", e.mMessage.c_str());
		return;
	}
}

// this version sets maxSize and always reads

LSoundFile::LSoundFile(float maxDurInSecs, string tpath)
		: Abst_SoundFile(tpath, -1, -1), 
		  mSFInfo(new SF_INFO), 
		  mSndfile(NULL),
		  mMaxDurInSecs (maxDurInSecs) {
	mSFInfo->format = 0;
	mNumFrames = 0;
#ifdef CSL_USE_SRConv
	mSRateConv = NULL;
#endif
	try {
		openForRead();						// read and cache whole file
		setToEnd();
	} catch (CException & e) {
	}
}

// Copy constructor -- shares sample buffer

LSoundFile::LSoundFile(LSoundFile & otherSndFile)
			: Abst_SoundFile(otherSndFile), 
			  mSFInfo(otherSndFile.sfInfo()), 
			  mSndfile(otherSndFile.sndFile()) {
	if ( ! otherSndFile.isCached())
		logMsg(kLogError, "Cannot copy uncached sound file \"%s\"", mPath.c_str());
//	logMsg("Open sound file \"%s\"", mPath.c_str());
#ifdef CSL_USE_SRConv
	mSRateConv = NULL;
#endif
}

// Clean up data allocated

LSoundFile::~LSoundFile() {
	if (mSndfile)
		sf_close(mSndfile);
	freeBuffer();
	delete mSFInfo;
#ifdef CSL_USE_SRConv
	if (mSRateConv)
		src_delete (mSRateConv);
#endif
}

//
// set up the receiver's variables based on the file
//

void LSoundFile::initFromSndfile() {
	mIsValid = (mSndfile != NULL);
	if ( ! mIsValid) {
//		logMsg(kLogError, "Cannot open sound file \"%s\"", mPath.c_str());
		return;
	}
	mFrameRate = (unsigned) mSFInfo->samplerate;
	mNumChannels = (unsigned) mSFInfo->channels;
	mNumFrames = (unsigned) mSFInfo->frames;
	if (mMaxDurInSecs > 0.0 && (mNumFrames > (unsigned) (mMaxDurInSecs * mFrameRate)))				// truncate if too long
		mNumFrames = mMaxDurInSecs * mFrameRate;
	mBase = 0;
	switch (mSFInfo->format & 0x0f) {
		case 1:		mBytesPerSample = 1;	break;			// Signed 8 bit
		case 2:		mBytesPerSample = 2;	break;			// Signed 16 bit
		case 3:		mBytesPerSample = 3;	break;			// Signed 24 bit
		case 4:		mBytesPerSample = 4;	break;			// Signed 32 bit
		case 5:		mBytesPerSample = 1;	break;			// unsigned 8 bit
		case 6:		mBytesPerSample = 4;	break;			// 32 bit float
		case 7:		mBytesPerSample = 8;	break;			// 64 bit float
	}
	if (mStart > 0)		seekTo(mStart);
	else				mStart = 0;
	if (mStop < 0)		mStop = mNumFrames;
	
#ifdef CSL_USE_SRConv
	if (mMode == kSoundFileRead)	{						// sample rate convertor (SRC) state struct
		mSRateConv = src_new (CSL_SRC_MODE, mNumChannels, & mSRCReturn);
		if (mSRateConv == NULL)
			logMsg (kLogError, "SampleRateConvertor creation error : %s\n", src_strerror (mSRCReturn));
	}
#endif
}

//void LSoundFile::checkBuffer(unsigned numFrames) {
//	Abst_SoundFile::checkBuffer(numFrames);
//#ifdef CSL_USE_SRConv
//	if ( ! mSRConvBuffer.mAreBuffersAllocated) {					// if no SRC buffer allocated
//		mSRConvBuffer.setSize(1, CGestalt::maxBufferFrames() * mNumChannels);
//		mSRConvBuffer.allocateBuffers();
//	}
//#endif
//}

///< answer if file has all of its samples in RAM

bool LSoundFile::isCached() {	
	return (mWavetable.mNumFrames == mNumFrames);
}

///< answer if file has X samples in RAM

bool LSoundFile::isCached(unsigned samps) {	
	return (mWavetable.mNumFrames > (mCurrentFrame + samps));
}

SoundFileFormat LSoundFile::format() {
	switch (mSFInfo->format) {
		case SF_FORMAT_WAV:
			return kSoundFileFormatWAV;		break;
		case SF_FORMAT_AIFF:
			return kSoundFileFormatAIFF;	break;
		case SF_FORMAT_AU:
			return kSoundFileFormatSND;		break;
		case SF_FORMAT_RAW:
			return kSoundFileFormatRaw;		break;
		default:
			logMsg("Error: Couldn't get the file format.");
	}
	return kSoundFileFormatRaw;
}

// ~~~~ Open functions ~~~~

void LSoundFile::openForRead(bool load) throw (CException) {
	if (mMode == kSoundFileRead)
		return;
	mMode = kSoundFileRead;
	mSndfile = sf_open(mPath.c_str(), SFM_READ, mSFInfo);		// libsndfile open call
	if (mSndfile != NULL) {										// check result
		this->initFromSndfile();
		if (load && (mNumFrames <= CGestalt::maxSndFileFrames())) {	// read file if size < global max
//		logMsg("Open/read sound file \"%s\" %d frames %g sec %d channels", 
//				mPath.c_str(), duration(), durationInSecs(), channels());		
			this->readBufferFromFile(mNumFrames);					// read entire file, de-interleave
		}
//		mCurrentFrame = mStart;
//		this->setWaveform(mSampleBuffer);						// set up oscillator pointers
//		mWavetable.mDidIAllocateBuffers = true;
//		if (this->isCached())
//			logMsg("\t\tSound file cached");
	}
	if (mIsValid)
		this->readTags();
}

void LSoundFile::openForWrite(SoundFileFormat tformat, unsigned tchannels, 
					unsigned rate, unsigned bitDepth) throw (CException) {
	if (mMode == kSoundFileWrite)
		return;
	mMode = kSoundFileWrite;
	mSFInfo->samplerate = rate;
	mSFInfo->channels = tchannels;
	switch (tformat) {
		case kSoundFileFormatWAV:
			mSFInfo->format = SF_FORMAT_WAV;	break;
		case kSoundFileFormatAIFF:
			mSFInfo->format = SF_FORMAT_AIFF;	break;
		case kSoundFileFormatSND:
			mSFInfo->format = SF_FORMAT_AU;		break;
		case kSoundFileFormatRaw:
		default:
			mSFInfo->format = SF_FORMAT_RAW;	break;
	}
	if (bitDepth == 16)
		mSFInfo->format = mSFInfo->format | SF_FORMAT_PCM_16;
	else if (bitDepth == 32)
		mSFInfo->format = mSFInfo->format | SF_FORMAT_FLOAT;
	else
		logMsg("Invalid bitDepth for sound file %s. Use either 16 or 32 bits.", mPath.c_str());

	if ( ! sf_format_check(mSFInfo)) {
		logMsg(kLogError, "Invalid format for sound file %s", mPath.c_str());
		mIsValid = false;
		return;
	}
	mSndfile = sf_open(mPath.c_str(), SFM_WRITE, mSFInfo);
	initFromSndfile();
}

void LSoundFile::openForReadWrite() throw (CException) {
	mMode = kSoundFileReadWrite;
	mSndfile = sf_open(mPath.c_str(), SFM_RDWR, mSFInfo);
	initFromSndfile();
}

void LSoundFile::close() {
	sf_close(mSndfile);
	mSndfile = NULL;
	freeBuffer();
}

// file seek via sf_seek call

unsigned LSoundFile::seekTo(int position, SeekPosition whence) throw(CException) {
	int whenceInt;
	if (this->isCached())
		return mCurrentFrame;
	switch (whence) {
		case kPositionStart: whenceInt = SEEK_SET; break;
		case kPositionCurrent: whenceInt = SEEK_CUR; break;
		case kPositionEnd: whenceInt = SEEK_END; break;
		default: 
			whenceInt = SEEK_CUR;
			logMsg("Error: Invalid position seek flag. Used kPositionCurrent.");			
			break;
	}									// sf_seek call
	mCurrentFrame = sf_seek(mSndfile, position, whenceInt);
	mBase = mCurrentFrame;
	return mCurrentFrame;
}

///////////////// next_buffer -- the work is done here //////////

void LSoundFile::nextBuffer(Buffer &outputBuffer) throw(CException) {
	unsigned numFrames = outputBuffer.mNumFrames;
	unsigned currentFrame = mCurrentFrame;
	DECLARE_SCALABLE_CONTROLS;							// declare the scale/offset buffers and values
	bool atEnd = false;

	if (currentFrame >= (unsigned) mStop) {			// if done
		outputBuffer.zeroBuffers();
		return;
	}
	if (currentFrame + numFrames >= (unsigned) mStop) {		// if final window
		numFrames = mStop - currentFrame;
		outputBuffer.zeroBuffers();
		atEnd = true;
	}
	if ( ! this->isCached(numFrames)) {				// if not playing from cache buffer
		if (! atEnd)
			this->readBufferFromFile(numFrames);		// read from file
	}
	LOAD_SCALABLE_CONTROLS;							// load the scaleC and offsetC from the constant or dynamic value
	if (mRate == 1.0) {								// if playing at normal rate
		if (IS_UNSCALED) {							// and there's no scale/offset
			unsigned numBytes = numFrames * sizeof(sample);			// buffer copy loop
			for (unsigned i = 0; i < outputBuffer.mNumChannels; i++) {
				SampleBuffer dataOutPtr = mWavetable.buffer(csl_min(i, (mNumChannels - 1))) + currentFrame;
				memcpy(outputBuffer.buffer(i), dataOutPtr, numBytes);
			}
		} else {									// do full scaled/offset copy
			sample samp;
			for (unsigned i = 0; i < outputBuffer.mNumChannels; i++) {
				SampleBuffer buffer = outputBuffer.buffer(i);		// get pointer to the selected output channel
				SampleBuffer dPtr = mWavetable.buffer(csl_min(i, (mNumChannels - 1))) + currentFrame;

				for (unsigned i = 0; i < numFrames; i++) {			// sample loop
					samp = (*dPtr++ * scaleValue) + offsetValue; 	// get and scale the file sample
					*buffer++ = samp;
					UPDATE_SCALABLE_CONTROLS;						// update the dynamic scale/offset
				}
				scalePort->resetPtr();
				offsetPort->resetPtr();
			}
		}
		currentFrame += numFrames;					// incrememnt buf ptr
	} else {						// if mRate != 1.0, 
	
#ifndef CSL_USE_SRConv				// normal case: use wavetable interpolation
		for (unsigned i = 0; i < mNumChannels; i++)
			WavetableOscillator::nextBuffer(outputBuffer, i);	
		currentFrame += numFrames;					// incrememnt buf ptr
		
#else								// else use Sample rate convertor
		int answer;									// if we're doing SRC transposition
		if (this->isCached()) 						// if playing from cache buffer
			mSRateData.data_in = mWavetable.buffer(0) + (currentFrame * mNumChannels);	
		else
			mSRateData.data_in = mWavetable.buffer(0);	
		mSRateData.data_out = mSRateData.data_in;	// store for later
		mSRateData.input_frames = numFrames;	
		mSRateData.output_frames = numFrames;	
		mSRateData.data_out = mSRConvBuffer.buffer(0);	
		mSRateData.end_of_input = 0;	
		mSRateData.src_ratio = mRate;	
		if (answer = src_set_ratio (mSRateConv, mRate))			// set the transposition ratio
			logMsg (kLogError, "SampleRateConvertor configuration error : %s\n", 
						src_strerror (answer));
						
		if (answer = src_process (mSRateConv, & mSRateData))	// run SampleRateConvertor
			logMsg (kLogError, "SampleRateConvertor runtime error : %s\n", 
						src_strerror (answer));
						
		mSRateData.data_out = mSRConvBuffer.buffer(0);		// reset out ptr
		currentFrame += mSRateData.input_frames_used;			// input frames actually read
		numFrames = mSRateData.output_frames_gen;				// output frames actually written
#endif
	}
	if ((currentFrame >= (unsigned) mStop) && mIsLooping)	// if we are past the end of the file...
		currentFrame = 0;						// this will click, have to call nextBuffer() recursively here
	mCurrentFrame = currentFrame;				// store back to member
	return;
}

// write a CSL buffer to the interleaved output file

void LSoundFile::writeBuffer(Buffer &inputBuffer) throw(CException) {
	unsigned numFrames = inputBuffer.mNumFrames;
	if (mSFInfo->channels > 1) {				// interleave stereo-to-mono
		mWavetable.setSize(1, mNumChannels * numFrames);
		mWavetable.allocateBuffers();
												// call interleaver
		mInterleaver.interleave(inputBuffer, mWavetable.buffer(0), 
					numFrames, mSFInfo->channels);
		sf_writef_float(mSndfile, mWavetable.buffer(0), numFrames);		// libsndfile write 
	} else {									// mono file write
		sf_writef_float(mSndfile, inputBuffer.buffer(0), numFrames);	// libsndfile write 
	}
	mCurrentFrame += numFrames;
	return;
}

void LSoundFile::writeBuffer(Buffer &inputBuffer, unsigned fromFrame, unsigned toFrame) throw (CException) {
	unsigned maxFrames = inputBuffer.mNumFrames;
	if (maxFrames > toFrame)
		maxFrames = toFrame;
	unsigned numFrames = maxFrames - fromFrame;
	if (mSFInfo->channels > 1) {				// interleave stereo-to-mono
		mWavetable.setSize(1, mNumChannels * numFrames);
		mWavetable.allocateBuffers();
												// call interleaver
		mInterleaver.interleave(inputBuffer, mWavetable.buffer(0) + fromFrame,
					numFrames, mSFInfo->channels);
		sf_writef_float(mSndfile, mWavetable.buffer(0), numFrames);		// libsndfile write 
	} else {									// mono file write
		sf_writef_float(mSndfile, inputBuffer.buffer(0) + fromFrame, numFrames);	// libsndfile write
	}
	mCurrentFrame += numFrames;
	return;
}

// readBufferFromFile - read some samples from the file into the temp buffer

void LSoundFile::readBufferFromFile(unsigned numFrames) {
	sf_count_t numFramesRead;
	unsigned currentFrame = mCurrentFrame;
	unsigned myChannels = mSFInfo->channels;

	this->checkBuffer(numFrames);			// check my buffer, allocate if necessary
	SampleBuffer sampleBufferPtr;
	if ((myChannels > 1)) {					// read into temp buffer to multichannel files, then de-interleave
		SAFE_MALLOC(sampleBufferPtr, sample, (myChannels * numFrames));
	} else {
		sampleBufferPtr = mWavetable.buffer(0);
	}
											// if we are at the end of the file and not looping
	if ((currentFrame >= (unsigned) mStop) && !mIsLooping) {
		memset(sampleBufferPtr, 0, numFrames * myChannels * sizeof(sample));
		return;
	}
	mBase = sf_seek(mSndfile, 0, SEEK_CUR);	// get position
											// libsndfile read function
	numFramesRead = sf_readf_float(mSndfile, sampleBufferPtr, numFrames);
	if ((unsigned) numFramesRead != numFrames)
		logMsg(kLogError, "LSoundFile::readBufferFromFile sf_readf_float requested %d got %d", 
							numFrames, numFramesRead);
//	mWavetable.mNumFrames = numFrames;
	mCurrentFrame = 0;
											// if we are past the end of the file...
	if ((mBase + numFrames) > (unsigned) mStop) {
		unsigned numFramesRemaining = numFrames - numFramesRead;
		SampleBuffer tempBufferPtr = sampleBufferPtr + (numFramesRead * myChannels);
		if (mIsLooping) {													// loop back to start of file
		    while ((unsigned) numFramesRead < numFrames) {
				currentFrame = sf_seek(mSndfile, 0, SEEK_SET);				// libsndfile seek/read function
				numFramesRead += sf_readf_float(mSndfile, tempBufferPtr, numFramesRemaining);
				currentFrame += numFramesRead;
			}
		} else {
			unsigned bytesToClear = numFramesRemaining * myChannels * sizeof(sample);
			memset(tempBufferPtr, 0, bytesToClear);
		}
	}
	if (myChannels > 1) {					// auto-de-interleave if multichannel
		mInterleaver.deinterleave(mWavetable, sampleBufferPtr, numFrames, mNumChannels);
		SAFE_FREE(sampleBufferPtr);
	}
	mCurrentFrame = mStart;
}

#ifdef CSL_USE_SRConv						// sample-rate conversion

#define	BUFFER_LEN		4096				/*-(1<<16)-*/

// fcn prototypes below

static sf_count_t sample_rate_convert (SNDFILE *infile, SNDFILE *outfile, 
									int converter, double src_ratio, int channels, double * gain);
		
static double apply_gain (float * data, long frames, int channels, double max, double gain);

#endif // SR_Conv

//-------------------------------------------------------------------------------

// SF_Virtual structure
/*
typedef sf_count_t		(*sf_vio_get_filelen)	(void *user_data) ;
typedef sf_count_t		(*sf_vio_seek)		(sf_count_t offset, int whence, void *user_data) ;
typedef sf_count_t		(*sf_vio_read)		(void *ptr, sf_count_t count, void *user_data) ;
typedef sf_count_t		(*sf_vio_write)		(const void *ptr, sf_count_t count, void *user_data) ;
typedef sf_count_t		(*sf_vio_tell)		(void *user_data) ;

struct SF_VIRTUAL_IO
{	sf_vio_get_filelen	get_filelen ;
	sf_vio_seek			seek ;
	sf_vio_read			read ;
	sf_vio_write		write ;
	sf_vio_tell			tell ;
} ;
*/

// Call-back functions for SF_VIRTUAL_IO struct

//sf_vio_get_filelen lsf_getFileLen(void * vBuffer) {
//	Buffer * buf = (Buffer *) vfile
//}
//
//sf_vio_seek lsf_seek(sf_count_t offset, int whence, void * vBuffer) {
//
//}
//
//sf_vio_tell lsf_tell(void * vBuffer) {
//
//}
//
//
//sf_vio_read lsf_read(void *ptr, sf_count_t count, void * vBuffer) {
//
//}
//
//
//sf_vio_write lsf_write(const void *ptr, sf_count_t count, void * vBuffer) {
//
//}



// OLD WAY

//void LSoundFile::convertRate(char * mTempPath, int fromRate, int toRate) {
//	SNDFILE	*infile, *outfile;
//	SF_INFO sfinfo;
//	sf_count_t count;
//	char tempName[CSL_NAME_LEN];
//	
//	double src_ratio = -1.0, gain = 1.0;
//	int converter;
//	converter = SRC_SINC_BEST_QUALITY;
//	logMsg("Convert-rate of \"%s\" from %d to %d", mTempPath, fromRate, toRate);
//												// create temp name and rename file
//	sprintf(tempName, "%s.TEMP", mTempPath);
//	if (rename(mTempPath, tempName) != 0) {
//		logMsg(kLogError, "Convert-rate cannot rename \"%s\" to \"%s\"", mTempPath, tempName);
//		perror("SoundFile::convertRate");
//		return;
//	}
//	infile = sf_open(tempName, SFM_READ, &sfinfo);
//	src_ratio = (double) toRate / (double) fromRate;
//	sfinfo.samplerate = toRate;
//	if (src_is_valid_ratio (src_ratio) == 0) {
//		logMsg (kLogError, "Error: Sample rate change out of valid range.");
//		sf_close (infile);
//		return;
//	}
//	outfile = sf_open (mTempPath, SFM_WRITE, &sfinfo);
//	if (outfile == NULL) {
//		logMsg (kLogError, "Error: Cannot open output file.");
//		sf_close (infile);
//		return;
//	}
//	sf_command (outfile, SFC_SET_UPDATE_HEADER_AUTO, NULL, SF_TRUE);
//	sf_command (outfile, SFC_SET_CLIPPING, NULL, SF_TRUE);
//	do {
//		count = sample_rate_convert(infile, outfile, converter, src_ratio, sfinfo.channels, &gain);
//	} while (count < 0);
//
//	logMsg("Convert-rate: output frames %ld", (long) count);
//	sf_close(infile);
//	sf_close(outfile);
//	if (remove(tempName) != 0)
//		logMsg(kLogError, "Convert-rate cannot remove temp file \"%s\"\n", tempName);
//}

/////////////////////////
// These 2 functions are taken from libSampleRate
//

//static sf_count_t sample_rate_convert (SNDFILE *infile, SNDFILE *outfile, int converter, 
//				double src_ratio, int channels, double * gain) {
//	static float input [BUFFER_LEN];
//	static float output [BUFFER_LEN];
//	SRC_STATE * src_state;
//	SRC_DATA src_data;
//	int	error;
//	double max = 0.0;
//	sf_count_t	output_count = 0;
//	sf_seek (infile, 0, SEEK_SET);
//	sf_seek (outfile, 0, SEEK_SET);
//
//	/* Initialize the sample rate converter. */
//	if ((src_state = src_new (converter, channels, &error)) == NULL) {	
//		logMsg ("Error : src_new() failed : %s.", src_strerror (error));
//		exit (1);
//	};
//
//	src_data.end_of_input = 0; /* Set this later. */
//
//	/* Start with zero to force load in while loop. */
//	src_data.input_frames = 0;
//	src_data.data_in = input;
//	src_data.src_ratio = src_ratio;
//	src_data.data_out = output;
//	src_data.output_frames = BUFFER_LEN /channels;
//
//	while (1) {
//		/* If the input buffer is empty, refill it. */
//		if (src_data.input_frames == 0) {
//			src_data.input_frames = sf_readf_float (infile, input, BUFFER_LEN / channels);
//			src_data.data_in = input;
//
//			/* The last read will not be a full buffer, so snd_of_input. */
//			if (src_data.input_frames < BUFFER_LEN / channels)
//				src_data.end_of_input = SF_TRUE;
//			}
//
//		if ((error = src_process (src_state, &src_data))) {
//			logMsg ("SRC Error : %s", src_strerror (error));
//			return (0);
//			}
//
//		/* Terminate if done. */
//		if (src_data.end_of_input && src_data.output_frames_gen == 0)
//			break;
//
//		max = apply_gain (src_data.data_out, src_data.output_frames_gen, channels, max, *gain);
//
//		/* Write output. */
//		sf_writef_float (outfile, output, src_data.output_frames_gen);
//		output_count += src_data.output_frames_gen;
//
//		src_data.data_in += src_data.input_frames_used * channels;
//		src_data.input_frames -= src_data.input_frames_used;
//		}
//
//	src_state = src_delete (src_state);
//
//	if (max > 1.0) {	*gain = 1.0 / max;
//		logMsg ("Output has clipped. Restarting conversion to prevent clipping.");
//		output_count = 0;
//		sf_command (outfile, SFC_FILE_TRUNCATE, &output_count, sizeof (output_count));
//		return -1;
//		}
//
//	return output_count;
//} /* sample_rate_convert */
//
//static double apply_gain (float * data, long frames, int channels, double max, double gain) {
//	for (long k = 0; k < frames * channels; k++) {
//		double val = (double) data [k];
//		val *= gain;
//		data[k] = val;
//		val = fabs (val);
//		if (val > max)
//			max = val;
//	}
//	return max;
//} /* apply_gain */
