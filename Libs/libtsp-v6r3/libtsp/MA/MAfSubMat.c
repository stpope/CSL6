/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  float **MAfSubMat (const float *A[], int n, int m, int Nrow)

Purpose:
  Set up pointers to a submatrix of a matrix of float values

Description:
  This routine allocates an array of row pointers.  These pointers access a
  submatrix of a previously allocated matrix.  The returned value is a pointer
  to an array of pointers to the rows of the submatrix.  The calling routine
  can treat the returned value as a two-dimensional matrix of float values.
  Consider the invocation
    float **A;
    float **sA;
    ...
    A = MAfAllocMat (Nrow, Ncol);
    sA = MAfSubMat ((const float **) A, n, m, Nrow-n);
        ...  now sA[0][0] == A[n][m]
  Then sA[i][j] is a matrix of float values for 0 <= i < N-n and
  0 <= j < Ncol-m.  The array of pointers to the submatrix should be
  deallocated using the routine UTfree.

Parameters:
  <-  float **MAfSubMat
      Returned pointer to array of row pointers for the submatrix
   -> const float *A[]
      A is an array of pointers to the rows of an N by N matrix.  Note that
      with ANSI C, if the actual parameter is not declared to have the const
      attribute, an explicit cast to (const float **) is required.
   -> int n
      Index for the first row of the submatrix
   -> int m
      Index for the first column of the submatrix
   -> int Nrow
      Number of rows in the submatrix

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.8 $  $Date: 2003/05/09 01:45:58 $

-------------------------------------------------------------------------*/

#include <libtsp.h>


float **
MAfSubMat (const float *A[], int n, int m, int Nrow)

{
  float **sA;
  int i;

/* Allocate the row pointers */
  sA = (float **) UTmalloc (Nrow * sizeof (float *));

/* Set up the pointers to the rows */
  for (i = 0; i < Nrow; ++i)
    sA[i] = (float *) &A[n+i][m];

  return sA;
}
