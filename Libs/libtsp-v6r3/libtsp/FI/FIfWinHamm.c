/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  void FIfWinHamm (float win[], int N, double a)

Purpose:
  Generate a generalized Hamming window

Description:
  A generalized Hamming window is specified by the following equation,
                            2 pi i
    win[i] = (1-a) - a cos (------) , i = 0, ..., N-1
                              N-1

  The parameter a is 0.46 for a conventional Hamming window, 0.5 for a full
  raised-cosine window, and 0 for a rectangular window.  Note that for the
  full raised-cosine window, the two end points of the window are zero.

  Define the effective window length as the length of a rectangular window
  which has the same energy as the Hamming window.  Then the effective length
  of the Hamming window is
    L = N - 2a(N+1) + a^2 (3N+5)/2, for N > 3.
        2 - 8a + 8a^2,              for N = 2
        1                           for N = 1 (win[0] = 1)

Parameters:
  <-  float win[]
      Array containing the window values
   -> int N
      Number of window values
   -> double a
      Window parameter; a=0.46 for a conventional Hamming window, a=0.5 for a
      full raised-cosine window, a=0 for a rectangular window.  The window is
      non-negative for 0 <= a <= 0.5.

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.19 $  $Date: 2003/05/09 01:29:45 $

-------------------------------------------------------------------------*/

#include <math.h>

#include <libtsp.h>

#define PI	3.14159265358979323846


void
FIfWinHamm (float win[], int N, double a)

{
  int i, k;
  double w;

  if (N > 1)
    w = PI / (N - 1);
  else
    w = 0.0;

  for (i = 0, k = N-1; i <= k; ++i, --k) {
    win[i] = (float)((1.0-a) + a * cos (w*(2*i - (N-1))));
    win[k] = win[i];
  }

  return;
}
