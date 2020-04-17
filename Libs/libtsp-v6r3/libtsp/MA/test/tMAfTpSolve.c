/*
   Test MAfTpSolve

   $Id: tMAfTpSolve.c 1.3 1999/06/04 libtsp-v6r3 $
*/

#include <libtsp.h>

#define NMAX 10


int
main (int argc, const char *argv[])

{
  int N;
  int i, k;
  int ier;
  float R[NMAX];
  float b[NMAX];
  float c[NMAX];

  k = 1;
  N = argc / 2;
  for (i = 0; i < N; ++i, ++k)
    sscanf (argv[k], "%g", &R[i]);
  VRfPrint (stdout, "Input matrix (first row):", R, N);

  for (i = 0; i < N; ++i, ++k)
    sscanf (argv[k], "%g", &b[i]);
  VRfPrint (stdout, "Input vector:", b, N);

  ier = MAfTpSolve (R, b, c, N);
  if (ier != 0)
    printf ("Error code: %d\n", ier);
  VRfPrint (stdout, "Solution vector:", c, N);

  return ier;
}
