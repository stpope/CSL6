/*
  Test FNgcd

  $Id: tFNgcd.c 1.2 1999/06/04 libtsp-v6r3 $
*/

#include <libtsp.h>


int
main (int argc, const char *argv[])

{
  long int u, v, gcd;

/* Input value */
  sscanf (argv[1], "%ld", &u);
  sscanf (argv[2], "%ld", &v);

  gcd = FNgcd (u, v);
  printf ("FNgcd: gcd (%ld,%ld) -> %ld\n", u, v, gcd);

  return 0;
}
