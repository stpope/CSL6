/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  void SPcorBWexp (double bwexp, const float rxx[], float rxxb[], int Nc)

Purpose:
  Bandwidth expand a set of correlation values

Description:
  The procedure forms the bandwidth expanded version of a set of correlation
  values.  The correlation coefficients are multiplied by a Gaussian function.
  This is equivalent to convolving the power spectral density by a Gaussian
  function.  The bandwidth expansion is measured in terms of the width of
  the Gaussian function in the frequency domain.  A power spectrum consisting
  of spectral lines will have each spectral line converted into a Gaussian
  function with this bandwidth.

  The Gaussian function is specified in terms of its normalized bandwidth
  (measured at the 1 standard deviation point).  The 3 dB bandwidth is
  related to the 1 sigma bandwidth by the following relationship.
    B3dB = sqrt (2 ln 2) Bsigma = 1.1774 Bsigma
  The ITU-T G.729 8 kb/s CS-ACELP speech coder uses a 1 sigma bandwidth
  expansion of 60 Hz relative to a sampling rate of 8000 Hz. This expansion
  is achieved by setting,
    bwexp = 60/8000 = 0.015.
  Note that the actual bandwidth measured on both sides of a peak is twice
  this value.

  Reference:
  Y. Tohkura, F. Itakura, and S. Hashimoto, "Spectral smoothing technique in
  PARCOR speech analysis-synthesis", IEEE Trans. Acoustics, Speech, Signal
  Proc., vol. ASSP-26, pp. 587-596, Dec. 1978.

Parameters:
   -> double bwexp
      Bandwidth expansion value (normalized one-sided bandwidth value relative
      to the sampling frequency)
   -> const float rxx[]
      Input correlation coefficients (Nc values)
  <-  float rxxb[]
      Output correlation coefficients (Nc values).  The output values can
      overlay the input values.
   -> int Nc
      Number of correlation coefficients

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.5 $  $Date: 2003/05/09 02:45:14 $

-------------------------------------------------------------------------*/

#include <math.h>

#include <libtsp.h>

#define PIx2		6.28318530717958647693

/*
  The Fourier transform of a discrete-time Gaussian response is an aliased
  Gaussian frequency response.  This transform can be written in terms of the
  Jacobi Elliptic Theta function.

  For sufficiently small bandwidths, the aliasing is negligible because of the
  rapid decrease in the tail of the Gaussian frequency response.  Multiplying
  the (symmetrical) sequence of correlation values by the (symmetrical)
  Gaussian time function, is equivalent to convolving the (symmetric positive)
  power spectrum by the aliased (symmetric positive) Gaussian frequency
  function.  The resulting frequency function is still symmetric and positive.
  Thus its inverse transform is a valid (positive definite) correlation
  function.

  The effect of convolving the power spectrum with the aliased Gaussian
  function is to expand the bandwidth of the resonances in the power spectrum.
  A very narrow resonance takes on the bandwidth of the Gaussian function.
  The Gaussian function is normalized in amplitude such that the total power is
  preserved.

  In continuous-time, the weighting function is
    g(t) = exp(-0.5 (a t)^2).
  The frequency response
    G(f) = sqrt(2 pi)/a exp(-0.5 (2 pi f / a)^2).
  This response has a 1-sigma bandwidth of a/(2 pi) Hz.

  The discrete-time weighting function is
    g[k] = exp (-0.5 (a k / Fs)^2).
  Ignoring aliasing, the resulting function has a normalized bandwidth of
    bwexp = a / (2 pi Fs).
  Then, given bwexp (normalized bandwidth expansion), the discrete-time
  weighting function is
    g[k] = exp (-0.5 (2 pi bwexp k)^2).

  Since g[0] is unity, the zero'th correlation coefficient (energy) is
  unchanged.
*/


void
SPcorBWexp (double bwexp, const float rxx[], float rxxb[], int Nc)

{
  int k;
  double w;

  /* Loop over the coefficients */
  w = PIx2 * bwexp;
  for (k = 0; k < Nc; ++k)
    rxxb[k] = (float) exp (-0.5 * ((w*k) * (w*k))) * rxx[k];

  return;
}
