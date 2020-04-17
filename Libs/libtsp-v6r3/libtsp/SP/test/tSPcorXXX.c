/*
   Test LPC analysis using autocorrelation methods

   $Id: tSPcorXXX.c 1.5 1999/06/04 libtsp-v6r3 $
*/

#include <libtsp.h>

#define ABSV(x)		(((x) < 0) ? -(x) : (x))

#define NPMAX	20
#define NXMAX	300


int
main (int argc, const char *argv[])

{
  int i, j, k, Np, Nx;
  float x[NXMAX];
  float Rx[NPMAX+1], pc[NPMAX];
  float **R;
  double perr;

  sscanf (argv[1], "%d", &Np);
  k = 2;
  Nx = argc - k;
  for (i = 0; i < Nx; ++i, ++k)
    sscanf (argv[k], "%g", &x[i]);

  VRfPrint (stdout, "Data values:", x, Nx);

  /* Calculate the autocorrelation values */
  SPautoc (x, Nx, Rx, Np + 1);
  VRfPrint (stdout, "Autocorrelation vector:", Rx, Np+1);
  R = MAfAllocMat (Np + 1, Np + 1);
  for (i = 0; i <= Np; ++i) {
    for (j = 0; j <= Np; ++ j)
      R[i][j] = Rx[ABSV(i-j)];
  }

/* Autocorrelation solutions */
  perr = SPcovXpc ((const float **) R, pc, Np);
  VRfPrint (stdout, "SPcovXpc, using autocorrelation matrix:", pc, Np);
  printf ("mse, SPcovXpc:  %g\n", perr);

  perr = SPcovMXpc ((const float **) R, pc, Np);
  VRfPrint (stdout, "SPcovMXpc, using autocorrelation matrix:", pc, Np);
  printf ("mse, SPcovMXpc: %g\n", perr);

  perr = SPcovLXpc ((const float **) R, pc, Np);
  VRfPrint (stdout, "SPcovLXpc, using autocorrelation matrix:", pc, Np);
  printf ("mse, SPcovLXpc: %g\n", perr);

  perr = SPcovCXpc ((const float **) R, pc, Np);
  VRfPrint (stdout, "SPcovCXpc, using autocorrelation matrix:", pc, Np);
  printf ("mse, SPcovCXpc: %g\n", perr);

  perr = SPcorFilt (Rx[0], Rx, &Rx[1], pc, Np);
  VRfPrint (stdout, "SPcorFilt:", pc, Np);
  printf ("mse, SPcorFilt:  %g\n", perr);

  perr = SPcorXpc (Rx, pc, Np);
  VRfPrint (stdout, "SPcorXpc:", pc, Np);
  printf ("mse, SPcorXpc:  %g\n", perr);

  perr = SPcorPmse (pc, Rx, Np);
  printf ("mse, SPcorPmse: %g\n", perr);
  perr = SPcorFmse (pc, Rx[0], Rx, &Rx[1], Np);
  printf ("mse, SPcorFmse: %g\n", perr);

  MAfFreeMat (R);

  return 0;
}
