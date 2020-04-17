/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  void VRiZero (int y[], int N)

Purpose:
  Zero an array of ints

Description:
  This routine zeros N elements of a given int array,
    y[i] = 0,  0 <= i < N.

Parameters:
  <-  int y[]
      Array of ints (N elements)
   -> int N
      Number of elements

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.7 $  $Date: 2003/05/09 03:33:11 $

-------------------------------------------------------------------------*/

#include <libtsp.h>


void
VRiZero (int y[], int N)

{
  int i;

  for (i = 0; i < N; ++i)
    y[i] = 0;

  return;
}
