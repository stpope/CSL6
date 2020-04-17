/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  double VRfSum (const float x[], int N)

Purpose:
  Calculate the sum of elements in an array of floats

Description:
  This routine calculates the sum of the elements in an array of floats,
        N-1
    s = SUM x[i] .
        i=0

Parameters:
  <-  double VRfSum
      Sum of the elements
   -> const float x[]
      Input array (N elements)
   -> int N
      Number of elements in the array (may be zero)

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.5 $  $Date: 2003/05/09 03:33:11 $

-------------------------------------------------------------------------*/

#include <libtsp.h>


double
VRfSum (const float x[], int N)

{
  int i;
  double sum;

  sum = 0.0;
  for (i = 0; i < N; ++i)
    sum += x[i];

  return sum;
}
