/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  double SPcovXpc (const float *Cov[], float pc[], int Np)

Purpose:
  Find predictor coefficients using the covariance method

Description:
  This procedure finds the predictor coefficients for a linear predictor which
  minimizes the mean-square error using the covariance method.  Note that the
  resulting predictor coefficients may give a non-minimum phase prediction
  error filter.

  Consider a linear predictor with Np coefficients,
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
    R(i,j) = Cov(i,j) = E[x(k-i) x(k-j)],  for 1 <= i,j <= Np,
      r(i) = Cov(0,i) = E[x(k) x(k-i)],    for 1 <= i <= Np.

  The solution is found using a Cholesky decomposition of the matrix R.  The
  resulting mean-square prediction error can be expressed as

    perr = Ex - 2 p'r + p'R p
         = E0 - p'r ,

  where Ex is the mean-square value of the input signal,

    Ex = Cov(0,0) = E[x(k)^2].

  The expectation operator E[.] is often replaced by a sum over k over a finite
  interval.  Minimization of the prediction error over this interval defines
  the so-called covariance method for determining the linear prediction
  coefficients.

  If the coefficient matrix is numerically not positive definite, or if the
  prediction error energy becomes negative at some stage in the calculation,
  the remaining predictor coefficients are set to zero.  This is equivalent to
  truncating the coefficient matrix at the point at which it is positive
  definite.

  Predictor coefficients are usually expressed algebraically as vectors with
  1-offset indexing.  The correspondence to the 0-offset C-arrays is as
  follows.
    p(1) <==> pc[0]       predictor coefficient corresponding to lag 1
    p(i) <==> pc[i-1]     1 <= i < Np

Parameters:
  <-  double SPcovXpc
      Resultant prediction error energy
   -> const float *Cov[]
      Cov is an array of pointers to the rows of an Np+1 by Np+1 symmetric
      positive definite correlation matrix.  Only the lower triangular portion
      of Cov is accessed.  Note that with ANSI C, if the actual parameter
      is not declared to have the const attribute, an explicit cast to
      (const float **) is required.
  <-  float pc[]
      Vector of predictor coefficients (Np values).  These are the coefficients
      of the predictor filter, with pc[0] being the predictor coefficient
      corresponding to lag 1, and pc[Np-1] corresponding to lag Np.
   -> int Np
      Number of predictor coefficients

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.9 $  $Date: 2003/05/09 02:45:14 $

-------------------------------------------------------------------------*/

#include <libtsp.h>


double
SPcovXpc (const float *Cov[], float pc[], int Np)

{
  double perr;
  int i;
  float **R;

/* Allocate space for the submatrix pointers C */
  R = MAfSubMat ((const float **) Cov, 1, 1, Np);

/* Copy the first column of Cov to pc (vector r).  This copying is necessary to
   restrict access to the lower triangular part of Cov.
*/
  for (i = 0; i < Np; ++i)
    pc[i] = Cov[i+1][0];

/* Solve R pc = r */
  perr = SPcovFilt (Cov[0][0], (const float **) R, pc, pc, Np);

  UTfree ((void *) R);
  return perr;
}
