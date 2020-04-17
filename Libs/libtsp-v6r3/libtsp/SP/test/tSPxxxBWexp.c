/*
   Test SPxxxBWexp

   $Id: tSPxxxBWexp.c 1.2 1999/06/04 libtsp-v6r3 $
*/

#include <string.h>

#include <libtsp.h>

#define NXMAX 20


int
main (int argc, const char *argv[])

{
  int i, k, Nx;
  double b;
  float x[NXMAX];
  float xb[NXMAX];

  sscanf (argv[2], "%lg", &b);

  k = 3;
  Nx = argc - k;
  for (i = 0; i < Nx; ++i, ++k)
    sscanf (argv[k], "%g", &x[i]);

  if (strcmp (argv[1], "P") == 0)
    SPpcBWexp (b, x, xb, Nx);
  else if (strcmp (argv[1], "C") == 0)
    SPcorBWexp (b, x, xb, Nx);
  else
    UThalt ("Invalid option");

  VRfPrint (stdout, "Input values:", x, Nx);
  VRfPrint (stdout, "Output values:", xb, Nx);

  return 0;
}
