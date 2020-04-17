/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  int AFreadData (AFILE *AFp, long int offs, float Dbuff[], int Nreq)

Purpose:
  Read data from an audio file (return float values)

Description:
  Use AFfReadData instead.

Parameters:
  <-  int AFreadData
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
  <-  float Dbuff[]
      Array of floats to receive the Nreq samples
   -> int Nreq
      Number of samples requested.  Nreq may be zero.

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.50 $  $Date: 2003/05/09 01:03:46 $

-------------------------------------------------------------------------*/

#include <libtsp.h>


int
AFreadData (AFILE *AFp, long int offs, float Dbuff[], int Nreq)

{
  int Nout;

  Nout = AFfReadData (AFp, offs, Dbuff, Nreq);

  return Nout;
}
