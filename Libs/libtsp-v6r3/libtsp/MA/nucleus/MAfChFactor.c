/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  int MAfChFactor (const float *A[], float *L[], int N);

Purpose:
  Triangular factorization of a positive definite matrix

Description:
  This routine uses the Cholesky decomposition algorithm to produce the
  triangular decomposition of the given positive definite matrix.

    A = L U,  where U = L' and L and U are triangular matrices.

  This routine requires:
         N       square roots,
     (N^2-N)/2   divides, and
    (N^3-NN)/6   multiplies and adds.

Parameters:
  <-  int MAfChSolve
      Error code.
      ier = 0, No error
      ier = k, The decomposition of matrix A failed.  The matrix A is not
               positive definite, possibly due to loss of significance.  The
	       value k indicates that the square of the diagonal element
	       L[k-1][k-1] is non-positive.  All of the elements of L up to
	       and including the k-1'st row and k-1'st column are filled in
	       as computed.  The remaining elements each row and column are
	       zeroed.
   -> const float *A[]
      A is an array of pointers to the rows of an N by N matrix.  The matrix is
      assumed to be symmetric and positive definite.  Only the lower triangular
      portion of the matrix is accessed.
  <-  float *L[]
      L is an array of pointers to the rows of an N by N matrix.  On return,
      the elements of the matrix are set to the lower triangular matrix
      representing one component of the factorization of matrix A.  The other
      component is the transpose of matrix L.
   -> int N
      Size of the matrix and the vectors

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.11 $  $Date: 2003/05/09 01:47:19 $

-------------------------------------------------------------------------*/

#include <math.h>

#include <libtsp/nucleus.h>


int
MAfChFactor (const float *A[], float *L[], int N)

{
  int i, j, k;
  int ier;
  double sum;

/* Cholesky decomposition
           N-1
  a(i,j) = SUM l(i,k) u(k,j)         (A = L U)
           k=0

           N-1
         = SUM l(i,k) l(j,k)         (U = L')
           k=0

          min(i,j)
         = SUM l(i,k) l(j,k)         (l(i,j)=0 for j > i)
           k=0

  Solve this equation for L iteratively, i=0, 1, ...

                     j-1
  l(i,j) = [a(i,j) - SUM l(i,k) l(j,k)] / l(j,j) ,  for j <= i
                     k=0
*/

  for (i = 0; i < N; ++i) {

    /* Off-diagonal elements */
    for (j = 0; j < i; ++j) {
      sum = A[i][j];
      for (k = 0; k < j; ++k)
	sum = sum - L[i][k] * L[j][k];
      L[i][j] = (float) sum / L[j][j];
      L[j][i] = 0.0F;
    }

    /* Diagonal elements */
    sum = A[i][i];
    for (k = 0; k < i; ++k)
      sum = sum - L[i][k] * L[i][k];
    if (sum <= 0.0)
      break;
    L[i][i] = (float) sqrt (sum);
  }

/* Normal return */
  if (i >= N)
    return 0;

/* Incomplete factorization, zero the remainder of L */
  ier = i + 1;
  for (; i < N; ++i) {
    for (j = 0; j < i; ++j) {
      L[i][j] = 0.0F;
      L[j][i] = 0.0F;
    }
    L[i][i] = 0.0F;
  }

  return ier;
}
