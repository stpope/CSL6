/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  void *UTmalloc (int size)

Purpose:
  Allocate a block of memory

Description:
  This routine invokes the system routine malloc to allocate a block of memory.
  Unlike the system routine malloc, this routine uses a signed value for
  the size of the block of memory requested.  If the size requested is less
  than or equal to zero, a NULL pointer is returned.  This is not an error
  condition.  However, if the system routine malloc returns an error, an
  error message is printed and execution is terminated.  The block of memory
  allocated can be freed up using UTfree.

Parameters:
  <-  void *UTmalloc
      Pointer to the allocated memory.  This should be cast to the appropriate
      pointer type.
   -> int size
      Size of the memory block to be allocated.  This is a signed value.  If
      size is less than or equal to zero, a NULL pointer is returned.

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.10 $  $Date: 2003/05/09 03:20:37 $

-------------------------------------------------------------------------*/

#include <stdlib.h>		/* malloc */

#include <libtsp.h>


void *
UTmalloc (int size)

{
  void *p;

  if (size <= 0)
    p = NULL;
  else {
    p = malloc ((size_t) size);
    if (p == NULL)
      UTerror ("UTmalloc: Error return from malloc");
  }
  return p;
}
