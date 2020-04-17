/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  void VRdSymPart (int Sym, const double x[], double y[], int N)

Purpose:
  Find the symmetric or anti-symmetric part of a vector

Description:
  This routine creates an output vector which is the symmetric or
  anti-symmetric part of the input vector,
    y[i] = 0.5 (x[i] +/- x[N-i-1]),  0 <= i < N.

Parameters:
   -> int Sym
      Symmetry flag, +1 or symmetric and -1 for anti-symmetric
   -> const double x[]
      Input array (N elements)
  <-  double y[]
      Output array (N elements).  The output array can be the same as the
      input array.
   -> int N
      Number of elements in the arrays (may be zero).

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.3 $  $Date: 2003/05/09 03:33:10 $

-------------------------------------------------------------------------*/

#include <libtsp.h>


void
VRdSymPart (int Sym, const double x[], double y[], int N)

{ 
  int i, j;

  if (Sym >= 0) {
    for (i = 0, j = N-1; i < j; ++i, --j) {
      y[i] = 0.5 * (x[i] + x[j]);
      y[j] = y[i];
    }
    if (i == j)
      y[i] = x[i];
  }
  else {
    for (i = 0, j = N-1; i < j; ++i, --j) {
      y[i] = 0.5 * (x[i] - x[j]);
      y[j] = -y[i];
    }
    if (i == j)
      y[i] = 0.0;
  }

  return;
}
