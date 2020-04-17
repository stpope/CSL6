/*
   Test FNxxx

   $Id: tFNxxx.c 1.3 1999/06/04 libtsp-v6r3 $
*/

#include <libtsp.h>


int
main (int argc, const char *argv[])

{
  double x;

/* Input value */
  sscanf (argv[2], "%lg", &x);

  if (argv[1][0] == 'B')
    printf ("FNbessI0: %g -> %g\n", x, FNbessI0(x));
  else if (argv[1][0] == 'S')
    printf ("FNsinc: %g -> %g\n", x, FNsinc(x));
  else
    UThalt ("Invalid option");

  return 0;
}
