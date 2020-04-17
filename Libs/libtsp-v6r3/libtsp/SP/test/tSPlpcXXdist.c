/*
   Test SPlpcISdist and SPlpcLSdist

   $Id: tSPlpcXXdist.c 1.4 1999/06/04 libtsp-v6r3 $
*/

#include <string.h>

#include <libtsp.h>

#define NPMAX 20


int
main (int argc, const char *argv[])

{
  int i, k, Np;
  float dist;
  float rc[NPMAX];
  float pc1[NPMAX];
  float pc2[NPMAX];

/*
   Accept either reflection coefficients (initial "R") or predictor
   coefficients
*/
  if (strcmp (argv[2], "R") == 0) {
    k = 3;
    Np = (argc - k) / 2;
    for (i = 0; i < Np; ++i, ++k)
      sscanf (argv[k], "%g", &rc[i]);
    VRfPrint (stdout, "Reflection coefficients (Set 1):", rc, Np);
    SPrcXpc (rc, pc1, Np);
    for (i = 0; i < Np; ++i, ++k)
      sscanf (argv[k], "%g", &rc[i]);
    VRfPrint (stdout, "Reflection coefficients (Set 2):", rc, Np);
    SPrcXpc (rc, pc2, Np);
  }
  else {
    k = 1;
    Np = (argc - k) / 2;
    for (i = 0; i < Np; ++i, ++k)
      sscanf (argv[k], "%g", &pc1[i]);
    for (i = 0; i < Np; ++i, ++k)
      sscanf (argv[k], "%g", &pc2[i]);
  }
  VRfPrint (stdout, "Predictor coefficients - Set 1:", pc1, Np);
  VRfPrint (stdout, "Predictor coefficients - Set 2:", pc2, Np);

/* Choose which distance measure to use */
  if (strcmp (argv[1], "IS") == 0) {
    dist = SPlpcISdist (pc1, pc2, Np);
    printf ("SPlpcISdist = %.5g\n", dist);
  }
  else if (strcmp (argv[1], "LS") == 0) {
    dist = SPlpcLSdist (pc1, pc2, Np, 4 * Np);
    printf ("SPlpcLSdist = %.5g\n", dist);
  }
  else
    UThalt ("Invalid distance measure specification");

  return 0;
}
