//
//  SoundFile.cpp -- sound file class using JUCE
//
//	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
//

#include "SoundFileJ.h"

using namespace csl;

//
// JSoundFile implementation
//

// Factory method

JSoundFile * JSoundFile::openSndfile(string tpath, int tstart, int tstop, bool doRead) {
	return new JSoundFile(tpath, tstart, tstop);
}

//JSoundFile * JSoundFile::openSndfile(string path, juce::MemoryInputStream * strm) {
//    return new JSoundFile(path, strm);
//}


//JSoundFile * JSoundFile::openSndfile(float maxDurInSecs, string tpath) {
//	return new JSoundFile(maxDurInSecs, tpath);
//}

JSoundFile::JSoundFile(string tpath, int tstart, int tstop)
	: Abst_SoundFile(tpath, tstart, tstop),
		mAFReader(0),
		mAFWriter(0),
		mIOFile(0) {
	try {
		openForRead();						// read and cache whole file
		setToEnd();
	} catch (CException & e) {
		logMsg(kLogError, "File open exception caught: %s", e.mMessage.c_str());
		return;
	}
}

//JSoundFile::JSoundFile(string tpath, juce::MemoryInputStream * strm)
//    : Abst_SoundFile(tpath, -1, -1),
//        mAFReader(0),
//        mAFWriter(0),
//        mIOFile(0) {
//    try {
//        openForRead(strm);                        // read and cache whole file
//        setToEnd();
//    } catch (CException & e) {
//        logMsg(kLogError, "File open exception caught: %s", e.mMessage.c_str());
//        return;
//    }
//}

//JSoundFile::JSoundFile(string folder, string tpath, int tstart, int tstop)
//	: Abst_SoundFile(folder, tpath, tstart, tstop),
//		mAFReader(0),
//		mAFWriter(0),
//		mIOFile(0),
//		mOutStream(0) {
//	try {
//		openForRead();						// read and cache whole file
//		setToEnd();
//	} catch (CException & e) {
//		logMsg(kLogError, "File open exception caught: %s", e.mMessage.c_str());
//		return;
//	}
//}

JSoundFile::JSoundFile(JSoundFile & otherSndFile)
	: Abst_SoundFile(otherSndFile),
		mAFReader(otherSndFile.mAFReader),
		mAFWriter(otherSndFile.mAFWriter),
		mIOFile(otherSndFile.mIOFile) { }

// d'tor cleans up poinger members

JSoundFile::~JSoundFile() {
	if (mAFReader)
		delete mAFReader;
	if (mAFWriter)
		delete mAFWriter;
//	if (mOutStream)
//		delete mOutStream;
	if (mIOFile)
		delete mIOFile;
}

// Accessors

unsigned JSoundFile::duration() const {
	return mAFReader ? (unsigned) mAFReader->lengthInSamples : 0;
}

SoundFileFormat JSoundFile::format() {
	juce::String fmt = mAFReader->getFormatName();
	if (fmt[0] == 'A') 
		return kSoundFileFormatAIFF;
	else if (fmt[0] == 'W')
		return kSoundFileFormatWAV;
	return kSoundFileFormatOther;
}

// set up the receiver's variables based on the file

void JSoundFile::initFromSndfile() {
	mIsValid = ((mAFReader != NULL) || (mAFWriter != NULL));
	if ( ! mIsValid) {
//		logMsg(kLogError, "Cannot open sound file \"%s\"", mPath.c_str());
		return;
	}
    mCurrentFrame = 0;
	if (mAFReader) {
		mFrameRate = (unsigned) mAFReader->sampleRate;
		mNumChannels = (unsigned) mAFReader->numChannels;
		mNumFrames = (unsigned) mAFReader->lengthInSamples;
		mBytesPerSample = (unsigned) mAFReader->bitsPerSample / 8;
	} else if (mAFWriter) {
		mFrameRate = (unsigned) mAFWriter->getSampleRate();
		mNumChannels = (unsigned) mAFWriter->getNumChannels();
		mNumFrames = 0;
		mBytesPerSample = (unsigned) mAFWriter->getBitsPerSample() / 8;
	}
	if (mStart > 0)
		seekTo(mStart, kPositionStart);
	else
		mStart = 0;
	if ((mStop < 0) && mAFReader)
		mStop = (int) mAFReader->lengthInSamples;
}

void JSoundFile::openForRead(bool load) throw (CException) {
	mMode = kSoundFileRead;
	juce::String fname(mPath.c_str());
	mIOFile = new juce::File(fname);					// create a JUCE file object
	if ( ! mIOFile->exists()) {
//		logMsg(kLogError, "Cannot find sound file \"%s\"", mPath.c_str());
		return;
	}
	juce::AudioFormatManager formatManager;			// get a format manager
	formatManager.registerBasicFormats();		// set it up with the basic types (wav and aiff).
	mAFReader = formatManager.createReaderFor(*mIOFile);

	this->initFromSndfile();
	if ( ! mIsValid) {
		logMsg(kLogError, "Cannot open sound file \"%s\"", mPath.c_str());
		return;
	}
	if (mNumFrames <= CGestalt::maxSndFileFrames()) {		// read file if size < global max
//		logMsg("Read sound file \"%s\" %d frames %g sec %d channels", 
//				mPath.c_str(), duration(), durationInSecs(), channels());
		unsigned numFrames = (unsigned) mAFReader->lengthInSamples;

		this->readBufferFromFile(numFrames);				// read entire file

		mCurrentFrame = mStart;
	}
}

// openForRead given a MemoryInputStream

//void JSoundFile::openForRead(juce::MemoryInputStream * memStrm) throw (CException) {
//    mMode = kSoundFileRead;
////    String fname(mPath.c_str());
////    mIOFile = new File(fname);                    // create a JUCE file object
////    if ( ! mIOFile->exists()) {
//////        logMsg(kLogError, "Cannot find sound file \"%s\"", mPath.c_str());
////        return;
////    }
//    juce::AudioFormatManager formatManager;        // get a format manager
//    formatManager.registerBasicFormats();            // set it up with the basic types (wav and aiff).
//    unique_ptr<juce::MemoryInputStream> mems2 = std::move(memStrm->get());
//    mAFReader = formatManager.createReaderFor(mems2);
//
//    this->initFromSndfile();
//    if ( ! mIsValid) {
//        logMsg(kLogError, "Cannot open sound file \"%s\"", mPath.c_str());
//        return;
//    }
//    if (mNumFrames <= CGestalt::maxSndFileFrames()) {        // read file if size < global max
////        logMsg("Read sound file \"%s\" %d frames %g sec %d channels", 
////                mPath.c_str(), duration(), durationInSecs(), channels());
//        unsigned numFrames = mAFReader->lengthInSamples;
//
//        this->readBufferFromFile(numFrames);                // read entire file
//
//        mCurrentFrame = mStart;
//    }
//}

void JSoundFile::openForWrite(SoundFileFormat tformat, unsigned tchannels, unsigned rate, unsigned bitDepth) 
					throw (CException) {
	mMode = kSoundFileWrite;
	juce::String fname(mPath.c_str());
	mIOFile = new juce::File(fname);					    // create a JUCE file object
	mOutStream = mIOFile->createOutputStream(); // .get();
	juce::StringPairArray metaDict;
	juce::AiffAudioFormat afmt;
	juce::WavAudioFormat wfmt;
//            createWriterFor (OutputStream* out, double sampleRate, unsigned int numberOfChannels,
//                    int bitsPerSample, const StringPairArray& metadataValues, int /*qualityOptionIndex*/)
	switch (tformat) {
		case kSoundFileFormatAIFF:
			mAFWriter = afmt.createWriterFor(mOutStream, (double) rate, tchannels, bitDepth, metaDict, 0);
			break;
		case kSoundFileFormatWAV:
			mAFWriter = wfmt.createWriterFor(mOutStream, (double) rate, tchannels, bitDepth, metaDict, 0);
			break;
		default:
			logMsg (kLogError, "Unsupported sound file format");
			throw IOError("Unsupported sound file format");
	}
	initFromSndfile();
}

void JSoundFile::close() {
	freeBuffer();
	if (mMode == kSoundFileWrite)
		mOutStream->flush();
}

// read some samples from the file into the temp buffer

void JSoundFile::readBufferFromFile(unsigned numFrames) {
	SampleBuffer sampleBufferPtr;
	unsigned currentFrame = mCurrentFrame;
	unsigned myChannels = mNumChannels;

	this->checkBuffer(numFrames);			// check my buffer, allocate if necessary
											// if we are at the end of the file and not looping
	if ((currentFrame >= (unsigned) mStop) && !mIsLooping) {
		for (unsigned i = 0; i < mNumChannels; i++) {
			sampleBufferPtr = mWavetable.buffer(i);
			memset(sampleBufferPtr, 0, numFrames * sizeof(sample));
		}
		return;
	}
											// JUCE read fcn
//	logMsg ("JSound file read %d", numFrames);
											// create a temp AudioSampleBuffer for the wavetable
	juce::AudioSampleBuffer asBuffer(mWavetable.buffers(), myChannels, numFrames);
    mAFReader->read(&asBuffer, mCurrentFrame, numFrames, 0, true, true);
	currentFrame += numFrames;	
//	} else {
//		logMsg (kLogError, "Sound file read error");
//		throw IOError("Sound file read error");
//	}
															// if we are past the end of the file...
	if (currentFrame > (unsigned) mStop) {
		unsigned numFramesRemaining = currentFrame - mStop;
		unsigned numFramesRead = numFrames - numFramesRemaining;
//		SampleBuffer tempBufferPtr = sampleBufferPtr + (numFramesRead * myChannels);
		if (mIsLooping) {									// loop back to start of file
			while (numFramesRead < numFrames) {
				currentFrame = seekTo(0, kPositionStart);
															// call JUCE read function
//				mAFReader->read(mWavetable.buffers(), myChannels, mCurrentFrame, numFrames, false);
				mAFReader->read(&asBuffer, mCurrentFrame, numFrames, 0, true, true);
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
}

//void JSoundFile::convertFormat(unsigned num, unsigned start) {
//	SampleBuffer sampleBufferPtr;
//	int * intBufferPtr;
//	sample normFactor = 1.0 / (float) (32768 << 16);
//	
//	for (unsigned channelIndex = 0; channelIndex < mNumChannels; channelIndex++) {
///		sampleBufferPtr = mWavetable.buffer(channelIndex) + start;
//		intBufferPtr = mConvBuffer[channelIndex];
//		for (unsigned j = 0; j < num; j++) {
//			*sampleBufferPtr++ = ((float) *intBufferPtr++) * normFactor;
//		}
//	}
//}

// Seek

unsigned JSoundFile::seekTo(int position, SeekPosition whence) throw(CException) {
	int whenceInt;
	if (this->isCached()) {
		mCurrentFrame = position;
		return mCurrentFrame;
	}
	switch (whence) {
		case kPositionStart: 
			whenceInt = position; 
			break;
		case kPositionCurrent: 
			whenceInt = mCurrentFrame - position; 
			break;
		case kPositionEnd: 
			whenceInt = duration() - position; 
			break;
		default: 
			whenceInt = SEEK_CUR;
			logMsg("Error: Invalid position seek flag. Used kPositionCurrent.");			
			break;
	}
	if (mOutStream)
		mOutStream->setPosition(whenceInt);
	return mCurrentFrame;
}

// write a CSL buffer to the interleaved output file

void JSoundFile::writeBuffer(Buffer &inputBuffer) throw(CException) {
	unsigned numFrames = inputBuffer.mNumFrames;
	unsigned numChans = inputBuffer.mNumChannels;
											// use an AudioSampleBuffer
	juce::AudioSampleBuffer asb(numChans, numFrames);
	asb.setDataToReferTo (inputBuffer.buffers(),numChans, numFrames);
	mAFWriter->writeFromAudioSampleBuffer (asb, 0, numFrames);
//	if (mAFWriter->writeFromFloatArrays((const float **)inputBuffer.buffers(), 1, numFrames)) {
//		mCurrentFrame += numFrames;
//	} else {
//		logMsg (kLogError, "Sound file write error");
//		throw IOError("Sound file write error");
//	}
}

// write a CSL buffer to the interleaved output file

void JSoundFile::writeBuffer(Buffer &inputBuffer, unsigned fromFrame, unsigned toFrame) throw(CException) {
	unsigned numFrames = inputBuffer.mNumFrames;	
											// JUCE write fcn
//				   writeFromFloatArrays (const float **channels, int numChannels, int numSamples)
	SampleBufferArray inp = inputBuffer.buffers();
	for (unsigned i = 0; i < mNumChannels; i++)
		inp[i] += fromFrame;
//	if (mAFWriter->writeFromFloatArrays((const float **)inp, 1, (toFrame - fromFrame))) {
//		mCurrentFrame += numFrames;	
//	} else {
//		logMsg (kLogError, "Sound file write error");
		throw IOError("Sound file write error");
//	}
	for (unsigned i = 0; i < mNumChannels; i++)
		inp[i] -= fromFrame;
}

