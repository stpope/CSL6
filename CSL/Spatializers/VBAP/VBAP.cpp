///
/// VBAP.h -- Vector Base Amplitude Panning class (v2 - Nov 2006)
///	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
/// Doug McCoy, 2004.
/// 

#include "VBAP.h"

#define MAX_NUM_VBAP_TRIPLETS 512
#define MIN_VOLUME_TO_LENGTH_RATIO 0.01

using namespace csl;

typedef struct {
	int firstNode;
	int secondNode;
} Connection;


double vecAngle(CPoint v1, CPoint v2) {
	double inner = v1*v2/(v1.len() * v2.len());
	if (inner > 1.0)
		inner = 1.0;
	else if (inner < -1.0)
		inner = -1.0;
	return fabs(acos(inner) );
}

CPoint vectorMultiply(CPoint &v, CSLMatrix &m) {
	unsigned dimensions = m.RowNo();
	double temp[3] = {0};
	for (unsigned i = 0; i < dimensions; i++)  // for each node, insert speaker coordinates in matrix
		for (unsigned j = 0; j < dimensions; j++)  // for each node, insert speaker coordinates in matrix
			temp[i] += v(j) * m(j,i);

	CPoint result(temp[0], temp[1], temp[2]);
	return result;
}

SpeakerSet::SpeakerSet(unsigned size) {
	nodes = new unsigned[size];

}

SpeakerSet::~SpeakerSet() {
	delete [] nodes;

}

void SpeakerSet::dump() {
	cout << nodes[0] << "\t" << nodes[1] << "\t" << nodes[2] << endl;
}

///////////////////////////////////////////////////
/*     SPEAKER TRIPLET LAYOUT IMPLEMENTATION     */
///////////////////////////////////////////////////

SpeakerSetLayout::SpeakerSetLayout(SpeakerLayout *aLayout, VBAPMode mode) {
	mSpeakerLayout = aLayout; 
	
//	mSpeakerLayout->attachObserver(this); // Tell the speaker layout to inform you of any changes
	mMode = mode;
	// Be smart about setting the mode.
	if (mMode == kAuto) {
		if (mSpeakerLayout->isPeriphonic())
			mMode = kPeriphonic;
		else
			mMode = kPantophonic;
	}
	
//	if (mSpeakerLayout->isPeriphonic()) {
//		if (mMode == 0)
//			mMode = kPeriphonic;
//	} else
//		mMode = kPantophonic;
	
	mTriplets = (SpeakerSet **) malloc(MAX_NUM_VBAP_TRIPLETS * sizeof(SpeakerSet*));
	mNumTriplets = 0;
	if (mMode == kPantophonic)
		findSpeakerPairs();
	else
		findSpeakerTriplets();

	dump();
}

SpeakerSetLayout::~SpeakerSetLayout() {

//	mSpeakerLayout->detachObserver(this); // Remove yourself from the list of observers.

	unsigned i = 0;
	for (i = 0; i < mNumTriplets; i++)
		delete mTriplets[i];

	free(mTriplets);
}

void SpeakerSetLayout::addTriple(SpeakerSet *lst) {
	if (mNumTriplets == MAX_NUM_VBAP_TRIPLETS) {
		printf("Max number of VBAP triplets exceeded. Triplet not added\n");
		return;
	}
	mTriplets[mNumTriplets++] = lst;
	return;
}

void SpeakerSetLayout::removeTriple(SpeakerSet *s) {

	// locate the item
	for (unsigned i = 0; ((i < mNumTriplets)); i++) {
		if (mTriplets[i] ==  s) {
			delete mTriplets[i];
			--mNumTriplets;
			
			// shift the upper items down 1 in the list
			for (; i < mNumTriplets; i++) {
				mTriplets[i] = mTriplets[i+1];
			}
			break;
		}
	}
	
	return;
}

void SpeakerSetLayout::dump() {
	cout << "Number of SpeakerSets: " << mNumTriplets << endl;
	for (unsigned i = 0; i < mNumTriplets; i++) {
		cout << "Triple #" << i << ": ";
		mTriplets[i]->dump();
	}
}

void SpeakerSetLayout::invertTripleMatrix(SpeakerSet *lst) {
	unsigned dimensions = mMode;
	CSLMatrix L(dimensions,dimensions);
	for (unsigned i = 0; i < dimensions; i++) { // for each node, insert speaker coordinates in matrix
		CPoint temp = mSpeakerLayout->speakerAtIndex(lst->nodes[i])->position();
		for (unsigned j = 0; j < dimensions; j++)  // for each node, insert speaker coordinates in matrix
			L(i,j) = temp(j);
	}
	lst->invL = new CSLMatrix (L.Inv());		// invert matrix and populate SpeakerSet member
	return;
}

bool SpeakerSetLayout::evaluateCrossing(CPoint &li, CPoint &lj, CPoint &ln, CPoint &lm) {
	CPoint cross((li^lj) ^ (ln^lm));
	float ang_ci = vecAngle(li, cross);
	float ang_cj = vecAngle(lj, cross);
	float ang_ij = vecAngle(li, lj);
	float ang_cn = vecAngle(ln, cross);
	float ang_cm = vecAngle(lm, cross);
	float ang_nm = vecAngle(ln, ln);
	
	if (((ang_ci + ang_cj) == ang_ij) && ((ang_cn + ang_cm) == ang_nm) )
		return true;
	else {
		ang_ci = vecAngle(li, -cross);
		ang_cj = vecAngle(lj, -cross);
		ang_cn = vecAngle(ln, -cross);
		ang_cm = vecAngle(lm, -cross);
		
		return (((ang_ci + ang_cj) == ang_ij) && ((ang_cn + ang_cm) == ang_nm) );
	}
}

// If 2D VBAP, then find the pair of speakers.
void SpeakerSetLayout::findSpeakerPairs() throw(CException) {

	unsigned numSpeakers = mSpeakerLayout->numSpeakers();
	unsigned j, index;
	unsigned speakerMapping[numSpeakers]; // To map unordered speakers into an ordered set.
	float speakerAngles[numSpeakers];
	float indexAngle;
	
	logMsg("VBAP::finding loudspeaker pairs");
	
	// Build a map to the speakers, that points to speaker indexes.
	for (unsigned i = 0; i < numSpeakers; i++) {
		speakerAngles[i] = mSpeakerLayout->speakerAtIndex(i)->azimuth(); 
		speakerMapping[i] = i;
	}

	// Sort speakers into the map
	for (unsigned i = 1; i < numSpeakers; i++) {
		// Only sort speakers that have elevation == 0. Ignore all other.
		if (mSpeakerLayout->speakerAtIndex(i)->elevation() == 0) {
	
			indexAngle = speakerAngles[i];
			index = speakerMapping[i];
			j = i;
			
			while ((j > 0) && (speakerAngles[j-1] > indexAngle)) {
				speakerAngles[j] = speakerAngles[j-1];
				speakerMapping[j] = speakerMapping[j-1];
				j = j - 1;
				
			}
			speakerAngles[j] = indexAngle; 
			speakerMapping[j] = index;
		}
	}

//#ifdef CSL_DEBUG
	for (unsigned i = 0; i < numSpeakers; i++)
		mSpeakerLayout->speakerAtIndex(speakerMapping[i])->dump();
//#endif

	for (unsigned i = 1; i < numSpeakers; i++){
		SpeakerSet *sPair = new SpeakerSet(2);
		sPair->nodes[0] = speakerMapping[i-1];
		sPair->nodes[1] = speakerMapping[i];
		invertTripleMatrix(sPair);
		addTriple(sPair);
	}
	// Add the last speaker
	SpeakerSet *sPair = new SpeakerSet(2);
	sPair->nodes[0] = speakerMapping[numSpeakers-1];
	sPair->nodes[1] = speakerMapping[0];
	invertTripleMatrix(sPair);
	addTriple(sPair);


}

void SpeakerSetLayout::findSpeakerTriplets() throw(CException) {
	unsigned numSpeakers = mSpeakerLayout->numSpeakers();
	// form all possible triples
	for (unsigned i = 0; i < numSpeakers; i++){
		for (unsigned j = i+1; j < numSpeakers; j++){
			for (unsigned k = j+1; k < numSpeakers; k++){
				SpeakerSet *triplet = new SpeakerSet;
				triplet->nodes[0] = i;
				triplet->nodes[1] = j;
				triplet->nodes[2] = k;
				addTriple(triplet);
			}
		}
	}

	// remove too narrow triples
	for (unsigned i = 0; i < mNumTriplets; i++) {
		SpeakerSet *trip = mTriplets[i];
		CPoint p0, p1, p2;
		p0 = mSpeakerLayout->speakerAtIndex(trip->nodes[0])->position();
		p1 = mSpeakerLayout->speakerAtIndex(trip->nodes[1])->position();
		p2 = mSpeakerLayout->speakerAtIndex(trip->nodes[2])->position();

		CPoint xprod = p0 ^ p1;	// ^ is cross product operator
		float volume = fabsf(xprod * p2);
		float length = fabsf(vecAngle(p0, p1) ) + fabsf(vecAngle(p0, p2) ) + fabsf(vecAngle(p1, p2) );	
		float ratio;
		if (length > 0.00001)
			ratio =  volume / length;
		else
			ratio = 0.0;

		if (ratio < MIN_VOLUME_TO_LENGTH_RATIO) {
			removeTriple(trip);
			i--;
		}
	}

	// resolve sides that cross
	for (unsigned i = 0; i < mNumTriplets; i++) {
		// Holders to the triplets being tested, so they can be removed if needed.
		SpeakerSet *currentOutterTriple, *currentInnerTriple; 
		Connection connections[6];
		connections[0].firstNode = mTriplets[i]->nodes[0];
		connections[0].secondNode = mTriplets[i]->nodes[1];
		
		connections[1].firstNode = mTriplets[i]->nodes[1];
		connections[1].secondNode = mTriplets[i]->nodes[2];
		
		connections[2].firstNode = mTriplets[i]->nodes[0];
		connections[2].secondNode = mTriplets[i]->nodes[2];

		currentOutterTriple = mTriplets[i];
		for (unsigned n = i+1; n < mNumTriplets; n++) {
			connections[3].firstNode = mTriplets[n]->nodes[0];
			connections[3].secondNode = mTriplets[n]->nodes[1];
			
			connections[4].firstNode = mTriplets[n]->nodes[1];
			connections[4].secondNode = mTriplets[n]->nodes[2];
			
			connections[5].firstNode = mTriplets[n]->nodes[0];
			connections[5].secondNode = mTriplets[n]->nodes[2];

			currentInnerTriple = mTriplets[n];			
			bool removed = false;
			bool startOver = false;
			for (unsigned j = 0; j < 3; j++) {
				CPoint v1 = mSpeakerLayout->speakerAtIndex(connections[j].firstNode)->position();
				CPoint v2 = mSpeakerLayout->speakerAtIndex(connections[j].secondNode)->position();
				for (unsigned k = 3; k < 6; k++) {
					CPoint v3 = mSpeakerLayout->speakerAtIndex(connections[k].firstNode)->position();
					CPoint v4 = mSpeakerLayout->speakerAtIndex(connections[k].secondNode)->position();
					
					if (evaluateCrossing(v1, v2, v3, v4 ) ) {			// if connections intersect
						float length1 = v1.distance(v2);
						float length2 = v3.distance(v4);
						if (length1 > length2)	{					// remove triple with longer connection
							removeTriple(currentOutterTriple );
							i--;									// ith triple deleted so don't increment i on next loop
							startOver = true;
						}
						else {
							removeTriple(currentInnerTriple );
							n--;		// nth triple deleted so don't increment n on next loop
						}
						removed = true;
					}
					if (removed) break;	// break k loop
				}
				if (removed) break;	// break j loop  i.e. start a new n loop 
			}
			if (startOver) break;	// break n loop  i.e. removed ith triple
		}	
	}
	
	// remove triangles that contain other Speakers
	for (unsigned ii = 0; ii < mNumTriplets; ii++) {
		invertTripleMatrix(mTriplets[ii]);
//		cout << "outside invert: " << *(mTriplets[ii]->invL) << endl;

		for (unsigned jj = 0; jj < numSpeakers; jj++) {
			// check to see if the current speaker is one of the nodes of the triple
			if ((jj == mTriplets[ii]->nodes[0]) || (jj == mTriplets[ii]->nodes[1]) || (jj == mTriplets[ii]->nodes[2]) )
				continue;
			CPoint p = mSpeakerLayout->speakerAtIndex(jj)->position();
			CPoint v = vectorMultiply(p, (*(mTriplets[ii]->invL)) );
			bool x_inside, y_inside, z_inside;
			x_inside = (v.x >= -1e-4);	// inside if positive or negative near zero, -1e-4 is a magic number
			y_inside = (v.y >= -1e-4);
			z_inside = (v.z >= -1e-4);
			
			if ((x_inside) && (y_inside) && ((mMode == 2) || (z_inside) ) ) 	{ 
				removeTriple(mTriplets[ii] );		// if LSpeaker is within triangle, delete triple
				ii--;
				break;
			}
		}
	}
	if (mNumTriplets == 0 )
		throw LogicError("No SpeakerSets found. Check mode setting or speaker layout.");

	return;
}	// end find_triples()

//////////////////////////////////////
/*       VBAP IMPLEMENTATION       */
/////////////////////////////////////

/// The two parameters are optional. \n The mode defines wether to consider elevation (full 3D) positions, or horizontal only (kPantophonic).
/// If not specified, the mode is chosen automatically. If the SpeakerLayout contains speakers at different elevations, then it sets the mode to 3D.
/// Otherwise, it is set to 2D. \n The layout parameter allows to specify a different layout than the default. The recomended use is to create a layout
/// and set it as default. Then any Panner can make use of it. \n This parameter overrides the use of the default layout, using the layout passed here.
VBAP::VBAP(VBAPMode mode, SpeakerLayout *layout) : SpatialPanner(layout), mMode(mode) {
	
	setCopyPolicy(kIgnore); // This is needed so the default kCopy doesn't overide the multiple channel panning done here.

	// Be smart about setting the mode.
	if (mMode == kAuto) {
		if (mSpeakerLayout->isPeriphonic())
			mMode = kPeriphonic;
		else
			mMode = kPantophonic;
	}
	setNumChannels(mSpeakerLayout->numSpeakers()); // Set the number of (output) channels to the number of loudspeakers in the layout
	mSpeakerSetLayout = new SpeakerSetLayout(mSpeakerLayout, mMode);
	
}

VBAP::~VBAP() {
	delete mSpeakerSetLayout;
}

void VBAP::speakerLayoutChanged() {

	setNumChannels(mSpeakerLayout->numSpeakers()); // Set the number of (output) channels to the number of loudspeakers in the layout
	delete mSpeakerSetLayout; // Delete the old triplets
	mSpeakerSetLayout = new SpeakerSetLayout(mSpeakerLayout); // create a new triplet layout

}

void *VBAP::cache() {
	return (void *)new VBAPSourceCache(); // Returns a pointer to an alocated cache data for its own use.
}

void VBAP::nextBuffer(Buffer &outputBuffer) throw(CException) {
	this->nextBuffer(outputBuffer, 0);
}

void VBAP::nextBuffer(Buffer &outputBuffer, unsigned outBufNum) throw(CException) {
	unsigned numFrames = outputBuffer.mNumFrames;
	unsigned numTriplets = mSpeakerSetLayout->mNumTriplets;
	outputBuffer.zeroBuffers();		// clear output buffer

#ifdef CSL_DEBUG
	logMsg("VBAP::nextBuffer");
#endif
	
	for (unsigned i = 0; i < mSources.size(); i++) {	// i loops through inputs
		SpatialSource *source = (SpatialSource*) mSources[i]; // Get the sound source
		VBAPSourceCache *tcache = (VBAPSourceCache *) mCache[i]; // Get any cached data belonging to the sound source.
		
		if (source->isActive()) {				// if source is active
			if (source->positionChanged()) {		// if the position of the source has changed, recompute gains.
				CPoint currentGains;
				CPoint sourcePosition = *source->position(); // Get the source position.
				sourcePosition.normalize();			// to keep calculations on sphere of Speakers
				
				unsigned currentTripletIndex = tcache->tripletIndex; // Cached source placement, so it starts searching from there.
				
				// Search thru the triplets array in search of a match for the source position.
				for (unsigned count = 0; count < numTriplets; ++count) {
					currentGains = vectorMultiply(sourcePosition, (*(mSpeakerSetLayout->mTriplets[currentTripletIndex]->invL)) );
					if ((currentGains.x >= 0) && (currentGains.y >= 0) && ((mMode == 2) || (currentGains.z >= 0)) )	// need more sophisticated condition eventually
						break;

					++currentTripletIndex;				
					if (currentTripletIndex >= numTriplets)
						currentTripletIndex = 0;
				}
				
				tcache->tripletIndex = currentTripletIndex; // Store the new index

				double soundPowerConstant = 1;
				double gainScale = sqrt(soundPowerConstant) / currentGains.len();
				currentGains *= gainScale;
				
				// Store the gains for possible future use.
				tcache->gains[0] = currentGains.x;
				tcache->gains[1] = currentGains.y;
				tcache->gains[2] = currentGains.z;
			} // end of if (positionChanged)
		
			SpeakerSet *currentTriplet = mSpeakerSetLayout->mTriplets[tcache->tripletIndex];			

			source->nextBuffer(mTempBuffer); // Ask the source to fill the buffer with the data to be processed

			for (unsigned j = 0; j < mMode; j++) {
				SampleBuffer out1 = outputBuffer.buffer(currentTriplet->nodes[j]); // get a pointer to the buffer of the speaker that needds some gain applied.
				SampleBuffer opp1 = mTempBuffer.buffer(0); // this buffer has the data of the input source. Only mono for now. TO BE FIXED AND EXTENDED TO ... 
				
				for (unsigned k = 0; k < numFrames; k++) {	// k loops through sample buffers
					*out1++ += tcache->gains[j] * (*opp1++);
				}
			}

#ifdef CSL_DEBUG
	logMsg("Speaker Gains: %.2f, \t %.2f, \t %.2f", tcache->gains[0], tcache->gains[1], tcache->gains[2]);
#endif

		}	// end if active
	}	// end for loop
	return;
}


//////////////////////////////////////
/*  STEREO PANNER IMPLEMENTATION   */
/////////////////////////////////////

StereoPanner::StereoPanner() : VBAP(kPantophonic, new StereoSpeakerLayout) { }

StereoPanner::~StereoPanner() {
	delete mSpeakerLayout; // This time I allocated the memory, so I have to get rid of it.
}

////////////////////////////////////////
/*  SURRORUND PANNER IMPLEMENTATION   */
///////////////////////////////////////

SurroundPanner::SurroundPanner(unsigned numSpeakers, bool useSubwoofer) : VBAP(kPantophonic, new StereoSpeakerLayout) { }

SurroundPanner::~SurroundPanner() {
	delete mSpeakerLayout; // This time I allocated the memory, so I have to get rid of it.
}

