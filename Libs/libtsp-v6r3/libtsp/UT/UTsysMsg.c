/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  void UTsysMsg (const char Warnmsg[], ...)

Purpose:
  Print user and system warning messages

Description:
  This routine prints the system error message corresponding to the last system
  error encountered.  It first prints a user supplied text, and then prints
  the system error message.  Messages are written to stderr (standard error).

  The system error message is printed using perror.  The preamble to the system
  error message is either the name of this routine or the string supplied to
  UTsetProg.  An example of the use of this routine is as follows.
    UTsetProg ("XXProg");
    ...
    fp = fopen (Fname, ...);
    if (fp == NULL)
      UTsysMsg ("XXProc - Cannot open file \"%s\"", Fname);
  If fopen fails, a typical output to stderr would be: 
    XXProc - Cannot open file "abc"
    XXProg: No such file or directory

Parameters:
   -> const char Warnmsg[]
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
  $Revision: 1.8 $  $Date: 2003/05/09 03:20:37 $

-------------------------------------------------------------------------*/

#include <errno.h>
#include <stdarg.h>		/* ANSI C variable-length argument list */

#include <libtsp.h>


void
UTsysMsg (const char Warnmsg[], ...)

{
  va_list ap;
  char *p;

  va_start (ap, Warnmsg);

/* Print the warning message */
  vfprintf (stderr, Warnmsg, ap);
  fprintf (stderr, "\n");

  va_end (ap);

/* Print the text for the system error message */
  if (errno) {
    p = UTgetProg ();
    if (*p != '\0')
      perror (p);
    else
      perror ("UTsysMsg");
  }

  return;
}
