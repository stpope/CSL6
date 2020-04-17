/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  void SPpcXec (const float pc[], float ec[], int Np)

Purpose:
  Convert predictor coefficients to prediction error filter coefficients

Description:
  The error filter coefficients are related to the predictor filter
  coefficients by,
    ec[0]  = 1
    ec[1]  = -pc[0]
          ...
    ec[Np] = -pc[Np-1]

  Predictor coefficients are usually expressed algebraically as vectors with
  1-offset indexing.  The correspondence to the 0-offset C-arrays is as
  follows.
    p(1) <==> pc[0]       predictor coefficient corresponding to lag 1
    p(i) <==> pc[i-1]     1 <= i < Np

Parameters:
   -> const float pc[]
      Vector of predictor coefficients (Np values).  These are the coefficients
      of the predictor filter, with pc[0] being the predictor coefficient
      corresponding to lag 1, and pc[Np-1] corresponding to lag Np.
  <-  float ec[]
      Prediction error (residual) filter coefficients (Np + 1 values)
   -> int Np
      Number of predictor coefficients (Np can be zero)

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.9 $  $Date: 2003/05/09 02:45:04 $

-------------------------------------------------------------------------*/

#include <libtsp.h>


void
SPpcXec (const float pc[], float ec[], int Np)

{
  int i;

  ec[0] = 1.0F;
  for (i = 1; i <= Np; ++i)
    ec[i] = -pc[i-1];

  return;
}
