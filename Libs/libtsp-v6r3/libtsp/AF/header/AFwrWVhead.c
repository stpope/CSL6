/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  AFILE *AFwrWVhead (FILE *fp, struct AF_write *AF)

Purpose:
  Write header information to a WAVE file

Description:
  This routine opens and writes header information to a WAVE format audio
  file.

  WAVE file:
   Offset Length Type    Contents
      0     4    char   "RIFF" file identifier
      4     4    int    Chunk length
      8     4    char   "WAVE" file identifier
     12     4    char   "fmt " chunk identifier
     16     4    int    Chunk length
     20     2    int      Audio data type
     22     2    int      Number of interleaved channels
     24     4    int      Sample rate
     28     4    int      Average bytes/sec
     32     2    int      Block align
     34     2    int      Data word length (bits)
     36     2    int      Extra info size
      C     4    int    "fact" chunk identifier (only for non-PCM data)
    C+4     4    int    Chunk length
    C+8     4    int      Number of samples (per channel)
    ...   ...    ...    ...
      D     4    char   "data" chunk identifier
    D+4     4    int    Chunk length
    D+8    ...   ...      Audio data

Parameters:
  <-  AFILE *AFwrWVhead
      Audio file pointer for the audio file.  In case of error, the audio file
      pointer is set to NULL.
   -> FILE *fp
      File pointer for the audio file
  <-> struct AF_write *AFw
      Structure containing file parameters

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.69 $  $Date: 2003/05/13 01:57:49 $

-------------------------------------------------------------------------*/

#include <assert.h>
#include <setjmp.h>
#include <string.h>

#include <libtsp.h>
#include <libtsp/nucleus.h>
#include <libtsp/AFheader.h>
#include <libtsp/AFmsg.h>
#define AF_DATA_LENGTHS
#define AF_SPKR_MASKS
#include <libtsp/AFpar.h>
#define WV_CHANNEL_MAP
#define WAVEFORMATEX_SUBTYPE
#include <libtsp/WVpar.h>

#define ICEILV(n,m)	(((n) + ((m) - 1)) / (m))	/* int n,m >= 0 */
#define RNDUPV(n,m)	((m) * ICEILV (n, m))		/* Round up */

#define MCOPY(src,dest)		memcpy ((void *) (dest), \
					(const void *) (src), sizeof (dest))
#define WRPAD(fp,size,align) \
     AFwriteHead (fp, NULL, 1, (int) (RNDUPV(size, align) - (size)), \
		  DS_NATIVE);

#define ALIGN		2	/* Chunks padded out to a multiple of ALIGN */

/* setjmp / longjmp environment */
extern jmp_buf AFW_JMPENV;

static void
AF_setFMT (struct WV_CKfmt *CKfmt, const struct AF_write *AFw);
static void
AF_wrRIFF (FILE *fp, const struct WV_CKRIFF *CKRIFF);
static int
AF_wrFMT (FILE *fp, const struct WV_CKfmt *CKfmt);
static uint4_t
AF_encChannelConfig (const unsigned char *SpkrConfig);


AFILE *
AFwrWVhead (FILE *fp, struct AF_write *AFw)

{
  AFILE *AFp;
  int Lw;
  long int size, Ldata;
  struct WV_CKRIFF CKRIFF;

/* Set the long jump environment; on error return a NULL */
  if (setjmp (AFW_JMPENV))
    return NULL;	/* Return from a header write error */

/* Set up the encoding parameters */
  Lw = AF_DL[AFw->DFormat.Format];
  if (AFw->Nframe != AF_NFRAME_UNDEF)
    Ldata = AFw->Nframe * AFw->Nchan * Lw;
  else if (FLseekable (fp))
    Ldata = 0L;
  else {
    UTwarn ("AFwrWVhead - %s", AFM_WV_WRAccess);
    return NULL;
  }

  /* RIFF chunk */
  MCOPY ("RIFF", CKRIFF.ckID);
  /* defer filling in the chunk size */ 
  MCOPY ("WAVE", CKRIFF.WAVEID);

  /* fmt chunk */
  AF_setFMT (&CKRIFF.CKfmt, AFw);

  /* fact chunk */
  if (CKRIFF.CKfmt.wFormatTag != WAVE_FORMAT_PCM) {
    MCOPY ("fact", CKRIFF.CKfact.ckID);
    CKRIFF.CKfact.ckSize = (uint4_t) sizeof (CKRIFF.CKfact.dwSampleLength);
    CKRIFF.CKfact.dwSampleLength = (uint4_t) 0;
  }

  /* data chunk */
  MCOPY ("data", CKRIFF.CKdata.ckID);
  CKRIFF.CKdata.ckSize = (uint4_t) Ldata;

  /* Fill in the RIFF chunk size */
  size = 4 + 8 + RNDUPV(CKRIFF.CKfmt.ckSize, ALIGN) +
             8 + RNDUPV(CKRIFF.CKdata.ckSize, ALIGN);
  if (CKRIFF.CKfmt.wFormatTag != WAVE_FORMAT_PCM)
    size += 8 + RNDUPV(CKRIFF.CKfact.ckSize, ALIGN);
  CKRIFF.ckSize = (uint4_t) size;

/* Write out the header (error return via longjmp) */
  AFw->DFormat.Swapb = DS_EL;
  AF_wrRIFF (fp, &CKRIFF);

/* Set the parameters for file access */
  AFp = AFsetWrite (fp, FT_WAVE, AFw);

  return AFp;
}

/* Fill in the fmt chunk */


static void
AF_setFMT (struct WV_CKfmt *CKfmt, const struct AF_write *AFw)

{
  int Lw, Ext, Res, NbS;
  uint2_t FormatTag;

  Lw = AF_DL[AFw->DFormat.Format];
  Res = 8 * Lw;
  NbS = AFw->DFormat.NbS;

  /* Determine whether to use an extensible header */
  Ext = 0;
  if (AFw->Ftype == FTW_WAVE)
    Ext = (AFw->Nchan > 2 || NbS != Res ||
	   AFw->DFormat.Format == FD_INT24 ||
	   AFw->DFormat.Format == FD_INT32 ||
	   (AFw->SpkrConfig[0] != AF_X_SPKR && AFw->SpkrConfig[0] != '\0'));

  switch (AFw->DFormat.Format) {
  case FD_UINT8:
  case FD_INT16:
  case FD_INT24:
  case FD_INT32:
    FormatTag = WAVE_FORMAT_PCM;
    break;
  case FD_MULAW8:
    FormatTag = WAVE_FORMAT_MULAW;
    break;
  case FD_ALAW8:
    FormatTag = WAVE_FORMAT_ALAW;
    break;
  case FD_FLOAT32:
  case FD_FLOAT64:
    FormatTag = WAVE_FORMAT_IEEE_FLOAT;
    break;
  default:
    FormatTag = WAVE_FORMAT_UNKNOWN;
    assert (0);
  }

  /* Set the chunk size (minimum size for now) */
  MCOPY ("fmt ", CKfmt->ckID);
  CKfmt->ckSize = sizeof (CKfmt->wFormatTag) + sizeof (CKfmt->nChannels)
    + sizeof (CKfmt->nSamplesPerSec) + sizeof (CKfmt->nAvgBytesPerSec)
    + sizeof (CKfmt->nBlockAlign) + sizeof (CKfmt->wBitsPerSample);

  CKfmt->nChannels = (uint2_t) AFw->Nchan;
  CKfmt->nSamplesPerSec = (uint4_t) (AFw->Sfreq + 0.5);	/* Rounding */
  CKfmt->nAvgBytesPerSec = (uint4_t) (CKfmt->nSamplesPerSec * AFw->Nchan * Lw);
  CKfmt->nBlockAlign = (uint2_t) (Lw * AFw->Nchan);

  if (Ext) {
    CKfmt->wBitsPerSample = (uint2_t) Res;
    CKfmt->wFormatTag = WAVE_FORMAT_EXTENSIBLE;
    CKfmt->cbSize = 22;
    if (FormatTag == WAVE_FORMAT_PCM || FormatTag == WAVE_FORMAT_IEEE_FLOAT)
      CKfmt->wValidBitsPerSample = (uint2_t) NbS;
    else
      CKfmt->wValidBitsPerSample = (uint2_t) Res;
    CKfmt->dwChannelMask = AF_encChannelConfig (AFw->SpkrConfig);
    CKfmt->SubFormat.wFormatTag = FormatTag;
    MCOPY (WAVEFORMATEX_TEMPLATE.guidx, CKfmt->SubFormat.guidx);
  }
  else {
    /* Use wBitsPerSample to specify the "valid bits per sample" */
    if (RNDUPV(NbS, 8) != Res) {
      UTwarn (AFMF_WV_InvNbS, "AFwrWVhead -", NbS, Res);
      NbS = Res;
    }
    CKfmt->wBitsPerSample = (uint2_t) NbS;
    CKfmt->wFormatTag = FormatTag;
    CKfmt->cbSize = 0;
  }

  /* Update the chunk size with the size of the chunk extension */
  if (CKfmt->wFormatTag != WAVE_FORMAT_PCM)
    CKfmt->ckSize += (sizeof (CKfmt->cbSize) + CKfmt->cbSize);

  return;
}

/* Encode channel/speaker information */


static uint4_t
AF_encChannelConfig (const unsigned char *SpkrConfig)

{
  int i, n, Nspkr;
  uint4_t ChannelMask;

  if (SpkrConfig[0] == AF_X_SPKR || SpkrConfig[0] == '\0')
    Nspkr = 0;
  else
    Nspkr = strlen ((const char *) SpkrConfig);

  ChannelMask = 0;
  for (i = 0; i < Nspkr; ++i) {
    n = SpkrConfig[i];
    if (i > 0 && n < SpkrConfig[i-1]) {
      UTwarn ("AFwrWVhead - %s", AFM_WV_BadSpkr);
      ChannelMask = 0;
      break;
    }
    if (n != AF_SPKR_X)
      ChannelMask = ChannelMask | WV_ChannelMap[n-1];
  }

  return ChannelMask;
}

/* Write out the header */


static void
AF_wrRIFF (FILE *fp, const struct WV_CKRIFF *CKRIFF)

{
  WHEAD_S (fp, CKRIFF->ckID);
  WHEAD_V (fp, CKRIFF->ckSize, DS_EL);
  WHEAD_S (fp, CKRIFF->WAVEID);

  AF_wrFMT (fp, &CKRIFF->CKfmt);

  /* Write the Fact chunk if the data format is not PCM */
  if (!(CKRIFF->CKfmt.wFormatTag == WAVE_FORMAT_PCM ||
      (CKRIFF->CKfmt.wFormatTag == WAVE_FORMAT_EXTENSIBLE &&
       CKRIFF->CKfmt.SubFormat.wFormatTag == WAVE_FORMAT_PCM))) {
    WHEAD_S (fp, CKRIFF->CKfact.ckID);
    WHEAD_V (fp, CKRIFF->CKfact.ckSize, DS_EL);
    WHEAD_V (fp, CKRIFF->CKfact.dwSampleLength, DS_EL);
  }

  WHEAD_S (fp, CKRIFF->CKdata.ckID);
  WHEAD_V (fp, CKRIFF->CKdata.ckSize, DS_EL);

  return;
}

/* Write the format chunk */


static int
AF_wrFMT (FILE *fp, const struct WV_CKfmt *CKfmt)

{
  long int offs, LFMT;

  offs  = WHEAD_S (fp, CKfmt->ckID);
  offs += WHEAD_V (fp, CKfmt->ckSize, DS_EL);
  LFMT = CKfmt->ckSize + 8;
  offs += WHEAD_V (fp, CKfmt->wFormatTag, DS_EL);
  offs += WHEAD_V (fp, CKfmt->nChannels, DS_EL);
  offs += WHEAD_V (fp, CKfmt->nSamplesPerSec, DS_EL);
  offs += WHEAD_V (fp, CKfmt->nAvgBytesPerSec, DS_EL);
  offs += WHEAD_V (fp, CKfmt->nBlockAlign, DS_EL);
  offs += WHEAD_V (fp, CKfmt->wBitsPerSample, DS_EL);
  if (offs < LFMT)
    offs += WHEAD_V (fp, CKfmt->cbSize, DS_EL);
  if (offs < LFMT) {
    offs += WHEAD_V (fp, CKfmt->wValidBitsPerSample, DS_EL);
    offs += WHEAD_V (fp, CKfmt->dwChannelMask, DS_EL);
    offs += WHEAD_V (fp, CKfmt->SubFormat.wFormatTag, DS_EL);
    offs += WHEAD_S (fp, CKfmt->SubFormat.guidx);
  }

  assert (offs == LFMT);

  return offs;
}
