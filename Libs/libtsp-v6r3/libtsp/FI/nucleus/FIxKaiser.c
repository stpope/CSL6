/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  double FIxKaiser (double x, double alpha)

Purpose:
  Generate a sample from a Kaiser window

Description:
  A Kaiser window is specified by the following equation in continuous time,
           0 ,                         x < -1

           I0(a sqrt(1 - x^2)
    w(x) = ------------------ ,  -1 <= x <= 1
                 I0(a)

           0                      1 <  x
  This window sits on a pedestal of height 1/I0(a).  The discrete-time window
  of length N is obtained by setting t = 2n/(N-1) - 1, for 0 <= n < N.

  The parameter a (alpha) determines the shape of the window, with increasing
  a giving a larger mainlobe width.  For a=0, the window is a rectangular.  For
  a = 5.4414, the window has the same mainlobe width as a Hamming window.

  Reference:
  J. F. Kaiser, "Nonrecursive digital filter design using the I0-sinh window
  function", Proc. 1974 IEEE Int. Symp. on Circuits and Syst., pp. 20-23,
  April 1974.

Parameters:
  <-  doulbe FIxKaiser
      Sample from the Kaiser window
   -> double x
      Normalized sample point
   -> double alpha
      Window parameter

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.4 $  $Date: 2003/05/09 01:30:46 $

-------------------------------------------------------------------------*/

#include <math.h>

#include <libtsp.h>
#include <libtsp/nucleus.h>


double
FIxKaiser (double x, double alpha)

{
  static double alpha_prev = 0.0;
  static double Ia = 1.0;
  double beta, win;

  /* Avoid recalculating the denominator Bessel function */
  if (alpha != alpha_prev) {
    Ia = FNbessI0 (alpha);
    alpha_prev = alpha;
  }

  if (x < -1.0 || x > 1.0)
    win = 0.0;
  else {
    beta = alpha * sqrt (1.0 - x * x);
    win = FNbessI0 (beta) / Ia;
  }

  return win;
}
