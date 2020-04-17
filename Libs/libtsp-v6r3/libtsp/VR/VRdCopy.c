/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  void VRfCopy (const double x[], double y[], int N)

Purpose:
  Copy an array of doubles

Description:
  This routine copys the elements from one double array to another,
    y[i] = x[i],  0 <= i < N.
  The copying operation procedes from lowest index to highest index.  For this
  reason, the input and output arrays can only have overlapping storage if the
  output array starts at a lower address than the input array.

Parameters:
   -> const double x[]
      Array of doubles (N elements)
  <-  double y[]
      Output array of doubles (N elements)
   -> int N
      Number of elements in each array

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.2 $  $Date: 2003/05/09 03:33:10 $

-------------------------------------------------------------------------*/

#include <libtsp.h>


void
VRdCopy (const double x[], double y[], int N)

{
  int i;

  for (i = 0; i < N; ++i)
    y[i] = x[i];

  return;
}
