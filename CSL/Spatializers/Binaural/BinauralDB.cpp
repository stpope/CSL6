///
///  BinauralDB.cpp -- HRTF-based binaural panner/spatializers
///
/// Classes
///		HRTF: holds the data that corresponds to an HRTF for a single position.
///		HRTFDatabase: vector of HRTFs; implemented as a Singleton because it's large.
///		BinauralPanner: place sources in 3D using block-wise convolution with an HRTF.
///		BinauralSourceCache: used for caching previous state of spatial sources.
///
///	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
///	Created by Jorge Castellanos on 7/19/06.
///	Rewritten for FFT wrappers and pluggable sound file APIs in 8/09 by STP.
///	Inspired by and partially based on the VST HRTF Plug-in written by Ryan Avery.
///

#include "BinauralDB.h"

using namespace csl;
using namespace std;

// HRTF Constructor allocates memory for the HRTF data

HRTF::HRTF() {
	unsigned hrirLength = HRIR_SIZE;
	unsigned framesPerBlock = HRTF_BLOCK_SIZE;
	unsigned mWindowSize = 2 * framesPerBlock;
	mNumFFTBlocks = hrirLength / framesPerBlock;
	unsigned hrtfLength = framesPerBlock + 1;
												// allocate HRTF: L/R vectors of complex buffers
	SAFE_MALLOC(mHrtfL, SampleComplexVector, mNumFFTBlocks);
	for (unsigned i = 0; i < mNumFFTBlocks; i++) {
		SAFE_MALLOC(mHrtfL[i], SampleComplex, hrtfLength);
	}
	SAFE_MALLOC(mHrtfR, SampleComplexVector, mNumFFTBlocks);
	for (unsigned i = 0; i < mNumFFTBlocks; i++) {
		SAFE_MALLOC(mHrtfR[i], SampleComplex, hrtfLength);
	}
}

// HRTF Constructor allocates memory for the HRTF data and then reads the HRIR file 
// and performs the block-wise FFT into the complex arrays mHrtfL[i] and mHrtfR[i]

HRTF::HRTF(char * filename, FFT_Wrapper & fft) {
	unsigned hrirLength = HRIR_SIZE;
	unsigned framesPerBlock = HRTF_BLOCK_SIZE;
	unsigned mWindowSize = 2 * framesPerBlock;
	mNumFFTBlocks = hrirLength / framesPerBlock;
	unsigned hrtfLength = framesPerBlock + 1;
	float theta, phi;
												// allocate HRTF data storage
	SAFE_MALLOC(mHrtfL, SampleComplexVector, mNumFFTBlocks);
	for (unsigned i = 0; i < mNumFFTBlocks; i++) {
		SAFE_MALLOC(mHrtfL[i], SampleComplex, hrtfLength);
	}
	SAFE_MALLOC(mHrtfR, SampleComplexVector, mNumFFTBlocks);
	for (unsigned i = 0; i < mNumFFTBlocks; i++) {
		SAFE_MALLOC(mHrtfR[i], SampleComplex, hrtfLength);
	}
												// I/O buffers
	Buffer mInBuf(1, mWindowSize), mOutBuf(1, mWindowSize);
	mInBuf.allocateBuffers();					// mInBuf has actual storage

	char *cursor = strstr(filename, "_T");		// parse the name to get theta and phi
	if (cursor == NULL) {						// e.g., IRC_1012_R_R0195_T000_P330.wav
	logMsg(kLogError, "Cannot parse HRIR file name for theta: \"%s\"\n", filename);	
		throw IOError("Cannot parse HRIR filename");
	}
	theta = strtof((cursor + 2), (char **)NULL); // Read the number (degree) for theta, in filename.
	
	cursor = strstr(filename, "_P");
	if (cursor == NULL) {
	logMsg(kLogError, "Cannot parse HRIR file namefor phi: \"%s\"\n", filename);	
		throw IOError("Cannot parse HRIR filename");
	}
	phi = strtof((cursor + 2), (char **)NULL);	// Read the number (degree) for phi, in filename.
	mPosition.set('s', 1.0f, theta / CSL_DEGS_PER_RAD, phi / CSL_DEGS_PER_RAD);
//	logMsg("HRTF reading %g @ %g", theta, phi);

	SoundFile hrirFile(filename);				// open the HRIR file
	hrirFile.openForRead();						// load HRIR file
	
	if ( ! hrirFile.isValid()) {				// check validity
		logMsg("Cannot open sound file \"%s\"", filename);
		throw IOError("Sound file open error");
	}											// Check file's #channels and #frames
	if ((hrirFile.channels() != 2) || (hrirFile.duration() < HRIR_SIZE)) {
		logMsg("Sound file \"%s\" is wrong format: %d ch, %d frames", 
				filename, hrirFile.channels(), hrirFile.duration());
		throw IOError("Sound file open error");
	}
												// Load in HRIR file and convert it to HRTF buffers
	for (unsigned i = 0; i < mNumFFTBlocks; i++) {
		mInBuf.zeroBuffers();					// clear buffer (no zero-padding needed this way)
		memcpy(mInBuf.buffer(0), 				// copy samples from file to buffer
				(hrirFile.mWavetable.buffer(0)) + (i * framesPerBlock), 
				(framesPerBlock * sizeof(sample)));
		mOutBuf.setBuffer(0, (SampleBuffer) mHrtfL[i]);

		fft.nextBuffer(mInBuf, mOutBuf);		// execute Left FFT

		mInBuf.zeroBuffers();
		memcpy(mInBuf.buffer(0), 
				(hrirFile.mWavetable.buffer(1)) + (i * framesPerBlock), 
				(framesPerBlock * sizeof(sample)));
		mOutBuf.setBuffer(0, (SampleBuffer) mHrtfR[i]);

		fft.nextBuffer(mInBuf, mOutBuf);		// execute Right FFT
	}
	hrirFile.close();							// Close the file.
}

// destructor clears memory

HRTF::~HRTF() {
	for (unsigned i = 0; i < mNumFFTBlocks; i++) {
		SAFE_FREE(mHrtfL[i]);
		SAFE_FREE(mHrtfR[i]);
	}
	SAFE_FREE(mHrtfR);
	SAFE_FREE(mHrtfR);
}

// pretty print

void HRTF::dump() {
	logMsg("HRTF with Azimuth: %f \t Elevation: %f", 
			mPosition.theta() * CSL_DEGS_PER_RAD, 
			mPosition.ele() * CSL_DEGS_PER_RAD);
}

// Answer the HRTF's storage size in Bytes

unsigned HRTF::size() {
	return ((mNumFFTBlocks * 2 * (HRTF_BLOCK_SIZE + 1) * sizeof(SampleComplex)) 
		  + (mNumFFTBlocks * 2 * sizeof(SampleComplexVector)));
}

//////////////////////////////////////////////////////////////////////////////////////
///		HRTFDatabase
///

// Static "Singleton" HRTF Database

HRTFDatabase * HRTFDatabase::mDatabase = 0;

// Static Methods

// Access the DB, initializing it lazily

HRTFDatabase* HRTFDatabase::Database() {
	if (mDatabase == NULL) {
		string folder(CGestalt::dataFolder());
		folder += DEFAULT_HRTF_FOLDER;
		mDatabase = new HRTFDatabase(folder.c_str());
	}
	return mDatabase;
}

// Flush the DB

void HRTFDatabase::Destroy() {
	if (mDatabase) {
		delete mDatabase;
		mDatabase = NULL;
	}
}

// constructor that takes an HRTF vector

//void HRTFDatabase::Reload(vector <HRTF *> hrtfs) {
//						// create new DB
//	HRTFDatabase * newDB = new HRTFDatabase(hrtfs);
//						// be careful to avoid race conditions
//	HRTFDatabase * oldDB = mDatabase;
//	mDatabase = newDB;	// set new DB
//	if (oldDB)
//		delete oldDB;	// free old one
//}

// load a new folder into the DB

void HRTFDatabase::Reload(char * folder) {
						// create new DB
	HRTFDatabase * newDB = new HRTFDatabase(folder);
						// be careful to avoid race conditions
	HRTFDatabase * oldDB = mDatabase;
	mDatabase = newDB;	// set new DB
	if (oldDB)
		delete oldDB;	// free old one
}

// HRTFDatabase Constructor (is private)

HRTFDatabase::HRTFDatabase(const char * folder) {
	mWindowSize = 2 * HRTF_BLOCK_SIZE;
	mHRTFLength = HRTF_BLOCK_SIZE ; // + 1;
	mHRIRLength = HRIR_SIZE;
	
	logMsg("Loading the HRTFDatabase %s", folder);
	if (strcasestr(folder, ".dat") != NULL) {	// if the name looks like a DB name
		try {
			loadFromDB(folder);
		} catch (CException ex) {				// handler: log error and exit the program.
			logMsg(kLogError, "Loading the HRTFDatabase: %s\n", ex.mMessage.c_str());	
			exit(0);
		}
	} else if (folder[strlen(folder) - 1] == '/') {	// if the name looks like a folder name
		try {
			char realFolder[CSL_NAME_LEN] = "";
			if (folder[0] == '~') {				// handle paths relative to "~"
				strcpy(realFolder, getenv("HOME"));
				if (strlen(folder) > 1)
					strcat(realFolder, &folder[1]);
			} else
				strcpy(realFolder, folder);
			logMsg("Loading HRTF data from %s", realFolder);

			this->loadFromFolder(realFolder);	// load the folder
			
		} catch (CException ex) {				// handler: log error and exit the program.
			logMsg(kLogError, "Loading the HRTFDatabase: %s\n", ex.mMessage.c_str());	
			exit(0);
		}
	} else {									// if the name looks like the list file name
		try {
//			logMsg("Loading HRTF files from %s", folder);
			loadFromFile(folder);				// load the folder
		} catch (CException ex) {				// handler: log error and exit the program.
			logMsg(kLogError, "Loading the HRTFDatabase: %s\n", ex.mMessage.c_str());	
			exit(0);
		}
	}
}
 
// HRTFDatabase::loadFromFolder: load an IRCAM-format HRTF database folder.
//		This assumes there's a file named "files.txt" in the folder with a list of files.
//		Create this with "ls IRC* > files"
//		ToDo: use directory enumeration or popen("ls IRC*") for this

void HRTFDatabase::loadFromFolder (const char *folder) throw (CException) {
	char filename[CSL_NAME_LEN];	
	strcpy(filename, folder);			// Copy the path to the folder into the "filename" string
	strcat(filename, FLIST_NAME);		// Append the name of the file containing the list of hrtf files
	this->loadFromFile(filename);
}

// Load HRTF data from the given file list

void HRTFDatabase::loadFromFile (const char *listname) throw (CException) {
	char lineBuffer[CSL_NAME_LEN];
	char dirname[CSL_NAME_LEN];
	char filename[CSL_NAME_LEN];
	FILE * listFile;									// text file with the list of HRTF files in the directory
	FFT_Wrapper fft(mWindowSize, CSL_FFT_COMPLEX);		// create a raw-format forward FFT

	listFile = fopen(listname, "r");					// Open the file that contains the list.
	if ( ! listFile) {
		logMsg("Cannot open list file %s for reading.\n", listname);
		return;
	}
	strcpy(dirname, listname);							// copy list file name to get directory name
	char * ext = rindex(dirname, '/');
	ext++;
	*ext = 0;
	while ( ! feof(listFile)) {							// Go thru the file list
		fgets(lineBuffer, 1024, listFile);				// Read one full line
//		logMsg("Read HRTF file %s", lineBuffer);
		int len = strlen(lineBuffer) - 1;
		if (len < 2)
			continue;
		if (lineBuffer[len] == '\n')					// remove EOL from line
			lineBuffer[len] = '\0'; 
		strcpy(filename, dirname);						// Copy the path to the files directory into filename
		strcat(filename, lineBuffer);					// Copy the name from the line in
//		cout << "	Read HRTF file " << filename << endl;

		HRTF * tHRTF = new HRTF(filename, fft);			// Create the HRTF from the file
		mHRTFVector.push_back(tHRTF);					// Add the HRTF to the vector.
	}
	fclose(listFile);									// Close the list file.
//#endif
#ifdef CSL_DEBUG
	dump();
#endif
}

// byte-swap the buffer

#ifdef IPHONE											// byte-swap floats
void swapBuffer(float * buf, unsigned numFloats) {
	float * ptr = buf;
	for (unsigned b = 0; b < numFloats; b++) {
		unsigned int val = (unsigned int) *ptr;
		val = CFSwapInt32LittleToHost(val);
		*ptr++ = (float) val;
	}
}
#endif

// Load HRTF data from the given file list

void HRTFDatabase::loadFromDB (const char *dbName) throw (CException) {
	char lineBuffer[CSL_NAME_LEN];
	char tok[8];
	char nam[16];
	int nHRTFs, nBlocks, hrirLen, fftLen;
	FILE * dbFile;										// text file with the list of HRTF files in the directory
	PointVector hrtfPoints;
	int azim, elev;
	
	dbFile = fopen(dbName, "r");						// Open the file that contains the list.
	if ( ! dbFile) {
		logMsg("Cannot open DB file %s for reading.\n", dbName);
		return;
	}
	fgets(lineBuffer, 1024, dbFile);					// Read magic # = "HRTF"
	if (strncmp(lineBuffer, "HRTF", 4) != 0) {
		logMsg("DB file %s wrong format.\n", dbName);
		fclose(dbFile);
		return;
	}													// read header line
	int numFound = sscanf(lineBuffer, "%s\t%s\t%d\t%d\t%d\t%d", 
				tok, nam, & nHRTFs, & hrirLen, & fftLen, & nBlocks);
	if (numFound != 6) {
		logMsg("Line format: %s\n", lineBuffer);
		fclose(dbFile);
		return;
	}
	mWindowSize = 2 * fftLen;							// set receiver's members from line
	mHRTFLength = fftLen;
	mHRIRLength = hrirLen;
	if (nBlocks != numBlocks()) {
		logMsg("Line format: %s\n", lineBuffer);
		fclose(dbFile);
		return;
	}
	fgets(lineBuffer, CSL_LINE_LEN, dbFile);			// read the first geometry line
	while(strlen(lineBuffer) > 1) {						// loop through file to an empty line
														// Read 2 ints from a line
		if(sscanf(lineBuffer, "%d\t%d", & azim, & elev) != 2) {
			logMsg("Line format: %s\n", lineBuffer);
			fclose(dbFile);
			return;
		}												// create a point
		CPoint * cpt = new CPoint(kPolar, 1.0f, azim / CSL_DEGS_PER_RAD, elev / CSL_DEGS_PER_RAD);
//		cpt->dumpPol();									// verbose list of points
		hrtfPoints.push_back(cpt);						// add pt to list
		fgets(lineBuffer, CSL_LINE_LEN, dbFile);		// read next line
	}													// end of loop; empty line read
//	logMsg("Found %d positions (%d)", hrtfPoints.size(), nHRTFs);

	if (hrtfPoints.size() != nHRTFs) {
		logMsg("Found %d positions ( ! = %d)", hrtfPoints.size(), nHRTFs);
		fclose(dbFile);
		return;
	}
	
	int hdrSize = (int) ftell(dbFile);					// start of data section
	for (unsigned i = 0; i < nHRTFs; i++) {				// loop over 180 HRTFs
		HRTF * hrtf = new HRTF();						// Create an empty HRTF
		hrtf->mPosition.set('s', 1.0f,					// set its pos
							hrtfPoints[i]->theta(), 
							hrtfPoints[i]->ele());
		for (unsigned b = 0; b < nBlocks; b++) {		// loop over 16 blocks
														// read 2 complex buffers from file
			size_t nRead = fread(hrtf->mHrtfL[b], sizeof(SampleComplex), mHRTFLength, dbFile);
			if (nRead != mHRTFLength) {
				logMsg(kLogError, "HRTF load read error %d\n", nRead);
				perror("Error reading HRTF  ");
				throw IOError("Error reading HRTF");
			}
			nRead = fread(hrtf->mHrtfR[b], sizeof(SampleComplex), mHRTFLength, dbFile);
			if (nRead != mHRTFLength) {
				logMsg(kLogError, "HRTF load read error %d\n", nRead);
				perror("Error reading HRTF  ");
				throw IOError("Error reading HRTF");
			}
		}
		mHRTFVector.push_back(hrtf);					// Add the HRTF to the vector.
	}
	int dataSize = (int) ftell(dbFile) - hdrSize;		// end of file
	fgets(lineBuffer, 1024, dbFile);					// Read "HRTF" trailer from the file
	if (strncmp(lineBuffer, "HRTF", 4) != 0)
		logMsg("Error: Trailer format: %s\n", lineBuffer);
	fclose(dbFile);
//	printf("\nLoad HRTF DB \"%s\"\n\t%d HRTFs of %d blocks of %d-len complex arrays;\n\thdr = %d data = %5.2f\n",
//			dbName, nHRTFs, nBlocks, fftLen, hdrSize, ((float) dataSize / 1000000.0f));
}

// copy constructor

//HRTFDatabase::HRTFDatabase(HRTFVector hrtfs) {
//	mWindowSize = 2 * HRTF_BLOCK_SIZE;
//	mHRTFLength = mWindowSize / 2 + 1;
//	mHRIRLength = HRIR_SIZE;
//	for (HRTFVector::iterator it = hrtfs.begin(); it != hrtfs.end(); it++)
//		mHRTFVector.push_back(*it);
//}

// Prints the number of HRTFs in the database, and then calls dump() an each of them, printing their position.
//		:: HRTF: 188 files = 24.7 MB (w: 1024 i: 8192 f: 511 b: 16)

void HRTFDatabase::dump(bool verbose) {
	logMsg("HRTF: %d files = %4.1f MB (w: %d i: %d f: %d b: %d)", 
				numHRTFs(), ((float) size() / 1000000.0f),
				windowSize(), hrirLength(), hrtfLength(), numBlocks());
	if (verbose)
		for (unsigned i = 0; i < numHRTFs(); i++)
			mHRTFVector[i]->dump();
}

// Answer the HRTFDatabase's storage size in Bytes

unsigned HRTFDatabase::size() {
	unsigned siz = 0;
	for (unsigned i = 0; i < this->numHRTFs(); i++)
		siz += mHRTFVector[i]->size();
	return siz;
}

// the main HRTFDatabase accessor by index

HRTF * HRTFDatabase::hrtfAt(unsigned index) {
	unsigned numHRTFs = this->numHRTFs();
	if ( ! numHRTFs) {					// empty HRTF
		logMsg(kLogError, "HRTFDatabase::hrtfAt - empty data");
		return 0;
	}
	return mHRTFVector[index];
}

// answer the index of the HRTF closest to the given source position by linear search

unsigned HRTFDatabase::hrtfAt(CPoint sourcePos) {
	double tmpVal = 0;					
	double lowVal = 10000;				// > than the largest reasonable distance
	unsigned localHRTF = 0;  
	unsigned numHRTFs = this->numHRTFs();
	if ( ! numHRTFs) {					// empty HRTF
		logMsg(kLogError, "HRTFDatabase::hrtfAt - empty data");
		return 0;
	}
	CPoint sPt = sourcePos;		
	sPt.normalize();					// Make it a unit vector, so its at the same distance as the hrtfs
	for (unsigned j = 0; j < numHRTFs - 1; j++) {
		tmpVal = sPt.distance2(mHRTFVector[j]->mPosition); // Get the distance from one point to the other.
		if (tmpVal < lowVal) {
			lowVal = tmpVal;
			localHRTF = j;
		}
	}
	return localHRTF;
}

// Total number of HRTFs loaded into the database.

unsigned HRTFDatabase::numHRTFs() { 
	return mHRTFVector.size(); 
}

// The size of the analysis window (in samples).

unsigned HRTFDatabase::windowSize() { 
	return mWindowSize; 
}

// The length (in samples) of the Transfer Function buffer.

unsigned HRTFDatabase::hrtfLength() { 
	return mHRTFLength; 
}

// The length (in samples) of the impulse responses loaded.

unsigned HRTFDatabase::hrirLength() { 
	return mHRIRLength; 
}

// The length (in blocks) of the impulse responses loaded.

unsigned HRTFDatabase::numBlocks() { 
	return mHRIRLength / (windowSize() / 2); 
}

// dump the DB as a single binary file
//	format is:
//		a nul-term string with the label and geometry list,
//		label is name #-hrtfs #-blocks blk-size
//			e.g., HRTF 1047 188 16 513
//		name list is in the same format as the files.txt list,
//		followed by the blocks of complex, blocked HRTFs
//		16 blocks/set, each has 513 complex values for L and R
//			= 8 * 2 * 513 * 16 = 131 kB / HRTF * 188 = 24.6 MB / DB

void HRTFDatabase::storeToDB(const char *filename, const char *sname) throw (CException) {
	unsigned numBlocks = hrirLength() / (windowSize() / 2);
	unsigned num2Store = numHRTFs();			// store all by default
	
	FILE * store = fopen(filename, "w");
	if ( ! store) {
		printf("Error saving HRTF DB - cannot open file\n");
		return;
	}
	fprintf(store, "HRTF %s\t%d\t%d\t%d\t%d\n",			// write store header
			sname, num2Store, hrirLength(), hrtfLength(), numBlocks);
														// write store HRTF position list
	for (unsigned j = 0; j < num2Store; j++) {
//	for (unsigned j = 0; j < numHRTFs() /* num2Store */; j++) {
//		if (mHRTFVector[j]->mPosition.ele() == 0)
			fprintf(store, "%g\t%g\n", 
				mHRTFVector[j]->mPosition.theta() * CSL_DEGS_PER_RAD, 
				mHRTFVector[j]->mPosition.ele() * CSL_DEGS_PER_RAD);
	}
	fprintf(store, "\n");								// write a blank line to end list
	int hdrSize = (int) ftell(store);
														// write HRTF data
	for (unsigned i = 0; i < num2Store; i++) {			// loop over 180 HRTFs
//	for (unsigned i = 0; i < numHRTFs() /* num2Store */; i++) {			// loop over 180 HRTFs
//		if (mHRTFVector[i]->mPosition.ele() == 0) {		// store only elev = 0
			HRTF * hrtf = mHRTFVector[i];
			for (unsigned b = 0; b < numBlocks; b++) {		// loop over 16 blocks
															// write 2 complex buffers
				fwrite(hrtf->mHrtfL[b], sizeof(SampleComplex), mHRTFLength, store);
				fwrite(hrtf->mHrtfR[b], sizeof(SampleComplex), mHRTFLength, store);
			}
			fflush(store);
//		}
	}
	int dataSize = (int) ftell(store) - hdrSize;
	fprintf(store, "HRTF\n");							// write footer
	fclose(store);
	printf("Saved HRTF DB %s with %d HRTFs %d blocks of %d-len arrays;\n\thdr = %d B, data = %5.2f MB\n\n",
			sname, num2Store, numBlocks, hrtfLength(), hdrSize, ((float) dataSize / 1000000.0f));
}

// bulk translation of IRCAM HRTFs to CSL-format block data files

void HRTFDatabase::convertDB(const char *listname) throw (CException) {
	char lineBuffer[CSL_NAME_LEN];
	char filename[CSL_NAME_LEN];
	char tok[8];
	FILE * listFile;									// text file with the list of HRTF files in the directory

	listFile = fopen(listname, "r");					// Open the file that contains the list.
	if ( ! listFile) {
		logMsg("Cannot open list file %s for reading.\n", listname);
		return;
	}
	while ( ! feof(listFile)) {							// Go thru the file list
		fgets(lineBuffer, 1024, listFile);				// Read one full line
		int len = strlen(lineBuffer) - 1;
		if (lineBuffer[len] == '\n')					// remove EOL from line
			lineBuffer[len] = '\0'; 
		logMsg("Read HRTF folder %s", lineBuffer);
		sprintf(filename, "%sIRCAM_HRTF/%s/",
					CGestalt::dataFolder().c_str(), lineBuffer);

		HRTFDatabase::Reload(filename);					// Load the new data
		logMsg("Loaded %d HRTF files = %4.1f MB", 
				HRTFDatabase::Database()->numHRTFs(),
				((float) HRTFDatabase::Database()->size() / 1000000.0f));

		char * cpos = strstr(filename, "IRC_");
		if (cpos) {
			int index = atoi(cpos + 4);
			sprintf(filename, "%sIRCAM_HRTF/HRTF_%d.dat",
					CGestalt::dataFolder().c_str(), index);	// CSL data folder location
			sprintf(tok, "%d", index);
			HRTFDatabase::Database()->storeToDB(filename, tok);
		}
	}
}

// Stale
//
//#ifdef IPHONE
//	NSBundle * bundle = [NSBundle mainBundle];
//	CFURLRef fileURL = (CFURLRef)[[NSURL fileURLWithPath:
//			[bundle pathForResource: @"IRC_1047_R" ofType: @""]] 
//		retain];
//	CFStringRef namestring = CFURLGetString (fileURL);
//	char theName[CSL_NAME_LEN];
//	Boolean aok = CFStringGetCString (namestring, theName, CSL_NAME_LEN, kCFStringEncodingMacRoman);
//	logMsg("Loading HRTF files from %s", theName);
//	NSArray * pathsArray =[[NSBundle mainBundle] 
//		pathsForResourcesOfType: @"wav" 
//		inDirectory: [NSString stringWithCString: HRTF_RESOURCE]];
//	int	pathCount = [pathsArray count];
//	int i;
//	NSString*	presetPath = NULL;
//	for(i = 0; i < pathCount; i++) {
//		presetPath = [pathsArray objectAtIndex:i];
//		aok = CFStringGetCString (presetPath, theName, CSL_NAME_LEN, kCFStringEncodingMacRoman);
//		mHRTFVector.push_back(new HRTF(theName, fft));	//// Add the HRTF to the vector.
//	}
//#else
