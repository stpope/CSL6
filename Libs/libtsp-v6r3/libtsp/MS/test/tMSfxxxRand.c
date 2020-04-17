/*
   Test MSfUnifRand and MSfGaussRand

   $Id: tMSfxxxRand.c 1.4 1999/06/05 libtsp-v6r3 $
*/

#include <math.h>

#include <libtsp.h>

#define MINV(a, b)	(((a) < (b)) ? (a) : (b))
#define NMAX 10000


int
main (int argc, const char *argv[])

{
  int i, N, seed;
  float x[NMAX];
  const char *dist;
  float sum, sumsq, amin, amax;
  float mean, sd;

  dist = argv[1];
  sscanf (argv[2], "%d", &seed);
  sscanf (argv[3], "%d", &N);
  if (N <= 0 || N > NMAX)
    UThalt ("Invalid number of points");

  if (seed >= 0)
    MSrandSeed (seed);

  sum = 0.0;
  sumsq = 0.0;
  amin = 1e20;
  amax = -1e20;
  for (i = 0; i < N; ++i) {
    if (dist[0] == 'U')
      x[i] = MSfUnifRand ();
    else if (dist[0] == 'G')
      x[i] = MSfGaussRand (1.0);
    else
      UThalt ("Invalid distribution code");
    sum += x[i];
    sumsq += x[i] * x[i];
    if (amin > x[i])
      amin = x[i];
    if (amax < x[i])
      amax = x[i];
  }

  if (seed == 0)
    printf ("tMSfxxxRand: time-based sequence, %s\n", UTdate (3));
  VRfPrint (stdout, "tMSfxxxRand: data values:", x, MINV (5,N));
  mean = sum / N;
  sd = sqrt ((sumsq - sum * sum / N) / (N - 1));
  printf ("  mean, sd: %g, %g\n", mean, sd);
  printf ("  min, max: %g, %g\n", amin, amax);

  return 0;
}
