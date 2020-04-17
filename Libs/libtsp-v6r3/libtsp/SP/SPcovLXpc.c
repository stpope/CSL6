/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  double SPcovLXpc (const float *Cov[], float pc[], int Np)

Purpose:
  Find predictor coefficients using the covariance lattice method

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
  where x(i) is the input signal.  This routine finds the predictor
  cofficients.

  This procedure is based on Makhoul's covariance lattice method.  The
  reflection coefficients are chosen to minimize the sum of the forward and
  backward prediction errors.

  The computation uses a correlation matrix Cov with elements defined as

    Cov(i,j) = E[x(k-i) x(k-j)],  for 0 <= i,j <= Np.

  The expectation operator E[.] is often replaced by a sum over k over a finite
  interval.  Minimization of the prediction error over this interval defines
  the so-called covariance method for determining the linear prediction
  coefficients.

  Reference:
  1: J. Makhoul, "Stable and efficient lattice methods for linear prediction",
     IEEE Trans. Acoustics, Speech, Signal Processing, vol. ASSP-25, pp.
     423-428, October 1977.

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
  $Revision: 1.9 $  $Date: 2003/05/09 02:45:14 $

-------------------------------------------------------------------------*/

#include <libtsp.h>

#define GAMMA	0.5

static double
SP_cerr (const float a[], const float *Cov[], const float ar[], int N);


double
SPcovLXpc (const float *Cov[], float pc[], int Np) 

{
  int m;
  float *rc, *a, *ar;
  float **Cpp;
  double Ferr, Berr, Cerr;
  double denom;

  Cpp = MAfSubMat (Cov, 1, 1, Np);
  rc = (float *) UTmalloc ((Np+2*(Np+1)) * sizeof (float));
  a = rc + Np;
  ar = a + Np+1;

  for (m = 1; m <= Np; ++m) {

/* Calculate the forward, backward and cross errors */
    SPpcXec (pc, a, m-1);
    VRfRev (a, ar, m);
    Ferr = MAfSyQuad (Cov, a, m);
    Berr = MAfSyQuad ((const float **) Cpp, ar, m);
    Cerr = SP_cerr (a, Cov, ar, m);

/*
  New reflection coefficient
  If both Berr and Ferr are zero, then (theoretically) Cerr is also zero.
  With zero error, the next reflection coefficient should be chosen to be zero.
*/
    denom = GAMMA * Berr + (1.0-GAMMA) * Ferr;
    if (denom != 0.0)
      rc[m-1] = (float) (-Cerr / denom);
    else
      rc[m-1] = 0.0F;

/* Update the direct form coefficients */
    SPrcXpc (rc, pc, m);
  }

/* Return the forward error */
  SPpcXec (pc, a, Np);
  Ferr = MAfSyQuad (Cov, a, Np+1);

  UTfree ((void *) rc);
  UTfree ((void *) Cpp);

  return Ferr;
}

/*
  This routine calculates the cross error term.  The present implementation
  does not take advantage of the symmetry of the underlying matrix to reduce
  the computational complexity, though it does use the symmetry to ensure that
  only the lower triangular part of the matrix Cov is accessed.
  */


static double
SP_cerr (const float a[], const float *Cov[], const float ar[], int N)

{
  int i, j;
  double sum;

  sum = 0.0;
  for (i = 0; i < N; ++i) {
    for (j = 0; j < i; ++j)
      sum += a[i] * Cov[i][j+1] * ar[j];
    for (j = i; j < N; ++j)
      sum += a[i] * Cov[j+1][i] * ar[j];
  }
  return sum;
}
