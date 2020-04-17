/*
   Test MSdNint

   $Id: tMSdNint.c 1.3 1999/06/04 libtsp-v6r3 $
*/

#include <libtsp.h>


int
main (int argc, const char *argv[])

{
  double x;

  sscanf (argv[1], "%lg", &x);
  printf ("MSdNint (%.5g) = %.5g\n", x, MSdNint (x));

  return 0;
}
