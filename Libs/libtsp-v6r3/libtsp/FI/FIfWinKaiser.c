/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  void FIfWinKaiser (float win[], int N, double alpha)

Purpose:
  Generate a Kaiser window

Description:
  A Kaiser window is specified by the following equation in continuous time,
           0 ,                         x < -1

           I0(a sqrt(1 - x^2)
    w(x) = ------------------ ,  -1 <= x <= 1
                 I0(a)

           0                      1 <  x
  This window sits on a pedestal of height 1/I0(a).  The discrete-time window
  of length N is obtained by setting x = 2n/(N-1)-1, for 0 <= n < N.

  The parameter a (alpha) determines the shape of the window, with increasing
  a giving a larger mainlobe width.  For a=0, the window is a rectangular.  For
  a = 5.4414, the window has the same mainlobe width as a Hamming window.

  Reference:
  J. F. Kaiser, "Nonrecursive digital filter design using the I0-sinh window
  function", Proc. 1974 IEEE Int. Symp. on Circuits and Syst., pp. 20-23,
  April 1974.

Parameters:
  <-  float win[]
      Array containing the window values
   -> int N
      Number of window values
   -> double alpha
      Window parameter

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.11 $  $Date: 2003/05/09 01:29:45 $

-------------------------------------------------------------------------*/

#include <libtsp.h>
#include <libtsp/nucleus.h>


void
FIfWinKaiser (float win[], int N, double alpha)

{
  int i, k;

  for (i = 0, k = N-1; i <= k; ++i, --k) {
    win[i] = (float) FIxKaiser ((double) (2*i - (N-1)) / (N-1), alpha);
    win[k] = win[i];
  }

  return;
}
