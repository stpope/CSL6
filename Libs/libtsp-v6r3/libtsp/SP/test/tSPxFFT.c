/*
   Test FFT/DCT routines

   $Id: tSPxFFT.c 1.4 2001/03/08 libtsp-v6r3 $
*/

#include <string.h>

#include <libtsp.h>

#define NMAX	128


int
main (int argc, const char *argv[])

{
  int i, k, N;
  const char *Opt;
  float  xs[NMAX], ys[NMAX];
  double xd[NMAX], yd[NMAX];

  k = 1;
  Opt = argv[k];
  ++k;
   
  N = (argc - k) / 2;

  for (i = 0; i < N; ++i, ++k) {
    sscanf (argv[k], "%lg", &xd[i]);
    xs[i] = xd[i];
  }

  for (i = 0; i < N; ++i, ++k) {
    sscanf (argv[k], "%lg", &yd[i]);
    ys[i] = yd[i];
  }

  if (strcmp (Opt, "fCFFT") == 0) {
    fprintf (stdout, "SPfCFFT:\n");
    VRfPrint (stdout, "  x:", xs, N);
    VRfPrint (stdout, "  y:", ys, N);

    SPfCFFT (xs, ys, N, 1);
    VRfPrint (stdout, "  DFT x:", xs, N);
    VRfPrint (stdout, "  DFT y:", ys, N);

    SPfCFFT (xs, ys, N, -1);
    VRfPrint (stdout, "  invDFT x:", xs, N);
    VRfPrint (stdout, "  invDFT y:", ys, N);
  }
  else if (strcmp (Opt, "dCFFT") == 0) {
    fprintf (stdout, "SPdCFFT:\n");
    VRdPrint (stdout, "  x:", xd, N);
    VRdPrint (stdout, "  y:", yd, N);

    SPdCFFT (xd, yd, N, 1);
    VRdPrint (stdout, "  DFT x:", xd, N);
    VRdPrint (stdout, "  DFT y:", yd, N);

    SPdCFFT (xd, yd, N, -1);
    VRdPrint (stdout, "  invDFT x:", xd, N);
    VRdPrint (stdout, "  invDFT y:", yd, N);
  }
  else if (strcmp (Opt, "dRFFT") == 0) {
    VRdCopy (yd, &xd[N], N);
    N = 2 * N;
    fprintf (stdout, "SPdRFFT:\n");
    VRdPrint (stdout, "  x:", xd, N);

    SPdRFFT (xd, N, 1);
    yd[0] = 0.0;
    for (i = 1; i < N/2; ++i)
      yd[i] = xd[i+N/2];
    yd[N/2] = 0.0;
    VRdPrint (stdout, "  DFT x:", xd, N/2+1);
    VRdPrint (stdout, "  DFT y:", yd, N/2+1);

    SPdRFFT (xd, N, -1);
    VRdPrint (stdout, "  invDFT x:", xd, N);
  }
  else if (strcmp (Opt, "fRFFT") == 0) {
    VRfCopy (ys, &xs[N], N);
    N = 2 * N;
    fprintf (stdout, "SPfRFFT:\n");
    VRfPrint (stdout, "  x:", xs, N);

    SPfRFFT (xs, N, 1);
    ys[0] = 0.0;
    for (i = 1; i < N/2; ++i)
      ys[i] = xs[i+N/2];
    ys[N/2] = 0.0;
    VRfPrint (stdout, "  DFT x:", xs, N/2+1);
    VRfPrint (stdout, "  DFT y:", ys, N/2+1);

    SPfRFFT (xs, N, -1);
    VRfPrint (stdout, "  invDFT x:", xs, N);
  }
  else if (strcmp (Opt, "fDCT") == 0) {
    VRfCopy (ys, &xs[N], N);
    N = 2 * N;
    fprintf (stdout, "SPfDCT:\n");
    VRfPrint (stdout, "  x:", xs, N);

    SPfDCT (xs, N, 1);
    VRfPrint (stdout, "  DCT x:", xs, N);

    SPfDCT (xs, N, -1);
    VRfPrint (stdout, "  invDCT x:", xs, N);
  }

  return 0;
}
