/*
   Test FIgdelFIR

   $Id: tFIgdelFIR.c 1.3 1999/06/04 libtsp-v6r3 $
*/

#include <libtsp.h>

#define PI	3.14159265358979323846

#define MAXCOF	51


int
main (int argc, const char *argv[])

{
  int k, Ncof;
  double f, gdel;
  float h[MAXCOF];

  sscanf (argv[1], "%lg", &f);

  for (k = 2; k < argc; ++k) {
    sscanf (argv[k], "%g", &h[k-2]);
  }
  Ncof = k - 2;
  if (Ncof >=  MAXCOF)
    UThalt ("Invalid number of filter coefficients");

  gdel = FIgdelFIR (2.0 * PI * f, h, Ncof);
  printf ("tFIgdelFIR - group delay = %g\n", gdel);

  return 0;
}
