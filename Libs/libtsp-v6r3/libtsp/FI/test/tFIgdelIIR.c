/*
   Test FIgdelIIR

   $Id: tFIgdelIIR.c 1.3 1999/06/04 libtsp-v6r3 $
*/

#include <libtsp.h>

#define PI	3.14159265358979323846

#define MAXSEC	5


int
main (int argc, const char *argv[])

{
  int i, k, ii, jj, Nsec;
  double f, gdel;
  float h[MAXSEC][5];

  sscanf (argv[1], "%lg", &f);

  ii = 0;
  jj = 0;
  for (k = 2, i = 0; k < argc; ++k, ++i) {
    ii = i / 5;
    jj = i % 5;
    sscanf (argv[k], "%g", &h[ii][jj]);
  }
  Nsec = ii + 1;
  if (Nsec >=  MAXSEC || jj != 4)
    UThalt ("Invalid number of filter coefficients");

  gdel = FIgdelIIR (2.0 * PI * f, (const float (*)[5]) h, Nsec);
  printf ("tFIgdelIIR - group delay = %g\n", gdel);

  return 0;
}
