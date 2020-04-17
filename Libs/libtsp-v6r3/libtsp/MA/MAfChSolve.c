/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  int MAfChSolve (const float *A[], const float b[], float x[], int N);

Purpose:
  Solve a positive definite set of equations

Description:
  This routine uses the Cholesky decomposition algorithm to solve a set of
  equations specified by a positive definite matrix.  Cholesky decomposition is
  used to express the matrix A in terms of a product of upper and lower
  triangular matrices,

    A = L U,  where U = L' and L and U are triangular matrices.

  The set of equations A x = b, becomes

    L y = b, where y = U x .

  The first triangular system of equations is solved by substitution to get
  the vector y.  The vector y is then used to solve the second triangular set
  of equations to get the solution vector x.

  This routine requires
            N         square roots,
       (N^2+3N)/2     divides, and
    (N^3+6N^2-7N)/6   multiplies and adds.

Parameters:
  <-  int MAfChSolve
      Error code, zero for no error. If the error code is nonzero, the
      decomposition of matrix A failed.  The matrix A is not positive definite,
      possibly due to loss of significance.  The value k indicates that the
      factorization failed at step k.  The equations are solved for the (k-1)
      by (k-1) sub-system, ignoring the rest of the matrix A and the rest of
      the vector b.  The solution appears as the first k-1 elements of vector
      x.  The remaining elements of x are set to zero.
   -> const float *A[]
      A is an array of pointers to the rows of an N by N matrix.  The matrix is
      assumed to be symmetric and positive definite.  Only the lower triangular
      portion of the matrix is accessed.  Note that with ANSI C, if the actual
      parameter is not declared to have the const attribute, an explicit cast
      to (const float **) is required.
   -> const float b[]
      Right hand side vector (N elements)
  <-  float x[]
      Solution vector (N elements).  The vectors b and x can occupy the same
      storage locations.
   -> int N
      Number of equations

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.12 $  $Date: 2003/05/09 01:45:58 $

-------------------------------------------------------------------------*/

#include <libtsp.h>
#include <libtsp/nucleus.h>


int
MAfChSolve (const float *A[], const float b[], float x[], int N)

{
  float **L;
  int ier;
  int Na;
  int i;

/* Allocate space for the lower triangular matrix L */
  L = MAfAllocMat (N, N);

/* Cholesky decomposition */
  ier = MAfChFactor (A, L, N);
  Na = N;
  if (ier != 0)
    Na = ier - 1;

/* Solve L y = b; the solution appears in x */
  MAfLTSolve ((const float **) L, b, x, Na);

/* Solve U x = y; the solution replaces the values in x */
  MAfTTSolve ((const float **) L, x, x, Na);

/* Release the space for L */
  MAfFreeMat (L);

/* Zero out the solution for Na <= i < N */
  for (i = Na; i < N; ++i)
    x[i] = 0.0;

  return ier;
}
