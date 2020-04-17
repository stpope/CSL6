/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  void VRfPow10 (const float x[], float y[], int N)

Purpose:
  Find 10 raised to the power of each element of an array

Description:
  This routine finds the anti-logarithm (base 10) for each element of an array.
  For each element in the input array, the output is given as
    y[i] = 10 ^ (x[i]),  0 <= i < N.

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
  $Revision: 1.8 $  $Date: 2003/05/09 03:33:10 $

-------------------------------------------------------------------------*/

#include <math.h>

#include <libtsp.h>


void
VRfPow10 (const float x[], float y[], int N)

{
  int i;

  for (i = 0; i < N; ++i)
    y[i] = (float) pow (10.0, (double) x[i]);

  return;
}
