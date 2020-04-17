/*-------------- Telecommunications & Signal Processing Lab ---------------

Routine:
  AFILE *AFrdAIhead (FILE *fp)

Purpose:
  Get file format information from an AIFF or AIFF-C sound file

Description:
  This routine reads the header for an AIFF or AIFF-C sound file.  The header
  information is used to set the file data format information in the audio
  file pointer structure.

  AIFF or AIFF-C sound file:
   Offset Length Type    Contents
      0     4    char   "FORM" file identifier
      4     4    int    Chunk length
      8     4    char   File identifier ("AIFF" or "AIFC")
    ...   ...    ...    ...
      C     4    char   "COMM" chunk identifier ("COMM")
     +4     4    int    Chunk length
     +8     2    int      Number of interleaved channels
    +10     4    int      Number of sample frames
    +14     2    int      Bits per sample
    +16    10    float    Sample frames per second
    +26     4    int      Compression type
    +30   ...    char     Compression name
    ...    ...   ...    ...
      S     4    char   "SSND" chunk identifier
     +4     4    int    Chunk length
     +8     4    int      Data offset
    +12     4    int      Block size
    +16   ...    ...      Audio data

Parameters:
  <-  AFILE *AFrdAIhead
      Audio file pointer for the audio file
   -> FILE *fp
      File pointer for the file

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.68 $  $Date: 2003/11/03 18:53:37 $

-------------------------------------------------------------------------*/

#include <setjmp.h>
#include <string.h>

#include <libtsp.h>
#include <libtsp/nucleus.h>
#include <libtsp/AFdataio.h>
#include <libtsp/AFheader.h>
#include <libtsp/AFmsg.h>
#include <libtsp/AFpar.h>
#include <libtsp/AIpar.h>

#define ICEILV(n,m)	(((n) + ((m) - 1)) / (m))	/* int n,m >= 0 */
#define MINV(a, b)	(((a) < (b)) ? (a) : (b))
#define RNDUPV(n,m)	((m) * ICEILV (n, m))		/* Round up */

#define SAME_CSTR(str,ref) 	(memcmp (str, ref, sizeof (str)) == 0)

#define ALIGN		2	/* Chunks padded out to a multiple of ALIGN */

/* setjmp / longjmp environment */
extern jmp_buf AFR_JMPENV;

static int
AF_rdFORM_AIFF (FILE *fp, struct AI_CkFORM *CkFORM, int *Ftype);
static int
AF_rdCOMM (FILE *fp, int Ftype, struct AI_CkCOMM *CkCOMM);
static void
AF_decCOMM (struct AI_CkCOMM *CkCOMM, struct AF_read *AFr);
static int
AF_checkFVER (FILE *fp, struct AI_CkFVER *CkFVER);
static long int
AF_decSSND (FILE *fp, struct AI_CkSSND *CkSSND);
static int
AF_rPstring (FILE *fp, char string[], int ncMax);

AF_READ_DEFAULT(AFr_default);	/* Define the AF_read defaults */


AFILE *
AFrdAIhead (FILE *fp)

{
  AFILE *AFp;
  long int offs, LFORM, Dstart, EoD;
  int Ftype, AtData;
  char Info[AF_MAXINFO];
  struct AF_read AFr;
  struct AI_CkFORM CkFORM;
  struct AI_CkPreamb CkHead;
  struct AI_CkCOMM CkCOMM;

/* Set the long jump environment; on error return a NULL */
  if (setjmp (AFR_JMPENV))
    return NULL;	/* Return from a header read error */

/* Each chunk has an identifier and a length.  The length gives the number of
   bytes in the chunk (not including the ckID or ckDataSize fields).  If the
   ckDataSize field is odd, a zero byte fills the space before the start of the
   next chunk.
   - Some AIFF-C files do not have this zero byte at the end of the file,
     i.e. the total file length is odd, yet the FORM chunk size indicates that
     the padding byte is there.
   - The chunks after the FORM chunk can appear in any order.
   - The standard is written in such a way that it is implied that one should
     scan the whole file for chunks.  Here we are trying to allow for non-
     seekable input.
     - If the COMM chunk comes ahead of the SSND chunk all is fine.  We get to
       the SSND chunk and then skip forward to the data.
     - If the COMM chunk follows the SSND chunk we will have to seek back to
       the data in the SSND chunk.  The file has to be seekable.
     - If the number of samples is zero (in the COMM chunk), there need be no
       SSND chunk.  When we get to the end-of-file, we will declare this to be
       the start of data.
*/

/* Defaults and inital values */
  AFr = AFr_default;
  AFr.InfoX.Info = Info;
  AFr.InfoX.Nmax = AF_MAXINFO;

/* Check the FORM chunk */
  if (AF_rdFORM_AIFF (fp, &CkFORM, &Ftype))
    return NULL;
  offs = 12L;	/* Positioned after FORM/AIFF preamble */
  LFORM = CkFORM.ckSize + 8;
  
  Dstart = 0L;
  EoD = 0L;
  AtData = 0;
  while (offs < LFORM-8) {	/* Leave 8 bytes for the chunk preamble */

    /* Read the chunk preamble */
    offs += RHEAD_S (fp, CkHead.ckID);

    /* COMM chunk */
    if (SAME_CSTR (CkHead.ckID, CKID_COMM)) {
      offs += AF_rdCOMM (fp, Ftype, &CkCOMM);
      AF_decCOMM (&CkCOMM, &AFr);
    }

    /* FVER chunk */
    else if (Ftype == FT_AIFF_C && SAME_CSTR (CkHead.ckID, CKID_FVER)) {
      offs += AF_checkFVER (fp, &CkFORM.CkFVER);
    }
    /* Text chunks */
    else if (SAME_CSTR (CkHead.ckID, CKID_NAME)) {
      offs += RHEAD_V (fp, CkHead.ckSize, DS_EB);
      offs += AFrdTextAFsp (fp, (int) CkHead.ckSize, "name: ",
			    &AFr.InfoX, ALIGN);
    }
    else if (SAME_CSTR (CkHead.ckID, CKID_AUTHOR)) {
      offs += RHEAD_V (fp, CkHead.ckSize, DS_EB);
      offs += AFrdTextAFsp (fp, (int) CkHead.ckSize, "author: ",
			    &AFr.InfoX, ALIGN);
    }
    else if (SAME_CSTR (CkHead.ckID, CKID_COPYRIGHT)) {
      offs += RHEAD_V (fp, CkHead.ckSize, DS_EB);
      offs += AFrdTextAFsp (fp, (int) CkHead.ckSize, "copyright: ",
			    &AFr.InfoX, ALIGN);
    }
    /* Annotation chunk */
    else if (SAME_CSTR (CkHead.ckID, CKID_ANNOTATION)) {
      offs += RHEAD_V (fp, CkHead.ckSize, DS_EB);
      offs += AFrdTextAFsp (fp, (int) CkHead.ckSize, "annotation: ",
			    &AFr.InfoX, ALIGN);
    }

    /* SSND chunk */
    else if (SAME_CSTR (CkHead.ckID, CKID_SSND)) {
      offs += AF_decSSND (fp, &CkFORM.CkSSND);
      AFr.NData.Ldata = CkFORM.CkSSND.ckSize - 8 - CkFORM.CkSSND.offset;
      offs += RSKIP (fp, CkFORM.CkSSND.offset);
      Dstart = offs;
      EoD = RNDUPV (Dstart + AFr.NData.Ldata, ALIGN);
      if (EoD >= LFORM || ! FLseekable (fp)) {
	AtData = 1;
	break;
      }
      else {
	AtData = 0;
	offs += RSKIP (fp, EoD - Dstart);
      }
    }

    /* Miscellaneous chunks */
    else {
      offs += RHEAD_V (fp, CkHead.ckSize, DS_EB);
      offs += RSKIP (fp, RNDUPV (CkHead.ckSize, ALIGN));
    }
 
  }

  /* Error Checks */
  if (AFr.DFormat.Format == FD_UNDEF) {
    UTwarn ("AFrdAIhead - %s", AFM_AIFF_BadHead);
    return NULL;
  }
  if ((! AtData && offs != LFORM) || (AtData && EoD != LFORM))
    UTwarn ("AFrdAIhead - %s", AFM_AIFF_BadSize);

  /* If no SSND chunk has been found, check for a zero data size (does not
     need a SSND chunk)
  */
  if (Dstart == 0L) {
    if (CkCOMM.numSampleFrames == 0) {
      AFr.NData.Ldata = 0L;
      AtData = 1;
      Dstart = offs;
    }
    else {
      UTwarn ("AFrdAIhead - %s", AFM_AIFF_NoSSND);
      return NULL;
    }
  }

  /* Position at the start of data */
  if (! AtData) {
    if (AFseek (fp, Dstart, NULL))
      return NULL;
  }

/* Set the parameters for file access */
  AFp = AFsetRead (fp, Ftype, &AFr, AF_FIX_NSAMP_HIGH + AF_FIX_LDATA_HIGH);

  return AFp;
}

/* Check the RIFF/AIFF file preamble */


static int
AF_rdFORM_AIFF (FILE *fp, struct AI_CkFORM *CkFORM, int *Ftype)

{
  long int Lfile, LFORM;

  RHEAD_S (fp, CkFORM->ckID);
  if (! SAME_CSTR (CkFORM->ckID, FM_IFF)) {
    UTwarn ("AFrdAIhead - %s", AFM_AIFF_BadId);
    return 1;
  }

  RHEAD_V (fp, CkFORM->ckSize, DS_EB);
  LFORM = CkFORM->ckSize + 8;

  if (FLseekable (fp)) {
    Lfile = FLfileSize (fp);
    if (LFORM > Lfile) {
      CkFORM->ckSize = Lfile - 8;
      UTwarn ("AFrdAIhead - %s", AFM_AIFF_FixFORM);
    }
  }
 
  RHEAD_S (fp, CkFORM->ckID);
  if (SAME_CSTR (CkFORM->ckID, FM_AIFF_C))
    *Ftype = FT_AIFF_C;
  else if (SAME_CSTR (CkFORM->ckID, FM_AIFF))
    *Ftype = FT_AIFF;
  else {
    UTwarn ("AFrdAIhead - %s", AFM_AIFF_BadId);
    return 1;
  }

  return 0;
}

/* Read the COMM chunk */


static int
AF_rdCOMM (FILE *fp, int Ftype, struct AI_CkCOMM *CkCOMM)

{
  int offs;

  offs  = RHEAD_V (fp, CkCOMM->ckSize, DS_EB);
  offs += RHEAD_V (fp, CkCOMM->numChannels, DS_EB);
  offs += RHEAD_V (fp, CkCOMM->numSampleFrames, DS_EB);
  offs += RHEAD_V (fp, CkCOMM->sampleSize, DS_EB);
  offs += RHEAD_S (fp, CkCOMM->sampleRate);

  /* Set the compressionType field for both AIFF and AIFF-C files */
  if (Ftype == FT_AIFF_C) {
    offs += RHEAD_S (fp, CkCOMM->compressionType);
    offs += AF_rPstring (fp, CkCOMM->compressionName, CNAME_MAX);
  }
  else {
    memcpy (CkCOMM->compressionType, CT_NONE, sizeof (CT_NONE));
    CkCOMM->compressionName[0] = '\0';
  }

  /* Some AIFF-C COMM chunks are declared to be of length 18 (the size for
     AIFF files) when they are in fact larger.  Here we find the actual
     length of the COMM chunk.
  */
  if (offs - 4 > (int) CkCOMM->ckSize) {
    UTwarn ("AFrdAIhead - %s", AFM_AIFF_FixCOMM);
    CkCOMM->ckSize = offs - 4;
  }
  offs += RSKIP (fp, RNDUPV (CkCOMM->ckSize, ALIGN) - (offs-4));

  return offs;
}

/* Read a P-string */


static int
AF_rPstring (FILE *fp, char string[], int ncMax)

{
  int offs, nc, ncP, nr;
  char slen[1];
 
  offs = RHEAD_S (fp, slen);	/* 1 byte length */

  nc = (int) slen[0];
  ncP = RNDUPV (nc + 1, 2);
  nr = MINV (nc, ncMax);

  offs += RHEAD_SN (fp, string, nr);
  string[nr] = '\0';
  offs += RSKIP (fp, ncP - (nc + 1));

  return offs;
}

/* Decode the data format: On error return via longjmp */


static void
AF_decCOMM (struct AI_CkCOMM *CkCOMM, struct AF_read *AFr)

{
  int NBytesS;

  /* Uncompressed */
  if (SAME_CSTR (CkCOMM->compressionType, CT_NONE)) {

    NBytesS = ICEILV (CkCOMM->sampleSize, 8);
    AFr->DFormat.NbS = (int) CkCOMM->sampleSize;
    if (NBytesS == 1)
      AFr->DFormat.Format = FD_INT8;
    else if (NBytesS == 2)
      AFr->DFormat.Format = FD_INT16;
    else if (NBytesS == 3)
      AFr->DFormat.Format = FD_INT24;
    else if (NBytesS == 4)
      AFr->DFormat.Format = FD_INT32;
    else {
      UTwarn ("AFrdAIhead - %s: \"%d\"", AFM_AIFF_UnsSSize,
	      (int) CkCOMM->sampleSize);
      longjmp (AFR_JMPENV, 1);
    }
  }

  /* AIFF-C, compressed */
  /* A-law and mu-law are compressed formats; for these formats
   CkCOMM.sampleSize = 16 (not checked) */
  else if (SAME_CSTR (CkCOMM->compressionType, CT_ULAW) ||
	   SAME_CSTR (CkCOMM->compressionType, CT_X_ULAW)) {
    AFr->DFormat.Format = FD_MULAW8;
    AFr->DFormat.NbS = 8 * FDL_MULAW8;
  }
  else if (SAME_CSTR (CkCOMM->compressionType, CT_ALAW) ||
	   SAME_CSTR (CkCOMM->compressionType, CT_X_ALAW)) {
    AFr->DFormat.Format = FD_ALAW8;
    AFr->DFormat.NbS = 8 * FDL_ALAW8;
  }
  else if (SAME_CSTR (CkCOMM->compressionType, CT_FLOAT32) ||
	   SAME_CSTR (CkCOMM->compressionType, CT_FL32)) {
    AFr->DFormat.Format = FD_FLOAT32;
    AFr->DFormat.NbS = 8 * FDL_FLOAT32;
    if (! UTcheckIEEE ())
      UTwarn ("AFrdAIhead - %s", AFM_NoIEEE);
  }
  else if (SAME_CSTR (CkCOMM->compressionType, CT_FLOAT64) ||
	   SAME_CSTR (CkCOMM->compressionType, CT_FL64)) {
    AFr->DFormat.Format = FD_FLOAT64;
    AFr->DFormat.NbS = 8 * FDL_FLOAT64;
    if (! UTcheckIEEE ())
      UTwarn ("AFrdAIhead - %s", AFM_NoIEEE);
  }
  else {
    UTwarn ("AFrdAIhead - %s: \"%.4s\" (\"%s\")", AFM_AIFF_UnsComp,
	    CkCOMM->compressionType, STstrDots (CkCOMM->compressionName, 32));
    longjmp (AFR_JMPENV, 1);
  }
  AFr->DFormat.Swapb = DS_EB;

  AFr->Sfreq = UTdIEEE80 (CkCOMM->sampleRate);
  AFr->NData.Nsamp = CkCOMM->numSampleFrames * CkCOMM->numChannels;
  AFr->NData.Nchan = CkCOMM->numChannels;

  return;
} 

/* Check the format version */


static int
AF_checkFVER (FILE *fp, struct AI_CkFVER *CkFVER)

{
  int offs;
  
  offs  = RHEAD_V (fp, CkFVER->ckSize, DS_EB);
  offs += RHEAD_V (fp, CkFVER->timestamp, DS_EB);
  if (CkFVER->timestamp != AIFCVersion1)
    UTwarn ("AFrdAIhead - %s", AFM_AIFF_BadVer);
  offs += RSKIP (fp, RNDUPV (CkFVER->ckSize + 4, ALIGN) - offs);

  return offs;
}

/* Decode the SSND chunk
   Position left at the beginning of data or at the end of the SSND chunk
*/


static long int
AF_decSSND (FILE *fp, struct AI_CkSSND *CkSSND)

{
  long int offs;

  /* Data size, skip to start of data */
  offs  = RHEAD_V (fp, CkSSND->ckSize, DS_EB); 
  offs += RHEAD_V (fp, CkSSND->offset, DS_EB);
  offs += RHEAD_V (fp, CkSSND->blockSize, DS_EB);

  return offs;
}
