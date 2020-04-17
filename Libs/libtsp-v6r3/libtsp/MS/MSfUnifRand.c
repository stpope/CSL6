/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  double MSfUnifRand (void)
  void MSrandSeed (int seed)

Purpose:
  Generate a uniformly distributed pseudo-random value
  Reseed the uniform pseudo-random value

Description:
  This routine generates uniformly distributed pseudo-random numbers.  The
  multiplicative congruential method is used to generate integer pseudo-random
  numbers, is = (a*is) % m.  The values a=16807 and m=2147483647 are used.
  These values have been shown to give good results.  Further, the values
  generated are shuffled to give better serial correlation properties.  The
  pseudo-random number generation routine is self initializing, but if a
  different sequence of pseudo-random values is desired, the routine MSrandSeed
  can be used to supply a starting (seed) value.

  Reference:
      W. H. Press, S. A. Teukolsky, W. T. Vetterling, B. P. Flannery,
      "Numerical Recipes in C" (2nd ed.) (p. 280), Cambridge University Press,
      1992.

Parameters:
  MSfUnifRand:
  <-  double MSfUnifRand
      Uniform deviate with values between 0 and 1, exclusive of the end points.
  MSrandSeed:
   -> int seed
      Seed value.  A positive seed value sets the pseudo-random generation to a
      new sequence.  A zero value sets the actual seed to a value based on the
      current time.  The default sequence can be restarted by setting the seed
      value to 12345.

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.18 $  $Date: 2003/05/09 02:29:38 $

-------------------------------------------------------------------------*/

#include <time.h>

#include <libtsp.h>

#define A	16807
#define M	2147483647	/* M = A*Q + R */
#define Q	(M / A)		/* 127773 */
#define R	(M % A)		/* 2836 */
#define NTAB	32
#define NDIV	(1 + (M-1) / NTAB)
#define EPS	1.2e-7
#define RANMAX	(1.0 - EPS)
#define DSEED	12345

static long int y = 0;
static long int is;
static long int iv[NTAB];


double
MSfUnifRand (void)

{
  int j;
  long int k;
  double fv;

/*
   Generate a random integer value using a multiplicative congruential method,
     is = (A*is) % M
   Schrage's algorithm is used to calculate (A*z) % M while avoiding overflow,
   (requires R < Q and 0 < z < M-1)
     (A*z) % M = A * (z%Q) - R * [z/Q] + i*M,  (i=0 or 1, to bring the result
                                                to [0,M-1]).
     M=A*Q+R, R < A, here, A=16807.
*/
/* Initialization */
  if (y == 0)
    MSrandSeed (DSEED);

/* Generate a random number is.  The value y generates j which selects an
   entry in the table 0 <= j < NTAB.  This table entry is used to generate the
   current output pseudo-random number.  The new integer pseudo-random number
   generated in this round is stored in its place.
*/
  k = is / Q;
  is = A * (is - k * Q) - R * k;
  if (is < 0)
    is += M;
  j = (int) (y / NDIV);
  y = iv[j];
  iv[j] = is;

/* Convert to float, float pseudo-random number is limited to (0,1-eps] */
  fv = (1.0 / M) * y;
  if (fv > RANMAX)
    return RANMAX;
  else
    return fv;
}

/* Initialize the array of internal values */


void
MSrandSeed (int seed)
{
  int j;
  long int t, k;
  static long int timev = 0;

/*
  If seed == 0, the seed value is derived from the time.  However, if the time
  from two successive invocations does not increase (often time has a 1 second
  resolution), the time value is incremented.
*/
/* Seed value */
  if (seed == 0) {
    t = (long int) time (NULL);
    if (t <= timev)
      t = timev + 1;
    timev = t;
    is = timev;
  }
  else
    is = seed;
  if (is < 0)
    is = -is;
  else if (is == 0)
    is = DSEED;

/*  Generate 8 extra warm up points, save one of the points as y */
  for (j = NTAB + 7; j >= 0; --j) {
    k = is / Q;
    is = A * (is - k * Q) - R * k;
    if (is < 0)
      is += M;
    if (j < NTAB)
      iv[j] = is;
  }
  y = iv[0];
}
