/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  void VRdLinInc (double xmin, double xmax, double x[], int N)

Purpose:
  Generate a linearly spaced array of values

Description:
  This routine generates a linearly spaced array of values
    y[i] = xmin + i*(xmax-xmin)/(N-1),  0 <= i < N.

Parameters:
   -> double xmin
      Minimum value
   -> double xmax
  <-  double x[]
      Array of doubles (N elements)
   -> int N
      Number of elements

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.3 $  $Date: 2003/05/09 03:35:46 $

-------------------------------------------------------------------------*/

#include <libtsp.h>


void
VRdLinInc (double xmin, double xmax, double x[], int N)

{
  int i;
  double dx;

  /* Make sure output includes xmin and xmax, exactly */
  dx = 0.0;
  if (N > 0) {
    x[0] = xmin;
    dx = (xmax - xmin) / (N-1);
  }
  for (i = 1; i < N-1; ++i)
    x[i] = xmin + i * dx;

  if (N > 1)
    x[N-1] = xmax;

  return;
}
