/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  double SPrcXpc (const float rc[], float pc[], int Np)

Purpose:
  Convert reflection coefficients to predictor coefficients

Description:
  This routine generates the vector of predictor coefficients corresponding
  to the given set of reflection coefficients.  The relationship between the
  reflection coefficients, k(i), and the predictor coefficients p(i) is
  determined from the following update equations.  The predictor coefficients
  at iteration j+1 (primed quantities) are determined from reflection
  coefficient j and the predictor coefficients at iteration j.

    p'(j) = -k(j)
    p'(i) = p(i) + k(j) p(j-i) ,   1 < i < j

  This algorithm requires
    Np*(Np-1)/2 multiplies and
    Np*(Np-1)/2 adds.

  Reflection coefficients and predictor coefficients are usually expressed
  algebraically as vectors with 1-offset indexing.  The correspondence to the
  0-offset C-arrays is as follows.
    k(1) <==> rc[0]       first reflection coefficient
    k(i) <==> rc[i-1]     1 <= i < Np
    p(1) <==> pc[0]       predictor coefficient corresponding to lag 1
    p(i) <==> pc[i-1]     1 <= i < Np

Parameters:
  <-  double SPrcXpc
      Normalized mean-square prediction error.  This is the energy of the
      prediction residual for a case in which the given reflection coefficients
      are matched to the signal.  Note that this value may be negative if the
      given reflection coefficients exceed unity in magnitude.
   -> const float rc[]
      Vector of Np reflection coefficients.  The sign of these coefficients is
      assumed to be such that rc[Np-1] = -pc[Np-1].
  <-  float pc[]
      Vector of predictor coefficients (Np values).  These are the coefficients
      of the predictor filter, with pc[0] being the predictor coefficient
      corresponding to lag 1, and pc[Np-1] corresponding to lag Np.
   -> int  Np
      Number of coefficients

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.16 $  $Date: 2003/05/09 02:45:04 $

-------------------------------------------------------------------------*/

#include <libtsp.h>
#include <libtsp/SPmsg.h>


double
SPrcXpc (const float rc[], float pc[], int Np)

{
  int i, j, k;
  int minphase;
  float tp;
  double perr;

  minphase = 1;
  perr = 1.0;
  for (k = 0; k < Np; ++k) {
    perr = perr * (1.0 - rc[k] * rc[k]);
    if (perr <= 0.0)
      minphase = 0;
    pc[k] = -rc[k];
    for (i = 0, j = k - 1; i < j; ++i, --j) {
      tp = pc[i] + rc[k] * pc[j];
      pc[j] = pc[j] + rc[k] * pc[i];
      pc[i] = tp;
    }
    if (i == j)
      pc[i] = pc[i] + rc[k] * pc[i];
  }
  if (! minphase)
    UTwarn ("SPrcXpc - %s", SPM_NonMinPhase);

  return perr;
}
