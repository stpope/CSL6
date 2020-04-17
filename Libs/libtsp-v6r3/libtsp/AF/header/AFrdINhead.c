/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  AFILE *AFrdINhead (FILE *fp)

Purpose:
  Get file format information from an INRS-Telecommunications audio file.

Description:
  This routine reads the header for an INRS-Telecommunications audio file.  The
  header information is used to set the file data format information in the
  audio file pointer structure.

  INRS-Telecommunications audio file:
   Offset Length Type    Contents
      0     4    float  Sampling Frequency (VAX float format)
      6    20    char   Creation time (e.g. " 8-DEC-1982 16:52:50")
     26     4    int    Number of speech samples in the file
    512    ...   --     Audio data
  The number of speech samples is checked against the total extent of the file.
  If unspecified, the file size is used to determine the number of samples.

Parameters:
  <-  AFILE *AFrdINhead
      Audio file pointer for the audio file
   -> FILE *fp
      File pointer for the file

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.66 $  $Date: 2003/11/03 18:55:08 $

-------------------------------------------------------------------------*/

#include <setjmp.h>
#include <string.h>

#include <libtsp.h>
#include <libtsp/AFheader.h>
#include <libtsp/AFmsg.h>
#include <libtsp/AFpar.h>
#include <libtsp/UTtypes.h>

#define SAME_CSTR(str,ref) 	(memcmp (str, ref, sizeof (str)) == 0)

#define LHEAD		512L
#define IN_NOSIZE       (~((uint4_t) 0))	/* Unspecified data length */

/* Common sampling frequencies (VAX floating-point values) in file byte order.
   The sampling frequencies recognized are 6500, 20000/3, 8000, 10000, 12000,
   16000, and 20000 Hz. */
#define NINRS	7
static const char *FM_INRS[NINRS] = {
  "\313\106\0\040",   "\320\106\125\125", "\372\106\0\0",   "\034\107\0\100",
  "\073\107\0\200",   "\172\107\0\0",     "\234\107\0\100"
};
static const double VSfreq[NINRS] = {
  6500., 20000./3., 8000., 10000., 12000., 16000., 20000.
};

#define NDT		20
struct IN_head {
  char Sfreq[4];	/* Sampling freq (VAX float) */
/* int2_t fill; */
/*  char Datetime[NDT]; */
  uint4_t Nsamp;	/* No. samples */
};
#define IN_MAXINFO	LHEAD

/* setjmp / longjmp environment */
extern jmp_buf AFR_JMPENV;

AF_READ_DEFAULT(AFr_default);	/* Define the AF_read defaults */


AFILE *
AFrdINhead (FILE *fp)

{
  AFILE *AFp;
  int iSF;
  long int offs;
  char Info[IN_MAXINFO];
  struct IN_head Fhead;
  struct AF_read AFr;

/* Set the long jump environment; on error return a NULL */
  if (setjmp (AFR_JMPENV))
    return NULL;	/* Return from a header read error */

/* Notes:
  - Very old INRS-Telecom audio files (generated on a PDP-11) have ~0 values
    for unwritten bytes in the header.
  - Old INRS-Telecom audio file used the Fhead.Nsamp field.  These files were
    an integral number of disk blocks long (512 bytes per disk block), with
    possibly part of the last block being unused.
  - Old INRS-Telecom audio files (generated on a PDP-11) have only an 18 byte
    date and time, followed by two 0 or two ~0 bytes.
*/

/* Defaults and inital values */
  AFr = AFr_default;
  AFr.InfoX.Info = Info;
  AFr.InfoX.Nmax = IN_MAXINFO;

/* Read in the header */
  offs  = RHEAD_S (fp, Fhead.Sfreq);
  offs += RSKIP (fp, 6L - offs);
  offs += AFrdTextAFsp (fp, NDT, "date: ", &AFr.InfoX, 1);
  offs += RHEAD_V (fp, Fhead.Nsamp, DS_EL);

/* Use the sampling frequency as a file magic value */
  for (iSF = 0; iSF < NINRS; iSF++) {
    if (SAME_CSTR (Fhead.Sfreq, FM_INRS[iSF]))
      break;
  }
  if (iSF >= NINRS) {
    UTwarn ("AFrdINhead - %s", AFM_INRS_BadId);
    return NULL;
  }

/* Position at the start of data */
  RSKIP (fp, LHEAD - offs);

/* Set the parameters for file access */
  AFr.Sfreq = VSfreq[iSF];
  AFr.DFormat.Format = FD_INT16;
  AFr.DFormat.Swapb = DS_EL;
  if (Fhead.Nsamp != 0 && Fhead.Nsamp != IN_NOSIZE)
    AFr.NData.Nsamp = (long int) Fhead.Nsamp;

  AFp = AFsetRead (fp, FT_INRS, &AFr, AF_NOFIX);

  return AFp;
}
