/*------------ Telecommunications & Signal Processing Lab -------------
                         McGill University

Routine:
  int FLterm (FILE *fp)

Purpose:
  Determine if a file pointer is associated with a terminal

Description:
  This routine determines if a file specified by its file pointer is associated
  with a terminal device.

Parameters:
  <-  int FLterm
      Return value, 1 for a terminal, 0 otherwise
   -> FILE *fp
      File pointer

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.14 $  $Date: 2003/05/09 01:39:26 $

----------------------------------------------------------------------*/

#include <stdio.h>	/* fileno (Section 8.2.1.1 of Posix) */

#include <libtsp/nucleus.h>
#include <libtsp/sysOS.h>

#if (SY_POSIX)
#  include <unistd.h>	/* isatty */
#else
#  include <io.h>
#endif


int
FLterm (FILE *fp)

{
  return (isatty (fileno (fp)) != 0);
}
