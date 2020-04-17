/*
   Test MSxConvCof & MSdDeconvolCof

   $Id: tMSxConvCof.c 1.5 2000/12/19 libtsp-v6r3 $
*/

#include <string.h>

#include <libtsp.h>

#define NMAX	128


int
main (int argc, const char *argv[])

{
  int i, k, Nx, Ny, Nz, Nq;
  int Convol;
  float xs[NMAX], ys[NMAX];
  double xd[NMAX], yd[NMAX], qd[NMAX], rd[NMAX];

  k = 1;
  Convol = 1;
  for (i = 0; k < argc - 1; ++k, ++i) {
    if (strcmp (argv[k], "x") == 0) {
      Convol = 1;
      break;
    }
    else if (strcmp (argv[k], "/") == 0) {
      Convol = 0;
      break;
    }
    sscanf (argv[k], "%lg", &xd[i]);
    xs[i] = xd[i];
  }
  Nx = i;

  ++k;
  for (i = 0; k < argc; ++k, ++i) {
    sscanf (argv[k], "%lg", &yd[i]);
    ys[i] = yd[i];
  }
  Ny = i;

  VRdPrint (stdout, "x:", xd, Nx);
  VRdPrint (stdout, "y:", yd, Ny);

  if (Convol) {
    Nz = MSfConvCof (xs, Nx, ys, Ny, xs);
    VRfPrint (stdout, "MSfConvCof: x * y (float):", xs, Nz);

    Nz = MSdConvCof (xd, Nx, yd, Ny, xd);
    VRdPrint (stdout, "MSdConvCof: x * y (double):", xd, Nz);
  }
  else {
    Nq = MSdDeconvCof (xd, Nx, yd, Ny, qd, rd);
    VRdPrint (stdout, "MSdDeconvCof: x / y (double):", qd, Nq);
    VRdPrint (stdout, "MSdDeconvCof: rem (x / y) (double):", rd, Nx);
  }

  return 0;
}
