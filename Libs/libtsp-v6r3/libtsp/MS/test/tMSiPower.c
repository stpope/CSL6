/*
   Test MSiPower

   $Id: tMSiPower.c 1.3 1999/06/04 libtsp-v6r3 $
*/

#include <libtsp.h>


int
main (int argc, const char *argv[])

{
  int a;
  int n;

  sscanf (argv[1], "%d", &a);
  sscanf (argv[2], "%d", &n);

  printf ("MSiPower (%d, %d) = %d\n", a, n, MSiPower (a, n));

  return 0;
}
