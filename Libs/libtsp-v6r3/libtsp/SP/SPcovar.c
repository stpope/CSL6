/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  void SPcovar (x, Nx, Cov, Np)

Purpose:
  Calculate a correlation (covariance) matrix for a data sequence

Description:
  This routine fills in the elements of a correlation matrix given an array of
  data.  The elements of the matrix are calculated as
             N+Np-1
    C(i,j) =  SUM x(n-i) x(n-j) ,    0 <= i,j <= Np
              n=Np
  where Np is the number of correlation lags and N is the number of terms in
  each correlation calculation.  To compute the correlation terms, the
  following recursion is used,

    C(i,j) = C(i-1,j-1) + x(Np-i) x(Np-j) - x(N+Np-i) x(N+Np-j) ,

  starting from C(i,0)=C(0,i).  Note that finite precision effects in
  calculating the recursion can result in a non-positive definite matrix.

Parameters:
   -> const float x[]
      Input data (Nx elements).  The first Np values normally represent data
      from a previous frame of data.
   -> int Nx
      Number of data elements.  Nx=N+Np, where N is the number of terms in
      each correlation sum and Np is the number of correlation lags to be
      calculated.  Note that Nx must be larger than Np.
  <-  float *Cov[]
      Cov is an array of pointers to the rows of a Np+1 by Np+1 matrix.  On
      return, the (symmetric) matrix contains the correlation values.
   -> int Np
      Maximum correlation lag

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.10 $  $Date: 2003/05/09 02:45:14 $

-------------------------------------------------------------------------*/

#include <libtsp.h>
#include <libtsp/SPmsg.h>


void
SPcovar (const float x[], int Nx, float *Cov[], int Np)

{
  double sum;
  int i, j, k;

  if (Nx <= Np)
    UThalt ("SPcovar: %s", SPM_BadFrLen);

/* Form the initial correlation sum at lag k */
  for (k = 0; k <= Np; ++k) {
    sum = VRfDotProd (&x[Np], &x[Np-k], Nx - Np);
    Cov[0][k] = (float) sum;
    Cov[k][0] = (float) sum;

/* Fill in the covariance matrix, going down diagonals of constant lag k=i-j */
    for (i = k+1, j = 1; i <= Np; ++i, ++j) {
      sum = sum + (double) x[Np-i] * x[Np-j] - (double) x[Nx-i] * x[Nx-j];
      Cov[i][j] = (float) sum;
      Cov[j][i] = (float) sum;
    }
  }
}
