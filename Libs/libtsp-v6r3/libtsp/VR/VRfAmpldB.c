/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  void VRfAmpldB (const float ampl[], float dB[], int N)

Purpose:
  Convert each value in an array of amplitudes to a decibel value

Description:
  This routine converts the elements of an array of amplitudes to decibels.
    dB[i] = 20 * log10(ampl[i]),  0 <= i < N.
  The value ampl[i] must be non-negative.

Parameters:
   -> const float ampl[]
      Input amplitude array (N elements).  This values must be non-negative.
  <-  float y[]
      Output array of decibel values (N elements).  The output array can be the
      same as the input array.
   -> int N
      Number of elements in the arrays (may be zero)

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.9 $  $Date: 2003/05/09 03:33:10 $

-------------------------------------------------------------------------*/

#include <math.h>

#include <libtsp.h>
#include <libtsp/VRmsg.h>


void
VRfAmpldB (const float ampl[], float dB[], int N)

{
  int i;

  for (i = 0; i < N; ++i) {
    if (ampl[i] >= 0.0F)
      dB[i] = 20.0F * (float) log10 ((double) ampl[i]);
    else
      UThalt ("VRfAmpldB: %s", VRM_NegAmpl);
  }

  return;
}
