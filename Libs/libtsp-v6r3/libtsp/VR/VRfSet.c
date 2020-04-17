/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  void VRfSet (double a, float y[], int N)

Purpose:
  Set each element in an array of floats to a given value

Description:
  This routine sets each element in a given float vector to the value a,
    y[i] = a,  0 <= i < N.

Parameters:
   -> double a
      Value to be set
  <-  float y[]
      Array of floats (N elements)
   -> int N
      Number of elements

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.6 $  $Date: 2003/05/09 03:33:10 $

-------------------------------------------------------------------------*/

#include <libtsp.h>


void
VRfSet (double a, float y[], int N)

{
  int i;

  for (i = 0; i < N; ++i)
    y[i] = (float) a;

  return;
}
