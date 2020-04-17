/*
   Test NaN and Inf

   $Id: tNaN.c 1.4 2003/05/09 libtsp-v6r3 $
*/

#include <limits.h>
#include <math.h>
#include <stdio.h>

/*
 Assume that the lengths of integers are as follows
 (short int, int, long int) are (2,2,4), (2,4,4) or (2,4,8) bytes
*/
#if (INT_MAX == SHRT_MAX)	/* (2,2,4) case */
   typedef unsigned long int uint4_t;	/* 4-byte unsigned int */
   typedef long int int4_t;		/* 4-byte int */
#else
   typedef unsigned int uint4_t;	/* 4-byte unsigned int */
   typedef int int4_t;			/* 4-byte int */
#endif

typedef unsigned short int uint2_t;	/* 2-byte unsigned int */
typedef short int int2_t;		/* 2-byte int */
typedef unsigned char uint1_t;		/* 1-byte unsigned int */
typedef signed char int1_t;		/* 1-byte signed int */

int
main (int argc, const char *argv[])

{
  double dv, yv, dmant1, dmant2;
  int exp1, exp2, exp, error;

  error = 0;

  /* Check unsigned to double conversion */
  dv = (uint4_t) 0xFFFFFFFF;
  yv = (uint4_t) 0x7FFFFFFF;
  if (yv + 2147483648. != dv) {
    error = 1;
    fprintf (stderr, "tNaN - Error in uint4_t to double conversion\n");
  }

  /* Check frexp for small values */
  dv = 4e-308;
  dmant1 = frexp (dv, &exp1);
  dmant2 = frexp (0.5*dv, &exp2);
  if (dmant1 != dmant2 && exp1 - exp2 != 1) {
    error = 1;
    fprintf (stderr, "tNaN - Error in frexp for input %g\n", 0.5*dv);
  }

  /* Check if inf - inf generates NaN */
  dv = sqrt (1.) / 0.0;		/* Inf */
  yv = -sqrt (1.) / 0.0;	/* -Inf */
  printf ("%g + %g = %g\n", dv, yv, dv + yv);

  /* Check tests with NaN */
  dv = sqrt (-1.0);		/* NaN */
  printf ("%g  < 1.: %d, %g <= 1.: %d,  %g == 1.: %d\n",
	  dv, (dv  < 1.), dv, (dv <= 1.), dv, (dv == 1.));
  printf ("%g >= 1.: %d, %g  > 1.: %d\n", dv, (dv >= 1.), dv, (dv > 1.));
  printf ("%g == %g: %d\n", dv, dv, (dv == dv));

  yv = frexp (dv, &exp);
  printf ("frexp (%g): mantissa = %g, exponent = %d\n", dv, yv, exp);

  /* Check tests with infinity */
  dv = sqrt (1.) / 0.0;		/* Inf */
  printf ("%g  < 1.: %d, %g <= 1.: %d,  %g == 1.: %d\n",
	  dv, (dv  < 1.), dv, (dv <= 1.), dv, (dv == 1.));
  printf ("%g >= 1.: %d, %g  > 1.: %d\n", dv, (dv >= 1.), dv, (dv > 1.));
  printf ("%g == %g: %d\n", dv, dv, (dv == dv));

  yv = frexp (dv, &exp);
  printf ("frexp (%g): mantissa = %g, exponent = %d\n", dv, yv, exp);

  return error;
}
