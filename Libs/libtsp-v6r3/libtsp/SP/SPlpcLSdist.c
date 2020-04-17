/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  double SPlpcLSdist (const float pc1[], const float pc2[], int Np, int Ncep)

Purpose:
  Calculate the RMS log LPC spectral distance measure

Description:
  This function calculates Markel and Gray's RMS log LPC spectral distance
  measure.  Consider the prediction error filter A(z),

                Np       -k
    A(z) = 1 - SUM p(k) z  .
               k=1

  The power spectrum corresponding to the all-pole LPC filter is

    S(w) = 1 / |A(w)|^2 ,

  where A(w) is short-hand notation for A(exp(jw)).  The RMS log LPC spectral
  distance measure for two filters A1(z) and A2(z) is given by
                pi
    D = sqrt{ int (10 log[|A1(w)|^2] - 10 log[|A2(w)|^2])^2 dw } .
               -pi

  The power spectrum is the Fourier transform of the (infinite) set of
  autocorrelation coefficients.  The cepstrum for the autocorrelation
  coefficients is given by inverse Fourier transform of the log power spectrum.
  Equivalently the cepstral coefficients are the Fourier series coefficients
  for the (periodic) log power spectrum.

  The RMS log LPC spectral distance  measure is equivalent to the root
  mean-square difference between the cepstral coefficients corresponding to the
  two LPC filters.  This routine finds the cepstral coefficients corresponding
  to the two sets of input predictor coefficients.  Theoretically, this
  requires an infinite number of cepstral coefficients.  This routine
  calculates an approximation to the distance from two finite length sets of
  cepstral coefficients.

  Reference:
      A. H. Gray, Jr. and J. D. Markel, "Distance measures for speech
      processing", IEEE Trans. Acoustics, Speech, and Signal Processing,
      vol. ASSP-24, pp. 380-391, October 1976.

Parameters:
  <-  double SPlpcLSdist
      Resultant distance measure (dB units). 0 dB corresponds to identical
      sets of predictor coefficients.
   -> const float pc1[]
      Predictor coefficients corresponding to the first spectrum
   -> const float pc2[]
      Predictor coefficients corresponding to the second spectrum
   -> int Np
      Number of predictor coefficients in each of pc1 and pc2
   -> int Ncep
      Number of cepstral coefficients used for the distance calculation
      (maximum 150).  This number does not include the zero'th cepstral
      coefficient.  A recommended number is 3*Np.

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.15 $  $Date: 2003/05/09 02:45:04 $

-------------------------------------------------------------------------*/

#include <math.h>

#include <libtsp.h>
#include <libtsp/SPmsg.h>

#define MAXNCEP		150
#define LOG10E		0.43429448190325182765
#define SQRT2		1.41421356237309504880


double
SPlpcLSdist (const float pc1[], const float pc2[], int Np, int Ncep)

{
  int i;
  double sum, t, dist;
  float cep1[MAXNCEP+1];
  float cep2[MAXNCEP+1];

  if (Ncep > MAXNCEP)
    UThalt ("SPlpcDiffLS: %s", SPM_TooManyCep);

/* Calculate the cepstral coefficients */
  SPpcXcep (pc1, Np, cep1, Ncep+1);
  SPpcXcep (pc2, Np, cep2, Ncep+1);

/*
   The log power spectrum can be expressed in terms of the cepstral
   coefficients as
                            inf
     ln [|A(exp(jw))|^2] =  SUM  c(k) exp(-jkw)
                           k=-inf

    with c(-k) = c(k) and c(0) = 0.  In this form, this is a Fourier series.
    For two spectra with coefficients c1(k) and c2(k), Parseval's relationship
    expresses the integral of the squared error in the log power spectra as the
    as the sum of the squared differences of the Fourier series coefficients.

       inf                      inf
       SUM [c1(k)-c2(k)]^2 = 2 SUM [c1(k)-c2(k)]^2
      k=-inf                    k=1
*/

/* Distance calculation */
  sum = 0.0;
  for (i = 1; i <= Ncep; ++i) {
    t = cep1[i] - cep2[i];
    sum = sum + t * t;
  }

/* Convert to dB units */
  dist = (10.0 * LOG10E * SQRT2) * sqrt (sum);

  return dist;
}
