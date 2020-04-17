/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  double VRfDiffSq (const float x1[], const float x2[], int N)

Purpose:
  Find the sum of squares of the differences for two float arrays

Description:
  This routine calculates the sum of the squares of the differences between
  corresponding elements of two float arrays,
         N-1                2
    e2 = SUM (x1[i] * x2[i]) .
         i=0

Parameters:
  <-  double VRfDiffSq
      Sum of the squares of the element differences
   -> const float x1[]
      Input array (N elements)
   -> const float x2[]
      Input array (N elements)
   -> int N
      Number of elements in the arrays (may be zero)

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.7 $  $Date: 2003/05/09 03:33:10 $

-------------------------------------------------------------------------*/

#include <libtsp.h>


double
VRfDiffSq (const float x1[], const float x2[], int N)

{
  int i;
  double d, e2;

  e2 = 0.0;
  for (i = 0; i < N; ++i) {
    d = (double) x1[i] - x2[i];
    e2 += d * d;
  }

  return e2;
}
