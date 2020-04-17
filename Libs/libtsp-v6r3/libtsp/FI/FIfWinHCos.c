/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  void FIfWinHCos (float win[], double T1, double T2, double T3, double a)

Purpose:
  Generate a Hamming/cosine window

Description:
  A Hamming/cosine window is an asymmetric window, consisting of half of a
  Hamming window and a quarter cycle of a cosine,

  Consider the continuous-time variable t.  The half Hamming window starts at
  t=0, and reaches its peak value (unity) at t=T1.  The window is constant at
  unity from t=T1 to t=T2. The quarter cycle cosine window is unity at t=T2 and
  decreases to zero at t=T3.
         = 0 ,                            t  < 0
                          pi t
         = (1-a) - a cos (----) ,    0 <= t < T1
                           T1
    h(t) = 1 ,                      T1 <= t < T2
                pi (t-T2)
         = cos (---------) ,        T2 <= t < T3
                2 (T3-T2)
         = 0 ,                      T3 <= t

  A window of this form is used for the ITU-T G.729 8 kb/s coder and its
  derivatives.  The G.729 discrete-time window has 240 non-zero samples, and
  is obtained by setting a = 0.46, T1 = 199.5, T2 = 200, T3 = 239.75.  The
  discrete-time window is sampled at t = i,  with 0 <= i < 240.  The G.729
  window can be obtained with the following call
    FIwinHCos (win, 199.5, 200.0, 239.75, 0.46)

Parameters:
  <-  float win[]
      Array containing the window values
   -> double T1
      Length of the half Hamming window.
   -> double T2
      End point of the constant region, T2 >= T1.
   -> double T3
      End point of the quarter cycle cosine rollof, T3 >= T2.  The number of
      points in the window is floor (T3) + 1.
   -> double a
      Window parameter for the first part of the window; a=0.46 for a
      conventional Hamming window, a=0.5 for a full raised-cosine window, a=0
      for a rectangular window.  The window is non-negative for 0 <= a <= 0.5.

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.7 $  $Date: 2003/05/09 01:29:45 $

-------------------------------------------------------------------------*/

#include <math.h>

#include <libtsp.h>
#include <libtsp/FImsg.h>

#define PI	3.14159265358979323846


void
FIfWinHCos (float win[], double T1, double T2, double T3, double a)

{
  int i;
  double w1, w2;

  if (T1 < 0.0 || T2 < T1 || T3 < T2)
    UThalt ("FIwinHCos: %s", FIM_BadWPar);
  if (T1 > 0.0)
    w1 = PI / T1;
  else
    w1 = 0.0;
  if (T3 > T2)
    w2 = PI / (2.0 * (T3-T2));
  else
    w2 = 0.0;

  for (i = 0; i < T1; ++i)
    win[i] = (float) ((1.0 - a) - a * cos (w1 * i));
  for (; i < T2; ++i)
    win[i] = 1.0F;
  for (; i < T3; ++i)
    win[i] = (float) cos (w2 * (i - T2));

  return;
}
