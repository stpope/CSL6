/*
  Test floor function

  Under cgywin-b20, this program compiled as
    gcc -O3 tfloor.c -lm -o tfloor
  gives a negative value for the expression "AV - floor(AV)".

  $Id: tfloor.c 1.2 1999/06/04 libtsp-v6r3 $
*/

#include <math.h>
#include <stdio.h>


int
main (int argc, const char *argv[])

{
  double AV;

  AV = 0.05;
  AV = 1.0 / (AV - floor (AV));
  printf ("AV = %g, AV - floor(AV) = %g\n", AV, AV - floor(AV));

  return 0;
};
