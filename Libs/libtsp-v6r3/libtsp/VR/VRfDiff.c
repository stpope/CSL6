/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  void VRfDiff (const float x1[], const float x2[], float y[], int N)

Purpose:
  Element-by-element difference of two float arrays

Description:
  This routine calculates the difference of corresponding elements of two float
  arrays,
    y[i] = x1[i] - x2[i],  0 <= i < N.

Parameters:
   -> const float x1[]
      Input array (N elements)
   -> const float x2[]
      Input array (N elements)
  <-  float y[]
      Output array (N elements).  The output array can be the same as either
      of the input arrays.
   -> int N
      Number of elements in the arrays (may be zero)

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.2 $  $Date: 2003/05/09 03:33:10 $

-------------------------------------------------------------------------*/

#include <libtsp.h>


void
VRfDiff (const float x1[], const float x2[], float y[], int N)

{
  int i;

  for (i = 0; i < N; ++i)
    y[i] = x1[i] - x2[i];

  return;
}
