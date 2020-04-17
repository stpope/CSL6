/*
   Test window generation routines

   $Id: tFIxWinXXX.c 1.9 2005/02/01 libtsp-v6r3 $
*/

#include <string.h>

#include <libtsp.h>

#define NMAX 300


int
main (int argc, const char *argv[])

{
  int N, Nadv;
  float fWin[NMAX];
  double dWin[NMAX];
  const char *Routine;
  double a, T1, T2, T3;
  char *h;

  h = "Error";
  Routine = argv[1];
  sscanf (argv[2], "%d", &N);
  if (strcmp (Routine, "FIdWinHamm") == 0) {
    sscanf (argv[3], "%lg", &a);
    FIdWinHamm (dWin, N, a);
    h = "FIdWinHamm: generalized Hamming window";
  }
  else if (strcmp (Routine, "FIfWinRCos") == 0) {
    sscanf (argv[3], "%d", &Nadv);
    FIfWinRCos (fWin, N, Nadv);
    h = "FIfWinRCos: Raised Cosine window:";
  }
  else if (strcmp (Routine, "FIfWinHamm") == 0) {
    sscanf (argv[3], "%lg", &a);
    FIfWinHamm (fWin, N, a);
    h = "FIfWinHamm: generalized Hamming window:";
  }
  else if (strcmp (Routine, "FIdWinKaiser") == 0) {
    sscanf (argv[3], "%lg", &a);
    FIdWinKaiser (dWin, N, a);
    h = "FIdWinKaiser: Kaiser window:";
  }
  else if (strcmp (Routine, "FIfWinKaiser") == 0) {
    sscanf (argv[3], "%lg", &a);
    FIfWinKaiser (fWin, N, a);
    h = "FIfWinKaiser: Kaiser window:";
  }
  else if (strcmp (Routine, "FIfWinHCos") == 0) {
    sscanf (argv[3], "%lg", &a);
    T2 = N * 200./240.;
    T1 = T2 - 0.5;
    T3 = N - 0.25;
    FIfWinHCos (fWin, T1, T2, T3, a);
    h = "FIfWinHCos: Hamming/Cosine window:";
  }
  else
    UThalt ("tFIxWinXXX: Invalid window type");

  if (h[2] == 'f')
    VRfPrint (stdout, h, fWin, N);
  else
    VRdPrint (stdout, h, dWin, N);

  return 0;
}
