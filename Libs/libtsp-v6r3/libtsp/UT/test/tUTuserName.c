/*
   Test UTuserName

   $Id: tUTuserName.c 1.3 1999/06/04 libtsp-v6r3 $
*/

#include <libtsp.h>

/*
 This routine should be tested under a number of circumstances
 - login with an entry in /etc/utmp (say through a telnet login)
 - login with an entry in /etc/utmp, then a su to another user
 - login without an entry in /etc/utmp (say xterm -ut)
 - login without an entry in /etc/utmp, then su to another user
*/


int
main (int argc, const char *argv[])

{
  printf ("UTuserName: %s\n", UTuserName());

  return 0;
}
