/*
   Test UTxIEEE80

   $Id: tUTxIEEE80.c 1.7 2000/07/20 libtsp-v6r3 $
*/

#include <math.h>
#include <stdio.h>
#include <string.h>

#include <libtsp.h>
#include <libtsp/nucleus.h>
#include <libtsp/UTtypes.h>

#ifndef HUGE_VAL
#  define HUGE_VAL	HUGE
#endif

#define TNAN(x)		(!(x <= 1.) && !(x > 1.))


int
main (int argc, const char *argv[])

{
  int i;
  double dv, yv;
  unsigned char b[10];

  if (strcmp (argv[1], "HUGE_VAL") == 0)
    dv = HUGE_VAL;
  else if (strcmp (argv[1], "-HUGE_VAL") == 0)
    dv = -HUGE_VAL;
  else if (strcmp (argv[1], "infinity") == 0)
    dv = sqrt (1.) / 0.0;
  else if (strcmp (argv[1], "-infinity") == 0)
    dv = -sqrt (1.) / 0.0;
  else if (strcmp (argv[1], "NaN") == 0)
    dv = sqrt (-1.);
  else
    sscanf (argv[1], "%lg", &dv);

/* Test double to extended-double and back again */
  UTeIEEE80 (dv, b);
  yv = UTdIEEE80 (b);

  printf ("Input value: %g, IEEE80: ", dv);
  for (i = 0; i < 10; ++i)
    printf ("%02x", b[i]);
  printf ("\n");
  if (! (dv == yv) && ! (TNAN (dv) && TNAN (yv)))
    UThalt (">>> Value mismatch: value: %g, converted value: %g", dv, yv);

  return 0;
}
