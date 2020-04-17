/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  void SPcepXpc (const float cep[], float pc[], int Np)

Purpose:
  Convert cepstral coefficients to predictor coefficients

Description:
  This routine calculates the predictor coefficients corresponding to a set of
  cepstral coefficients.  Consider the prediction error filter A(z),

                Np       -k
    A(z) = 1 - SUM p(k) z  .
               k=1

  The power spectrum corresponding to the all-pole LPC filter is

    S(w) = g^2 / |A(w)|^2 ,

  where A(w) is short-hand notation for A(exp(jw)).  The power spectrum is
  the Fourier transform of the (infinite) set of autocorrelation coefficients
  The cepstrum for the autocorrelation coefficients is given by the inverse
  Fourier transform of the log power spectrum. Equivalently the cepstral
  coefficients are the Fourier series coefficients of the (periodic) log power
  spectrum,

    ln[S(w)] = 2 ln[g] - ln[|A(w)|^2]

                inf
             =  SUM  c(k) exp(-jkw) .
               k=-inf

  The cepstral coefficients are symmetric, c(-k) = c(k).  For a minimum-phase
  A(z), the integral of ln[|A(w)|^2] is zero.  The coefficient c(0) is then the
  average of the log power spectrum, c(0) = 2 ln[g].  This cepstral coefficient
  does not affect the values of the coefficients of A(z).  An expression for
  the coefficients of A(z) is obtained by expanding ln[A(w)] in a Laurent
  series and taking the derivatives.  This gives the recursion

                  1  n-1
    p(n) = c(n) - -  SUM (n-k) c(n-k) p(k),  n=1,2,...,Np .
                  n  k=1

  Reference:
      J. D. Markel and A. H. Gray, Jr., "Linear Prediction of Speech",
      Springer-Verlag, 1976.

  Predictor coefficients are usually expressed algebraically as vectors with
  1-offset indexing.  The correspondence to the 0-offset C-arrays is as
  follows.
    p(1) <==> pc[0]       predictor coefficient corresponding to lag 1
    p(i) <==> pc[i-1]     1 <= i < Np

Parameters:
   -> const float cep[]
      Cepstral coefficients (Np+1 values).  The first cepstral coefficient
      cep[0] corresponds to the zero quefrency term and is not used in the
      calculation.
  <-  float pc[]
      Vector of predictor coefficients (Np values).  These are the coefficients
      of the predictor filter, with pc[0] being the predictor coefficient
      corresponding to lag 1, and pc[Np-1] corresponding to lag Np.
   -> int Np
      Number of predictor coefficients

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.12 $  $Date: 2003/05/09 02:45:14 $

-------------------------------------------------------------------------*/

#include <libtsp.h>

/* one-offset vector */
#define P(i)	pc[i-1]


void
SPcepXpc (const float cep[], float pc[], int Np)

{
  int n, k;
  float sum;

  for (n = 1; n <= Np; ++n) {
    sum = 0.0F;
    for (k = 1; k <= n - 1; ++k)
      sum = sum + (n - k) * P(k) * cep[n-k];
    P(n) = cep[n] - sum / n;
  }

  return;
}
