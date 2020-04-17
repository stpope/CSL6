/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  void *UTrealloc (void *ptr, int size)

Purpose:
  Reallocate a block of memory

Description:
  This routine invokes the system routine realloc to reallocate a block of
  memory.  Given a pointer to a block of memory allocated by UTmalloc or
  UTrealloc, this routine returns a pointer to a (possiby moved) block of
  memory of the given size.  The contents will be unchanged up to the lesser of
  the new and old sizes.  Unlike the system routine realloc, this routine uses
  a signed value for the size of the block of memory requested.  If the size
  requested is less than or equal to zero, any previous memory allocated is
  freed up and a NULL pointer is returned.  This is not an error condition.
  However, if the system routine realloc returns an error, an error message is
  printed and execution is terminated.  The block of memory allocated can be
  freed up using UTfree.

Parameters:
  <-  void *UTrealloc
      Pointer to the new allocated memory.  This should be cast to the
      appropriate pointer type.
   -> void *ptr
      Pointer to the old block of memory.  If ptr is NULL, this routine does
      the initial allocation.
   -> int size
      Size of the new memory block to be allocated.  This is a signed value.
      If size is less than or equal to zero, a NULL pointer is returned.

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.11 $  $Date: 2003/05/09 03:20:37 $

-------------------------------------------------------------------------*/

#include <stdlib.h>		/* malloc */

#include <libtsp.h>


void *
UTrealloc (void *ptr, int size)

{
  void *p;

  if (size <= 0) {
    UTfree (ptr);
    p = NULL;
  }
  else if (ptr == NULL) {
    p = malloc ((size_t) size);	/* For non-ANSI versions of realloc */
    if (p == NULL)
      UTerror ("UTrealloc: Error return from malloc");
  }
  else {
    p = realloc ((char *) ptr, (size_t) size);	/* Sun CC needs cast */
    if (p == NULL)
      UTerror ("UTrealloc: Error return from realloc");
  }
  return p;
}
