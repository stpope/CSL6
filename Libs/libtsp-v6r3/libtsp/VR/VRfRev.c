/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  void VRfRev (const float x[], float y[], int N)

Purpose:
  Reverse the order of the elements of an array

Description:
  This routine reverses the order of an array,
    y[i] = x[N-i-1],  for i from 0 to N-1.

Parameters:
   -> const float x[]
      Input array (N elements)
  <-  float y[]
      Output array (N elements).  The output array can be the same as the input
      array.
   -> int N
      Number of elements in the arrays (may be zero)

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.5 $  $Date: 2003/05/09 03:33:10 $

-------------------------------------------------------------------------*/

#include <libtsp.h>


void
VRfRev (const float x[], float y[], int N)

{
  float temp;
  int i, k;

  for (i = 0, k = N-1; i <= k; ++i, --k) {
    temp = x[k];
    y[k] = x[i];
    y[i] = temp;
  }

  return;
}
