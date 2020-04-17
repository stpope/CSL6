/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  AFILE *AFopnWrite (const char Fname[], int Ftype, int Dtype, long int Nchan,
                     double Sfreq, FILE *fpinfo)

Purpose:
  Open an audio file for writing

Description:
  This routine opens an audio file for writing.  This routine sets up the audio
  file parameters to write data of the given format to the audio file.  After
  writing data to the file, the routine AFclose should be called to update the
  file header information and close the file.

  By default, information consisting of the date and the program creating the
  file (see UTsetProg) is written to the audio file header or trailer.  The
  routine AFsetInfo can be called before calling this routine to specify
  additional information to be written to the file.
 
  This routine can write AU audio files, AIFF-C files, WAVE files, and
  headerless audio files.

  AU audio file:
    8-bit mu-law, 8-bit A-law, 8/16/24/32-bit integer, and 32/64-bit IEEE
   floating-point data formats are supported.
  WAVE file:
    8-bit mu-law, 8-bit A-law, offset-binary 8-bit integer, 16/24/32-bit
    integer, and 32/64-bit IEEE floating-point, data formats are supported.
  AIFF sound file:
    8/16/24/32-bit integer data formats are supported.
  AIFF-C sound file:
    8-bit mu-law, 8-bit A-law, 8/16/24/32-bit integer, and 32/64-bit IEEE
    floating-point data formats are supported.
  Headerless file:
    8-bit mu-law, 8-bit A-law, offset-binary 8-bit integer, 8/16/24/32-bit
    integer, 32/64-bit floating-point, and text data formats are supported.
    A text format file has the data in character form, one value to a line.

  For the fixed point file data representations, input values in the range
  [-1,  +1) ([-.5, +0.5) for 8-bit data) will be converted without clipping.
  The scaling factor shown below is applied to the data before they are
  written to the file.
     data format   scaling factor   file data values
    8-bit mu-law    32,768         [-32,124, +32,124]
    8-bit A-law     32,768         [-32,256, +32,256]
    8-bit integer   256            [-128, 127]
    16-bit integer  32,768         [-32,768, +32,767]
    24-bit integer  8,388,608      [-8,388,608, +8,388,607]
    32-bit integer  2,147,483,648  [-2,147,483,648, 2,147,483,647]
  For files containing floating-point data, values are scaled by unity before
  being written to the file.

Parameters:
  <-  AFILE *AFopnWrite
      Audio file pointer for the audio file
   -> const char Fname[]
      Character string specifying the file name.  The file name "-" means
      standard output.
   -> int Ftype
      Output file type code
        FTW_AU        = 1,      AU audio file
        FTW_WAVE      = 2,      WAVE file (auto extensible format)
        FTW_WAVE_NOEX = 2 + 16, WAVE file (no extensible format)
        FTW_AIFF_C    = 3,      AIFF-C sound file
        FTW_NH_EB     = 4 +  0, Headerless file (big-endian byte order)
        FTW_NH_EL     = 4 + 16, Headerless file (little-endian byte order)
        FTW_NH_NATIVE = 4 + 32, Headerless file (native byte order)
        FTW_NH_SWAP   = 4 + 48, Headerless file (byte-swapped byte order)
	FTW_AIFF      = 5,      AIFF sound file
   -> int Dformat
      Data format code
        FD_MULAW8  = 1,  mu-law 8-bit data
        FD_ALAW8   = 2,  A-law 8-bit data
        FD_UINT8   = 3,  offset-binary 8-bit integer data
        FD_INT8    = 4,  two's-complement 8-bit integer data
        FD_INT16   = 5,  two's-complement 16-bit integer data
        FD_INT24   = 6,  two's-complement 24-bit integer data
        FD_INT32   = 7,  two's-complement 32-bit integer data
        FD_FLOAT32 = 8,  32-bit floating-point data
        FD_FLOAT64 = 9,  64-bit floating-point data
        FD_TEXT    = 10,  text data
   -> long int Nchan
      Number of channels
   -> double Sfreq
      Sampling frequency
   -> FILE *fpinfo
      File pointer for printing audio file information.  If fpinfo is not NULL,
      information about the audio file is printed on the stream selected by
      fpinfo.

Author / revision:
  P. Kabal  Copyright (C) 2006
  $Revision: 1.7 $  $Date: 2006/06/06 13:53:15 $

-------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>		/* EXIT_FAILURE */
#include <string.h>

#include <libtsp.h>
#include <libtsp/AFheader.h>
#include <libtsp/AFmsg.h>
#include <libtsp/AFpar.h>

#define ERR_MSG	0
#define SYS_MSG	1

static void
AF_error (const char Fname[], int sysFlag);
static FILE*
AF_open_write (const char Fname[], int Dformat);

/* If ErrorHalt is clear, execution continues after an error */


AFILE *
AFopnWrite (const char Fname[], int Ftype, int Dformat, long int Nchan,
	    double Sfreq,  FILE *fpinfo)

{
  AFILE *AFp;
  FILE *fp;
  struct AF_write AFw;

/* Set up and check the audio file parameters */
  if (AFpreSetWPar (Ftype, Dformat, Nchan, Sfreq, &AFw)) {
    AF_error (Fname, ERR_MSG);
    return NULL;
  }

/* Open the file for writing */
  fp = AF_open_write (Fname, AFw.DFormat.Format);
  if (fp == NULL)
    return NULL;

  switch (AFw.Ftype) {
  case FTW_AU:
    AFp = AFwrAUhead (fp, &AFw);
    break;
  case FTW_WAVE:
  case FTW_WAVE_NOEX:
    AFp = AFwrWVhead (fp, &AFw);
    break;
  case FTW_AIFF:
  case FTW_AIFF_C:
    AFp = AFwrAIhead (fp, &AFw);
    break;
  case FTW_NH_EB:
  case FTW_NH_EL:
  case FTW_NH_NATIVE:
  case FTW_NH_SWAP:
    AFp = AFsetNHwrite (fp, &AFw);
    break;
  default:
    AFp = NULL;
    break;
  }

/* Error messages */
  if (AFp == NULL) {
    fclose (fp);
    AF_error (Fname, ERR_MSG);
    return NULL;
  }

/* Reset options */
  (void) AFresetOptions (AF_OPT_OUTPUT);

/* Print the header information */
  AFprAFpar (AFp, Fname, fpinfo);

return AFp;
}

/* Open a file for writing */

#include <libtsp/sysOS.h>
#if (SY_OS == SY_OS_WINDOWS || SY_OS == SY_OS_CYGWIN)
#  include <io.h>	/* setmode */
#  include <fcntl.h>	/* IO_BINARY */
#endif

static FILE *
AF_open_write (const char Fname[], int Dformat)

{
  FILE *fp;

  if (strcmp (Fname, "-") == 0) {

    /* Output to standard output */
    fp = stdout;
#if (SY_OS == SY_OS_WINDOWS || SY_OS == SY_OS_CYGWIN)
    if (Dformat == FD_TEXT) {
      if (setmode (fileno(fp), O_TEXT) == -1)
	UTwarn ("AFopnWrite - %s", AFM_NoWTextMode);
    }
    else {
      if (setmode (fileno(fp), O_BINARY) == -1)
	UTwarn ("AFopnWrite - %s", AFM_NoWBinMode);
    }
#endif

  }
  else {

    /* Output to a file */
    if (Dformat == FD_TEXT)
      fp = fopen (Fname, "w");
    else
      fp = fopen (Fname, "wb");
  }

  if (fp == NULL) {
    AF_error (Fname, SYS_MSG);
    return NULL;
  }

  return fp;
}

/* Print an error message with the file name */


static void
AF_error (const char Fname[], int sysFlag)

{
  const char *fn;
  struct AF_opt *AFopt;

  if (strcmp (Fname, "-") == 0)
    fn = "<stdout>";
  else
    fn = Fname;

  if (sysFlag)
    UTsysMsg ("AFopnWrite: %s \"%s\"", AFM_OpenWErr, fn);
  else
    UTwarn ("AFopnWrite: %s \"%s\"", AFM_OpenWErr, fn);
  AFopt = AFoptions ();
  if (AFopt->ErrorHalt)
    exit (EXIT_FAILURE);

  /* Reset options */
  (void) AFresetOptions (AF_OPT_OUTPUT);

  return;
}
