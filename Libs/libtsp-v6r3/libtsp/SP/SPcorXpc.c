/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  double SPcorXpc (const float rxx[], float pc[], int Np)

Purpose:
  Find predictor coefficients from autocorrelation values

Description:
  This routine calculates the coefficients for a linear predictor which
  minimizes the resulting mean square error given the autocorrelation of the
  input signal.  Consider a linear predictor with Np coefficients,
            Np
    y(k) = SUM p(i) x(k-i) ,
           i=1
  where x(k) is the input signal.  The prediction error is

    e(k) = x(k) - y(k) .

  To minimize the mean-square prediction error, solve

    R p = r,

  where R is a symmetric positive definite covariance matrix, p is a vector
  of predictor coefficients and r is a vector of correlation values.  The
  matrix R and and vector r are defined as follows

    R(i,j) = E[x(k-i) x(k-j)],  for 1 <= i,j <= Np,
      r(i) = E[x(k) x(k-i)],    for 1 <= i <= Np.

  The resulting mean-square prediction error can be expressed as

    perr = Ex - 2 p'r + p'R p
         = Ex - p'r ,

  where Ex is the mean-square value of the input signal,

    Ex = E[x(k)^2].

  For this routine, the matrix R must be symmetric and Toeplitz.  Then

    R(i,j) = rxx(|i-j|)
      r(i) = rxx(i)

  If the correlation matrix is numerically not positive definite, or if the
  prediction error energy becomes negative at some stage in the calculation,
  the remaining predictor coefficients are set to zero.  This is equivalent to
  truncating the autocorrelation coefficient vector at the point at which it
  is positive definite.

  This subroutine solves for the predictor coefficients using
  Durbin's recursion.
  This algorithm requires
    Np    divides,
    Np*Np multiplies, and
    Np*Np adds.

  Predictor coefficients are usually expressed algebraically as vectors with
  1-offset indexing.  The correspondence to the 0-offset C-arrays is as
  follows.
    p(1) <==> pc[0]       predictor coefficient corresponding to lag 1
    p(i) <==> pc[i-1]     1 <= i < Np

Parameters:
  <-  double SPcorXpc
      Resultant mean-square prediction error
   -> const float rxx[]
      Np+1 element vector of autocorrelation values.  Element rxx[i] is the
      autocorrelation at lag i.
  <-  const float pc[]
      Np element vector of predictor coefficients.  Coefficient pc[i] is the
      predictor coefficient corresponding to lag i+1.
  -> const float rxx[]
      Np+1 element vector of autocorrelation values.  Element rxx[i] is the
      autocorrelation at lag i.
  ->  int Np
      Number of predictor coefficients

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.19 $  $Date: 2003/05/09 02:45:14 $

-------------------------------------------------------------------------*/

#include <libtsp.h>
#include <libtsp/SPmsg.h>


double
SPcorXpc (const float rxx[], float pc[], int Np)

{
  int i, j, k;
  float rc, tp;
  double perr, t, sum;

  perr = rxx[0];
  if (perr < 0.0)
    UThalt ("SPcorXpc: %s", SPM_NegEnergy);

  for (k = 0; k < Np; ++k) {

    /* Calculate the next reflection coefficient */
    sum = rxx[k+1];
    for (i = 0; i < k; ++i)
      sum = sum - rxx[k-i] * pc[i];

    /* Check for zero prediction error */
    if (perr == 0.0 && sum == 0.0)
      rc = 0.0F;
    else
      rc = (float) (-sum / perr);

/* Calculate the error (equivalent to perr = perr * (1-rc^2))
   A change in sign of perr means that rc (reflection coefficient for stage k)
   has a magnitude greater than unity (corresponding to an unstable synthesis
   filter)
*/
    t = perr + rc * sum;
    if (t < 0.0) {
      UTwarn ("SPcorXpc - %s", SPM_FixNegEnergy);
      for (i = k; i < Np; ++i)
        pc[i] = 0.0F;
      break;
    }
    perr = t;

    /* Convert from reflection coefficients to predictor coefficients */
    pc[k] = -rc;
    for (i = 0, j = k - 1; i < j; ++i, --j) {
      tp = pc[i] + rc * pc[j];
      pc[j] = pc[j] + rc * pc[i];
      pc[i] = tp;
    }
    if (i == j)
      pc[i] = pc[i] + rc * pc[i];
  }

  return perr;
}
