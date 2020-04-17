/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  double SPcovPmse (const float pc[], const float *Cov[], int Np)

Purpose:
  Calculate the mean-square prediction error (covariance specified)

Description:
  This function calculates the squared prediction error for a linear predictor.
  Consider a linear predictor with Np coefficients,
            Np
    y(k) = SUM p(i) x(k-i) ,
           i=1
  where x(i) is the input signal.  The prediction error is

    e(k) = x(k) - y(k) .

  The mean-square prediction error is E[e(k)^2] or in vector-matrix notation

    E = Ex - 2 p'r + p'R p .

  The mean-square value Ex, matrix R and vector p are defined as follows
        Ex = Cov(0,0) = E[x(k)^2]
    R(i,j) = Cov(i,j) = E[x(k-i) x(k-j)],  for 1 <= i,j <= Np,
      r(i) = Cov(0,i) = E[x(k) x(k-i)],    for 1 <= i <= Np.

  The expectation operator E[.] is often replaced by a sum over k over a finite
  interval.

  Predictor coefficients are usually expressed algebraically as vectors with
  1-offset indexing.  The correspondence to the 0-offset C-arrays is as
  follows.
    p(1) <==> pc[0]       predictor coefficient corresponding to lag 1
    p(i) <==> pc[i-1]     1 <= i < Np

Parameters:
  <-  double SPcovPmse
      Resulting filter error energy
   -> const float pc[]
      Np element vector of predictor coefficients.  Coefficient pc[i] is the
      predictor coefficient corresponding to lag i+1.
   -> const float *Cov[]
      Cov is an array of pointers to the rows of an Np+1 by Np+1 symmetric
      positive definite correlation matrix.  Only the lower triangular portion
      of Cov is accessed.  Note that with ANSI C, if the actual parameter
      is not declared to have the const attribute, an explicit cast to
      (const float **) is required.
   -> int Np
      Number of predictor coefficients

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.7 $  $Date: 2003/05/09 02:45:14 $

-------------------------------------------------------------------------*/

#include <libtsp.h>


double
SPcovPmse (const float pc[], const float *Cov[], int Np)

{
  double perr;
  float *ec;

/* Evaluate the error as a'R a, where a is a vector of error filter coeffs */
  ec = (float *) UTmalloc ((Np+1) * sizeof (float));
  SPpcXec (pc, ec, Np);

/* Calculate the error */
  perr = MAfSyQuad (Cov, ec, Np+1);

  UTfree ((void *) ec);

  return perr;
}
