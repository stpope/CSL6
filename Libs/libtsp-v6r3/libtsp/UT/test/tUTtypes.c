/*
   Test UTtypes.h

   $Id: tUTtypes.c 1.3 1999/06/04 libtsp-v6r3 $
*/

#include <libtsp.h>
#include <libtsp/UTtypes.h>

int
main (int argc, const char *argv[])

{
  int LL, LI, LS, LC;
  int LD, LF;
  int U4, U2, U1;
  int I4, I2;
  int D8, F4;

  LL = sizeof (long int);
  LI = sizeof (int);
  LS = sizeof (short);
  LC = sizeof (char);
  printf ("sizeof (long int, int, short int, char): %d %d %d %d\n",
	  LL, LI, LS, LC);
  LD = sizeof (double);
  LF = sizeof (float);
  LL = sizeof (long double);
  printf ("sizeof (long double, double, float): %d %d %d\n", LL, LD, LF);
  U4 = sizeof (uint4_t);
  U2 = sizeof (uint2_t);
  U1 = sizeof (uint1_t);
  printf ("sizeof (uint4_t, uint2_t, uint1_t): %d %d %d\n", U4, U2, U1);
  I4 = sizeof (int4_t);
  I2 = sizeof (int2_t);
  printf ("sizeof (int4_t, int2_t): %d, %d\n", I4, I2);
  D8 = sizeof (double8_t);
  F4 = sizeof (float4_t);
  printf ("sizeof (double8_t, float4_t): %d %d\n", D8, F4);

  if (U4 != 4 || U2 != 2 || I4 != 4 || I2 != 2 || D8 != 8 || F4 != 4 ||
      U1 != 1)
    UThalt ("++++Unexpected data size++++");

  return 0;
}
