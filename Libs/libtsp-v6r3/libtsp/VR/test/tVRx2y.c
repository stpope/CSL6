/*
   Test VR vector to vector routines

   $Id: tVRx2y.c 1.14 2005/02/01 libtsp-v6r3 $
*/

#include <assert.h>
#include <string.h>

#include <libtsp.h>

#define NMAX	128

int
main (int argc, const char *argv[])

{
  double xd[NMAX], zd[NMAX], yd[NMAX];
  float  xs[NMAX], zs[NMAX], ys[NMAX];
  int i, k, Nx, Nz, Ny;
  const char *Routine;
  double xx;

  Routine = argv[1];
  k = 2;

  for (i = 0; k < argc; ++i, ++k) {
    if (strcmp (argv[k], "//") == 0)
      break;
    sscanf (argv[k], "%lg", &xd[i]);
    xs[i] = xd[i];
  }
  Nx = i;

  ++k;
  for (i = 0; k < argc; ++k, ++i) {
    sscanf (argv[k], "%lg", &zd[i]);
   zs[i] = zd[i];
  }
  Nz = i;

  if (Nz > 0)
    Ny = Nz;
  else
    Ny = Nx;

  xx = 999999;
  if (strcmp(Routine, "VRdAdd") == 0)
    VRdAdd (xd, zd, yd, Ny);

  else if (strcmp(Routine, "VRdSymPart") == 0)
    VRdSymPart (xd[0], zd, yd, Ny);

  else if (strcmp(Routine, "VRdCopy") == 0)
    VRdCopy (xd, yd, Ny);

  else if (strcmp(Routine, "VRdCorSym") == 0)
    xx = VRdCorSym (xd, Nx);
 
  else if (strcmp(Routine, "VRdDotProd") == 0)
    xx = VRdDotProd(xd, zd, Nx);

  else if (strcmp(Routine, "VRdLinInc") == 0)
    VRdLinInc (xd[0], xd[1], yd, Ny);

  else if (strcmp(Routine, "VRdMult") == 0)
    VRdMult (xd, zd, yd, Ny);

  else if (strcmp(Routine, "VRdRFFTMSq") == 0) {
    VRdRFFTMSq (xd, yd, Nx);
    Ny = (Nx+2) / 2;
  }
  else if (strcmp(Routine, "VRdScale") == 0)
    VRdScale (xd[0], zd, yd, Ny);
 
  else if (strcmp(Routine, "VRdSet") == 0)
    VRdSet (xd[0], yd, Ny);

  else if (strcmp(Routine, "VRdZero") == 0)
    VRdZero (yd, Ny);

  else if (strcmp(Routine, "VRfAdd") == 0)
    VRfAdd (xs, zs, ys, Ny);

  else if (strcmp(Routine, "VRfCopy") == 0)
    VRfCopy (xs, ys, Ny);

  else if (strcmp(Routine, "VRfCorSym") == 0)
    xx = VRfCorSym (xs, Nx);

  else if (strcmp(Routine, "VRfDotProd") == 0)
    xx = VRfDotProd(xs, zs, Nx);

  else if (strcmp(Routine, "VRfDiff") == 0)
    VRfDiff (xs, zs, ys, Ny);

  else if (strcmp(Routine, "VRfLog10") == 0)
    VRfLog10 (xs, ys, Ny);

  else if (strcmp(Routine, "VRfMax") == 0)
    xx = VRfMax (xs, Nx);
 
  else if (strcmp (Routine, "VRfMin") == 0)
    xx = VRfMin (xs, Nx);

  else if (strcmp(Routine, "VRfMult") == 0)
    VRfMult (xs, zs, ys, Ny);

  else if (strcmp(Routine, "VRfPow10") == 0)
    VRfPow10 (xs, ys, Ny);
 
  else if (strcmp(Routine, "VRfSet") == 0)
    VRfSet (xs[0], ys, Ny);

  else if (strcmp(Routine, "VRfAdB") == 0)
    VRfAmpldB (xs, ys, Ny);

  else if (strcmp(Routine, "VRfRev") == 0)
    VRfRev (xs, ys, Ny);

  else if (strcmp(Routine, "VRfScale") == 0)
    VRfScale (xs[0], zs, ys, Ny);
 
  else if (strcmp(Routine, "VRfSum") == 0)
    xx = VRfSum (xs, Nx);
 
   else if (strcmp(Routine, "VRfZero") == 0)
    VRfZero (ys, Ny);

  else
    assert (0);

  /* Print the input data */
  if (Nz > 0) {
    VRdPrint (stdout, "x1:", xd, Nx);
    VRdPrint (stdout, "x2:", zd, Nz);
  }
  else
    VRdPrint (stdout, "Input data:", xd, Nx);

  /* Print output vector */
  if (xx  == 999999) {
    if (Routine[2] == 'd')
      VRdPrint (stdout, Routine, yd, Ny);
    else
      VRfPrint (stdout, Routine, ys, Ny);
  }
  else
    printf ("%s: %g\n", Routine, xx);

  return 0;
}
