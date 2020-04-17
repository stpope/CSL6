/*
   Test FIxKaiserLPF

   $Id: tFIxKaiserLPF.c 1.4 2005/02/01 libtsp-v6r3 $
*/

#include <assert.h>
#include <string.h>

#include <libtsp.h>

#define NMAX 100


int
main (int argc, const char *argv[])

{
  int N;
  double hd[NMAX];
  float hs[NMAX];
  double Fc, alpha;
  const char *Routine;

  Routine = argv[1];

  sscanf (argv[2], "%d", &N);
  sscanf (argv[3], "%lg", &Fc);
  sscanf (argv[4], "%lg", &alpha);

  assert (N <= NMAX);

  if (strcmp (Routine, "FIdKaiserLPF") == 0) {
    FIdKaiserLPF (hd, N, Fc, alpha);
    VRdPrint (stdout, "Kaiser LPF (d):", hd, N);
  }
  else if (strcmp (Routine, "FIfKaiserLPF") == 0) {
    FIfKaiserLPF (hs, N, Fc, alpha);
    VRfPrint (stdout, "Kaiser LPF (f):", hs, N);
  }
  else
    assert (0);

  return 0;
}
