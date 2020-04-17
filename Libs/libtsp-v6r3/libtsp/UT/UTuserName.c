/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  char *UTuserName (void)

Purpose:
  Get the user name and host name

Description:
  This routine returns the user name and host name.  The returned string is of
  the form user@host, with the host or @user parts omitted if unavailable.

Parameters:
  <-  char UTuserName[]
      Pointer to a character string containing the user name.  This string is
      null terminated.  This is a pointer to an internal static storage area;
      each call to this routine overlays this storage.

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.34 $  $Date: 2003/05/09 03:20:37 $

-------------------------------------------------------------------------*/

#include <libtsp.h>
#include <libtsp/nucleus.h>

#if SY_POSIX
#  include <sys/param.h>		/* MAXHOSTNAMELEN on many systems */
#  ifndef MAXHOSTNAMELEN
#    include <netdb.h>			/* For Sun Solaris */
#  endif
#endif

#ifndef MAXHOSTNAMELEN
#  define MAXHOSTNAMELEN	257	/* Last resort */
#endif

#include <limits.h>
#ifndef LOGIN_NAME_MAX
#  define LOGIN_NAME_MAX	17
#endif

#define MAXLEN	MAXHOSTNAMELEN+LOGIN_NAME_MAX-1


char *
UTuserName (void)

{
  int n;
  static char User[MAXLEN];

  n = STcopyMax (UTgetUser (), User, MAXLEN-1);
  User[n] = '@';
  STcopyMax (UTgetHost (), &User[n+1], MAXLEN-(n+1)-1);
  if (User[n+1] == '\0')
    User[n] = '\0';	/* Kill the @ if there is no host name */

  return User;
}
