/*
   Test MSdPolyInt

   $Id: tMSdPolyInt.c 1.1 2001/01/29 libtsp-v6r3 $
*/

#include <string.h>

#include <libtsp.h>

#define NMAX	128


int
main (int argc, const char *argv[])

{
  int i, k;
  int N;
  double x, y;
  double xa[NMAX];
  double ya[NMAX];

  k = 1;
  sscanf (argv[k], "%lg", &x);
  ++k;
  for (i = 0; k < argc - 1; ++k, ++i) {
    sscanf (argv[k], "%lg", &xa[i]);
    ++k;
    sscanf (argv[k], "%lg", &ya[i]);
  }
  N = i;

  VRdPrint (stdout, "x reference:", xa, N);
  VRdPrint (stdout, "y reference:", ya, N);

  y = MSdPolyInt (x, xa, ya, N, NULL);
  fprintf (stdout, "MSdPolyInt(%g) = %g\n", x, y);

  return 0;
}
