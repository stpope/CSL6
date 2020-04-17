/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  void VRfCopy (const float x[], float y[], int N)

Purpose:
  Copy an array of floats

Description:
  This routine copys the elements from one float array to another,
    y[i] = x[i],  0 <= i < N.
  The copying operation procedes from lowest index to highest index.  For this
  reason, the input and output arrays can only have overlapping storage if the
  output array starts at a lower address than the input array.

Parameters:
   -> const float x[]
      Array of floats (N elements)
  <-  float y[]
      Output array of floats (N elements)
   -> int N
      Number of elements in each array

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.6 $  $Date: 2003/05/09 03:33:10 $

-------------------------------------------------------------------------*/

#include <libtsp.h>


void
VRfCopy (const float x[], float y[], int N)

{
  int i;

  for (i = 0; i < N; ++i)
    y[i] = x[i];

  return;
}
