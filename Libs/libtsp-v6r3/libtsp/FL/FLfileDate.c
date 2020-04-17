/*------------- Telecommunications & Signal Processing Lab -------------
                          McGill University

Routine:
  char *FLfileDate (FILE *fp, int format)

Purpose:
  Find the last modification time for an open file

Description:
  This routine finds the last modified time for a file specified by the file
  pointer to an open file.  This time is returned as a date/time string in one
  of a number of standard formats.

   Format  Example                      time zone    typical length
     0    Sun Sep 16 01:03:52 1973      local time   24 + null
     1    Sun Sep 16 01:03:52 EST 1973  local time   28* + null
     2    1994-01-23 09:59:53 EST       local time   23* + null
     3    1994-01-23 14:59:53 UTC       GMT          23 + null
               (*) the time zone length can vary

Parameters:
  <-  char *FLfileDate
      Pointer to a character string for the date and time.  This is a pointer
      to an internal static storage area; each call to this routine overlays
      this storage.
   -> FILE *fp
      File pointer for the file
   -> int format
      Date / time format code, taking on values from 0 to 3

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.23 $  $Date: 2003/05/09 01:36:43 $

----------------------------------------------------------------------*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <libtsp.h>
#include <libtsp/nucleus.h>

#define MAXDATE	128


char *
FLfileDate (FILE *fp, int format)

{
  struct stat Fstat;
  int status;
  static char Datetime[MAXDATE+1];

  status = fstat (fileno (fp), &Fstat);
  if (status == 0)
    STcopyMax (UTctime (&Fstat.st_mtime, format), Datetime, MAXDATE);
  else
    Datetime[0] = '\0';

  return Datetime;
}
