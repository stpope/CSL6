/*
   Test SPxxXpc and SPpcXxx

   Usage: tSPxxXxx in/out <input parameters>
   Input / Output code: two letters each of which is one of:
     K - reflection coefficients
     R - autocorrelation coefficients
     L - line spectral frequencies
     C - cepstral coefficients
     A - error filter coefficients
     P - predictor coefficients

   $Id: tSPxxXxx.c 1.9 2003/05/08 libtsp-v6r3 $
*/

#include <libtsp.h>

#define NPMAX 50
#define CEPEXTRA 4


int
main (int argc, const char *argv[])

{
  int i, k, Np;
  float pc[NPMAX];
  float rc[NPMAX];
  float rxx[NPMAX+1];
  float lsf[NPMAX];
  float ec[NPMAX+1];
  float cep[NPMAX+CEPEXTRA+1];
  float perr;
  const char *io;

  Np = 0;
  io = argv[1];

/* Input parameters */
  if (io[0] == 'K') {
    k = 2;
    Np = argc - k;
    for (i = 0; i < Np; ++i, ++k)
      sscanf (argv[k], "%g", &rc[i]);
    VRfPrint (stdout, "Reflection coefficients:", rc, Np);
    perr = SPrcXpc (rc, pc, Np);
    printf ("Normalized prediction error: %.5g\n", perr);
  }
  else if (io[0] == 'R') {
    k = 2;
    Np = argc - k - 1;
    for (i = 0; i <= Np; ++i, ++k)
      sscanf (argv[k], "%g", &rxx[i]);
    VRfPrint (stdout, "Autocorrelation coefficients:", rxx, Np + 1);
    perr = SPcorXpc (rxx, pc, Np);
    if (perr == 0.0 && rxx[0] == 0.0)
      printf ("Normalized prediction error: %.5g\n", 0.0);
    else
      printf ("Normalized prediction error: %.5g\n", perr/rxx[0]);
  }
  else if (io[0] == 'L') {
    k = 2;
    Np = argc - k;
    for (i = 0; i < Np; ++i, ++k)
      sscanf (argv[k], "%g", &lsf[i]);
    VRfPrint (stdout, "Line spectral frequencies:", lsf, Np);
    SPlsfXpc (lsf, pc, Np);
  }
  else if (io[0] == 'C') {
    k = 2;
    Np = argc - k - 1;
    for (i = 0; i <= Np; ++i, ++k)
      sscanf (argv[k], "%g", &cep[i]);
    VRfPrint (stdout, "Cepstral coefficients:", cep, Np+1);
    SPcepXpc (cep, pc, Np);
  }
  else if (io[0] == 'A') {
    k = 2;
    Np = argc - k - 1;
    for (i = 0; i <= Np; ++i, ++k)
      sscanf (argv[k], "%g", &ec[i]);
    VRfPrint (stdout, "Error filter coefficients:", ec, Np + 1);
    SPecXpc (ec, pc, Np);
  }
  else if (io[0] == 'P') {
    k = 2;
    Np = argc - k;
    for (i = 0; i < Np; ++i, ++k)
      sscanf (argv[k], "%g", &pc[i]);
    VRfPrint (stdout, "Predictor coefficients:", pc, Np);
  }
  else
    UThalt ("Unknown input parameter set");

/* Output parameters */
  if (io[1] == 'K') {
    perr = SPpcXrc (pc, rc, Np);
    VRfPrint (stdout, "Reflection coefficients:", rc, Np);
    printf ("Normalized prediction error: %.5g\n", perr);
  }
  else if (io[1] == 'R') {
    perr = SPpcXcor (pc, Np, rxx, Np + 1);
    VRfPrint (stdout, "Autocorrelation coefficients:", rxx, Np + 1);
    printf ("Normalized prediction error: %.5g\n", perr);
  }
  else if (io[1] == 'L') {
    SPpcXlsf (pc, lsf, Np);
    VRfPrint (stdout, "Line spectral frequencies:", lsf, Np);
  }
  else if (io[1] == 'C') {
    SPpcXcep (pc, Np, cep, Np+CEPEXTRA+1);
    VRfPrint (stdout, "Cepstral coefficients:", cep, Np+CEPEXTRA+1);
  }
  else if (io[1] == 'A') {
    perr = SPpcXrc (pc, rc, Np);
    SPpcXec (pc, ec, Np);
    VRfPrint (stdout, "Error filter coefficients:", ec, Np + 1);
    printf ("Normalized prediction error: %.5g\n", perr);
  }
  else if (io[1] == 'P') {
    perr = SPpcXrc (pc, rc, Np);
    VRfPrint (stdout, "Predictor coefficients:", pc, Np);
    printf ("Normalized prediction error: %.5g\n", perr);
  }
  else
    UThalt ("Unknown output parameter set");

  return 0;
}
