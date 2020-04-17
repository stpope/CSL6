/*
   Test FLxReadData

   $Id: tFLxReadData.c 1.4 2005/02/01 libtsp-v6r3 $
*/

#include <assert.h>
#include <string.h>

#include <libtsp.h>

#define MAXN	5


int
main (int argc, const char *argv[])

{
  double xd[MAXN];
  float xs[MAXN];
  int Nval;
  const char *Routine;
  const char *Fname;

  Routine = argv[1];
  Fname = argv[2];

  if (strcmp (Routine, "FLdReadData") == 0) {
    Nval = FLdReadData (Fname, MAXN, xd);
    VRdPrint (stdout, "FLdReadData:", xd, Nval);
  }
  else if (strcmp (Routine, "FLfReadData") == 0) {
    Nval = FLfReadData (Fname, MAXN, xs);
    VRfPrint (stdout, "FLfReadData:", xs, Nval);
  }
  else
    assert (0);

  return 0;
}
