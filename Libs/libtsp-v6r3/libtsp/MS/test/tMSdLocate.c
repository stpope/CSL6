/*
   Test MSdLocate

   $Id: tMSdLocate.c 1.3 2003/05/08 libtsp-v6r3 $
*/

#include <string.h>
#include <libtsp.h>

#define NMAX	128


int
main (int argc, const char *argv[])

{
  double x[NMAX], T[NMAX];
  int k, i, Nx, N;

  k = 1;
  for (i = 0; k < argc; ++k, ++i) {
    if (strcmp (argv[k], "//") == 0)
      break;
    sscanf (argv[k], "%lg", &x[i]);
  }
  Nx = i;

  for (++k, i = 0; k < argc; ++k, ++i)
    sscanf (argv[k], "%lg", &T[i]);
  N = i;

  VRdPrint (stdout, "Table Values:", T, N);
  for (i = 0; i < Nx; ++i) {
    printf ("MSdLocate: %.5g -> %d\n", x[i], MSdLocate (x[i], T, N));
  }

  return 0;
}
