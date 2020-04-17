/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  double SPpcXrc (const float pc[], float rc[], int Np)

Purpose:
  Convert predictor coefficients to reflection coefficients

Description:
  The given predictor coefficients are converted to reflection coefficients
  using a recursive algorithm.  The reflection coefficients lie between -1 and
  +1 if the corresponding synthesis filter is stable.  The procedure implements
  the following step-down recursion to get the reflection coefficients k(j)
  from the initial set of predictor coefficients.  The iteration index runs
  from Np to 1.  For each order j, the step-down procedure finds the predictor
  coefficients for a predictor of order j-1  from the predictor coefficients
  for a predictor of order j.  The equations are as follows, where the primed
  primed quantities indicate the j-1 term predictor obtained at iteration j.

    k(j) = -p(j)

            p(i) - k(j) p(j-i)
    p'(i) = ------------------ ,   0 < i < j,
              1 - k(j) k(j)

  Reflection coefficients and predictor coefficients are usually expressed
  algebraically as vectors with 1-offset indexing.  The correspondence to the
  0-offset C-arrays is as follows.
    p(1) <==> pc[0]       predictor coefficient corresponding to lag 1
    p(i) <==> pc[i-1]     1 <= i < Np
    k(1) <==> rc[0]       first reflection coefficient
    k(i) <==> rc[i-1]     1 <= i < Np

Parameters:
  <-  double SPpcXrc
      Normalized mean-square prediction error.  This is the energy of the
      prediction residual for a case in which the given predictor coefficients
      are matched to the signal.  Note that this value may be negative if the
      given predictor coefficients do not correspond to a minimum phase
      prediction error filter.
   -> const float pc[]
      Vector of predictor coefficients (Np values).  These are the coefficients
      of the predictor filter, with pc[0] being the predictor coefficient
      corresponding to lag 1, and pc[Np-1] corresponding to lag Np.
  <-  float rc[]
      Vector of Np reflection coefficients.  The sign of these coefficients is
      such that rc[Np-1] = -pc[Np-1].
   -> int Np
      Number of coefficients

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.15 $  $Date: 2003/05/09 02:45:04 $

-------------------------------------------------------------------------*/

#include <libtsp.h>
#include <libtsp/SPmsg.h>


double
SPpcXrc (const float pc[], float rc[], int Np)

{
  int minphase;
  double perr, E;
  float tp, e;
  float *Pc;
  int i, j, k;

/*
   The iterative procedure generates the optimal predictors of order
   Np, Np-1, ... , 1.  In order to preserve the input vector, the predictor
   is copied into the output array to be subsequently altered.
*/
  Pc = rc;
  for (i = 0; i < Np; ++i)
    Pc[i] = pc[i];

/* Main iteration loop */
  minphase = 1;
  perr = 1.0;
  for (k = Np - 1; k >= 0; --k) {

    rc[k] = -Pc[k];
    E = 1.0 - rc[k] * rc[k];
    perr = perr * E;
    if (perr <= 0.0)
      minphase = 0;
    e = (float) E;
    for (i = 0, j = k - 1; i < j; ++i, --j) {
      tp = (Pc[i] - rc[k] * Pc[j]) / e;
      Pc[j] = (Pc[j] - rc[k] * Pc[i]) / e;
      Pc[i] = tp;
    }
    if (i == j)
      Pc[i] = (Pc[i] - rc[k] * Pc[i]) / e;
  }
  if (! minphase)
    UTwarn ("SPpcXrc - %s", SPM_NonMinPhase);

  return perr;
}
