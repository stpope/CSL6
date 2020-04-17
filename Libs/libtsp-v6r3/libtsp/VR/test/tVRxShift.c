/*
   Test VRxShift

   $Id: tVRxShift.c 1.7 2005/02/01 libtsp-v6r3 $
*/

#include <assert.h>
#include <string.h>

#include <libtsp.h>

#define NMAX	128


int
main (int argc, const char *argv[])

{
  double xd[128];
  float xs[128];
  const char *Routine;
  int nshift;
  int nkeep;
  int i, k, N;

  Routine = argv[1];
  k = 2;

  sscanf (argv[k], "%d", &nshift);
  ++k;
  N = argc - k;

  for (i = 0; i < N; ++i, ++k) {
    sscanf (argv[k], "%lg", &xd[i]);
    xs[i] = xd[i];
  }

  if (nshift < 0)
    nkeep = N + nshift;
  else
    nkeep = N - nshift;
  assert (nkeep >= 0);

 if (strcmp(Routine, "VRfShift") == 0) {
    VRfPrint (stdout, "VRfShift: Input data:", xs, N);
    VRfShift (xs, nkeep, nshift);
    VRfPrint (stdout, "          Output data:", xs, N);
  }
  else if (strcmp(Routine, "VRdShift") == 0) {
    VRdPrint (stdout, "VRdShift: Input data:", xd, N);
    VRdShift (xd, nkeep, nshift);
    VRdPrint (stdout, "          Output data:", xd, N);
  }
  else
    assert (0);

  return 0;
}
