/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  double SPcorPmse (const float pc[], const float rxx[], int Np)

Purpose:
  Calculate the mean-square prediction error

Description:
  This function calculates the mean-square prediction error for a linear
  predictor.  Consider a linear predictor with Np coefficients,
            Np
    y(k) = SUM p(i) x(k-i) ,
           i=1
  where x(k) is the input signal.  The prediction error is

    e(k) = x(k) - y(k) .

  The mean-square prediction error is E[e(k)^2] or in vector-matrix notation

    E = Ex - 2 p'r + p'R p ,

  The mean-square value Ex, matrix R and vector p are defined as follows
        Ex = E[x(k)^2]
    R(i,j) = E[x(k-i) x(k-j)],  for 1 <= i,j <= Np,
      r(i) = E[x(k) x(k-i)],    for 1 <= i <= Np.

  For this routine, the matrix R must be symmetric and Toeplitz.  Then

    R(i,j) = rxx(|i-j|)
      r(i) = rxx(i)

  Predictor coefficients are usually expressed algebraically as vectors with
  1-offset indexing.  The correspondence to the 0-offset C-arrays is as
  follows.
    p(1) <==> pc[0]       predictor coefficient corresponding to lag 1
    p(i) <==> pc[i-1]     1 <= i < Np

Parameters:
  <-  double SPcorPmse
      Resultant mean-square prediction error
   -> const float pc[]
      Np element vector of predictor coefficients.  Coefficient pc[i] is the
      predictor coefficient corresponding to lag i+1.
   -> const float rxx[]
      Np+1 element vector of autocorrelation values.  Element rxx[i] is the
      autocorrelation at lag i.
  ->  int Np
      Number of predictor coefficients

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.6 $  $Date: 2003/05/09 02:45:14 $

-------------------------------------------------------------------------*/

#include <libtsp.h>


double
SPcorPmse (const float pc[], const float rxx[], int Np)

{
  double perr;
  float *ec;

  ec = (float *) UTmalloc ((Np+1) * sizeof (float));
  SPpcXec (pc, ec, Np);

/* Evaluate the error as a'R a, where a'=[1 pc'] */
  perr = MAfTpQuad (rxx, ec, Np+1);

  UTfree ((void *) ec);
  return perr;
}
