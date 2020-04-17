/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  int AFfWrTA (AFILE *AFp, const float Dbuff[], int Nval)

Purpose:
  Write text data to an audio file (float input values)

Description:
  This routine writes a specified number of samples to an audio file.  The
  input to this routine is a buffer of float values. The output file contains
  the text representation of the data values, one value to a line.

Parameters:
  <-  int AFfWrTA
      Number of samples written.  If this value is less than Nval, an error
      has occurred.
   -> AFILE *AFp
      Audio file pointer for an audio file opened by AFopnWrite
   -> const float Dbuff[]
      Array of floats with the samples to be written
   -> int Nval
      Number of samples to be written

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.2 $  $Date: 2003/05/09 01:11:34 $

-------------------------------------------------------------------------*/

#include <libtsp/AFdataio.h>
#include <libtsp/AFpar.h>


int
AFfWrTA (AFILE *AFp, const float Dbuff[], int Nval)

{
  int i, Nc;

  for (i = 0; i < Nval; ++i) {
    Nc = fprintf (AFp->fp, "%g\n", AFp->ScaleF * Dbuff[i]);
    if (Nc < 0)		/* fprintf returns a negative valued error code */
      break;
  }

  return i;
}
