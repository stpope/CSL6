///
///	Spectrograph_demo.cpp -- a simple CSL "spectrum analyzer" program -- load a file and display it
///	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
///

#include "Test_Support.cpp"		// include all of CSL core and the test support functions
#include "SoundFile.h"
#include "Spectral.h"
#include "NullIO.h"				// NullIO

/// Cmd-line options (see the handler in Test_Support.cpp):
///		-r samp-rate  -b blk-siz  -o num-out-ch  -i num-in-ch  -v verbosity  -l log-period  -p out-port  -u
///
/// Added options
///		"-w val" = set FFT size
///		"-h val" = set hop size

// Here's the raster header structure

typedef struct {
	long ras_magic;
	long ras_width;
	long ras_height;
	long ras_depth;
	long ras_length;
	long ras_type;
	long ras_maptype;
	long ras_maplength;
} rasterfile_header;

// raster file format magic number

#define	RAS_MAGIC	0x59a66a95

// file types

#define RT_OLD			0		/* Raw pixrect image in 68000 byte order */
#define RT_STANDARD		1	/* Raw pixrect image in 68000 byte order */
#define RT_BYTE_ENCODED	2	/* Run-length compression of bytes */
#define RT_FORMAT_RGB	3		/* XRGB or RGB instead of XBGR or BGR */
#define RT_FORMAT_TIFF	4		/* tiff <-> standard rasterfile */
#define RT_FORMAT_IFF	5		/* iff (TAAC format) <-> standard rasterfile */
#define RT_EXPERIMENTAL 0xffff	/* Reserved for testing */

// map types

#define RMT_NONE		0
#define RMT_EQUAL_RGB	1
#define RMT_RAW		2

///
/// Spectrogram is an observer of an FFT that holds onto all windows in a 2D array
///

class Spectrogram : public Observer {
public:
	Spectrogram(unsigned wide, unsigned high, UnitGenerator & model);
	~Spectrogram();

	unsigned mWidth;			// width  in windows
	unsigned mHeight;			// height (fft length)
	SampleBufferVector  mData;				// 2-D data array
	unsigned mPosition;			// offset pointer

	void normalize();				// Normalize to 0 to +1 after taking the logarithm
	void compress();				// Compress data by taking the square root after normalization
	void update(void * arg);		// handle update from my model
	void saveToRas(char * name);
};

// Constructor stores instance variables and allocates large data 2-D buffer

Spectrogram::Spectrogram(unsigned wide, unsigned high, UnitGenerator & model) {
	mWidth = wide;
	mHeight = high;								// Now allocate the buffer
	mData = (float **) malloc(mWidth * sizeof(float *));
	if ((char *) mData == NULL)
		throw MemoryError("can't allocate buffer");
	for (unsigned i = 0; i < mWidth; ++i) {
		mData[i] = (float *) malloc(mHeight * sizeof(float));
		if ((char *) mData[i] == NULL)
			throw MemoryError("can't allocate buffer");
	}
	mPosition = 0;
	model.attachObserver((Observer *) this);		// add me as an observer of the model FFT
	logMsg("Spect::constr(%d @ %d) [%x - %x]", mWidth, mHeight, this, mData[mPosition]);
}

Spectrogram::~Spectrogram() {
	for (unsigned i = 0; i < mWidth; i++)
		free(mData[i]);
	free(mData);
}

// Update just stores the data buffer

void Spectrogram::update(void * arg) {
	Buffer * buf = (Buffer *) arg;
//	logMsg("Spect::update(%d [%d @ %d (%d)] %x - %x)", mPosition, mWidth, mHeight, buf->mNumFrames, this, mData[mPosition]);
//	logMsg("update(%d [%d @ %d] %d)", mPosition, mWidth, mHeight, buf->mNumFrames);
	if (mPosition >= mWidth)
		return;
	memcpy(mData[mPosition], buf->monoBuffer(0), (mHeight * sizeof(sample)));
	mPosition++;
}

// Normalize to 0 to +1 after taking the logarithm

#define USE_LOG_SCALE

void Spectrogram::normalize() {
	float maxVal = 0.0;					// loop to find max val
	for (unsigned i = 0; i < mWidth; i++) {
		for (unsigned j = 0; j < mHeight; j++) {
			float thisVal = mData[i][j];
#ifdef USE_LOG_SCALE
			thisVal = log(thisVal);
			if (thisVal < 0.0)
				thisVal = 0.0;
			mData[i][j] = thisVal;
#endif
			if (thisVal > maxVal)
				maxVal = thisVal;
	}}									// loop to normalize data
	logMsg("Spect max = %7.5f", maxVal);
	for (unsigned i = 0; i < mWidth; i++)
		for (unsigned j = 0; j < mHeight; j++)
			mData[i][j] /= maxVal;
}
 
// Compress data by taking the square root after normalization

void Spectrogram::compress() {
	for (unsigned i = 0; i < mWidth; i++)
		for (unsigned j = 0; j < mHeight; j++)
			mData[i][j] = sqrt(sqrt(mData[i][j])); 
		//	mData[i][j] = mData[i][j] * mData[i][j]; 
}

// Store as a raster-format file (assumes byte-swapping is necessary -- i.e., Intel machines)

void Spectrogram::saveToRas(char * name) {
	FILE * fp;					// input file pointer
	fp = fopen(name, "wb");
	if (fp == NULL) {
		perror("Cannot open raster file");
		exit(1);
	}
	rasterfile_header head;
	head.ras_magic = RAS_MAGIC;
	head.ras_width = mWidth;
	head.ras_height = mHeight;
	head.ras_depth = 8;
	head.ras_length = (mWidth * mHeight);
	head.ras_type = RT_STANDARD;
	head.ras_maptype = RMT_NONE;
	head.ras_maplength = 0;
	unsigned long * ptr, swapped;
	ptr = (unsigned long *) & head;
	for (unsigned i = 0; i < (sizeof(rasterfile_header) / sizeof(unsigned long)); i++) {
		swapped = htonl( * ptr++);
		fwrite( & swapped, sizeof(unsigned long), 1, fp);
	}
	unsigned char pixmap[mWidth * mHeight];
	unsigned k = 0;			// now loop through data on row-major order from the top (scan lines)
	for (unsigned j = mHeight; j > 0; j--)
		for (unsigned i = 0; i < mWidth; i++)
			pixmap[k++] = 255 - (int) (mData[i][j] * 255.0);
	ptr = (unsigned long *) & pixmap;
	for (unsigned i = 0; i < (mWidth * mHeight / sizeof(unsigned long)); i++) {
		swapped = htonl( * ptr++);
		fwrite( & swapped, sizeof(unsigned long), 1, fp);
	}
	fclose(fp);
}

///
/// MAIN -- Save a spectrogram to a raster file
///

int main (int argc, const char * argv[]) {
	unsigned value;						// variable used for arg parsing
	unsigned fftLen = 1024;
	unsigned hopSize = 512;					
	csl::CGestalt::setBlockSize(hopSize);		// set a smaller block size for some tests

	READ_CSL_OPTS;								// parse the standard CSL options: 
	if (find_option(argc, argv, 'w', &value) > 0) {	
		fftLen = atoi(argv[value]);
		printf("Setting fft len to: %d\n", fftLen);
	}
	if (find_option(argc, argv, 'h', &value) > 0) {		
		hopSize = atoi(argv[value]);
		printf("Setting hop size to: %d\n", hopSize);
	}
//	SoundFile fi("/Volumes/Content/Sound/3-IKnowGod/Whales2/1b-long-calls-intro.aif");
	SoundFile fi("/Volumes/Content/Sound/3-IKnowGod/Whales2/test-tones.aif");
//	SoundFile fi("test_sweep.aiff");
	try {
		fi.openForRead();
	} catch (CException & e) {
		logMsg(kLogError, "File open exception caught: %s", e.mMessage.c_str());
		return 1;
	}
	float dur = fi.duration() / fi.rate();
	unsigned wins = (fi.duration() / hopSize) + 10;
	logMsg("playing sound file \"%s\"\n\t\trate: %d  #ch: %d  dur: %5.3f  wins: %d  size: %d\n", fi.mPath.c_str(), fi.rate(), fi.channels(), dur, wins, fftLen);

	FFT fft(fi, CSL_FFT_REAL, kFFTMeasure, fftLen);	// FFT
	Spectrogram spect(wins, fftLen/4, fft);		// Spectrogram takes width, height, model FFT
	PAIO * theIO = new PAIO;					// PortAudio IO object
//	NullIO * theIO = new NullIO;				// Null IO object
	
	logMsg("CSL playing FFT of snd file...");
	theIO->setRoot(fft);					// make some sound
	theIO->open();
	theIO->start();						// start the driver callbacks

	sleepSec(dur + 0.25);					// sleep for dur plus a bit

	logMsg("CSL done.");
	theIO->stop();						// stop the driver and close down
	theIO->close();	

	logMsg("formatting data and writing raster file.");
	spect.normalize();
	spect.compress();
	spect.saveToRas("spect1.ras");
		
	return 0;							// exit
}

