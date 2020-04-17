/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  double MAfSyBilin (const float *A[], const float x[], const float y[],
                     int N)

Purpose:
  Calculate a bilinear form for a symmetric matrix

Description:
  This routine calculates the bilinear form s = x'A y for vectors x and y, and
  symmetric matrix A,
        N-1 N-1
    s = SUM SUM x(i) A(i,j) y(j) .
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
   -> const float y[]
      Input vector (N elements)
   -> int N
      Number of of elements in x and y and the size of A

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.5 $  $Date: 2003/05/09 01:45:58 $

-------------------------------------------------------------------------*/

#include <libtsp.h>


double
MAfSyBilin (const float *A[], const float x[], const float y[], int N)

{
  double sum;
  double xi, yi;
  int i, j;

/*
  Let g(i,j) be x(i) A(i,j) y(j).  The double sum defining the bilinear form is
  calculated down the diagonal, then in the lower triangular part, and finally
  in the upper triangular part of g(i,j).

        N-1  N-1          N-1          N-1 i-1          N-1 j-1
    S = SUM  SUM g(i,j) = SUM g(i,i) + SUM SUM g(i,j) + SUM SUM g(i,j)
        i=0  j=0          i=0          i=1 j=0          j=1 i=0

  Using the symmetry of A(i,j),

        N-1                    i-1
    S = SUM { x(i)A(i,i)y(i) + SUM [x(i)y(j)+x(j)y(i)] A(i,j) }
        i=0                    j=0
*/

  sum = 0.0;
  for (i = 0; i < N; ++i) {
    xi = (double) x[i];
    yi = (double) y[i];
    sum += xi * A[i][i] * yi;
    for (j = 0; j < i; ++j)
      sum += (xi * y[j] + x[j] * yi) * A[i][j];
  }

  return sum;
}
