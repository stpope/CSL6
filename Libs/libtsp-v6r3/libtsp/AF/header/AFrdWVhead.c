/*-------------- Telecommunications & Signal Processing Lab ---------------

Routine:
  AFILE *AFrdWVhead (FILE *fp)

Purpose:
  Get file format information from a WAVE file

Description:
  This routine reads the header for a WAVE file.  The header information
  is used to set the file data format information in the audio file pointer
  structure.

  WAVE file:
   Offset Length Type    Contents
      0     4    char   "RIFF" file identifier
      4     4    int    Chunk length
      8     4    char   "WAVE" file identifier
    ...   ...    ...    ...
      F     4    char   "fmt " chunk identifier
     +4     4    int    Chunk length
     +8     2    int      Audio data type
    +10     2    int      Number of interleaved channels
    +12     4    int      Sample rate
    +16     4    int      Average bytes/sec
    +20     2    int      Block align
    +22     2    int      Data word length (bits)
    +24     2    int      Extra data count (for non-PCM or extensible format)
    +26     2    int        Valid bits per sample
    +28     4    int        Channel mask
    +32    16    int        Sub-format type
    ...    ...   ...
      C     4    char   "fact" chunk identifier (for non-PCM)
     +4     4    int    Chunk length
     +8     4    int      Number of samples (per channel)
    ...    ...   ...    ...
      D     4    char   "data" chunk identifier
     +4     4    int    Chunk length
     +8    ...   ...      Audio data

Parameters:
  <-  AFILE *AFrdWVhead
      Audio file pointer for the audio file
   -> FILE *fp
      File pointer for the file

Author / revision:
  P. Kabal  Copyright (C) 2004
  $Revision: 1.90 $  $Date: 2004/03/29 01:49:41 $

-------------------------------------------------------------------------*/

#include <assert.h>
#include <setjmp.h>
#include <string.h>

#include <libtsp.h>
#include <libtsp/AFdataio.h>
#include <libtsp/AFheader.h>
#include <libtsp/AFmsg.h>
#define AF_SPKR_MASKS
#include <libtsp/AFpar.h>
#define WV_CHANNEL_MAP
#define WAVEFORMATEX_SUBTYPE
#include <libtsp/WVpar.h>
#include <libtsp/nucleus.h>

#define ICEILV(n,m)	(((n) + ((m) - 1)) / (m))	/* int n,m >= 0 */
#define NELEM(array)	((int) ((sizeof array) / (sizeof array[0])))
#define RNDUPV(n,m)	((m) * ICEILV (n, m))		/* Round up */

#define SAME_CSTR(str,ref) 	(memcmp (str, ref, sizeof (str)) == 0)

#define ALIGN		2	/* Chunks padded out to a multiple of ALIGN */

#define WV_FMT_MINSIZE	16
#define WV_LHMIN	(8 + 4 + 8 + WV_FMT_MINSIZE + 8)

/* setjmp / longjmp environment */
extern jmp_buf AFR_JMPENV;

AF_READ_DEFAULT(AFr_default);	/* Define the AF_read defaults */

static int
AF_decFMT (const struct WV_CKfmt *CKfmt, struct AF_read *AFr);
static int
AF_rdDISP_text (FILE *fp, int Size, struct AF_infoX *InfoX);
static int
AF_rdFMT (FILE *fp, struct WV_CKfmt *CKfmt);
static int
AF_rdFACT (FILE *fp, struct WV_CKfact *CKfact);
static int
AF_rdLIST_INFO (FILE *fp, int Size, struct AF_infoX *InfoX);
static int
AF_rdRIFF_WAVE (FILE *fp, struct WV_CKRIFF *CKRIFF);
static void
AF_UnsFormat (int FormatTag);
static void
AF_decChannelConfig (uint4_t ChannelMask, unsigned char *SpkrConfig);


AFILE *
AFrdWVhead (FILE *fp)

{
  AFILE *AFp;
  int AtData, FoundAFsp;
  long int offs, LRIFF, Fact_Nsamp, Dstart, EoD;
  char Info[AF_MAXINFO];
  struct WV_CKRIFF CKRIFF;
  struct WV_CKpreamb CkHead;
  struct AF_read AFr;

/* Set the long jump environment; on error return a NULL */
  if (setjmp (AFR_JMPENV))
    return NULL;	/* Return from a header read error */

/* Defaults and inital values */
  AFr = AFr_default;
  AFr.InfoX.Info = Info;
  AFr.InfoX.Nmax = AF_MAXINFO;

  Fact_Nsamp = AF_NSAMP_UNDEF;

/* Check the file magic for a RIFF/WAVE file */
  if (AF_rdRIFF_WAVE (fp, &CKRIFF))
    return NULL;
  offs = 12L;	/* Positioned after RIFF/WAVE preamble */
  LRIFF = CKRIFF.ckSize + 8;

  Dstart = 0L;
  EoD = 0L;
  AtData = 0;
  FoundAFsp = 0;
  while (offs < LRIFF-8) {	/* Leave 8 bytes for the chunk preamble */

    /* Read the chunk preamble */
    offs += RHEAD_S (fp, CkHead.ckID);

    /* fmt chunk */
    if (SAME_CSTR (CkHead.ckID, "fmt ")) {
      offs += AF_rdFMT (fp, &CKRIFF.CKfmt);
      if (AF_decFMT (&CKRIFF.CKfmt, &AFr))
	return NULL;
    }

    /* fact chunk */
    else if (SAME_CSTR (CkHead.ckID, "fact")) {
      offs += AF_rdFACT (fp, &CKRIFF.CKfact);
      Fact_Nsamp = CKRIFF.CKfact.dwSampleLength;
    }

    /* data chunk */
    else if (SAME_CSTR (CkHead.ckID, "data")) {
      offs += RHEAD_V (fp, CKRIFF.CKdata.ckSize, DS_EL);
      AFr.NData.Ldata = CKRIFF.CKdata.ckSize;
      Dstart = offs;
      EoD = RNDUPV (Dstart + AFr.NData.Ldata, ALIGN);
      if (EoD >= LRIFF || ! FLseekable (fp)) {
	AtData = 1;
	break;
      }
      else {
	AtData = 0;
	offs += RSKIP (fp, EoD - Dstart);
      }
    }

    /* Text chunks */
    else if (SAME_CSTR (CkHead.ckID, "afsp")) {
      offs += RHEAD_V (fp, CkHead.ckSize, DS_EL);
      offs += AFrdTextAFsp (fp, (int) CkHead.ckSize, "AFsp: ", &AFr.InfoX,
			    ALIGN);
      FoundAFsp = 1;
    }
    else if (SAME_CSTR (CkHead.ckID, "DISP") && ! FoundAFsp) {
      offs += RHEAD_V (fp, CkHead.ckSize, DS_EL);
      offs += AF_rdDISP_text (fp, (int) CkHead.ckSize, &AFr.InfoX);
    }
    else if (SAME_CSTR (CkHead.ckID, "LIST") && ! FoundAFsp) {
      offs += RHEAD_V (fp, CkHead.ckSize, DS_EL);
      offs += AF_rdLIST_INFO (fp, (int) CkHead.ckSize, &AFr.InfoX);
    }
    /* Miscellaneous chunks */
    else {
      offs += RHEAD_V (fp, CkHead.ckSize, DS_EL);
      offs += RSKIP (fp, RNDUPV (CkHead.ckSize, ALIGN));
    }
  }
  /* Error Checks */
  /* Check that we found a fmt and a data chunk */
  if (AFr.DFormat.Format == FD_UNDEF || AFr.NData.Ldata == AF_LDATA_UNDEF) {
    UTwarn ("AFrdWVhead - %s", AFM_WV_BadHead);
    return NULL;
  }
  if ((! AtData && offs != LRIFF) || (AtData && EoD != LRIFF))
    UTwarn ("AFrdWVhead - %s", AFM_WV_BadSize);

  /* Use the number of samples from the fact chunk only for non-PCM
     or WAVE-EX files.  Some PCM files use the fact chunk, but it does
     NOT specify the number of samples */
  if (CKRIFF.CKfmt.wFormatTag != WAVE_FORMAT_PCM
      && Fact_Nsamp != AF_NSAMP_UNDEF)
    AFr.NData.Nsamp = AFr.NData.Nchan * Fact_Nsamp;

  /* Position at the start of data */
  if (! AtData) {
    if (AFseek (fp, Dstart, NULL))
      return NULL;
  }

/* Set the parameters for file access */
  AFp = AFsetRead (fp, FT_WAVE, &AFr, AF_FIX_NSAMP_HIGH + AF_FIX_LDATA_HIGH);

  return AFp;
}

/* Check the RIFF/WAVE file preamble */


static int
AF_rdRIFF_WAVE (FILE *fp, struct WV_CKRIFF *CKRIFF)

{
  long int Lfile, LRIFF;

  RHEAD_S (fp, CKRIFF->ckID);
  if (! SAME_CSTR (CKRIFF->ckID, "RIFF")) {
    UTwarn ("AFrdWVhead - %s", AFM_WV_BadId);
    return 1;
  }

  RHEAD_V (fp, CKRIFF->ckSize, DS_EL);
  LRIFF = CKRIFF->ckSize + 8;
  if (LRIFF < WV_LHMIN) {
    UTwarn ("AFrdWVhead - %s", AFM_WV_BadRIFF);
    return 1;
  }

  if (FLseekable (fp)) {
    Lfile = FLfileSize (fp);
    if (LRIFF > Lfile) {
      CKRIFF->ckSize = Lfile - 8;
      UTwarn ("AFrdWVhead - %s", AFM_WV_FixRIFF);
    }
  }
 
  RHEAD_S (fp, CKRIFF->WAVEID);
  if (! SAME_CSTR (CKRIFF->WAVEID, "WAVE")) {
    UTwarn ("AFrdWVhead - %s", AFM_WV_BadId);
    return 1;
  }

  return 0;
}

/* Read the fmt chunk, starting at ckSize */


static int
AF_rdFMT (FILE *fp, struct WV_CKfmt *CKfmt)

{
  int offs, NB;

  offs = RHEAD_V (fp, CKfmt->ckSize, DS_EL);
  if (CKfmt->ckSize < WV_FMT_MINSIZE) {
    UTwarn ("AFrdWVhead - %s", AFM_WV_BadfmtSize);
    longjmp (AFR_JMPENV, 1);
  }

  offs += RHEAD_V (fp, CKfmt->wFormatTag, DS_EL);
  offs += RHEAD_V (fp, CKfmt->nChannels, DS_EL);
  offs += RHEAD_V (fp, CKfmt->nSamplesPerSec, DS_EL);
  offs += RHEAD_V (fp, CKfmt->nAvgBytesPerSec, DS_EL);
  offs += RHEAD_V (fp, CKfmt->nBlockAlign, DS_EL);
  offs += RHEAD_V (fp, CKfmt->wBitsPerSample, DS_EL);

  NB = ((int) CKfmt->ckSize + 4) - offs;
  if (NB >= 24) {
    offs += RHEAD_V (fp, CKfmt->cbSize, DS_EL);
    if (CKfmt->cbSize >= 22) {
      offs += RHEAD_V (fp, CKfmt->wValidBitsPerSample, DS_EL);
      offs += RHEAD_V (fp, CKfmt->dwChannelMask, DS_EL);
      offs += RHEAD_V (fp, CKfmt->SubFormat.wFormatTag, DS_EL);
      offs += RHEAD_S (fp, CKfmt->SubFormat.guidx);
    }
    else
      CKfmt->cbSize = 0;
  }

  /* Skip over any extra data at the end of the fmt chunk */
  offs += RSKIP (fp, RNDUPV (CKfmt->ckSize + 4, ALIGN) - offs);

  return offs;
}

/* Decode the data format */


static int
AF_decFMT (const struct WV_CKfmt *CKfmt, struct AF_read *AFr)

{
  uint2_t FormatTag;
  int NBytesS;

/*
  Ordinary WAVE (not EXTENSIBLE format) files
  - The data format is over-specified
    - The number of bits per sample is wBitsPerSample.
    - The number of bytes per sample is NByteS = nBlockAlign/nChannels.
  - A reasonable approach is to get the container size from the block size
    and the data size from wBitsPerSample.
  - The CoolEdit float format specifies float data (flagged as 24 bits per
    sample, in a 4-byte container)
  - In calculating the container size, it is expected that nBlockAlign be a
    multiple of the number of channels.  Note, however, that some compressed
    schemes set nBlockAlign to 1.
  WAVE EXTENSIBLE files
  - wBitsPerSample now explicitly is the container size in bits and hence
    should agree with 8 * NByteS, and nBlockAlign must be a multiple of
    nChannels.
*/
  if (CKfmt->nChannels == 0)
    NBytesS = 0;
  else
    NBytesS = (int) (CKfmt->nBlockAlign / CKfmt->nChannels);

  if (CKfmt->wFormatTag == WAVE_FORMAT_EXTENSIBLE) {
    if (CKfmt->cbSize < 22 ||
	! IS_VALID_WAVEFORMATEX_GUID (CKfmt->SubFormat)) {
      UTwarn ("AFrdWVhead - %s", AFM_WV_BadfmtEx);
      return 1;
    }
    /* Extensible file: container size must match wBitsPerSample */
    AFr->DFormat.NbS = 8 * NBytesS;
    if (CKfmt->wBitsPerSample != 8 * NBytesS) {
      UTwarn ("AFrdWVhead - %s: %d <-> %d", AFM_WV_BadSS,
	      (int) CKfmt->wBitsPerSample, 8 * NBytesS);
    }
    AFr->DFormat.NbS = (int) CKfmt->wValidBitsPerSample;
    FormatTag = CKfmt->SubFormat.wFormatTag;
    AF_decChannelConfig (CKfmt->dwChannelMask, AFr->NData.SpkrConfig);
  }
  else {
    AFr->DFormat.NbS = (int) CKfmt->wBitsPerSample;
    FormatTag = CKfmt->wFormatTag;
  }

  switch (FormatTag) {
  case WAVE_FORMAT_PCM:
  
    if (NBytesS == FDL_INT16)
      AFr->DFormat.Format = FD_INT16;
    /* Special case: IEEE float from CoolEdit (samples are flagged as 24-bit
       integer, but are 32-bit IEEE float) */
    else if (CKfmt->wFormatTag == WAVE_FORMAT_PCM &&
	     AFr->DFormat.NbS == 24 && NBytesS == 4) {
      AFr->DFormat.Format = FD_FLOAT32;
      AFr->DFormat.NbS = 8 * FDL_FLOAT32;
      AFr->DFormat.ScaleF = AF_SF_INT24;	/* Same scaling as INT24 */
      if (! UTcheckIEEE ())
	UTwarn ("AFrdAIhead - %s", AFM_NoIEEE);
    }
    else if (NBytesS == FDL_INT24)
      AFr->DFormat.Format = FD_INT24;
    else if (NBytesS == FDL_INT32)
      AFr->DFormat.Format = FD_INT32;
    else if (NBytesS == FDL_UINT8)
      AFr->DFormat.Format = FD_UINT8;
    else {
      UTwarn ("AFrdWVhead - %s: \"%d\"", AFM_WV_UnsDSize, 8 * NBytesS);
      return 1;
    }
    break;

  case WAVE_FORMAT_MULAW:
    if (NBytesS != 1) {
      UTwarn ("AFrdWVhead - %s: \"%d\"", AFM_WV_BadMulaw, 8 * NBytesS);
      return 1;
    }      
    if (AFr->DFormat.NbS != 8) {
      UTwarn ("AFrdWVhead - %s: \"%d\"", AFM_WV_BadMulaw, AFr->DFormat.NbS);
      AFr->DFormat.NbS = 8;
    }
    AFr->DFormat.Format = FD_MULAW8;
    break;

  case WAVE_FORMAT_ALAW:
    if (NBytesS != 1) {
      UTwarn ("AFrdWVhead - %s: \"%d\"", AFM_WV_BadAlaw, 8 * NBytesS);
      return 1;
    }      
    if (AFr->DFormat.NbS != 8) {
      UTwarn ("AFrdWVhead - %s: \"%d\"", AFM_WV_BadAlaw, AFr->DFormat.NbS);
      AFr->DFormat.NbS = 8;
    }
    AFr->DFormat.Format = FD_ALAW8;
    break;

  case WAVE_FORMAT_IEEE_FLOAT:
    if (NBytesS == FDL_FLOAT32)
      AFr->DFormat.Format = FD_FLOAT32;
    else if (NBytesS == FDL_FLOAT64)
      AFr->DFormat.Format = FD_FLOAT64;
    else
      UTwarn ("AFrdWVhead - %s: \"%d\"", AFM_WV_BadFloat, 8 * NBytesS);
    if (! UTcheckIEEE ())
      UTwarn ("AFrdWVhead - %s", AFM_NoIEEE);
    break;

  /* Unsupported data formats */
  default:
    AF_UnsFormat ((int) CKfmt->wFormatTag);
    return 1;
  }

  AFr->NData.Nchan = (long int) CKfmt->nChannels;

  /* Error checks */
  if (AFr->NData.Nchan <= 0) {
    UTwarn ("AFrdWVhead - %s : %d", AFM_WV_BadNChan, CKfmt->nChannels);
    return 1;
  }
  if ((CKfmt->nBlockAlign % CKfmt->nChannels) != 0) {
    UTwarn ("AFrdWVhead - %s: %d", AFM_WV_BadBlock, CKfmt->nBlockAlign);
    return 1;
  }
  if (AFr->DFormat.NbS > 8 * NBytesS) {
    UTwarn ("AFrdWVhead - %s: %d", AFM_WV_BadNbS, AFr->DFormat.NbS);
    AFr->DFormat.NbS = 8 * NBytesS;
  }

  /* Consistency check */
  if (CKfmt->nAvgBytesPerSec != CKfmt->nChannels * CKfmt->nSamplesPerSec *
      NBytesS)
    UTwarn ("AFrdWVhead - %s", AFM_WV_BadBytesSec);

  AFr->DFormat.Swapb = DS_EL;
  AFr->Sfreq = (double) CKfmt->nSamplesPerSec;

  return 0;
}

/* Read the fact chunk */

static int
AF_rdFACT (FILE *fp, struct WV_CKfact *CKfact)

{
  int offs;

  offs = RHEAD_V (fp, CKfact->ckSize, DS_EL);
  if (CKfact->ckSize < 4) {
    UTwarn ("AFrdWVhead - %s", AFM_WV_BadFACT);
    longjmp (AFR_JMPENV, 1);
  }
  offs += RHEAD_V (fp, CKfact->dwSampleLength, DS_EL);
  offs += RSKIP (fp, RNDUPV (CKfact->ckSize + 4, ALIGN) - offs);

  return offs;
}

/* Unsupported data format */

static void
AF_UnsFormat (int FormatTag)

{
  switch (FormatTag) {
  case WAVE_FORMAT_ADPCM:
    UTwarn ("AFrdWVhead - %s: \"%s\"", AFM_WV_UnsData, "MS ADPCM");
    break;
  case WAVE_FORMAT_MPEGLAYER3:
    UTwarn ("AFrdWVhead - %s: \"%s\"", AFM_WV_UnsData, "MPEG-1 Layer 3");
    break;
  case WAVE_FORMAT_MSG723:
    UTwarn ("AFrdWVhead - %s: \"%s\"", AFM_WV_UnsData, "G.723.1 coding");
    break;
  default:
    UTwarn ("AFrdWVhead - %s: \"%d\"", AFM_WV_UnsData, FormatTag);
    break;
  }
}

/* Read a DISP chunk */


static int
AF_rdDISP_text (FILE *fp, int Size, struct AF_infoX *InfoX)

{
  int offs;
  uint4_t DISP_ID;

  offs = RHEAD_V (fp, DISP_ID, DS_EL);
  if (DISP_ID == CF_TEXT)
    offs += AFrdTextAFsp (fp, Size - offs, "display_name: ", InfoX, ALIGN);
  else
    offs += RSKIP (fp, RNDUPV (Size, ALIGN) - offs);

  return offs;
}     

/* Read the LIST-INFO records from the header */


struct WV_LI{
  char ckid[5];
  char *key;
};
static const struct WV_LI IID[] = {
  {"IARL", "archival_location: "},
  {"IART", "artist: "},
  {"ICMS", "commissioned: "},
  {"ICMT", "comments: "},
  {"ICOP", "copyright: "},
  {"ICRD", "creation_date: "},
  {"IENG", "engineer: "},
  {"IGNR", "genre: "},
  {"IKEY", "keywords: "},
  {"IMED", "medium: "},
  {"INAM", "name: "},
  {"IPRD", "product: "},
  {"ISBJ", "subject: "},
  {"ISFT", "software: "},
  {"ISRC", "source: "},
  {"ISRF", "source_form: "},
  {"ITCH", "technician: "}
};

#define NIID	NELEM (IID)


static int
AF_rdLIST_INFO (FILE *fp, int Size, struct AF_infoX *InfoX)

{
  int i, offs;
  char ID[4], key[7];
  struct WV_CKpreamb CkHead;

  offs = RHEAD_S (fp, ID);
  if (SAME_CSTR (ID, "INFO")) {

    while (offs < Size) {
      offs += RHEAD_S (fp, CkHead.ckID);
      offs += RHEAD_V (fp, CkHead.ckSize, DS_EL);

      /* Look for standard INFO ID values */
      for (i = 0; i < (int) NIID; ++i) {
	if (SAME_CSTR (CkHead.ckID, IID[i].ckid)) {
	  offs += AFrdTextAFsp (fp, (int) CkHead.ckSize, IID[i].key,
				InfoX, ALIGN);
	  break;
	}
      }

      /* No match, use the INFO ID field as the information record keyword */
      if (i == NIID) {
	strncpy (key, CkHead.ckID, 4);
	strcpy (&key[4], ": ");
	offs += AFrdTextAFsp (fp, (int) CkHead.ckSize, key, InfoX, ALIGN);
      }
    }
  }
  offs += RSKIP (fp, RNDUPV (Size, ALIGN) - offs);

  return offs;
}

/* Decode channel/speaker information */


static void
AF_decChannelConfig (uint4_t ChannelMask, unsigned char *SpkrConfig)

{
  int i, k;

  assert (AF_N_SPKR_NAMES - 1 == WV_N_CHANNEL_MAP);

  k = 0;
  if (ChannelMask & WV_SPEAKER_ALL)
    ;
  else if (ChannelMask & ~WV_SPEAKER_KNOWN)
    UTwarn ("AFrdWVhead - %s", AFM_WV_UnkChannel);
  else if (ChannelMask != 0) {
    for (i = 0, k = 0; i < WV_N_CHANNEL_MAP; ++i) {
      if (ChannelMask & WV_ChannelMap[i]) {
	SpkrConfig[k] = i+1;
	++k;
      }
    }
  }

  SpkrConfig[k] = '\0';
  return;
}
