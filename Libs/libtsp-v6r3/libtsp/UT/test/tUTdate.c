/*
   Test UTdate

   $Id: tUTdate.c 1.3 1999/06/04 libtsp-v6r3 $
*/

#include <libtsp.h>


int
main (int argc, const char *argv[])

{
  printf ("UTdate: %s\n", UTdate (0));
  printf ("UTdate: %s\n", UTdate (1));
  printf ("UTdate: %s\n", UTdate (2));
  printf ("UTdate: %s\n", UTdate (3));

  return 0;
}
