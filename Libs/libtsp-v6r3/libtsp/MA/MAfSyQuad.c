/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  double MAfSyQuad (const float *A[], const float x[], int N)

Purpose:
  Calculate a quadratic form for a symmetric matrix

Description:
  This routine calculates the quadaratic form s = x'A x for vector x and
  symmetric matrix A,
        N-1 N-1
    s = SUM SUM x(i) A(i,j) x(j) .
        i=0 j=0
  The symmetry of A is used to reduce the number of computations.  The result
  is accumulated as a double value and returned as a double value.

Parameters:
  <-  double MAfSyBilin
      Returned value
   -> const float *A[]
      A is an array of pointers to the rows of an N by N matrix.  The matrix
      is assumed to be symmetric.  Only the lower triangular portion of the
      matrix is accessed.  Note that with ANSI C, if the actual parameter
      is not declared to have the const attribute, an explicit cast to
      (const float **) is required.
   -> const float x[]
      Input vector (N elements)
   -> int N
      Number of of elements in x and y and the size of A

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.5 $  $Date: 2003/05/09 01:45:58 $

-------------------------------------------------------------------------*/

#include <libtsp.h>


double
MAfSyQuad (const float *A[], const float x[], int N)

{
  double sum, suma;
  double xi;
  int i, j;

/*
  Let g(i,j) be x(i) A(i,j) x(j).  The double sum defining the quadratic form
  is calculated down the diagonal, then in the lower triangular part, and
  finally in the upper triangular part of g(i,j).

      N-1  N-1          N-1          N-1 i-1          N-1 j-1
   S= SUM  SUM g(i,j) = SUM g(i,i) + SUM SUM g(i,j) + SUM SUM g(i,j)
      i=0  j=0          i=0          i=1 j=0          j=1 i=0

  Using the symmetry of g(i,j),

       N-1                      i-1
   S = SUM { x(i)A(i,i)x(i) + 2 SUM x(i)A(i,j)x(j) }
       i=0                      j=0
*/

  sum = 0.0;
  suma = 0.0;
  for (i = 0; i < N; ++i) {
    xi = (double) x[i];
    sum += xi * A[i][i] * xi;
    for (j = 0; j < i; ++j)
      suma += xi * x[j] * A[i][j];
  }
  sum += 2.0 * suma;

  return sum;
}
