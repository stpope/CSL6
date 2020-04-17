/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  void FIfWinRCos (float win[], int N, int Nadv)

Purpose:
  Generate a raised-cosine window

Description:
  This routine generates a raised-cosine window that can be used for processing
  overlapped frames of data.  The window has sinusoidal transitions at each
  end.  The middle portion is flat.  The values returned do not include the
  end-points, i.e. all of the values returned are non-zero.  The example below
  shows a raised-cosine window with N=9 and Nadv=7.  Only the points marked
  with x's are returned.

                x x x x x
              x           x
            x               x
    o-o-o-o-------------------o-o-o-o
            0 1 2 3 4 5 6 7 8

  The parameter Nadv is the number of points by which the window must be
  advanced for the transition regions of two adjacent windows to add up to
  unity.  The total window length is N+2, but the two end points which are zero
  are not returned.  The parameters must satisfy
    Nadv-1 <= N <= 2*Nadv-1 .
  The lefthand equality gives a rectangular window (the window coefficients are
  all equal to one); the righthand equality gives a full raised-cosine window.
  For values between these extremes, the window has a constant portion with
  2*Nadv-N points.

Parameters:
  <-  float win[]
      Array containing the window values
   -> int N
      Number of window values
   -> int Nadv
      Window advance

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.13 $  $Date: 2003/05/09 01:29:45 $

-------------------------------------------------------------------------*/

#include <libtsp.h>
#include <libtsp/nucleus.h>
#include <libtsp/FImsg.h>


void
FIfWinRCos (float win[], int N, int Nadv)

{
  int i, k;
  double alpha, x;

  if (Nadv > N || N > 2 * Nadv - 1)
    UThalt ("FIwinRCos: %s", FIM_BadWAdv);

  alpha = ((double) (N - Nadv + 1)) / Nadv;

/* Step x from -b to +b, where b = (1+alpha)*(N-1)/(N+1) */
   for (i = 0, k = N-1; i <= k; ++i, --k) {
     x = (double) (2 * i - (N - 1)) / Nadv;
     win[i] = (float) FIrCosF (x, alpha);
     win[k] = win[i];
   }

   return;
}
