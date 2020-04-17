/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  char *UTctime (time_t *timer, int format)

Purpose:
  Convert a time value to a date/time string

Description:
  This routine converts a time value to date/time string in a number of
  standard formats.  Formats 0, 1 and 2 are in local time.  Format 3 gives
  the date and time in GMT (Universal Coordinated Time).

  Format 0 is the standard C-language format (without the trailing newline
  character).  Format 1 includes the time zone abbreviation.  Formats 0 and 1
  use abbreviations for the day of the week and the month.  Formats 2 and 3
  avoid language dependent names (except for the time-zone code).

   Format  Example                      time zone    typical length
     0    Sun Sep 16 01:03:52 1973      local time   24 + null
     1    Sun Sep 16 01:03:52 EST 1973  local time   28* + null
     2    1994-01-23 09:59:53 EST       local time   23* + null
     3    1994-01-23 14:59:53 UTC       GMT          23 + null
               (*) the time zone length can vary

Parameters:
  <-  char UTctime[]
      Pointer to a character string for the date and time.  This is a pointer
      to an internal static storage area; each call to this routine overlays
      this storage.
   -> time_t *timer
      Input time value
   -> int format
      Date / time format code, taking on values from 0 to 3

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.22 $  $Date: 2003/05/09 03:49:05 $

-------------------------------------------------------------------------*/

#include <time.h>	/* strftime definition */
			/* tzset (Section 8.3.2.1 of Posix) */

#include <libtsp.h>
#include <libtsp/nucleus.h>

#define MAXDATE	64


char *
UTctime (time_t *timer, int format)

{
  int nc;
  char *stdtime;
  static char Datetime[MAXDATE+1];
  static int InitTZ = 0;

  /* Initialize the time zone setting */
  if (! InitTZ) {
    tzset ();
    InitTZ = 1;
  }

/* Notes:
   - On some systems, the time zone is verbose.  For insance, on Windows,
     with the MSVC compiler, the time zone is of the form
     "Eastern Daylight Time".
   - Setting the TZ environment variable will force the time zone names,
     for instance TZ=EST5EDT with force the use of the 3 letter short forms
     for the time zones.
*/

  switch (format) {

  case (0):
  default:
    stdtime = asctime (localtime (timer));
    STcopyNMax (stdtime, Datetime, 24, MAXDATE);
    break;

  case (1):
    stdtime = asctime (localtime (timer));
    nc = STcopyNMax (stdtime, Datetime, 20, MAXDATE);
    strftime (&Datetime[nc], MAXDATE - nc, "%Z %Y", localtime (timer));
    STtrim (&Datetime[nc], &Datetime[nc]); /* In case time zone is empty */
    break;

  case (2):
    strftime (Datetime, MAXDATE, "%Y-%m-%d %H:%M:%S %Z", localtime (timer));
    STtrim (Datetime, Datetime);	/* In case time zone is empty */
    break;

  case (3):
    strftime (Datetime, MAXDATE, "%Y-%m-%d %H:%M:%S UTC", gmtime (timer));
    break;
  }

  return Datetime;
}
