/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  double SPcorFmse (const float h[], double Ed, const float rxx[],
                    const float r[], int N)

Purpose:
  Calculate the mean-square filtering error

Description:
  This function calculates the mean-square error for a linear filter.  Consider
  a filter with N coefficients, with coefficient h(i) corresponding to
  lag Nd+i.  The filter output is
           N-1
    y(k) = SUM h(i) x(k-i-Nd) ,
           i=0
  where x(i) is the input signal.  The filter error is

    e(k) = d(k) - y(k) ,

  where d(k) is the desired signal. The mean-square filtering error is
  E[e(k)^2] or in vector-matrix notation

    ferr = Ed - 2 h'r + h' R h ,

  The mean-square value E0, matrix R and vector p are defined as follows

        Ed = E[d(k)^2]
    R(i,j) = E[x(k-i-Nd) x(k-j-Nd],  for 0 <= i,j < N,
      r(i) = E[d(k) x(k-i-Nd)],      for 0 <= i < N.

  For this routine, the matrix R must be symmetric and Toeplitz, viz.

    R(i,j) = rxx(|i-j|).

  Linear prediction can be cast into the above form, if we let Nd=1.  Also
  for linear prediction, usually d(k)=x(k), giving r(i)=rxx(i).

Parameters:
  <-  double SPcorFmse
      Resultant mean-square error
   -> const float h[]
      N element vector of filter coefficients.  Coefficient h[i] is the filter
      coefficient corresponding to lag Nd+i.
   -> double Ed
      Signal energy for the desired signal.  This value is used only for the
      computation of the mean-square error.
   -> const float rxx[]
      N element vector of autocorrelation values.  Element rxx[i] is the
      autocorrelation at lag i.
   -> const float r[]
      N element vector of cross-correlation values.  Element r[i] is the
      cross-correlation at lag Nd+i.
   -> int N
      Number of elements in each of the vectors rxx, h and r.

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.12 $  $Date: 2003/05/09 02:45:14 $

-------------------------------------------------------------------------*/

#include <libtsp.h>


double
SPcorFmse (const float h[], double Ed, const float rxx[], const float r[],
	   int N)

{
  double ferr;

  ferr = Ed - 2.0 * VRfDotProd (r, h, N) + MAfTpQuad (rxx, h, N);

  return ferr;
}
