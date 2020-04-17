/*
  Test FNiLog2

  $Id: tFNiLog2.c 1.3 1999/06/04 libtsp-v6r3 $
*/

#include <libtsp.h>


int
main (int argc, const char *argv[])

{
  int a;
  int L;

/* Input value */
  sscanf (argv[1], "%d", &a);

  L = FNiLog2 (a);
  printf ("FNiLog2: %d -> %d\n", a, L);

  return 0;
}
