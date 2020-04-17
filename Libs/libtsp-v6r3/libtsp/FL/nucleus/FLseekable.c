/*------------ Telecommunications & Signal Processing Lab -------------
                         McGill University

Routine:
  int FLseekable (FILE *fp)

Purpose:
  Determine if an I/O stream is random access.

Description:
  This routine determines if an I/O stream associated with a given file pointer
  is random access.

Parameters:
  <-  int FLseekable
      Return value, 1 if the file is random access, 0 otherwise
   -> FILE *fp
      File pointer

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.11 $  $Date: 2003/05/09 01:39:26 $

----------------------------------------------------------------------*/

#include <stdio.h>	/* fileno (Section 8.2.1.1 of Posix) */
#include <sys/types.h>
#include <sys/stat.h>

#include <libtsp/nucleus.h>

#ifndef S_ISREG		/* Defined by POSIX */
#  define S_ISREG(m)	(((m) & S_IFMT) == S_IFREG)
#endif


int
FLseekable (FILE *fp)

{
  struct stat Fstat;
  int status;

  status = fstat (fileno (fp), &Fstat);
  if (status == 0 && S_ISREG (Fstat.st_mode))
    return 1;
  else
    return 0;
}
