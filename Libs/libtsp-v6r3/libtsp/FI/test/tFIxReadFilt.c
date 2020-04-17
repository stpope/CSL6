/*
   Test FIxReadFilt

   $Id: tFIxReadFilt.c 1.4 2005/02/01 libtsp-v6r3 $
*/

#include <assert.h>
#include <string.h>

#include <libtsp.h>

#define MAXN	50


int
main (int argc, const char *argv[])

{
  int Ftype;
  double hd[MAXN];
  float hs[MAXN];
  int Ncof;
  const char *Routine;
  const char *Fname;

  Routine = argv[1];
  Fname = argv[2];

  if (strcmp (Routine, "FIdReadFilt") == 0) {
    Ftype = FIdReadFilt (Fname, MAXN, hd, &Ncof, stdout);
    printf ("FIdReadFilt: type = %d\n", Ftype);
    VRdPrint (stdout, "Filter coefficients:", hd, Ncof);
  }
  else if (strcmp (Routine, "FIfReadFilt") == 0) {
    Ftype = FIfReadFilt (Fname, MAXN, hs, &Ncof, stdout);
    printf ("FIfReadFilt: type = %d\n", Ftype);
    VRfPrint (stdout, "Filter coefficients:", hs, Ncof);
  }
  else
    assert (0);

  return 0;
}
