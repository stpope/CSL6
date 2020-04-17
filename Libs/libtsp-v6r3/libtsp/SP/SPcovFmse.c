/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  double SPcovFmse (const float h[], double Ed, const float *R[],
                    const float r[], int N)

Purpose:
  Find the mean-square error for a filter (covariance specified)

Description:
  This function calculates the mean-square error for a linear estimation
  problem.  Consider a filter with N coefficients, with coefficient h(i)
  corresponding to lag D(i).  The filter output is
           N-1
    y(k) = SUM h(i) x(k-D(i)) ,
           i=0
  where x(i) is the input signal.  The filter error is

    e(k) = d(k) - y(k) ,

  where d(k) is the desired signal.  The mean-square filtering error can be
  expressed as

    ferr = Ed - 2 h'r + h'R h .

  The mean-square value Ed, matrix R and vector p are defined as follows

        Ed = E[d(k)^2]
    R(i,j) = E[x(k-D(i)) x(k-D(j))],  for 0 <= i,j < N,
      r(i) = E[d(k) x(k-D(i))],       for 0 <= i < N.

  The expectation operator E[.] is often replaced by a sum over k over a finite
  interval.

Parameters:
  <-  double SPcovFmse
      Resultant filter error energy
   -> const float h[]
      N element vector of filter coefficients.  Coefficient h[i] is the filter
      coefficient corresponding to lag N1+i.
   -> double Ed
      Signal energy for the desired signal
   -> const float *R[]
      R is an array of pointers to the rows of an N by N positive definite
      correlation matrix.  Only the lower triangular portion of R is accessed.
      Note that with ANSI C, if the actual parameter is not declared to have
      the const attribute, an explicit cast to (const float **) is required.      -> const float r[]
      N element vector of cross-correlation values
   -> int N
      Number of filter coefficients

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.5 $  $Date: 2003/05/09 02:45:14 $

-------------------------------------------------------------------------*/

#include <libtsp.h>


double
SPcovFmse (const float h[], double Ed, const float *R[], const float r[],
	   int N)

{
  double ferr;

  ferr = Ed - 2.0 * VRfDotProd (r, h, N) + MAfSyQuad (R, h, N);

  return ferr;
}
