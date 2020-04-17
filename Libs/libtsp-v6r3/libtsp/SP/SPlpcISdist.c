/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  double SPlpcISdist (const float pc1[], const float pc2[], int Np)

Purpose:
  Calculate the Itakura-Saito LPC spectral distance measure

Description:
  This function calculates the Itakura-Saito LPC spectral distance measure
  kernel.  Given two LPC spectral models 1/A1(z) and 1/A2(z), this function
  calculates a spectral difference function.  Let the coefficient vectors
  corresponding to the all-pole filters be a1 and a2.  Consider a signal with
  correlation corresponding to the correlation matrix R1.  Define E1 as the
  minimum mean-square error that results when the signal is filtered using the
  optimum prediction filter A1(z) derived from the correlation R1.  Define E12
  as the mean-square error that results when the same signal is filtered using
  the prediction filter A2(z) (mismatched to the signal).  These errors can be
  calculated as  as
     E1  = a1' R1 a1 ,
     E12 = a2' R1 a2 ,
  This function returns the value E12/E1.

  The distance kernel can be used to calculate the Itakura-Saito maximum
  likelihood spectral distance between two spectra models g1/A1(z) and
  g2/A2(z),
            1    pi
    Dist = ---  Int [ exp(V(w)) - V(w) - 1 ] dw.
           2pi  -pi
                       g1^2             g2^2
    where  V(w) = ln ---------  - ln  --------- ,
                     |A1(w)|^2        |A2(w)|^2

     pi
    Int V(w) dw = 4 pi ln(g1/g2),
    -pi

     pi
    Int exp(V(w)) dw = (g1/g2)^2  E12/E1
    -pi

  where E1 is the residual energy for A1(z) and E12 is the residual energy for
  A2(z).  If the gains g1 and g2 are equal (ref. 1),

    Dist = E12/E1 -  1,

  An alternate spectral difference measure is the Itakura measure (see
  ref. 2),

           a1' R2 a1
    d = ln --------- .
           a2' R2 a2

  The term inside the logarithm is obtained by interchanging the roles of a1
  and a2 in the call to this routine.

  References:
  A. Gray Jr. and J. Markel, "Distance measures for speech processing", IEEE
  Trans. Acoustics, Speech, and Signal Processing, vol. ASSP-24, pp. 380-391,
  October 1976.

  M. R. Sambur and N.S. Jayant, "LPC analysis/synthesis from speech inputs
  containing quantizing noise or additive white noise", IEEE Trans. Acoustics,
  Speech, and Signal Processing, vol. ASSP-24, pp. 488-494, December 1976.

Parameters:
  <-  double SPlpcISdist
      Resultant distance measure
   -> const float pc1[]
      Predictor coefficients corresponding to the first spectrum
   -> const float pc2[]
      Predictor coefficients corresponding to the second spectrum
   -> int Np
      Number of predictor coefficients (maximum 50)

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.21 $  $Date: 2003/05/09 02:45:03 $

-------------------------------------------------------------------------*/

#include <libtsp.h>
#include <libtsp/SPmsg.h>


#define MAXNP	50


double
SPlpcISdist (const float pc1[], const float pc2[], int Np)

{
  double E1, E12;
  float rxx1[MAXNP+1];

  if (Np > MAXNP)
    UThalt ("SPlpcDiffIS: %s", SPM_TooManyCoef);

/*
   Calculate the residual error energy for a predictor matched to signal.
*/
  E1 = SPpcXcor (pc1, Np, rxx1, Np + 1);

/* Calculate the residual error for the mismatched signal */
  E12 = SPcorPmse (pc2, rxx1, Np);

  return (E12 / E1);
}
