/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  void VRfZero (float y[], int N)

Purpose:
  Zero an array of floats

Description:
  This routine zeros N elements of a given float array,
    y[i] = 0.0,  0 <= i < N.

Parameters:
  <-  float y[]
      Array of floats (N elements)
   -> int N
      Number of elements

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.7 $  $Date: 2003/05/09 03:33:11 $

-------------------------------------------------------------------------*/

#include <libtsp.h>


void
VRfZero (float y[], int N)

{
  int i;

  for (i = 0; i < N; ++i)
    y[i] = 0.0;

  return;
}
