/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  double SPcorFilt (double Ed, const float rxx[], const float r[], float h[],
                    int N)

Purpose:
  Find filter coefficients to minimize the mean-square error

Description:
  This procedure finds the filter coefficients for a linear filter which
  minimizes the mean-square error.  Consider a filter with N coefficients,
  with coefficient h(i) corresponding to lag Nd+i.  The filter output is
           N-1
    y(k) = SUM h(i) x(k-i-Nd) ,
           i=0
  where x(i) is the input signal.  The filter error is

    e(k) = d(k) - y(k) ,

  where d(k) is the desired signal.  To minimize the mean-square filtering
  error, solve

    R h = r,

  where R is a symmetric positive definite correlation matrix, h is a vector
  of filter coefficients and r is a vector of correlation values.  The matrix
  R and and vector r are defined as follows

    R(i,j) = E[x(k-i-Nd) x(k-j-Nd)],  for 0 <= i,j < N,
      r(i) = E[d(k) x(k-i-Nd],        for 0 <= i < N.

  For this routine, the matrix R must be symmetric and Toeplitz.  Then

    R(i,j) = rxx(|i-j|).

  The solution is determined using Levinson's method.  The resulting
  mean-square filtering error can be expressed as

    ferr = Ed - 2 h'r + h'R h
         = Ed - h'r ,

  where Ed is the mean-square value of the desired signal,

    Ed = E[d(k)^2] .

Parameters:
  <-  double SPcorFmse
      Resultant filter mean-square error
   -> double Ed
      Signal energy for the desired signal.  This value is used only for the
      computation of the mean-square error.
   -> const float rxx[]
      N element vector of autocorrelation values.  Element rxx[i] is the
      autocorrelation at lag i.
   -> const float r[]
      N element vector of cross-correlation values.  Element r[i] is the
      cross-correlation at lag Nd+i.
  <-  float h[]
      N element vector of filter coefficients.  Coefficient h[i] is the filter
      coefficient corresponding to lag Nd+i.
   -> int N
      Number of elements in each of the vectors rxx, h and r.

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.6 $  $Date: 2003/05/09 02:45:14 $

-------------------------------------------------------------------------*/

#include <libtsp.h>
#include <libtsp/SPmsg.h>


double
SPcorFilt (double Ed, const float rxx[], const float r[], float h[], int N)

{
  double ferr;
  int ier;

  if (Ed == 0.0) {
    VRfZero (h, N);
    ferr = 0.0;
  }
  else {
    ier = MAfTpSolve (rxx, r, h, N);
    if (ier != 0)
      UThalt ("SPcorFilt: %s", SPM_NonPosDef);
    ferr = SPcorFmse (h, Ed, rxx, r, N);
  }

  return ferr;
}
