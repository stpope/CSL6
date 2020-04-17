/*
   Test FLxWriteData

   $Id: tFLxWriteData.c 1.4 2005/02/01 libtsp-v6r3 $
*/

#include <assert.h>
#include <string.h>

#include <libtsp.h>

#define MAXN 20

int
main (int argc, const char *argv[])

{
  int i, k, Nx;
  double xd[MAXN];
  float xs[MAXN];
  const char *Routine;
  const char *Fname;

  Routine = argv[1];
  Fname = argv[2];

/* Input data */
  k = 3;
  Nx = argc - k;
  for (i = 0; i < Nx; ++i, ++k) {
    sscanf (argv[k], "%lg", &xd[i]);
    xs[i] = xd[i];
  }

/* Write data */
  printf ("%s:\n", Routine);
  if (strcmp (Routine, "FLdWriteData") == 0)
    FLdWriteData (Fname, xd, Nx);
  else if (strcmp (Routine, "FLfWriteData") == 0)
    FLfWriteData (Fname, xs, Nx);
  else
    assert (0);

  return 0;
}
