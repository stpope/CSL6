/*
   Test MAfPrint

   $Id: tMAfPrint.c 1.5 1999/06/04 libtsp-v6r3 $
*/

#include <libtsp.h>

#define N	2
#define M	23

#define LN2x		0.69314718055994530942e6
#define PIx		3.14159265358979323846e7
#define PI_2x		1.57079632679489661923e8
#define PI_4x		0.78539816339744830962e9

static const float Ap[N][M] = {
  {LN2x, 2e-5, 3e-5, 4e-5, 5e-5,
   LN2x, PIx, 3e-4, 4e-4, 5e-4,
   PI_2x, 2e-4, 3e-4, 4e-4, 5e-4,
   PI_4x, 2e-4, 3e-4, 4e-4, 5e-4,
   1e-4, 2e-4, 3e-4},
  {1e-5, 2e-5, 3e-5, 4e-5, 5e-5,
   1e-4, 2e-4, 3e-4, 4e-4, 5e-4,
   1e-4, 2e-4, 3e-4, 4e-4, 5e-4,
   1e-4, 2e-4, 3e-4, 4e-4, 5e-4,
   1e-4, 2e-4, 3e-4}};


int
main (int argc, const char *argv[])

{
  int i, j;
  float **A;

  A = MAfAllocMat (N, M);
  for (i = 0; i < N; ++i)
    for (j = 0; j < M; ++j)
      A[i][j] = Ap[i][j];

  MAfPrint (stdout, "Matrix A[23][2]:", (const float **) A, N, M);

  return 0;
}
