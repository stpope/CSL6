/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  AFILE *AFwrAIhead (FILE *fp, struct AF_write *AF)

Purpose:
  Write header information to an AIFF or AIFF-C sound file

Description:
  This routine opens and writes header information to an AIFF or AIFF-C format
  sound file.

  AIFF-C sound file:
   Offset Length Type    Contents
      0     4    char   File identifier ("FORM")
      4     4    int    Chunk length
      8     4    char   File identifier ("AIFC" or "AIFF")
     12     4    char   "FVER" chunk identifier (AIFF-C files only)
     16     4    int    Chunk length
     20     4    int      Format version identifier
      C     4    char   "COMM" chunk identifier (C is 12 or 24)
            4    int    Chunk length
            2    int      Number of interleaved channels
    +10     4    int      Number of sample frames
            2    int      Bits per sample
           10    float    Sample frames per second
            4    int      Compression type (AIFF-C files only)
        ...    char     Compression name (AIFF-C files only)
    ...    ...   ...    ...
      S     4    char   SSND chunk identifier ("SSND")
     +4     4    int    Chunk length
     +8     4    int      Data offset (0)
    +12     4    int      Block size (0)
    +16   ...    ...      Audio data

Parameters:
  <-  AFILE *AFwrAIhead
      Audio file pointer for the audio file.  In case of error, the audio file
      pointer is set to NULL.
   -> FILE *fp
      File pointer for the audio file
  <-> struct AF_write *AFw
      Structure containing file parameters

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.45 $  $Date: 2003/11/03 18:47:04 $

-------------------------------------------------------------------------*/

#include <assert.h>
#include <setjmp.h>
#include <string.h>

#include <libtsp.h>
#include <libtsp/nucleus.h>
#include <libtsp/AFheader.h>
#include <libtsp/AFmsg.h>
#define AF_DATA_LENGTHS
#include <libtsp/AFpar.h>
#include <libtsp/AIpar.h>

#define ICEILV(n,m)	(((n) + ((m) - 1)) / (m))	/* int n,m >= 0 */
#define RNDUPV(n,m)	((m) * ICEILV (n, m))		/* Round up */

#define MCOPY(src,dest)		memcpy ((void *) (dest), (void *) (src), \
					sizeof (dest))
#define WRPAD(fp,size,align) \
     AFwriteHead (fp, NULL, 1, (int) (RNDUPV(size, align) - (size)), \
		  DS_NATIVE);

#define ALIGN		2	/* Chunks padded out to a multiple of ALIGN */

/* setjmp / longjmp environment */
extern jmp_buf AFW_JMPENV;

static void
AF_setCOMM (struct AI_CkCOMM *CkCOMM, const struct AF_write *AFw);
static void
AF_wrFORM (FILE *fp, const struct AI_CkFORM *CkFORM,
	   const struct AF_infoX *InfoX);
static int
AF_wPstring (FILE *fp, const char string[]);
static int
AF_checkAISpeaker (const unsigned char *SpkrConfig, int Nchan);


AFILE *
AFwrAIhead (FILE *fp, struct AF_write *AFw)

{
  AFILE *AFp;
  int Lw;
  long int size, Ldata;
  struct AI_CkFORM CkFORM;

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
    UTwarn ("AFwrAIhead - %s", AFM_AIFF_WRAccess);
    return NULL;
  }

  /* FORM chunk */
  MCOPY (FM_IFF, CkFORM.ckID);
  /* defer filling in the chunk size */
  if (AFw->Ftype == FTW_AIFF_C)
    MCOPY (FM_AIFF_C, CkFORM.AIFFID);
  else
    MCOPY (FM_AIFF, CkFORM.AIFFID);

  /* FVER chunk */
  if (AFw->Ftype == FTW_AIFF_C) {
    MCOPY (CKID_FVER, CkFORM.CkFVER.ckID);
    CkFORM.CkFVER.ckSize = 4;
    CkFORM.CkFVER.timestamp = AIFCVersion1;
  }
  else
    CkFORM.CkFVER.ckSize = 0;

  /* COMM chunk */
  AF_setCOMM (&CkFORM.CkCOMM, AFw);

  /* ANNO chunk */
  MCOPY (CKID_ANNOTATION, CkFORM.CkANNO.ckID);
  CkFORM.CkANNO.ckSize = (uint4_t) (4 + AFw->InfoX.N);

  /* SSND chunk */
  MCOPY (CKID_SSND, CkFORM.CkSSND.ckID);
  CkFORM.CkSSND.ckSize = 8 + Ldata;
  CkFORM.CkSSND.offset = 0;
  CkFORM.CkSSND.blockSize = 0;

 /* Fill in the FORM chunk size */
  size = 4 + 8 + RNDUPV(CkFORM.CkCOMM.ckSize, ALIGN)
           + 8 + RNDUPV(CkFORM.CkFVER.ckSize, ALIGN)
           + 8 + RNDUPV(CkFORM.CkSSND.ckSize, ALIGN);
  if (AFw->InfoX.N > 0)
    size += 8 + RNDUPV(CkFORM.CkANNO.ckSize, ALIGN);
  CkFORM.ckSize = (uint4_t) size;

/* Write out the header */
  AFw->DFormat.Swapb = DS_EB;
  AF_wrFORM (fp, &CkFORM, &(AFw->InfoX));

/* Channel configuration */
  AF_checkAISpeaker (AFw->SpkrConfig, AFw->Nchan);

/* Set the parameters for file access */
  if (AFw->Ftype == FTW_AIFF_C)
    AFp = AFsetWrite (fp, FT_AIFF_C, AFw);
  else
    AFp = AFsetWrite (fp, FT_AIFF, AFw);

  return AFp;
}

/* Fill in the COMM chunk */


static void
AF_setCOMM (struct AI_CkCOMM *CkCOMM, const struct AF_write *AFw)

{
  int ncP, Res, NbS;

  MCOPY (CKID_COMM, CkCOMM->ckID);
  Res = 8  * AF_DL[AFw->DFormat.Format];
  NbS = AFw->DFormat.NbS;

/* Set up the encoding parameters */
  switch (AFw->DFormat.Format) {
  case FD_INT8:
  case FD_INT16:
  case FD_INT24:
  case FD_INT32:
    MCOPY (CT_NONE, CkCOMM->compressionType);
    strcpy (CkCOMM->compressionName, CN_NONE);
    if (RNDUPV(NbS, 8) != Res) {
      UTwarn (AFMF_AIFF_InvNbS, "AFwrAIhead -", NbS, Res);
      NbS = Res;
    }
    CkCOMM->sampleSize = NbS;
    break; 
  /* There are two codes for each of mu-law and A-law coding.  The codes
     "ULAW" and "ALAW" originated with SGI software.  The Apple QuickTime
     software only recognizes "ulaw" and "alaw". Here we use the QuickTime
     conventions.
  */
  case FD_MULAW8:
    MCOPY (CT_ULAW, CkCOMM->compressionType);
    strcpy (CkCOMM->compressionName, CN_ULAW);
    CkCOMM->sampleSize = 16;		/* Uncompressed data size in bits */
    break;
  case FD_ALAW8:
    MCOPY (CT_ALAW, CkCOMM->compressionType);
    strcpy (CkCOMM->compressionName, CN_ALAW);
    CkCOMM->sampleSize = 16;		/* Uncompressed data size in bits */
    break;
  case FD_FLOAT32:
    MCOPY (CT_FLOAT32, CkCOMM->compressionType);
    strcpy (CkCOMM->compressionName, CN_FLOAT32);
    CkCOMM->sampleSize = Res;
    break;
  case FD_FLOAT64:
    MCOPY (CT_FLOAT64, CkCOMM->compressionType);
    strcpy (CkCOMM->compressionName, CN_FLOAT64);
    CkCOMM->sampleSize = Res;
    break;
  default:
    assert (0);
  }

  CkCOMM->numChannels = (uint2_t) AFw->Nchan;
  if (AFw->Nframe == AF_NFRAME_UNDEF)
    CkCOMM->numSampleFrames = (uint4_t) 0;
  else
    CkCOMM->numSampleFrames = (uint4_t) AFw->Nframe;
  /* CkCOMM.sampleSize filled in above */
  UTeIEEE80 (AFw->Sfreq, CkCOMM->sampleRate);
  /* CkCOMM.compressionType filled in above */
  /* CkCOMM.compressionName filled in above */

  if (AFw->Ftype == FTW_AIFF_C) {
    ncP = AF_wPstring (NULL, CkCOMM->compressionName);
    CkCOMM->ckSize = 22 + ncP;
  }
  else {
    assert (AFw->DFormat.Format == FD_INT8 ||
	    AFw->DFormat.Format == FD_INT16 ||
	    AFw->DFormat.Format == FD_INT24 ||
	    AFw->DFormat.Format == FD_INT32);
    CkCOMM->ckSize = 18;
  }

  return;
}

/* Write out the header */


static void
AF_wrFORM (FILE *fp, const struct AI_CkFORM *CkFORM,
	   const struct AF_infoX *InfoX)

{
  WHEAD_S (fp, CkFORM->ckID);
  WHEAD_V (fp, CkFORM->ckSize, DS_EB);
  WHEAD_S (fp, CkFORM->AIFFID);

  if (CkFORM->CkFVER.ckSize > 0) {
    WHEAD_S (fp, CkFORM->CkFVER.ckID);
    WHEAD_V (fp, CkFORM->CkFVER.ckSize, DS_EB);
    WHEAD_V (fp, CkFORM->CkFVER.timestamp, DS_EB);
  }

  WHEAD_S (fp, CkFORM->CkCOMM.ckID);
  WHEAD_V (fp, CkFORM->CkCOMM.ckSize, DS_EB);
  WHEAD_V (fp, CkFORM->CkCOMM.numChannels, DS_EB);
  WHEAD_V (fp, CkFORM->CkCOMM.numSampleFrames, DS_EB);
  WHEAD_V (fp, CkFORM->CkCOMM.sampleSize, DS_EB);
  WHEAD_S (fp, CkFORM->CkCOMM.sampleRate);
  if (CkFORM->CkCOMM.ckSize > 18) {
    WHEAD_S (fp, CkFORM->CkCOMM.compressionType);
    AF_wPstring (fp, CkFORM->CkCOMM.compressionName);
    WRPAD (fp, CkFORM->CkCOMM.ckSize, ALIGN);
  }

  if (InfoX->N > 0) {
    WHEAD_S (fp, CkFORM->CkANNO.ckID);
    WHEAD_V (fp, CkFORM->CkANNO.ckSize, DS_EB);
    WHEAD_SN (fp, FM_AFSP, (sizeof FM_AFSP) - 1);	/* Omit null char */
    WHEAD_SN (fp, InfoX->Info, InfoX->N);
    WRPAD (fp, CkFORM->CkANNO.ckSize, ALIGN);
  }

  WHEAD_S (fp, CkFORM->CkSSND.ckID);
  WHEAD_V (fp, CkFORM->CkSSND.ckSize, DS_EB);
  WHEAD_V (fp, CkFORM->CkSSND.offset, DS_EB);
  WHEAD_V (fp, CkFORM->CkSSND.blockSize, DS_EB);

  return;
}

/* Write a P-string, returning the full length */
/* P-string:
   - number of characters in string (stored in a byte)
   - string
   - trailing null to make the overall size even
*/


static int
AF_wPstring (FILE *fp, const char string[])

{
  char slen;
  int nc, ncP;

  nc = strlen (string);
  ncP = RNDUPV (nc + 1, 2);

  if (fp != NULL) {
    slen = (char) nc;
    WHEAD_V (fp, slen, DS_EB);
    WHEAD_SN (fp, string, nc);
    WHEAD_SN (fp, NULL, ncP - (nc + 1));
  }

  return ncP;
}

/* Check the loudspeaker locations */

#define NELEM(array)	((int) ((sizeof array) / (sizeof array[0])))


static int
AF_checkAISpeaker (const unsigned char *SpkrConfig, int Nchan)

{
  int Nspkr, i, Nconfig, Err;

  /* Standard speaker configurations as per AIFF-C documentation */
  static const unsigned char S2[] = {AF_SPKR_FL, AF_SPKR_FR, '\0'};
  static const unsigned char S3[] = {AF_SPKR_FL, AF_SPKR_FR, AF_SPKR_FC, '\0'};
  static const unsigned char SQ[] = {AF_SPKR_FL, AF_SPKR_FR, AF_SPKR_BL,
				     AF_SPKR_BR, '\0'};
  static const unsigned char S4[] = {AF_SPKR_FL, AF_SPKR_FC, AF_SPKR_FR,
				     AF_SPKR_LF, '\0'};
  static const unsigned char S6[] = {AF_SPKR_FL, AF_SPKR_FLC, AF_SPKR_FC,
				     AF_SPKR_FR, AF_SPKR_FRC, AF_SPKR_LF,
				     '\0'};
  static const unsigned char *AI_SpkrConfig[] = {S2, S3, SQ, S4, S6};

  if (SpkrConfig[0] == AF_X_SPKR || SpkrConfig[0] == '\0')
    Nspkr = 0;
  else
    Nspkr = strlen ((const char *) SpkrConfig);

  if (Nchan == 1 || Nspkr == 0)
    Err = 0;
  else {
    Err = 1;
    if (Nchan == Nspkr) {
      Nconfig = NELEM (AI_SpkrConfig);
      for (i = 0; i < Nconfig; ++i) {
	if (strcmp ((const char *) SpkrConfig,
		    (const char *) AI_SpkrConfig[i]) == 0) {
	  Err = 0;
	  break;
	}
      }
    }
  }

  if (Err)
    UTwarn ("AFwrAIhead - %s", AFM_AIFF_BadSpkr);

  return Err;
}
