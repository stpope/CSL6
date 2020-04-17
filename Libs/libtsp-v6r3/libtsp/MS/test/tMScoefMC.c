/*
   Test MScoefMC

   $Id: tMScoefMC.c 1.3 1999/06/04 libtsp-v6r3 $
*/

#include <libtsp.h>
#include <libtsp/nucleus.h>


int
main (int argc, const char *argv[])

{
  double c[4];
  double *coef;
  double x1, x2, y1, y2, d1, d2;

  sscanf (argv[1], "%lg", &c[0]);
  sscanf (argv[2], "%lg", &c[1]);
  sscanf (argv[3], "%lg", &c[2]);
  sscanf (argv[4], "%lg", &c[3]);
  sscanf (argv[5], "%lg", &x1);
  sscanf (argv[6], "%lg", &x2);

  VRdPrint (stdout, "Input coefficients:", c, 4);

  y1 = c[0] + x1 * (c[1] + x1 * (c[2] + x1 * c[3]));
  y2 = c[0] + x2 * (c[1] + x2 * (c[2] + x2 * c[3]));
  d1 = c[1] + x1 * (2.0 * c[2] + x1 * 3.0 * c[3]);
  d2 = c[1] + x2 * (2.0 * c[2] + x2 * 3.0 * c[3]);
  coef = MScoefMC (x1, x2, y1, y2, d1, d2);

  VRdPrint (stdout, "Derived coefficients", coef, 4);

  return 0;
}
