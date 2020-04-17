/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  void UTerror (const char Errmsg[], ...)

Purpose:
  Print user and system error messages, stop with error status set

Description:
  This routine first prints a user supplied text.  It then prints the system
  error message corresponding to the last system error encountered.  Messages
  are written to stderr (standard error).  Execution is terminated and the
  exit status is set to EXIT_FAILURE.  Normally this routine is called after a
  system routine has returned an error condition.

  The system error message is printed using perror.  The preamble to the system
  error message is either the name of this routine or the string supplied to
  UTsetProg.  An example of the use of this routine is as follows.
    UTsetProg ("XXProg");
    ...
    fp = fopen (Fname, ...);
    if (fp == NULL)
      UTerror ("XXProc: Cannot open file \"%s\"", Fname);
  If fopen fails, a typical output to stderr would be: 
    XXProc: Cannot open file "abc"
    XXProg: No such file or directory

Parameters:
   -> const char Errmsg[]
      Character string to be printed.  This string can contain optional
      formatting codes.  The arguments corresponding to the formatting codes
      appear at the end of the argument list.  The input string should not
      normally have a terminating newline character, since this routine
      supplies a newline.
   -> <args...>
      Arguments corresponding to the formatting codes.  The format string and
      the variable number of arguments is passed on to the system routine
      vprintf.

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.27 $  $Date: 2003/05/09 03:20:37 $

-------------------------------------------------------------------------*/

#include <errno.h>
#include <stdarg.h>
#include <stdlib.h>

#include <libtsp.h>


void
UTerror (const char Errmsg[], ...)

{
  va_list ap;
  char *p;

  va_start (ap, Errmsg);

/* Print the user error message */
  vfprintf (stderr, Errmsg, ap);
  fprintf (stderr, "\n");

  va_end (ap);

/* Print the text for the system error message */
  if (errno) {
    p = UTgetProg ();
    if (*p != '\0')
      perror (p);
    else
      perror ("UTerror");
  }

  exit (EXIT_FAILURE);
}
