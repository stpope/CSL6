/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  AFILE *AFrdTAhead (FILE *fp)

Purpose:
  Get file format information from a text audio file

Description:
  This routine reads the header for a text audio file.  The header information
  is used to set the file data format information in the audio file pointer
  structure.

  Text audio files are used to store audio data (specifically noise samples) on
  CDROM's.  The audio data itself is stored as text.  The text specifies
  integer values, with one value per line.  Access to data should normally be
  sequential since moving backwards in the file is inefficient.

  Text audio header:
   Offset Length Type    Contents
      0     4    char   File identifier ("%//")
      -     -    char   Sampling frequency ("%sampling rate: <value> KHz")
      -     -    char   Audio data
  For text audio files, the audio data is the character representation of
  integer values, with one value per line.

Parameters:
  <-  AFILE *AFrdTAhead
      Audio file pointer for the audio file
   -> FILE *fp
      File pointer for the file

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.34 $  $Date: 2003/11/03 18:51:32 $

-------------------------------------------------------------------------*/

#include <string.h>

#include <libtsp.h>
#include <libtsp/AFdataio.h>
#include <libtsp/AFheader.h>
#include <libtsp/AFpar.h>
#include <libtsp/AFmsg.h>

#define FM_TXAUD	"%//"
#define MAX_NHEAD	100		/* Maximum number of lines */
#define TA_SRATE_STR	"%sampling rate:"

AF_READ_DEFAULT(AFr_default);	/* Define the AF_read defaults */


AFILE *
AFrdTAhead (FILE *fp)

{
  AFILE *AFp;
  char *line;
  double FkHz;
  int c, n, ErrCode;
  char Info[AF_MAXINFO];
  struct AF_read AFr;

/* Defaults and inital values */
  AFr = AFr_default;
  AFr.InfoX.Info = Info;
  AFr.InfoX.Nmax = AF_MAXINFO;

  AFp = NULL;
  ErrCode = 0;

/* Check the file magic */
  line = AFgetLine (fp, &ErrCode);
  if (line == NULL) {
    UTwarn ("AFrdTAhead - %s", AFM_UEoF);
    return NULL;
  }
  if (strcmp (line, FM_TXAUD) != 0) {
    UTwarn ("AFrdTAhead - %s", AFM_TA_BadId);
    return NULL;
  }

/* Check the header */
  FkHz = 0.0;
  for (n = 0; n < MAX_NHEAD; ++n) {

    /* Peek ahead to see if the header has ended */
    c = fgetc (fp);
    if (c == EOF)
      break;
    ungetc (c, fp);
    if (c != '%')
      break;

    /* Read a line */
    line = AFgetLine (fp, &ErrCode);
    if (ErrCode)
      return NULL;

    /* Save the line in the AFsp information structure */
    AFaddAFspRec ("", line, strlen (line), &AFr.InfoX);

    /* Check for a sampling frequency record */
    if (strncmp (line, TA_SRATE_STR, (sizeof (TA_SRATE_STR)) - 1) == 0)
      sscanf (line, "%%sampling rate:%lg KHz", &FkHz); /* "KHz" is ignored */
  }

/* Error checks */
  if (n >= MAX_NHEAD) {
    UTwarn ("AFrdTAhead - %s", AFM_TA_LongHead);
    return NULL;
  }
  if (FkHz == 0.0) {
    UTwarn ("AFrdTAhead - %s", AFM_TA_NoNSamp);
    return NULL;
  }

/* Set the parameters for file access */
  AFr.Sfreq = 1000. * FkHz;
  AFr.DFormat.Format = FD_TEXT;
  AFr.DFormat.ScaleF = AF_SF_INT16;

  AFp = AFsetRead (fp, FT_TXAUD, &AFr, AF_NOFIX);

  return AFp;
}
