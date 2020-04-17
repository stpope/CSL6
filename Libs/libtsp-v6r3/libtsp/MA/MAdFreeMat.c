/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  void MAdFreeMat (double *A[])

Purpose:
  Free an allocated double matrix

Description:
  This routine frees the space occupied by a double matrix.  This routine frees
  up space allocated by routine MAdAllocMat.

Parameters:
   -> double *A[]
      Pointer to an array of row pointers.  If A is NULL, no action is taken.

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.9 $  $Date: 2003/05/09 01:45:58 $

-------------------------------------------------------------------------*/

#include <libtsp.h>


void
MAdFreeMat (double *A[])

{
  if (A != NULL) {
    UTfree ((void *) A[0]);
    UTfree ((void *) A);
  }

  return;
}
