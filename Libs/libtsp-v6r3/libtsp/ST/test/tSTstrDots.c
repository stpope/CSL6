/*
   Test STstrDots

   $Id: tSTstrDots.c 1.2 1999/06/04 libtsp-v6r3 $
*/

#include <libtsp/nucleus.h>


int
main (int argc, const char *argv[])

{
  int M;
  char *p;

  sscanf (argv[2], "%d", &M);

  p = STstrDots (argv[1], M);
  printf ("STstrDots: \"%s\"\n", p);

  return 0;
}
