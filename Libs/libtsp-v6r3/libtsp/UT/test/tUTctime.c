/*
   Test UTctime

   $Id: tUTctime.c 1.2 1999/06/04 libtsp-v6r3 $
*/

#include <time.h>

#include <libtsp/nucleus.h>

int
main (int argc, const char *argv[])

{
  time_t timer;
  long int itimer;

  sscanf (argv[1], "%ld", &itimer);
  timer = itimer;

  printf ("UTctime: %s\n", UTctime (&timer, 0));
  printf ("UTctime: %s\n", UTctime (&timer, 1));
  printf ("UTctime: %s\n", UTctime (&timer, 2));
  printf ("UTctime: %s\n", UTctime (&timer, 3));

  return 0;
}
