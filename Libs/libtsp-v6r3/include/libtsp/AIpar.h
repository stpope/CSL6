/*------------ Telecommunications & Signal Processing Lab --------------
                         McGill University

Routine:
  AIpar.h

Description:
  Declarations for AIFF and AIFF-C sound file headers

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.21 $  $Date: 2003/05/13 01:48:10 $

----------------------------------------------------------------------*/

#ifndef AIpar_h_
#define AIpar_h_

#include <libtsp/UTtypes.h>		/* typedef for uint2_t, etc */

/* AIFF or AIFF-C file layout
  FORM chunk preamble  8
    File ID            4  
    FVER chunk         8 + 4  (AIFF-C files only)
    COMM chunk         8 + 18 + compressionType (4 bytes)
                         + compressionName (min length 2)
    SSND chunk         8 + 8 + data
*/

/* File magic in file byte order */
#define FM_IFF		"FORM"
#  define FM_AIFF	"AIFF"
#  define FM_AIFF_C	"AIFC"
#define CKID_COMM	"COMM"
#define CKID_FVER	"FVER"
#define CKID_SSND	"SSND"
#define CKID_NAME	"NAME"
#define CKID_AUTHOR	"AUTH"
#define CKID_COPYRIGHT	"(c) "
#define CKID_ANNOTATION	"ANNO"

#define CT_NONE		"NONE"
#define CT_ALAW		"alaw"
#define CT_ULAW		"ulaw"
#define CT_FLOAT32	"fl32"
#define CT_FLOAT64	"fl64"

#define CT_ADP4		"ADP4"	/* DVI ADPCM */
#define CT_DWVW		"DWVW"	/* TX16W Typhoon */
#define CT_FL32		"FL32"	/* Csound Float 32 */
#define CT_FL64		"FL64"
#define CT_IMA4		"ima4"	/* IMA */
#define CT_MAC3		"MAC3"	/* Apple MACE 3-to-1 */
#define CT_MAC6		"MAC6"	/* Appple MACE 6-to-1 */
#define CT_QDMC		"QDMC"	/* QDesign Music */
#define CT_QCLP		"Qclp"	/* Qualcomm PureVoice */
#define CT_RT24		"rt24"	/* Voxware RT24 */
#define CT_RT29		"rt29"	/* Voxware RT29 */

#define CT_X_ALAW	"ALAW"	/* "SGI CCITT G.711 A-law" */
#define CT_X_ULAW	"ULAW"	/* "SGI CCITT G.711 u-law" */
#define CT_G722		"G722"	/* "SGI CCITT G.722" */
#define CT_G726		"G726"	/* "CCITT G.726" */
#define CT_G728		"G728"	/* "CCITT G.728" */
#define CT_GSM		"GSM "	/* "GSM 06-10" */

/* Compression name strings (max CNAME_MAX characters) */
#define CNAME_MAX	32
#define CN_NONE		"not compressed"
#define CN_ULAW		"\265law 2:1"
#define CN_ALAW		"Alaw 2:1"
#define CN_FLOAT32	"IEEE 32-bit float"
#define CN_FLOAT64	"IEEE 64-bit float"
#define CN_X_ALAW	"ITU-T G.711 A-law"
#define CN_X_ULAW	"ITU-T G.711 mu-law"

#define AIFCVersion1	0xA2805140	/* AIFF-C Version 1 identifier */

struct AI_CkPreamb {
  char ckID[4];
  uint4_t ckSize;
};

struct AI_CkFVER {
  char ckID[4];
  uint4_t ckSize;
  uint4_t timestamp;
};

struct AI_CkCOMM {
  char ckID[4];
  uint4_t ckSize;
  uint2_t numChannels;
  uint4_t numSampleFrames;
  uint2_t sampleSize;
  unsigned char sampleRate[10];
  char compressionType[4];
  char compressionName[CNAME_MAX+1];	/* Null terminated string */
};

struct AI_CkANNO {
  char ckID[4];
  uint4_t ckSize;
};

struct AI_CkSSND {
  char ckID[4];
  uint4_t ckSize;
  uint4_t offset;
  uint4_t blockSize;
};

struct AI_CkFORM {
  char ckID[4];
  uint4_t ckSize;
  char AIFFID[4];
  struct AI_CkCOMM CkCOMM;
  struct AI_CkFVER CkFVER;
  struct AI_CkANNO CkANNO;
  struct AI_CkSSND CkSSND;
};

#endif	/* AIpar_h_ */
