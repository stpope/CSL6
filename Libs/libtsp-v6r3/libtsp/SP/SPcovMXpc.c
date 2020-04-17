/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  double SPcovMXpc (const float *Cov[], float pc[], int Np)

Purpose:
  Find predictor coefficients using the modified covariance method

Description:
  This procedure finds the predictor coefficients for a linear predictor using
  the modified covariance method which guarantees a minimum phase prediction
  error filter.

  Consider a linear predictor with Np coefficients,
            Np
    y(k) = SUM p(i) x(k-i) ,
           i=1
  where x(i) is the input signal.  This procedure starts by solving for
  predictors of increasing order as in the case of the standard covariance
  method.  The modified covariance procedure calculates the ratio of the error
  signal energies for predictors of successive orders.  In the autocorrelation
  method the residual error energy for an n'th order predictor is given by
                  n         2
    Perr(n) = Ex PROD [1 - k (i)],
                 i=0
  where k(i) is a reflection coefficient.  In the modified covariance method,
  the ratios of error energies for the covariance method solution are used to
  determine the reflection coefficients.  The sign of k(i) is determined from
  the sign of the i'th predictor coefficient.  The covariance method predictor
  coefficients are discarded; instead a set of predictor coefficients is
  derived from the reflection coefficients.  Since the error energy decreases
  with predictor order, the reflection coefficients are less than unity in
  magnitude.  This also means that the predictor coefficients derived from them
  correspond to a minimum phase prediction error filter.

  The equations for the standard covariance method are

    R p = r,

  where R is a symmetric positive definite covariance matrix, p is a vector
  of filter coefficients and r is a vector of correlation values.  The matrix
  R and and vector r are defined as follows

    R(i,j) = Cov(i,j) = E[x(k-i) x(k-j)],  for 1 <= i,j <= N,
      r(i) = Cov(0,i) = E[x(k) x(k-i)],    for 1 <= i <= N.

  The solution is found using a Cholesky decomposition of the matrix R.  The
  resulting mean-square prediction error for the standard covariance method can
  be expressed as

    perr = Ex - 2 p'r + p'R p
         = Ex - p'r ,

  where Ex is the mean-square value of the input signal,

    Ex = Cov(0,0) = E[x(k)^2].

  If the coefficient matrix is numerically not positive definite, or if the
  prediction error energy becomes negative at some stage in the calculation,
  the remaining predictor coefficients are set to zero.

  The expectation operator E[.] is often replaced by a sum over k over a finite
  interval.  Minimization of the prediction error over this interval defines
  the so-called covariance method for determining the linear prediction
  coefficients.

  Predictor coefficients are usually expressed algebraically as vectors with
  1-offset indexing.  The correspondence to the 0-offset C-arrays is as
  follows.
    p(1) <==> pc[0]       predictor coefficient corresponding to lag 1
    p(i) <==> pc[i-1]     1 <= i < Np

Parameters:
  <-  double SPcovXpc
      Resulting prediction error energy.  This is the prediction error for the
      coefficients returned by this routine.
   -> const float *Cov[]
      Cov is an array of pointers to the rows of an Np+1 by Np+1 symmetric
      positive definite correlation matrix.  Only the lower triangular portion
      of Cov is accessed.  Note that with ANSI C, if the actual parameter
      is not declared to have the const attribute, an explicit cast to
      (const float **) is required.
  <-  const float pc[]
      Np element vector of predictor coefficients.  Coefficient pc[i] is the
      predictor coefficient corresponding to lag i+1.
   -> int Np
      Number of predictor coefficients

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.10 $  $Date: 2003/05/09 02:45:14 $

-------------------------------------------------------------------------*/

#include <math.h>

#include <libtsp.h>
#include <libtsp/nucleus.h>
#include <libtsp/SPmsg.h>


double
SPcovMXpc (const float *Cov[], float pc[], int Np)

{
  double perr, y2;
  int i, ier;
  int Na;
  float *r, *rc, *y;
  float **R, **L;

/* Allocate space */
  R = MAfSubMat (Cov, 1, 1, Np);
  L = MAfAllocMat (Np, Np);
  r = (float *) UTmalloc (3*Np * sizeof (float));
  rc = r + Np;
  y = rc + Np;

/* Solve the covariance equations using Cholesky decompostion */
  ier = MAfChFactor ((const float **) R, L, Np);
  Na = Np;
  if (ier != 0) {
    Na = ier - 1;
    UTwarn ("SPcovMXpc - %s: %d", SPM_RedOrder, Na);
    VRfZero (&rc[Na], Np-Na);
  }

/* Copy the first column of Cov to pc (vector r).  This copying is necessary to
   restrict access to the lower triangular part of Cov.
*/
  for (i = 0; i < Na; ++i)
    r[i] = Cov[i+1][0];

/* Solve L y = r; the solution appears in y */
  MAfLTSolve ((const float **) L, r, y, Na);

/*
  The prediction error is
    perr = E0 - p'r
         = E0 - p'L y   (L y = r)
         = E0 - y'y     (p'L = y')
  Note that as the order of the prediction is increased, y grows in length by
  adding additional values to the end of the vector.
                  n         2            n-1  2
    perr(n) = E0 PROD (1 - k (i)] = E0 - SUM y (i) .
                 i=1                     i=0
 Then
                   y(n)
    k(n) = - --------------- , with sign chosen opposite to that for p(n).
             sqrt[perr(n-1)]
*/
  perr = Cov[0][0];
  if (perr < 0.0)
    UThalt ("SPcovMXpc: %s", SPM_NegEnergy);
  for (i = 0; i < Na; ++i) {
    y2 = (double) y[i] * y[i];
    if (y2 > perr) {
      UTwarn ("SPcovMXpc - %s", SPM_FixNegEnergy);
      VRfZero (&rc[i], Na-i);
      break;
    }
    else if (perr == 0.0)
      rc[i] = 0.0F;
    else {
      rc[i] = (float) (-y[i] / sqrt(perr));
      perr = perr - y2;
    }
  }

/* Convert to predictor coefficients */
  SPrcXpc (rc, pc, Np);

/* Deallocate storage */
  UTfree ((void *) R);
  MAfFreeMat (L);
  UTfree ((void *) r);

/* Mean-square error */
  perr = SPcovPmse (pc, Cov, Np);

  return perr;
}
