/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  int SPfQuantL (double x, const float Xq[], int Nreg)

Purpose:
  Binary search for a bounding interval

Description:
  This function returns the index of the quantizer region corresponding to
  a given input value. The quantizer is specified by an array of quantizer
  decision levels. A binary search is used to determine the quantizer region.

  The index value takes on values from 0 to Nreg-1. If Nreg is equal to one,
  the index is set to zero. Otherwise, the index is determined as shown in
  the following table. The number of decision levels is one less than the
  number of regions.
    index            input value
     0                    x < Xq[0]
     1           Xq[0] <= x < Xq[1]
     2           Xq[1] <= x < Xq[2]
    ...                  ...
     i         Xq[i-1] <= x < Xq[i]
    ...                  ...
   Nreg-2   Xq[Nreg-3] <= x < Xq[Nreg-2]
   Nreg-1   Xq[Nreg-2] <= x

Parameters:
  <-  int SPfQuantL
      Output index of the quantizer region containing the input value x.
      The index takes on values from 0 to Nreg-1.
   -> double x
      Value to be quantized
   -> const float Xq[]
      Array of Nreg-1 quantizer decision levels which delimit the Nreg
      quantizer regions.  These levels must be in ascending order.
   -> int Nreg
      Number of quantizer regions

Author / revision:
  P. Kabal  Copyright (C) 2004
  $Revision: 1.4 $  $Date: 2004/07/11 15:56:48 $

-------------------------------------------------------------------------*/

#include <libtsp.h>


int
SPfQuantL (double x, const float Xq[], int Nreg)

{
  int iL, iU, i;
  float X;

/* Binary search for the interval [Xq[iL], Xq[iU]) which brackets x */
  X = (float) x;
  iL = 0;
  iU = Nreg;

  /* Isolate the interval */
  while (iU > iL + 1) {
    i = (iL + iU) / 2;
    if (X < Xq[i-1])
      iU = i;
    else
      iL = i;
  }

/* Return the index */
  return iL;
}
