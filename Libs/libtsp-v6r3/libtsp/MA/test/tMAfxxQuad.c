/*
  Test MAfSyQuad MAfSyBilin MAfTpQuad

  $Id: tMAfxxQuad.c 1.3 1999/06/04 libtsp-v6r3 $
*/

#include <libtsp.h>
 
#define NMAX 10
 

int
main (int argc, const char *argv[])
 
{
  int n, N;
  int i, j, k;
  int Tp;
  float x[NMAX];
  float **A;
  float val;
  double dval;
 
  k = 1;
  n = argc - 1;
  for (N = 1; N <=NMAX; ++N) {
    if (N*N + N >= n)
      break;
  }
  if (N*N + N != n)
    UThalt ("tMAfxxQuad: Invalid number of data values");

  A = (float **) MAfAllocMat (N, N);
  for (i = 0; i < N; ++i) {
    for (j = 0; j < N; ++j) {
      sscanf (argv[k], "%g", &val);
      ++k;
      A[i][j] = val;
    }
  }
  MAfPrint (stdout, "Input matrix:", (const float **) A, N, N);

  /* Check for symmetry */
  for (i = 0; i < N; ++i) {
    for (j = 0; j < N; ++j) {
      if (A[i][j] != A[j][i])
	UThalt ("tMAfxxQuad:  Input matrix not symmetric");
    }
  }
 
  /* Check for Toeplitz */
  Tp = 1;
  for (i = 0; i < N-1; ++i) {
    for (j = 0; j < N-1; ++j) {
      if (A[i][j] != A[i+1][j+1]) {
	Tp = 0;
	break;
      }
    }
  }

  for (i = 0; i < N; ++i, ++k)
    sscanf (argv[k], "%g", &x[i]);
  VRfPrint (stdout, "Input vector:", x, N);

  dval = MAfSyQuad ((const float **) A, x, N);
  printf ("MAfSyQuad : %g\n", dval);

  dval = MAfSyBilin ((const float **) A, x, x, N);
  printf ("MAfSyBilin: %g\n", dval);
 
  if (Tp == 1) {
    dval = MAfTpQuad (A[0], x, N);
    printf ("MAfTpQuad : %g\n", dval);
  }

  return 0;
}
