/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  void VRdZero (double y[], int N)

Purpose:
  Zero an array of doubles

Description:
  This routine zeros N elements of a given double array,
    y[i] = 0.0,  0 <= i < N.

Parameters:
  <-  double y[]
      Array of doubles (N elements)
   -> int N
      Number of elements

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.7 $  $Date: 2003/05/09 03:33:10 $

-------------------------------------------------------------------------*/

#include <libtsp.h>


void
VRdZero (double y[], int N)

{
  int i;

  for (i = 0; i < N; ++i)
    y[i] = 0.0;

  return;
}
