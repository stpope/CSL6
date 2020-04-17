/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  char *UTgetHost (void)

Purpose:
  Get the host name

Description:
  This routine returns the host machine name.  Under MSDOS, a zero-length
  string is returned.

Parameters:
  <-  char UTgetHost[]
      Pointer to a character string containing the host name.  This string is
      null terminated.  This is a pointer to an internal static storage area;
      each call to this routine overlays this storage.

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.18 $  $Date: 2003/05/09 03:23:41 $

-------------------------------------------------------------------------*/

#include <stdlib.h>	/* getenv */
#include <string.h>

#include <libtsp.h>
#include <libtsp/nucleus.h>
#include <libtsp/sysOS.h>
#include <libtsp/UTmsg.h>

#define NELEM(array)	((int) ((sizeof array) / (sizeof array[0])))

#if (SY_POSIX)
#  include <sys/param.h>		/* MAXHOSTNAMELEN on many systems */
#  ifndef MAXHOSTNAMELEN
#    include <netdb.h>			/* For Sun Solaris */
#  endif
#  include <sys/utsname.h>		/* uname */
#endif

#ifndef MAXHOSTNAMELEN
#  define MAXHOSTNAMELEN	257	/* Last resort */
#endif

static const char *HostEnv[] = { "HOST", "HOSTNAME", "COMPUTERNAME" };
#define NENV	NELEM(HostEnv)


char *
UTgetHost (void)

{
  static char Host[MAXHOSTNAMELEN];
  char *p;
  int i;

#if (SY_POSIX)
  struct utsname UnameStruct;
#endif

  Host[0] = '\0';

#if (SY_POSIX)
  if (uname (&UnameStruct) >= 0)
    STcopyMax (UnameStruct.nodename, Host, MAXHOSTNAMELEN-1);
#endif

  for (i = 0; i < NENV && Host[0] == '\0'; ++i) {
    p = getenv (HostEnv[i]);
    if (p != NULL)
      STcopyMax (p, Host, MAXHOSTNAMELEN-1);
  }

  return Host;
}
