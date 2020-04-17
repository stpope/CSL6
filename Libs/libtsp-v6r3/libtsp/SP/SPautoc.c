/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  void SPautoc (const float x[], int Nx, float cor[], int Nt)

Purpose:
  Calculate the autocorrelation for a data sequence

Description:
  This routine calculate the autocorrelation for a given data vector.
             Nx-1-i
    cor[i] =  SUM  x[k] * x[k+i] , for 0 <= i < Nt.
              k=0

  This routine requires
    (Nt+1)*(Nx+Nt/2) multiplies and
    (Nt+1)*(Nx+Nt/2) adds.

Parameters:
   -> const float x[]
      Input data vector with Nx elements
   -> int Nx
      Number of data points
  <-  float cor[]
      Autocorrelation vector with Nt elements.  The element cor[i] is the
      autocorrelation with lag i.
   -> int Nt
      Number of autocorrelation terms

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.8 $  $Date: 2003/05/09 02:45:14 $

-------------------------------------------------------------------------*/

#include <libtsp.h>


void
SPautoc (const float x[], int Nx, float cor[], int Nt)

{
  int i;
  int N;

  N = Nt;
  if (Nt > Nx)
    N = Nx;

  for (i = 0; i < N; ++i)
    cor[i] = (float) VRfDotProd (x, &x[i], Nx - i);

  for (i = N; i < Nt; ++i)
    cor[i] = 0.0F;

  return;
}
