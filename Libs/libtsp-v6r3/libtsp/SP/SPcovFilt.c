/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  double SPcovFilt (double Ed, const float *R[], const float r[], float h[],
                    int N)

Purpose:
  Find filter coefficients using the covariance approach

Description:
  This procedure finds the filter coefficients for a linear filter which
  minimizes the mean-square error.  Consider a filter with N coefficients,
  with coefficient h(i) corresponding to lag D(i).  The filter output is
           N-1
    y(k) = SUM h(i) x(k-D(i)) ,
           i=0
  where x(i) is the input signal.  The filter error is

    e(k) = d(k) - y(k) ,

  where d(k) is the desired signal.  To minimize the mean-square filtering
  error, solve

    R h = r,

  where R is the symmetric positive definite covariance matrix, h is the vector
  of filter coefficients and r is a vector of correlation values.  The matrix
  R and and vector r are defined as follows
    R(i,j) = E[x(k-D(i)) x(k-D(j))],  for 0 <= i,j < N,
      r(i) = E[x(k) d(k-D(i))],       for 0 <= i < N.

  The solution is determined using a Cholesky decomposition of the matrix R.
  The resulting mean-square filtering error can be expressed as

    ferr = Ed - 2 h'r + h'R h
         = Ed - h'r ,

  where Ed is the mean-square value of the desired signal,

    Ed = E[d(k)^2] .

  The expectation operator E[.] is often replaced by a sum over k over an
  interval, say 0 <= k < Nk.  Minimization of the prediction error over this
  finite interval defines the so-called covariance method for determining the
  filter coefficients.  If the filter delays are of the form D(i) = Nd + i,
             Nk-1
    R(i,j) = SUM x(k-i-Nd) x(k-j-Nd) .
             k=0
  Furthermore, if the desired signal is x(k), the cross-correlation vector r
  becomes
           Nk-1
    r(i) = SUM x(k) x(k-i-Nd) .
           k=0

  If the coefficient matrix is numerically not positive definite, or if the
  error energy becomes negative at some stage in the calculation, the remaining
  filter coefficients are set to zero.

Parameters:
  <-  double SPcovFilt
      Resultant filter mean-square error
   -> double Ed
      Signal energy for the desired signal.  This value is used only for the
      computation of the mean-square error.
   -> const float *R[]
      R is an array of pointers to the rows of an N by N positive definite
      correlation matrix.  Only the lower triangular portion of R is accessed.
      Note that with ANSI C, if the actual parameter is not declared to have
      the const attribute, an explicit cast to (const float **) is required.
   -> const float r[]
      Vector of cross-correlations (N elements).  The vector r can be the same
      vector as h.
  <-  float h[]
      Vector of filter coefficients (N values)
   -> int N
      Number of filter coefficients

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.12 $  $Date: 2003/05/09 02:45:14 $

-------------------------------------------------------------------------*/

#include <libtsp.h>
#include <libtsp/nucleus.h>
#include <libtsp/SPmsg.h>


double
SPcovFilt (double Ed, const float *R[], const float r[], float h[], int N)

{
  double ferr, y2;
  int i, ier;
  int Na;
  float *y;
  float **L;

/* Allocate space for the lower triangular matrix L and the vector y */
  L = MAfAllocMat (N, N);
  y = (float *) UTmalloc (N * sizeof (float));

/* Solve the covariance equations using Cholesky decompostion */
  ier = MAfChFactor (R, L, N);
  Na = N;
  if (ier != 0) {
    Na = ier - 1;
    UTwarn ("SPcovFilt - %s: %d", SPM_RedOrder, Na);
    VRfZero (&h[Na], N-Na);
  }

/* Solve L y = r; the solution appears in y */
  MAfLTSolve ((const float **) L, r, y, Na);

/* Solve U h = y */
  MAfTTSolve ((const float **) L, y, h, Na);

/*
  The filtering error is
    ferr = Ed - h'r
         = Ed - h'L y   (L y = r)
         = Ed - y'y     (h'L = y')
*/
  ferr = Ed;
  if (ferr < 0.0)
    UThalt ("SPcovFilt: %s", SPM_NegEnergy);
  for (i = 0; i < Na; ++i) {
    y2 = (double) y[i] * y[i];
    if (y2 > ferr) {
      UTwarn ("SPcovFilt - %s", SPM_FixNegEnergy);
      VRfZero (&h[i], Na-i);
      break;
    }
    else
      ferr = ferr - y2;
  }

/* Deallocate storage */
  UTfree ((void *) y);
  MAfFreeMat (L);

  return ferr;
}
