/*
   Test MAfChSolve

   $Id: tMAfChSolve.c 1.4 1999/06/04 libtsp-v6r3 $
*/

#include <libtsp.h>

#define NMAX 10


int
main (int argc, const char *argv[])

{
  int N;
  int i, j, k;
  int ier;
  float b[NMAX];
  float x[NMAX];
  float **A;

  for (N = 1; N <= NMAX; ++N) {
    if (N*N + N == argc-1)
      break;
  }
  if (N*N + N != argc-1)
    return 1;

  A = MAfAllocMat (N, N);

  k = 1;
  for (i = 0; i < N; ++i) {
    for (j = 0; j < N; ++j, ++k)
      sscanf (argv[k], "%g", &A[i][j]);
  }
  MAfPrint (stdout, "Input matrix:", (const float **) A, N, N);

  for (i = 0; i < N; ++i, ++k)
    sscanf (argv[k], "%g", &b[i]);
  VRfPrint (stdout, "Input vector:", b, N);

  ier = MAfChSolve ((const float **) A, b, x, N);
  VRfPrint (stdout, "Solution vector:", x, N);

  MAfFreeMat (A);

  return 0;
}
