/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  void VRfLog10 (const float x[], float y[], int N)

Purpose:
  Find the logarithm (base 10) of each element of an array

Description:
  This routine finds the logarithm (base 10) for each element of an array.
    y[i] = log10(x[i]),  0 <= i < N.

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
  $Revision: 1.9 $  $Date: 2003/05/09 03:33:10 $

-------------------------------------------------------------------------*/

#include <math.h>

#include <libtsp.h>
#include <libtsp/VRmsg.h>


void
VRfLog10 (const float x[], float y[], int N)

{
  int i;

  for (i = 0; i < N; ++i) {
    if (x[i] >= 0.0F)
      y[i] = (float) log10 ((double) x[i]);
    else
      UThalt ("VRfLog10: %s", VRM_NegVal);
  }

  return;
}
