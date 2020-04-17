/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  int MAfTpSolve (const float R[], const float g[], float c[], int N)

Purpose:
  Solve a Toeplitz set of equations

Description:
  This routine solves the set of matrix equations

    R c = g

  where R is an N by N symmetric Toeplitz matrix and c and g are N element
  column vectors.  The Toeplitz matrix R has elements which are equal along the
  diagonals, i.e. R(i,j)=r(abs(i-j)).

  This routine uses Levinson's method to calculate the solution.  In the
  case that either numerical instability or an inappropriate set of matrix
  coefficients results in a matrix which is not positive definite or singular,
  an error code is returned.

  This routine uses
      2N-1    divides, and
    2N^2-3N   multiplies and adds.

  Reference:
      E. A. Robinson, "Multichannel time series analysis with digital computer
      programs", Holden-Day, 1967, p. 44.

Parameters:
  <-  int MAfTpSolve
      Error flag, zero for no error.  The return value is set to 1 if the
      matrix not positive definite.
   -> const float R[]
      Vector specifying the first column of the Toeplitz matrix
   -> const float g[]
      Righthand side vector
  <-  float c[]
      Solution vector
   -> int N
      Number of equations

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.6 $  $Date: 2003/05/09 01:45:58 $

-------------------------------------------------------------------------*/

#include <libtsp.h>


int
MAfTpSolve (const float R[], const float g[], float c[], int N)

{
  int i, j, k;
  float perr, rc, t, sum;
  float *pc;


/* Allocate a temporary array */
  pc = (float *) UTmalloc (N * sizeof (float));

  perr = R[0];
  if (perr <= 0.0)
    return 1;

  c[0] = g[0] / perr;
  for (k = 0; k < N - 1; ++k) {

    sum = R[k+1];
    for (i = 0; i < k; ++i)
      sum = sum - R[k-i] * pc[i];
    rc = -sum / perr;

/*
   Calculate the prediction error (equivalent to perr = perr * (1-rc^2))
   A change in sign of perr means that rc has a magnitude greater than unity
   (corresponding to a non-positive definite system of equations)
*/
    perr = perr + rc * sum;
    if (perr <= 0.0)
      return 1;

    pc[k] = -rc;
    for (i = 0, j = k - 1; i < j; ++i, --j) {
      t = pc[i] + rc * pc[j];
      pc[j] = pc[j] + rc * pc[i];
      pc[i] = t;
    }
    if (i == j)
      pc[i] = pc[i] + rc * pc[i];

    sum = g[k+1];
    for (i = 0, j = k + 1; i <= k; ++i, --j)
      sum = sum - c[i] * R[j];
    c[k+1] = sum / perr;

    for (i = 0, j = k; i <= k; ++i, --j)
      c[i] = c[i] - c[k+1] * pc[j];

  }
/* Deallocate the temporary array */
  UTfree ((void *) pc);

  return 0;
}
