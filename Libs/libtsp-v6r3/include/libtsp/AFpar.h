/*------------ Telecommunications & Signal Processing Lab --------------
                         McGill University

Routine:
  AFpar.h

Description:
  Declarations for the TSP audio file routines.

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.80 $  $Date: 2003/11/04 10:35:51 $

----------------------------------------------------------------------*/

#ifndef AFpar_h_
#define AFpar_h_

#include <float.h>		/* DBL_MAX */
#include <stdio.h>		/* typedef for FILE */
#include <libtsp/UTpar.h>	/* Byte swap codes DS_SWAP, etc. */

#ifndef	AFILE_t_
#  define	AFILE_t_
typedef struct AF_filepar AFILE;	/* Audio file parameters */
#endif

/* Loudspeaker locations */
/* The loudspeaker locations associated with the channels are stored in
   a byte array.  The values in the byte array start at 1.  A null byte marks
   the end of the list.
*/

#define AF_MAXN_SPKR	30	/* Maximum size for speaker position array */
#define AF_NC_SPKR	3	/* Maximum length of a speaker name */

#define AF_SPKR_FL	1	/* Front left */
#define AF_SPKR_FR	2	/* Front right */
#define AF_SPKR_FC	3	/* Front centre */
#define AF_SPKR_LF	4	/* Low frequency */
#define AF_SPKR_BL	5	/* Back left */
#define AF_SPKR_BR	6	/* Back right */
#define AF_SPKR_FLC	7	/* Front left of centre */
#define AF_SPKR_FRC	8	/* Front right of centre */
#define AF_SPKR_BC	9	/* Back centre */
#define AF_SPKR_SL	10	/* Side left */
#define AF_SPKR_SR	11	/* Side left */
#define AF_SPKR_TC	12	/* Top centre */
#define AF_SPKR_TFL	13	/* Top front left */
#define AF_SPKR_TFC	14	/* Top front centre */
#define AF_SPKR_TFR	15	/* Top front right */
#define AF_SPKR_TBL	16	/* Top back left */
#define AF_SPKR_TBC	17	/* Top back centre */
#define AF_SPKR_TBR	18	/* Top back right */
#define AF_SPKR_X	19	/* No position */

#define AF_N_SPKR_NAMES	19
#define AF_X_SPKR	(AF_N_SPKR_NAMES + 1)

#ifdef AF_SPKR_NAMES
static const char *AF_Spkr_Names[] =
{
  "FL",  "FR",  "FC",  "LF", 
  "BL",  "BR",  "FLC", "FRC",
  "BC",  "SL",  "SR",  "TC",
  "TFL", "TFC", "TFR", "TBL",
  "TBC", "TBR", "-",   NULL	/* Terminating null string */
};
#endif

/* Information records */
struct AF_infoX {
  char *Info;		/* Pointer to string */
  int N;		/* Number of characters (includes nulls) */
  int Nmax;		/* Maximum number of characters (size of string) */
};
/* Structures for audio file information */
struct AF_dformat {
  int Format;		/* Data format, FD_INT16, etc. */
  int Swapb;		/* Swap code, DS_EB, DS_NATIVE, etc. */
  int NbS;		/* Number of bits per sample */
  double ScaleF;	/* Scaling factor applied to file data */
  			/* (default value AF_SF_DEFAULT) */
};
struct AF_ndata {
  long int Ldata;	/* Data space in bytes */
  long int Nsamp;	/* Number of samples (all channels) */
  long int Nchan;	/* Number of channels */
  unsigned char SpkrConfig[AF_MAXN_SPKR+1];	/* Speaker configuration */
};
struct AF_read {
  double Sfreq;
  struct AF_dformat DFormat;
  struct AF_ndata NData;
  struct AF_infoX InfoX;
};
struct AF_write {
  double Sfreq;
  struct AF_dformat DFormat;
  long int Nchan;
  long int Nframe;	/* No. Frames: normally AF_NFRAME_UNDEF */
  int Ftype;		/* FTW_WAVE, etc. */
  unsigned char SpkrConfig[AF_MAXN_SPKR+1];	/* Speaker configuration */
  struct AF_infoX InfoX;
};

#define AF_READ_DEFAULT(x) \
	static const struct AF_read x = \
		{0.0, \
		 {FD_UNDEF, DS_NATIVE, 0, AF_SF_DEFAULT}, \
		 {AF_LDATA_UNDEF, AF_NSAMP_UNDEF, 1L, {AF_X_SPKR, 0}}, \
		 {NULL, 0, 0}}

#define FM_AFSP		"AFsp"
#define AF_MAXINFO	10240
struct AF_info {
  char *Info;		/* Pointer to string */
  int N;		/* Number of characters (includes nulls) */
};

/* Audio file parameter structure */
struct AF_filepar {
  FILE *fp;		/* File pointer */
  int Op;		/* Operation (read, write) */
  int Error;		/* Error flag (0 for no error) */
  long int Novld;	/* Number of points clipped */
  double Sfreq;		/* Sampling rate */
  int Ftype;		/* File type */
  int Format;		/* Data format, FD_INT16, etc. */
  int Swapb;		/* Swap code (DS_NATIVE or DS_SWAP) */
  int NbS;		/* Number of bits per sample */
  double ScaleF;	/* Scale factor applied to file data */
  long int Nchan;	/* Number of channels */
  unsigned char *SpkrConfig;	/* Speaker configuration mask */
  long int Start;	/* Start byte */
  long int Isamp;	/* Sample offset */
  long int Nsamp;	/* Number of samples */
  struct AF_info InfoS;	/* Information structure */
};

/* Error codes for the Error field in the audio parameter structure */
#define AF_NOERR	0
#define AF_UEOF		-1	/* Unexpected end-of-file on read */
#define AF_IOERR	1	/* Read or write error */
#define AF_DECERR	2	/* Data decoding error on read */

/* File operation types */
enum {
  FO_NONE	= 0,	/* closed */
  FO_RO 	= 1,	/* read */
  FO_WO 	= 2	/* write */
};

/* Data format types - must be sequential */
enum {
  FD_UNDEF	= 0,	/* undefined file data format */
  FD_MULAW8	= 1,	/* mu-law 8-bit data */
  FD_ALAW8	= 2,	/* A-law 8-bit data */
  FD_UINT8	= 3,	/* offset binary integer 8-bit data */
  FD_INT8	= 4,	/* two's complement integer 8-bit data */
  FD_INT16	= 5,	/* two's complement integer 16-bit data */
  FD_INT24	= 6,	/* two's complement integer 24-bit data */
  FD_INT32	= 7,	/* two's complement integer 32-bit data */
  FD_FLOAT32	= 8,	/* 32-bit float data */
  FD_FLOAT64	= 9,	/* 64-bit float data */
  FD_TEXT	= 10	/* text data */
};
#define NFD		(FD_TEXT+1)

/* Data format sizes (bytes) */
#define FDL_MULAW8	1
#define FDL_ALAW8	1
#define FDL_UINT8	1
#define FDL_INT8	1
#define FDL_INT16	2
#define FDL_INT24	3
#define FDL_INT32	4
#define	FDL_FLOAT32	4
#define FDL_FLOAT64	8
#define FDL_TEXT	0	/* Variable size */

#ifdef AF_DATA_LENGTHS
static const int AF_DL[NFD] = {
  0,
  FDL_MULAW8,
  FDL_ALAW8,
  FDL_UINT8,
  FDL_INT8,
  FDL_INT16,
  FDL_INT24,
  FDL_INT32,
  FDL_FLOAT32,
  FDL_FLOAT64,
  FDL_TEXT
};
#endif

/* Default scale factors */
/* For the fixed-point formats, data is returned in the range
   [-1, +1). An exception is 8-bit data which gives values
   [-1/2,1/2).
*/
#define AF_SF_UNDEF	(1.)
#define AF_SF_MULAW8	(1./32768.)
#define AF_SF_ALAW8	(1./32768.)
#define AF_SF_UINT8	(128./32768.)
#define AF_SF_INT8	(128./32768.)
#define AF_SF_INT16	(1./32768.)
#define AF_SF_INT24	(1./(256.*32768.))
#define AF_SF_INT32	(1./(65536.*32768.))
#define AF_SF_FLOAT32	(1.)
#define AF_SF_FLOAT64	(1.)
#define AF_SF_TEXT	(1.)
#define AF_SF_DEFAULT	(-(DBL_MAX))

/* Scale factors */
/* These are the scale factors for reading, the scale factors for
   writing are the inverses */
static const double AF_SF[NFD] = {
  AF_SF_UNDEF,
  AF_SF_MULAW8,
  AF_SF_ALAW8,
  AF_SF_UINT8,
  AF_SF_INT8,
  AF_SF_INT16,
  AF_SF_INT24,
  AF_SF_INT32,
  AF_SF_FLOAT32,
  AF_SF_FLOAT64,
  AF_SF_TEXT
};

#ifdef AF_DATA_TYPE_NAMES
static const char *AF_DTN[NFD] = {
  "undefined",
  "8-bit mu-law",
  "8-bit A-law",
  "offset-binary 8-bit integer",
  "8-bit integer",
  "16-bit integer",
  "24-bit integer",
  "32-bit integer",
  "32-bit float",
  "64-bit float",
  "text data"
};
#endif

#ifdef AF_DATA_TYPE_NAMES_X
static const char *AF_DTX[NFD] = {
  NULL,
  "%d-bit mu-law",
  "%d-bit A-law",
  "offset-binary %d-bit integer",
  "%d-bit integer",
  "%d-bit integer",
  "%d-bit integer",
  "%d-bit integer",
  "%d-bit float",
  "%d-bit float",
  "text data"
};
static const char *AF_DTXX[NFD] = {
  NULL,
  "%d/%d-bit mu-law",
  "%d/%d-bit A-law",
  "offset-binary %d/%d-bit integer",
  "%d/%d-bit integer",
  "%d/%d-bit integer",
  "%d/%d-bit integer",
  "%d/%d-bit integer",
  "%d/%d-bit float",
  "%d/%d-bit float",
  "text data"
};
#endif

/* === Input audio files === */
/* Internal codes for input audio file types - must be sequential for the
   standard file types
*/
enum {
  FT_UNKNOWN	= 0,	/* unknown audio file format */
  FT_NH		= 1,	/* headerless (non-standard or no header) audio file */
  FT_AU		= 2,	/* AU audio file */
  FT_WAVE	= 3,	/* WAVE file */
  FT_AIFF_C	= 4,	/* AIFF-C sound file */
  FT_SPHERE	= 5,	/* NIST SPHERE audio file */
  FT_ESPS	= 6,	/* ESPS sampled data feature file */
  FT_SF		= 7,	/* IRCAM soundfile */
  FT_SPPACK	= 8, 	/* SPPACK file */
  FT_INRS	= 9,	/* INRS-Telecom audio file */
  FT_AIFF	= 10,	/* AIFF sound file */
  FT_SPW	= 11,	/* Comdisco SPW Signal file */
  FT_NSP	= 12,	/* CSL NSP file */
  FT_TXAUD	= 13	/* Text audio file */
};
#define NFT	(FT_TXAUD+1)
#define FT_ERROR	(-1)	/* error, file type cannot be determined */
#define FT_AUTO		255	/* automatic mode (check file header) */
#define FT_UNSUP	256	/* unsupported audio file type */

#ifdef AF_FILE_TYPE_NAMES
static const char *AF_FTN[NFT] = {
  NULL,
  "Audio file",		/* Headerless or non-standard audio file */
  "AU audio file",
  "WAVE file",
  "AIFF-C sound file",
  "NIST SPHERE audio file",
  "ESPS audio file",
  "IRCAM soundfile",
  "SPPACK file",
  "INRS-Telecom audio file",
  "AIFF sound file",
  "Comdisco SPW Signal file",
  "CSL NSP file",
  "Text audio file"
};
#endif

/* Structure for headerless input audio file parameters */
struct AF_NHpar {
  int Format;		/* Data format */
  long int Start;	/* Offset in bytes to the start of data */
  double Sfreq;		/* Sampling frequency */
  int Swapb;		/* Byte swap flag */
  long int Nchan;	/* Number of channels */
  double ScaleF;	/* Scale factor */
};

/* Defines for data length checks */
/* Special values for Ldata, Nsamp and Nframe */
#define AF_LDATA_UNDEF	-1L
#define AF_NSAMP_UNDEF	-1L
#define AF_NFRAME_UNDEF	-1L

/* Header data length fixup flags */
#define AF_NOFIX		0
#define AF_FIX_NSAMP_HIGH	1
#define AF_FIX_NSAMP_LOW	2
#define AF_FIX_LDATA_HIGH	4

/* === Output audio files === */
/* Codes for output audio file types */
#define FTW_UNDEF	0	/* Undefined */
#define FTW_AU		1	/* AU audio file */
#define FTW_WAVE	2	/* WAVE file */
#define FTW_AIFF_C	3	/* AIFF-C sound file */
#define FTW_NH_X       	4	/* Headerless */
#define FTW_AIFF	5	/* AIFF sound file */

#define FTW_SUBTYPE_MOD	16

#define FTW_NH_EB	(FTW_NH_X + DS_EB * FTW_SUBTYPE_MOD)
#define FTW_NH_EL	(FTW_NH_X + DS_EL * FTW_SUBTYPE_MOD)
#define FTW_NH_NATIVE	(FTW_NH_X + DS_NATIVE * FTW_SUBTYPE_MOD)
#define FTW_NH_SWAP	(FTW_NH_X + DS_SWAP * FTW_SUBTYPE_MOD)

#define FTW_WAVE_NOEX	(FTW_WAVE + 1 * FTW_SUBTYPE_MOD)

#ifdef AF_OUTPUT_FILE_TYPE_NAMES
static const char *AF_FTWN[NFT] = {
  NULL,
  "AU audio file",
  "WAVE file",
  "AIFF-C sound file",
  "Headerless audio file",
  "AIFF sound file"
};
#endif

/* Default sampling frequency if the sampling frequency is zero or invalid */
#define AF_SFREQ_DEFAULT	8000.0

/* Structure for AF routine options */
struct AF_opt {
  int ErrorHalt;		/* Error handling:
				   0 - continue on error,
				   1 - halt on error */

/* Input files */
  int NsampND;			/* Number of samples requirement
				   0 - Nsamp must be known
				   1 - Nsamp=AF_NSAMP_UNDEF allowed */
  int RAccess;			/* Random access requirement:
                                   0 - input file can be sequential access or
				       random access
                                   1 - input file must be random access */
  int FtypeI;			/* Input file type, FT_AUTO, FT_AU, etc. */
  struct AF_NHpar NHpar;	/* Headerless input audio file parameters */

/* Output files */
  long int Nframe;		/* Number of frames
				   AF_NFRAME_UNDEF means this value is
				   undefined */
  int NbS;			/* Number of bits per sample
				   0 means use full precision of the data */
  unsigned char *SpkrConfig;	/* Speaker configuration */
  char *Uinfo;			/* User supplied information string */
};

/* Values for AFresetOptions category */
#define AF_OPT_GENERAL	1
#define AF_OPT_INPUT	2
#define AF_OPT_OUTPUT	4

#ifdef __cplusplus
extern "C" {
#endif

struct AF_opt *
AFoptions (void);
struct AF_opt *
AFresetOptions (unsigned int Cat);

#ifdef __cplusplus
}
#endif

#endif	/* AFpar_h_ */
