/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  AFILE *AFrdAUhead (FILE *fp)

Purpose:
  Get file format information from an AU audio file

Description:
  This routine reads the header for an AU audio file.  The header information
  is used to set the file data format information in the audio file pointer
  structure.

  AU audio file header:
   Offset Length Type    Contents
      0     4     int   File identifier (".snd")
      4     4     int   Header size (bytes)
      8     4     int   Audio data length (bytes)
     12     4     int   Data encoding format
     16     4     int   Sample rate (samples per second)
     20     4     int   Number of interleaved channels
     24     4     int   AFsp identifier ("AFsp")
     28    ...    --    Additional header information
      -    ...    --    Audio data

Parameters:
  <-  AFILE *AFrdAUhead
      Audio file pointer for the audio file
   -> FILE *fp
      File pointer for the file

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.89 $  $Date: 2003/11/04 12:25:49 $

-------------------------------------------------------------------------*/

#include <setjmp.h>
#include <string.h>

#include <libtsp.h>
#include <libtsp/nucleus.h>
#include <libtsp/AFheader.h>
#include <libtsp/AFmsg.h>
#include <libtsp/AFpar.h>
#include <libtsp/AUpar.h>

#define SAME_CSTR(str,ref) 	(memcmp (str, ref, sizeof (str)) == 0)

/* setjmp / longjmp environment */
extern jmp_buf AFR_JMPENV;

AF_READ_DEFAULT(AFr_default);	/* Define the AF_read defaults */


AFILE *
AFrdAUhead (FILE *fp)

{
  AFILE *AFp;
  long int offs;
  char Info[AF_MAXINFO];
  struct AU_head Fhead;
  struct AF_read AFr;

/* Set the long jump environment; on error return a NULL */
  if (setjmp (AFR_JMPENV))
    return NULL;	/* Return from a header read error */

/* Defaults and inital values */
  AFr = AFr_default;
  AFr.InfoX.Info = Info;
  AFr.InfoX.Nmax = AF_MAXINFO;

/* Check the file magic */
  offs = RHEAD_S (fp, Fhead.Magic);
  if (! SAME_CSTR (Fhead.Magic, FM_AU)) {
    UTwarn ("AFrdAUhead - %s", AFM_AU_BadId);
    return NULL;
  }

/* Read the data parameters */
  offs += RHEAD_V (fp, Fhead.Lhead, DS_EB);
  offs += RHEAD_V (fp, Fhead.Ldata, DS_EB);
  offs += RHEAD_V (fp, Fhead.Dencod, DS_EB);
  offs += RHEAD_V (fp, Fhead.Srate, DS_EB);
  offs += RHEAD_V (fp, Fhead.Nchan, DS_EB);

/* Pick up AFsp information records */
  AFrdTextAFsp (fp, (int) (Fhead.Lhead - offs), "AU info: ", &AFr.InfoX, 1);

/* Set up the decoding parameters */
  switch (Fhead.Dencod) {
  case AU_MULAW8:
    AFr.DFormat.Format = FD_MULAW8;
    break;
  case AU_ALAW8:
    AFr.DFormat.Format = FD_ALAW8;
    break;
  case AU_LIN8:
    AFr.DFormat.Format = FD_INT8;
    break;
  case AU_LIN16:
    AFr.DFormat.Format = FD_INT16;
    break;
  case AU_LIN24:
    AFr.DFormat.Format = FD_INT24;
    break;
  case AU_LIN32:
    AFr.DFormat.Format = FD_INT32;
    break;
  case AU_FLOAT32:
    AFr.DFormat.Format = FD_FLOAT32;
    if (! UTcheckIEEE ())
      UTwarn ("AFrdAUhead - %s", AFM_NoIEEE);
    break;
  case AU_DOUBLE64:
    AFr.DFormat.Format = FD_FLOAT64;
    if (! UTcheckIEEE ())
      UTwarn ("AFrdAUhead - %s", AFM_NoIEEE);
    break;
  case AU_G721:
    UTwarn ("AFrdAUhead - %s: \"%s\"", AFM_AU_UnsData, "G.721 ADPCM");
    return NULL;
  default:
    UTwarn ("AFrdAUhead - %s: \"%ld\"", AFM_AU_UnsData,
	    (long int) Fhead.Dencod);
    return NULL;
  }
  AFr.DFormat.Swapb = DS_EB;

/* Set the parameters for file access */
  AFr.Sfreq = (double) Fhead.Srate;
  if (Fhead.Ldata != AU_NOSIZE)
    AFr.NData.Ldata = (long int) Fhead.Ldata;
  AFr.NData.Nchan = (long int) Fhead.Nchan;

  AFp = AFsetRead (fp, FT_AU, &AFr, AF_FIX_LDATA_HIGH);

  return AFp;
}
