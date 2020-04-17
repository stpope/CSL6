/*
   Test FIxConvol

   $Id: tFIxConvol.c 1.4 2005/02/01 libtsp-v6r3 $
*/

#include <string.h>

#include <assert.h>
#include <string.h>

#include <libtsp.h>

#define NMAX	128


int
main (int argc, const char *argv[])

{
  int i, k;
  int Nx, Nh, Nout;
  double xd[NMAX];
  double hd[NMAX];
  float xs[NMAX];
  float hs[NMAX];
  const char *Routine;

  Routine = argv[1];

  k = 2;
  for (i = 0; k < argc - 1; ++k, ++i) {
    if (strcmp (argv[k], "//") == 0)
      break;
    sscanf (argv[k], "%lg", &xd[i]);
    xs[i] = xd[i];
  }
  Nx = i;

  for (++k, i = 0; k < argc; ++k, ++i) {
    sscanf (argv[k], "%lg", &hd[i]);
    hs[i] = hd[i];
  }
  Nh = i;

  if (strcmp (Routine, "FIdConvol") == 0) {
    VRdPrint (stdout, "x:", xd, Nx);
    VRdPrint (stdout, "h:", hd, Nh);
    Nout = Nx - (Nh-1);
    FIdConvol (xd, xd, Nout, hd, Nh);
    VRdPrint (stdout, "x * h:", xd, Nx);
  }
  else if (strcmp (Routine, "FIfConvol") == 0) {
    VRfPrint (stdout, "x:", xs, Nx);
    VRfPrint (stdout, "h:", hs, Nh);
    Nout = Nx - (Nh-1);
    FIfConvol (xs, xs, Nout, hs, Nh);
    VRfPrint (stdout, "x * h:", xs, Nx);
  }
  else
    assert (0);

  return 0;
}
