/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  int AFdReadData (AFILE *AFp, long int offs, double Dbuff[], int Nreq)

Purpose:
  Read data from an audio file (return double values)

Description:
  This routine returns a specified number of samples at a given sample offset
  in an audio file.  The data in the file is converted to double format on
  output.  The sample data in the file is considered to be preceded and
  followed by zero-valued samples.  Thus if the sample offset is negative or
  points to beyond the number of samples in the file, the appropriate number
  of zero-valued samples is returned.  The file must have been opened using
  routine AFopnRead.

  The following program fragment illustrates the use of this routine to read
  overlapping frames of data.  For the simpler case of sequential access to the
  data without overlap, the variable Lmem should be set to zero.

    AFp = AFopnRead (...);
    ...
    Lmem =...
    Lframe =...
    Nadv = Lframe-Lmem;
    offs = -Lmem;
    while (1) {
      Nout = AFdReadData (AFp, offs, Dbuff, Lframe);
      offs = offs+Nadv;
      if (Nout == 0)
        break;
      ...
    }

  On encountering an error, the default behaviour is to print an error message
  and halt execution.

Parameters:
  <-  int AFdReadData
      Number of data values transferred from the file.  On reaching the end of
      the file, this value may be less than Nreq, in which case the last
      elements are set to zero.  This value can be used by the calling routine
      to determine when the data from the file has been exhausted.
   -> AFILE *AFp
      Audio file pointer for an audio file opened by AFopnRead
   -> long int offs
      Offset into the file in samples.  If offs is positive, the first value
      returned is offs samples from the beginning of the data.  The file data
      is considered to be preceded by zeros.  Thus if offs is negative, the
      appropriate number of zeros will be returned.  These zeros before the
      beginning of the data are counted as part of the count returned in Nout.
  <-  double Dbuff[]
      Array of doubles to receive the Nreq samples
   -> int Nreq
      Number of samples requested.  Nreq may be zero.

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.2 $  $Date: 2003/05/09 01:03:46 $

-------------------------------------------------------------------------*/

#include <assert.h>
#include <stdlib.h>		/* EXIT_FAILURE */

#include <libtsp.h>
#include <libtsp/AFdataio.h>
#include <libtsp/AFmsg.h>
#include <libtsp/AFpar.h>

#define MINV(a, b)	(((a) < (b)) ? (a) : (b))
#define MAXV(a, b)	(((a) > (b)) ? (a) : (b))

/* Reading routines */
static int
(*AF_Read[NFD])(AFILE *AFp, double Dbuff[], int Nreq) =
  { NULL,	AFdRdMulaw,	AFdRdAlaw,	AFdRdU1,	AFdRdI1,
    AFdRdI2,	AFdRdI3,	AFdRdI4,	AFdRdF4,	AFdRdF8,
    AFdRdTA };

/*
  The option flag ErrorHalt affects error handling.
  If ErrorHalt is clear, execution continues after an error
   - Fewer than requested elements are returned.  To distinguish between an
     error and and end-of-file, AFp->Error must be examined.
   - An unexpected end-of-file is an error
   - AFp->Error must be zero on input to this routine
*/


int
AFdReadData (AFILE *AFp, long int offs, double Dbuff[], int Nreq)

{
  int i, Nb, Nv, Nr, Nout;

/* Check the operation  */
  assert (AFp->Op == FO_RO);
  assert (! AFp->Error);

/* Fill in zeros at the beginning of data */
  Nb = (int) MAXV (0, MINV (-offs, Nreq));
  for (i = 0; i < Nb; ++i) {
    Dbuff[i] = 0.0;
    ++offs;
  }
  Nout = Nb;

/* Position the file */
  AFp->Error = AFposition (AFp, offs);

/* The file reading routines expect that the file is positioned at the data
   to be read.  They use the following AFp fields:
     AFp->fp - file pointer
     AFp->Swapb - data swap indicator
     AFp->ScaleF - data scaling factor
  An error is detected on the outside by calling ferror() or by checking
  AFp->Error (for text data files).

  Errors:  Nr < Nreq - Nb  && ferror or AFp->Error set
  EOF:     Nr < Nreq - Nb  && ferror and AFp->Error not set

  This routine updates the following AFp values
    AFp->Error - Set for an error
    AFp->Isamp - Current data sample.  This value is set to the current
      position before reading and incremented by the number of samples read.
    AFp->Nsamp - Number of samples (updated if not defined initially and EOF
      is detected)
*/

/* Transfer data from the file */
  if (AFp->Nsamp == AF_NSAMP_UNDEF)
    Nv = Nreq - Nout;
  else
    Nv = (int) MINV (Nreq - Nout, AFp->Nsamp - offs);

  if (! AFp->Error && Nv > 0) {
    Nr = (*AF_Read[AFp->Format]) (AFp, &Dbuff[Nb], Nv);
    Nout += Nr;
    AFp->Isamp += Nr;

/* Check for errors */
    if (Nr < Nv) {
      if (ferror (AFp->fp)) {
	UTsysMsg ("AFdReadData - %s %ld", AFM_ReadErrOffs, AFp->Isamp);
	AFp->Error = AF_IOERR;
      }
      else if (AFp->Error)
	UTwarn ("AFdReadData - %s %ld", AFM_ReadErrOffs, AFp->Isamp);
      else if (AFp->Nsamp != AF_NSAMP_UNDEF) {
	UTwarn ("AFdReadData - %s %ld", AFM_UEoFOffs, AFp->Isamp);
	AFp->Error = AF_UEOF;
      }
      else
	AFp->Nsamp = AFp->Isamp;
    }
  }

/* Zeros at the end of the file */
  for (i = Nout; i < Nreq; ++i)
    Dbuff[i] = 0.0;

  if (AFp->Error && (AFoptions ())->ErrorHalt)
    exit (EXIT_FAILURE);

  return Nout;
}
