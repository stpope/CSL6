/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  void SPecXpc (const float ec[], float pc[], int Np)

Purpose:
  Convert prediction error filter coefficients to predictor coefficients

Description:
  The error filter coefficients are related to the predictor filter
  coefficients by,
                ec[0]  = 1 (not used)
       pc[0] = -ec[1]
            ...
    pc[Np-1] = -ec[Np]

  Predictor coefficients are usually expressed algebraically as vectors with
  1-offset indexing.  The correspondence to the 0-offset C-arrays is as
  follows.
    p(1) <==> pc[0]       predictor coefficient corresponding to lag 1
    p(i) <==> pc[i-1]     1 <= i < Np

Parameters:
   -> const float ec[]
      Prediction error (residual) filter coefficients (Np + 1 values).  It is
      considered an error if ec[0] is not equal to 1.
  <-  float pc[]
      Vector of predictor coefficients (Np values).  These are the coefficients
      of the predictor filter, with pc[0] being the predictor coefficient
      corresponding to lag 1, and pc[Np-1] corresponding to lag Np.
   -> int Np
      Number of predictor coefficients

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.9 $  $Date: 2003/05/09 02:45:15 $

-------------------------------------------------------------------------*/

#include <libtsp.h>
#include <libtsp/SPmsg.h>


void
SPecXpc (const float ec[], float pc[], int Np)

{
  int i;

  if (ec[0] != 1.0F)
    UThalt ("SPecXpc: %s", SPM_NonUnity);
  for (i = 0; i < Np; ++i)
    pc[i] = -ec[i+1];

  return;
}
