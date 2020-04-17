/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  AFILE *AFsetNHread (FILE *fp, const char Fname[])

Purpose:
  Get file format information for a headerless audio file

Description:
  This routine gets file information for an audio file with a non-standard
  header or with no header.  File format information is passed to this routine
  by calling AFsetNHpar before calling this routine.  This information is used
  to set the file data format information in the audio file pointer structure.

Parameters:
  <-  AFILE *AFsetNHread
      Audio file pointer for the audio file
   -> FILE *fp
      File pointer for the file
   -> const char Fname[]
      File name

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.68 $  $Date: 2003/05/09 01:21:35 $

-------------------------------------------------------------------------*/

#include <stdio.h>

#include <libtsp.h>
#include <libtsp/nucleus.h>
#include <libtsp/AFdataio.h>
#include <libtsp/AFheader.h>
#include <libtsp/AFmsg.h>
#include <libtsp/AFpar.h>
#include <libtsp/sysOS.h>

#if (SY_OS != SY_OS_UNIX)
#  include <io.h>	/* setmode */
#  include <fcntl.h>	/* O_TEXT */
#endif


AF_READ_DEFAULT(AFr_default);	/* Define the AF_read defaults */


AFILE *
AFsetNHread (FILE *fp, const char Fname[])

{
  AFILE *AFp;
  struct AF_NHpar *NHpar;
  struct AF_read AFr;

/* Defaults and inital values */
  AFr = AFr_default;

/* Get the file data parameters */
  NHpar = &((AFoptions ())->NHpar);

/* Check the data format */
  if (NHpar->Format == FD_UNDEF) {
    UTwarn ("AFsetNHread - %s", AFM_NH_NoFormat);
    return NULL;
  }

/* Reopen the file as a text file if necessary */
/* (For Unix, text files and binary files are the same) */
#if (SY_OS != SY_OS_UNIX)
  if (NHpar->Format == FD_TEXT) {
    if (fp == stdin) {
      if (setmode (fileno (fp), O_TEXT) == -1)
	UTwarn ("AFsetNHread - %s", AFM_NoRTextMode);
    }
    else {
      fp = freopen (Fname, "r", fp);
      if (fp == NULL) {
	UTsysMsg ("AFsetNHread - %s", AFM_ReopenErr);
	return NULL;
      }
    }
  }
#endif

/* Position at the start of data
   Note: There is an inconsistency here in the byte offset count for text
     files.  We read START bytes to move ahead to the starting point.  The
     read operation returns a single LF for the end-of-line, when for some
     operating systems, the file itself contains, for instance, CR/LF for the
     end-of-line.
*/
  if (RSKIP (fp, NHpar->Start) != NHpar->Start)
    return NULL;

/* Set the parameters for file access */
  AFr.Sfreq = NHpar->Sfreq;
  AFr.DFormat.Format = NHpar->Format;
  AFr.DFormat.Swapb = NHpar->Swapb;
  if (NHpar->ScaleF == AF_SF_DEFAULT)
    AFr.DFormat.ScaleF = AF_SF[NHpar->Format];
  else
    AFr.DFormat.ScaleF = NHpar->ScaleF;
  AFr.NData.Nchan = NHpar->Nchan;

  AFp = AFsetRead (fp, FT_NH, &AFr, AF_NOFIX);

  return AFp;
}
