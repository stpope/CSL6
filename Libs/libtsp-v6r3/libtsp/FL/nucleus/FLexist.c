/*------------ Telecommunications & Signal Processing Lab -------------
                         McGill University

Routine:
  int FLexist (const char Fname[])

Purpose:
  Determine if a file exists

Description:
  This routine determines if a file specified by its filename exists and is a
  regular file (not a directory, pipe or device).

Parameters:
  <-  int FLexist
      Return value, 1 for an existing regular file, 0 otherwise
   -> const char Fname[]
      File name

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.10 $  $Date: 2003/05/09 01:39:26 $

----------------------------------------------------------------------*/

#include <sys/types.h>
#include <sys/stat.h>

#include <libtsp/nucleus.h>

#ifndef S_ISREG		/* Defined by POSIX */
#  define S_ISREG(m)	(((m) & S_IFMT) == S_IFREG)
#endif


int
FLexist (const char Fname[])

{
  struct stat Fstat;
  int status;

  status = stat (Fname, &Fstat);
  if (status == 0 && S_ISREG (Fstat.st_mode))
    return 1;
  else
    return 0;
}
