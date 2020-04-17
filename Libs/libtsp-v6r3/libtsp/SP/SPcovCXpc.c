/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  double SPcovCXpc (const float *Cov[], float pc[], int Np)

Purpose:
  Find predictor coefficients using the covariance lattice method (Cumani)

Description:
  This routine calculates the parameters for a lattice predictor which
  minimizes the sum of the forward and backward errors.  The reflection
  coefficients which describe the lattice filter are converted to the predictor
  coefficients which are returned.  This procedure guarantees a minimum phase
  prediction error filter.

  Consider a linear predictor with Np coefficients,
            Np
    y(k) = SUM p(i) x(k-i) ,
           i=1
  where x(i) is the input signal.  This routine returns the predictor
  cofficients.

  This procedure is based on Makhoul's covariance lattice method as modified
  by Cumani to compute the result using reflection coefficients.  The
  reflection coefficients are chosen to minimize a weighted sum of the
  forward and backward prediction errors.

  The original covariance lattice method of Makhoul allowed for minimizing a
  weighted combination of the forward and backward prediction errors.  Here the
  forward and backward prediction errors have equal weight.  In addition, the
  computation of the reflection coefficients is modified as specified in the
  EIA/TIA IS-54-B digital mobile cellular radio standard.

  The computation uses a correlation matrix Cov with elements defined as

    Cov(i,j) = E[x(k-i) x(k-j)],  for 0 <= i,j <= Np.

  The expectation operator E[.] is often replaced by a sum over k over a finite
  interval.  Minimization of the prediction error over this interval defines
  the so-called covariance method for determining the linear prediction
  coefficients.

  References:
  1: J. Makhoul, "Stable and efficient lattice methods for linear prediction",
     IEEE Trans. Acoustics, Speech, Signal Processing, vol. ASSP-25, pp.
     423-428, October 1977.
  2: A. Cumani, "On a covariance-lattice algorithm for linear prediction",
     Proc. IEEE Conf. Acoustics, Speech, Signal Processing, Paris, pp. 651-654,
     May 1982.
  3: EIA/TIA Interim Standard IS-54-B, "Cellular System Dual-Mode Mobile
     Station - Base Station Compatibility Standard", Telecommunications
     Industry Association, April 1992.

  Predictor coefficients are usually expressed algebraically as vectors with
  1-offset indexing.  The correspondence to the 0-offset C-arrays is as
  follows.
    p(1) <==> pc[0]       predictor coefficient corresponding to lag 1
    p(i) <==> pc[i-1]     1 <= i < Np

Parameters:
  <-  double SPcovCXpc
      Resultant prediction error energy
   -> const float *Cov[]
      Cov is an array of pointers to the rows of an Np+1 by Np+1 symmetric
      positive definite correlation matrix.  Only the lower triangular portion
      of Cov is accessed.  Note that with ANSI C, if the actual parameter
      is not declared to have the const attribute, an explicit cast to
      (const float **) is required.
  <-  const float pc[]
      Np element vector of predictor coefficients.  Coefficient pc[i] is the
      predictor coefficient corresponding to lag i+1.
   -> int Np
      Number of prediction coefficients

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.12 $  $Date: 2003/05/09 02:55:04 $

-------------------------------------------------------------------------*/

#include <libtsp.h>


double
SPcovCXpc (const float *Cov[], float pc[], int Np)

{
  int i, j, m;
  float rm;
  float **F, **B, **C;
  float *rc;
  float Ferr, Ferr2, Berr2, Cerr2;
  double denom;

/*
  Consider stage m of a lattice filter.  The inputs are f[m-1](n) and
  b[m-1](n) and the outputs are f[m](n) and b[m](n),
    f[m](n) = f[m-1](n) + k[m] b[m-1](n-1),
    b[m](n) = k[m] f[m-1](n) + b[m-1](n-1).
  The inputs to the first stage are
    f[0](n) = b[0](n) = x(n).
  Define the correlation matrices
    F[m](i,j) = E{f[m](n-i) f[m](n-j)}
    B[m](i,j) = E{b[m](n-1-i) b[m](n-1-j)}
    C[m](i,j) = E{f[m](n-i) b[m](n-1-j)}.
  Define
    Cov(i,j) = E{x(n-i) x(n-j)}.
  Then
    F[0](i,j) = Cov(i,j)
    B[0](i,j) = Cov(i+1,j+1)
    C[0](i,j) = Cov(i,j+1).
  From the lattice equations, thecorrelation matrices can be updated as follows
    F[m](i,j) = F[m-1](i,j) + k[m] C[m-1](i,j) + k[m] C[m-1](j,i)
                + k[m]^2 B[m-1](i,j)
    B[m](i,j) = k[m]^2 F[m-1](i+1,j+1) + k[m] C[m-1](j+1,i+1)
                + k[m] C[m-1](i+1,j+1) + B[m-1](i+1,j+1)
    C[m](i,j) = k[m] F[m-1](i,j+1) + k[m] B[m-1](i,j+1) + C[m](i,j+1)
                + k[m]^2 C[m](i+1,j).
  For stage m, the value of k[m] is chosen to minimize the weighted sum of
  the energies of f[m](n-q) and b[m](n-q),
    Err(q) = g F[m](q,q) + (1-g) B[m](q-1,q-1)
           = g {F[m-1](q,q) + 2 k[m] C[m-1](q,q) + k[m]^2 B[m-1](q,q)}
             + (1-g) {k[m]^2 F[m-1](q,q) + 2 k[m] C[m-1](q,q) + B[m-1](q,q)}.
  Choosing k[m] to minimize Err(q) with gives
    k[m] = -C[m-1](q,q) / {g F[m-1](q,q) + (1-g) G[m-1](q,q)}.

  Notes:
  - In the case of the IS-54-B standard, the error that is minimized for
    stage m is 0.5 {Err(0) + Err(Np-m)}.
  - The correlation matrices F[m](i,j) and F[m](i,j) are symmetric,
    F[m](i,j) = F[m](j,i)
    B[m](i,j) = B[m](j,i).
  - The solution for k[m] requires F[m-1](q,q), B[m-1)(q,q) and C[m-1](q,q),
    where q = 0 and Np-m.  For the last stage (m = Np), we need F[Np-1](0,0),
    B[Np-1](0,0) and C[Np-1](0,0).  Working backwards, we can see that at
    stage m, we need Np-m by Np-m matrices, both to calculate the errors and
    to allow for the update of the correlation matrices at the next stage.
*/

  rm = 0.0F;
  F = NULL;
  B = NULL;
  C = NULL;
  rc = NULL;

/* Allocate space for the matrices */
  if (Np > 0) {
    F = (float **) MAfAllocMat (3*Np, Np);
    B = F + Np;
    C = B + Np;
    rc = (float *) UTmalloc (Np * sizeof (float));
  }

/* Initialize the matrices */
  for (i = 0; i < Np; ++i) {
    for (j = 0; j < i; ++j) {
      F[i][j] = Cov[i][j];
      B[i][j] = Cov[i+1][j+1];
      C[i][j] = Cov[i][j+1];
      C[j][i] = Cov[i+1][j];	/* = Cov[j][i+1] */
    }
    F[i][i] = Cov[i][i];
    B[i][i] = Cov[i+1][i+1];
    C[i][i] = Cov[i+1][i];
  }

  for (m = 1; m <= Np; ++m) {

/* Set the new reflection coefficient as the ratio of the cross-error to the
   weighted sum of the forward and backward errors.  This formula can be
   expressed as
     k(m) = -Cerr / (g * Berr + (1-g) * Ferr).
   Here g=0.5.  This equal weighting guarantees that the prediction error
   filter is minimum phase.

   The original covariance lattice formulation (Makhoul) uses
     Cerr = C[0][0],  Berr = B[0][0], Ferr = F[0][0].
   The formula is modified in IS-54-B to become
     Cerr = 0.5 * (C[0][0] + C[Np-m][Np-m]),
     Berr = 0.5 * (B[0][0] + B[Np-m][Np-m]),
     Ferr = 0.5 * (F[0][0] + F[Np-m][Np-m]).

   If both the forward error and the backward error are zero, then
   (theoretically) the cross-error should be zero.  With zero error, the
   next reflection coefficient should be chosen to be zero.
*/
    Ferr2 = F[0][0] + F[Np-m][Np-m];
    Berr2 = B[0][0] + B[Np-m][Np-m];
    Cerr2 = C[0][0] + C[Np-m][Np-m];
    denom = 0.5 * (Berr2 + Ferr2);
    if (denom != 0.0)
      rm = (float) (-Cerr2 / denom);
    else
      rm = 0.0F;
    rc[m-1] = rm;

/* Update the correlations F[m](i,j), B[m](i,j) and C[m](i,j)
   Notes:
   - The updates do not occur for m = Np
   - The updates are arranged so as to allow for in-place computations
*/
    for (i = 0; i < Np-m; ++i) {
      for (j = 0; j < i; ++j) {
        F[i][j] = F[i][j] + rm * (C[i][j] + C[j][i] + rm * B[i][j]);
        B[i][j] = B[i+1][j+1] + rm * (C[i+1][j+1] + C[j+1][i+1]
                                      + rm * F[i+1][j+1]);
        C[i][j] = C[i][j+1] + rm * (F[i][j+1] + B[i][j+1] + rm * C[j+1][i]);
        C[j][i] = C[j][i+1] + rm * (F[i+1][j] + B[i+1][j] + rm * C[i+1][j]);
      }
      F[i][i] = F[i][i] + rm * (C[i][i] + C[i][i] + rm * B[i][i]);
      B[i][i] = B[i+1][i+1] + rm * (C[i+1][i+1] + C[i+1][i+1]
                                    + rm * F[i+1][i+1]);
      C[i][i] = C[i][i+1] + rm * (F[i+1][i] + B[i+1][i] + rm * C[i+1][i]);
    }
  }

  if (Np > 0) {
    /* Convert to predictor coefficients */
    SPrcXpc (rc, pc, Np);

    /* Update to get the forward error F[m](0,0) */
    Ferr = F[0][0] + rm * (C[0][0] + C[0][0] + rm * B[0][0]);

    MAfFreeMat (F);
    UTfree ((void *) rc);
  }
  else
    Ferr = Cov[0][0];

  return Ferr;
}
