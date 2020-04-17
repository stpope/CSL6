/*
   Test MSxIntLin and MSxIntMC

   $Id: tMSxIntXxx.c 1.6 2001/03/07 libtsp-v6r3 $
*/

#include <string.h>

#include <libtsp.h>

#define NMAX	128


int
main (int argc, const char *argv[])

{
  const char *Opt;
  int i, k;
  int N, Ni;
  double xd[NMAX], yd[NMAX], xdi[NMAX], ydi[NMAX];
  float  xs[NMAX], ys[NMAX], xsi[NMAX], ysi[NMAX];

  k = 1;
  Opt = argv[k];
  ++k;
  for (i = 0; k < argc; ++k, ++i) {
    if (strcmp (argv[k], "//") == 0)
      break;
    sscanf (argv[k], "%lg", &xd[i]);
    xs[i] = xd[i];
    ++k;
    sscanf (argv[k], "%lg", &yd[i]);
    ys[i] = yd[i];
  }
  N = i;

  for (++k, i = 0; k < argc; ++k, ++i) {
    sscanf (argv[k], "%lg", &xdi[i]);
    xsi[i] = xdi[i];
  }

  Ni = i;

  if (strcmp (Opt, "fL") == 0) {
    MSfIntLin (xs, ys, N, xsi, ysi, Ni);
    fprintf (stdout, "MSfIntLin:\n");
    VRfPrint (stdout, "  x reference:", xs, N);
    VRfPrint (stdout, "  y reference:", ys, N);
    VRfPrint (stdout, "  x:", xsi, Ni);
    VRfPrint (stdout, "  y:", ysi, Ni);
  }
  else if (strcmp (Opt, "dL") == 0) {
    MSdIntLin (xd, yd, N, xdi, ydi, Ni);
    fprintf (stdout, "MSdIntLin:\n");
    VRdPrint (stdout, "  x reference:", xd, N);
    VRdPrint (stdout, "  y reference:", yd, N);
    VRdPrint (stdout, "  x:", xdi, Ni);
    VRdPrint (stdout, "  y:", ydi, Ni);
  }
  else if (strcmp (Opt, "fC") == 0) {
    MSfIntMC (xs, ys, N, xsi, ysi, Ni);
    fprintf (stdout, "MSfIntMC:\n");
    VRfPrint (stdout, "  x reference:", xs, N);
    VRfPrint (stdout, "  y reference:", ys, N);
    VRfPrint (stdout, "  x:", xsi, Ni);
    VRfPrint (stdout, "  y:", ysi, Ni);
  }
  else if (strcmp (Opt, "dC") == 0) {
    MSdIntMC (xd, yd, N, xdi, ydi, Ni);
    fprintf (stdout, "MSdIntMC:\n");
    VRdPrint (stdout, "  x reference:", xd, N);
    VRdPrint (stdout, "  y reference:", yd, N);
    VRdPrint (stdout, "  x:", xdi, Ni);
    VRdPrint (stdout, "  y:", ydi, Ni);
  }

  return 0;
}
