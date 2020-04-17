/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  double **MAdAllocMat (int Nrow, int Ncol)

Purpose:
  Allocate a matrix of double values

Description:
  This routine allocates a matrix of double values.  The returned value is
  a pointer to an array of pointers to the rows of the matrix.  The calling
  routine can treat the returned value as a two-dimensional matrix of double
  values.  Consider the invocation
    double **A;
    int Nrow, Ncol;
    ...
    A = MAdAllocMat (Nrow, Ncol);
  Then A[i][j] is a matrix of double values for 0 <= i < Nrow and
  0 <= j < Ncol.  The routine MAdAllocMat allocates an array of Nrow pointers
  and a contiguous block of memory sufficient to hold the Nrow*Ncol double
  values.  Each pointer is initialized to point to a row of the matrix.  A[0]
  is a pointer to the first row (Ncol double values) (and to the full matrix of
  Nrow*Ncol double values).  A[1] points to the next row (Ncol elements beyond
  A[0]), and so on.

  The space allocated by this routine should be deallocated using the routine
  MAdFreeMat.

Parameters:
  <-  double **MAdAllocMat
      Returned pointer to array of row pointers
   -> int Nrow
      Number of rows for the matrix
   -> int Ncol
      Number of elements in each row (number of columns)

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.7 $  $Date: 2003/05/09 01:45:58 $

-------------------------------------------------------------------------*/

#include <libtsp.h>


double **
MAdAllocMat (int Nrow, int Ncol)

{
  double **A;
  int i;

/* Allocate the row pointers */
  A = (double **) UTmalloc (Nrow * sizeof (double *));

/* Allocate the matrix of double values */
  A[0] = (double *) UTmalloc (Nrow * Ncol * sizeof (double));

/* Set up the pointers to the rows */
  for (i = 1; i < Nrow; ++i)
    A[i] = A[i-1] + Ncol;

  return A;
}
