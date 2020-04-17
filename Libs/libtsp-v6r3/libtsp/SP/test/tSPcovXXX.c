/*
   Test LPC analysis using covariance methods

   $Id: tSPcovXXX.c 1.17 1999/06/04 libtsp-v6r3 $
*/

#include <libtsp.h>

#define NPMAX	20
#define NXMAX	300


int
main (int argc, const char *argv[])

{
  int i, j, k, Np, Nx;
  float x[NXMAX];
  float pc[NPMAX], pcS[NPMAX];
  float **Cov, **CovS, **C;
  double perr, perrS;

  sscanf (argv[1], "%d", &Np);
  k = 2;
  Nx = argc - k;
  for (i = 0; i < Nx; ++i, ++k)
    sscanf (argv[k], "%g", &x[i]);

  VRfPrint (stdout, "Data values:", x, Nx);

  /* Calculate the covariance values */
  Cov = MAfAllocMat (Np+1, Np+1);
  SPcovar (x, Nx, Cov, Np);
  MAfPrint (stdout, "Covariance matrix:", (const float **) Cov, Np+1, Np+1);

  /* Fill in upper part of Cov with garbage */
  CovS = MAfAllocMat (Np+1, Np+1);
  for (i = 0; i <= Np; ++i) {
    for (j = 0; j <= i; ++j)
      CovS[i][j] = Cov[i][j];
    for (j = i+1; j <= Np; ++j)
      CovS[i][j] = 1E10;
  }

/* Covariance solutions */
  perr = SPcovXpc ((const float **) Cov, pc, Np);
  perrS = SPcovXpc ((const float **) CovS, pcS, Np);
  if (VRfDiffSq (pc, pcS, Np) != 0.0 || perr != perrS)
    UThalt ("tSPcovXXX: SPcovXpc using values above the diagonal");
  VRfPrint (stdout, "SPcovXpc, using covariance matrix:", pc, Np);
  printf ("mse, SPcovXpc:  %g\n", perr);

  perr = SPcovPmse (pc, (const float **) Cov, Np);
  printf ("mse, SPcovPmse: %g\n", perr);

  C = MAfSubMat ((const float **) Cov, 1, 1, Np);
  perr = SPcovFmse (pc, (double) Cov[0][0], (const float **) C,
                    &(Cov[0][1]), Np);
  printf ("mse, SPcovFmse: %g\n", perr);

  perr = SPcovMXpc ((const float **) Cov, pc, Np);
  perrS = SPcovMXpc ((const float **) CovS, pcS, Np);
  if (VRfDiffSq (pc, pcS, Np) != 0.0 || perr != perrS)
    UThalt ("tSPcovXXX: SPcovMXpc using values above the diagonal");
  VRfPrint (stdout, "SPcovMXpc, using covariance matrix:", pc, Np);
  printf ("mse, SPcovMXpc: %g\n", perr);
  perr = SPcovPmse (pc, (const float **) Cov, Np);
  printf ("mse, SPcovPmse: %g\n", perr);

  perr = SPcovLXpc ((const float **) Cov, pc, Np);
  perrS = SPcovLXpc ((const float **) CovS, pcS, Np);
  if (VRfDiffSq (pc, pcS, Np) != 0.0 || perr != perrS)
    UThalt ("tSPcovXXX: SPcovLXpc using values above the diagonal");
  VRfPrint (stdout, "SPcovLXpc, using covariance matrix:", pc, Np);
  printf ("mse, SPcovLXpc: %g\n", perr);
  perr = SPcovPmse (pc, (const float **) Cov, Np);
  printf ("mse, SPcovPmse: %g\n", perr);

  perr = SPcovCXpc ((const float **) Cov, pc, Np);
  perrS = SPcovCXpc ((const float **) CovS, pcS, Np);
  if (VRfDiffSq (pc, pcS, Np) != 0.0 || perr != perrS)
    UThalt ("tSPcovXXX: SPcovCXpc using values above the diagonal");
  VRfPrint (stdout, "SPcovCXpc, using covariance matrix:", pc, Np);
  printf ("mse, SPcovCXpc: %g\n", perr);
  perr = SPcovPmse (pc, (const float **) Cov, Np);
  printf ("mse, SPcovPmse: %g\n", perr);

  MAfFreeMat (Cov);
  UTfree ((void *) C);

  return 0;
}
