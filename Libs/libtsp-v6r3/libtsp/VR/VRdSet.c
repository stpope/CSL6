/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  void VRdSet (double a, double y[], int N)

Purpose:
  Set each element in an array of doubles to a given value

Description:
  This routine sets each element in a given double vector to the value a,
    y[i] = a,  0 <= i < N.

Parameters:
   -> double a
      Value to be set
  <-  double y[]
      Array of doubles (N elements)
   -> int N
      Number of elements

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.2 $  $Date: 2003/05/09 03:33:10 $

-------------------------------------------------------------------------*/

#include <libtsp.h>


void
VRdSet (double a, double y[], int N)

{
  int i;

  for (i = 0; i < N; ++i)
    y[i] = a;

  return;
}
