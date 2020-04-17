/*
   Test SPfQuantX

   $Id: tSPfQuantX.c 1.5 2001/03/07 libtsp-v6r3 $
*/

#include <string.h>

#include <libtsp.h>

#define NMAX	128


int
main (int argc, const char *argv[])

{
  const char *Opt;
  float x[NMAX], Xq[NMAX];
  int k, i, Nx, N;

  k = 1;
  Opt = argv[k];
  ++k;
  for (i = 0; k < argc; ++k, ++i) {
    if (strcmp (argv[k], "//") == 0)
      break;
    sscanf (argv[k], "%g", &x[i]);
  }
  Nx = i;

  for (++k, i = 0; k < argc; ++k, ++i)
    sscanf (argv[k], "%g", &Xq[i]);
  N = i;

  VRfPrint (stdout, "Decision Levels:", Xq, N);
  for (i = 0; i < Nx; ++i) {
    if (strcmp (Opt, "QL") == 0)
      printf ("SPfQuantL: %.5g -> %d\n", x[i], SPfQuantL (x[i], Xq, N+1));
    else if (strcmp (Opt, "QU") == 0)
      printf ("SPfQuantU: %.5g -> %d\n", x[i], SPfQuantU (x[i], Xq, N+1));
  }

  return 0;
}
