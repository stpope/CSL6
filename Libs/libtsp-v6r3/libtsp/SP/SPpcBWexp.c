/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  void SPpcBWexp (double bwexp, const float pc[], float pcb[], int Np)

Purpose:
  Bandwidth expand a set of predictor coefficients

Description:
  The procedure forms the bandwidth expanded version of a set of predictor
  coefficients.  The roots of the all-pole LPC filter are radially scaled by
  the factor bwexp.  This has the effect of increasing the bandwidth of the
  resonant peaks in the frequency response (speech formants).  Let the
  prediction error filter be
                Np       -k
    A(z) = 1 - SUM p(k) z  .
               k=1
  The corresponding all-pole filter is 1/A(z).  The filter with the radially
  scaled poles is 1/A(b*z).  The bandwith expanded polynomial A(b*z) has
  coefficients,

     p'(k) = p(k) * b^k  for 1 <= k <= Np.

  The effective bandwidth expansion of a sharp resonance can be determined
  from that for an isolated pole pair.  Consider a simple filter with poles at
  z1 = r exp (j w0) and z2 = r exp (-j w0),
                 z^2 - 1
    H(z) = ------------------------- .
           z^2 - 2r cos(w0) z  + r^2
  The frequency response of this filter has a peak at w = w0 with a 3 dB
  bandwidth
    BW = pi/2 - 2 atan (r^2)     normalized radians.

  If the pole positions are scaled radially, r' = r * b, with b < 1, the
  bandwidth of the resonance increases.  The increase in bandwidth can be
  computed to be
    dBW = 2 atan (r^2 (1-b^2) / (1 + b^2 r^4))
  For a 8 kHz sampling rate, the bandwidths expansions for different values
  of b are shown below (calculated for r=1).
       b       BW exp
     1.000      0
     0.996     10.2 Hz
     0.995     12.8 Hz
     0.994     15.3 Hz
     0.990     25.6 Hz
     0.980     51.4 Hz
  For speech analysis, bwexp is often chosen to give a bandwidth expansion of
  10 to 25 Hz.

  Predictor coefficients are usually expressed algebraically as vectors with
  1-offset indexing.  The correspondence to the 0-offset C-arrays is as
  follows.
    p(1)  <==> pc[0]       predictor coefficient corresponding to lag 1
    p(k)  <==> pc[k-1]     1 <= k < Np
    p'(1) <==> pcb[0]      predictor coefficient corresponding to lag 1
    p'(k) <==> pcb[k-1]    1 <= k < Np

Parameters:
   -> double bwexp
      Bandwidth expansion factor
   -> const float pc[]
      Input predictor coefficients (Np values)
  <-  float pcb[]
      Output predictor coefficients (Np values).  The output values can overlay
      the input values.
   -> int Np
      Number of predictor coefficients

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.12 $  $Date: 2003/05/09 02:45:04 $

-------------------------------------------------------------------------*/

#include <libtsp.h>

/* 
  Consider an analog filter with a pair of complex poles corresponding to a
  resonance at W = W0,
                     s
    H(s) = --------------------- .
           s^2 + (W0/Q) s + W0^2
  This analog filter has a zero at s=0 and at s=infinity.  In the s-plane the
  roots are at a distance of W0 from the origin, with real component equal to
  -W0/(2 Q).  The frequency response is
                   Q/W0
    H(jW) = -------------------- .
            1 + jQ (W/W0 - W0/W)

  The maximum occurs for W = W0 or W = -W0.  The 3 dB points occur when
  |Q (W/W0 - W0/W)| = 1.  Let u = W/W0.  Then the 3 dB points satisfy
    Q (u - 1/u)  = +/- 1 ,
  or
    u^2 -/+ u/Q - 1 = 0.
  or
    u = +/- 1/(2Q) +/- sqr (1 + 1/(4 Q^2)) .
  The second term is purely real for Q > 1/2, corresponding to complex poles.
  Assume this to be the case.  The equation has 4 solutions, two positive and
  two negative.  Consider the positive ones.
    u = sqr (1 + 1/(4 Q^2)) +/- 1/(2Q) .
  The two solutions are related by
    u1 u2 = 1 and u2 - u1 = 1/Q.
  In terms of W1 and W2,
    W1 W2 = W0^2 and W2 - W1 = W0/Q.
  The bandwidth of the resonance is W2 - W1 = W0/Q.

  Digital pole pair:
  Consider the digital filter found from the analog filter using a bilinear
  transformation,
          s + a         z - 1
    z = - ----- , s = a ----- .
          s - a         z + 1
  This transformation maps the zeros at s = 0 and s = infinity to z = +1 and
  z = -1,
                 z^2 - 1
    H(z) = ------------------------- .
           z^2 - 2r cos(w0) z  + r^2

  The magnitude of z directly from the bilinear transformation is given by
            |s|^2 + 2 a Re(s) + a^2
    |z|^2 = ----------------------- .
            |s|^2 - 2 a Re(s) + a^2
  At the poles, |s|^2 = W0^2 and Re(s) = -W0/(2 Q), giving for the poles of
  the digital filter,
          W0^2 - a W0/Q + a^2
    r^2 = ------------------- .
          W0^2 + a W0/Q + a^2
  From this relationship,
      a W0/Q     1 - r^2
    ---------- = ------- .
     a^2 + W0^2  1 + r^2
  
  The digital frequency w is related to the analog frequency W by
    w = 2 atan (W/a) .
  The bandwidth in the digital domain is then
    w2 - w1 = 2 atan (W2/a) - atan (W1/a)
                        W2/a - W1/a
            = 2 atan (----------------)
                      1 + W1 W2 / a^2)
                        a W0/Q
            = 2 atan (----------)
                      a^2 + W0^2
                      1 - r^2
            = 2 atan (-------)
                      1 + r^2

            = pi/2 - 2 atan (r^2) .
*/


void
SPpcBWexp (double bwexp, const float pc[], float pcb[], int Np)

{
  int k;
  float b, bk;

  /* Loop over the coefficients */
  b = (float) bwexp;
  bk = b;
  for (k = 0; k < Np; ++k) {
    pcb[k] = bk * pc[k];
    bk = bk * b;
  }
  return;
}
