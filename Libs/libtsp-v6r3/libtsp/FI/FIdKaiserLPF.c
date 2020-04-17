/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  void FIdKaiserLPF (double h[], int N, double Fc, double alpha)

Purpose:
  Generate a Kaiser windowed lowpass filter

Description:
  This routine calculates the impulse response of a symmetric lowpass filter
  which is obtained by applying a Kaiser window to a sin(x)/x function.
  Consider the continuous-time version of this response
                       sin(2 pi Fc t)
    h(t) = Kw(t/T, a)  -------------- ,
                            pi t
  where Kw(t/T,a) is a Kaiser window with parameter a, spanning -T <= t <= T.
  The cutoff of the lowpass filter is Fc.  The gain of the filter is set to
  approximate unity in the passband.  If it is desired to have the peak
  value of the time response equal to unity, h(t) must be divided by 2*Fc.

  The discrete-time impulse response of length N is obtained by setting the
  sampling rate to one; t = n - T and T = (N-1)/2.  The cutoff frequency Fc
  is then a normalized frequency.

  The parameter a (alpha) determines the shape of the window, with increasing
  a giving a wider transition region but better stopband attenuation.  The
  tradeoff is shown in the following table.
     stopband     a    transition   passband
    attenuation         width D      ripple
       30 dB    2.210     1.536    +/- 0.270 dB
       40 dB    3.384     2.228    +/- 0.0864 dB
       50 dB    4.538     2.926    +/- 0.0274 dB
       60 dB    5.658     3.621    +/- 0.00868 dB
       70 dB    6.764     4.317    +/- 0.00275 dB
       80 dB    7.865     5.015    +/- 0.00089 dB
       90 dB    8.960     5.712    +/- 0.00027 dB
      100 dB   10.056     6.408    +/- 0.00009 dB
  The transition width parameter D = (N-1) dF, where dF is the normalized
  transition width.

  Example:
  Consider a lowpass filter with a 90 dB stopband attenuation to be used for
  interpolation by a factor of 8.  The cutoff frequency is then 0.5/8 = 0.0625.
  The transition bandwidth should be less than 10 % of the cutoff frequency,
  giving dF = 0.00625.  The 90 dB attenuation is reached for a = 8.960.
  This gives D = 5.712.  Then N >= D/dF + 1, giving N = 915.  This is a long
  filter because of the narrow transition region and high attenuation
  specification, but in fact, for interpolation only 1/8 of these samples are
  used at any one time.

  References:
  J. F. Kaiser, "Nonrecursive digital filter design using the I0-sinh window
  function", Proc. 1974 IEEE Int. Symp. on Circuits and Syst., pp. 20-23,
  April 1974.

  A. Antoniou, Digital Filters: Analysis, Design and Applications, 2nd ed.,
  McGraw-Hill, 1993.

Parameters:
  <-  double h[]
      Array containing the impulse response samples.  The reference point in
      the array is at offset (N-1)/2.
   -> int N
      Number of impulse response samples
   -> double Fc
      Lowpass filter cutoff (normalized frequency, 0 to 1/2)
   -> double alpha
      Kaiser window parameter

Author / revision:
  P. Kabal  Copyright (C) 2005
  $Revision: 1.1 $  $Date: 2005/02/01 01:19:50 $

-------------------------------------------------------------------------*/

#include <libtsp.h>
#include <libtsp/nucleus.h>


void
FIdKaiserLPF (double h[], int N, double Fc, double alpha)

{
  int i, k;
  double t, T;

  /* Multiply the Kaiser window by the sin(x)/x response */
  T = 0.5 * (N-1);
  for (i = 0, k = N-1; i <= k; ++i, --k) {
    t = i - T;
    h[i] = 2.0*Fc * FNsinc (2.0*Fc * t) * FIxKaiser (t / T, alpha);
    if (i != k)
      h[k] = h[i];
  }

  return;
}
