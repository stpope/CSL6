/*
   Test STtrim

   $Id: tSTtrim.c 1.4 1999/06/04 libtsp-v6r3 $
*/

#include <libtsp.h>


int
main (int argc, const char *argv[])

{
  int n;
  char string[1024];

  n = STtrim (argv[1], string);
  printf ("STtrim: \"%s\" (%d chars)\n", string, n);

  return 0;
}
