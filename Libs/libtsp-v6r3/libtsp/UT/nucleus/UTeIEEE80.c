/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  void UTeIEEE80 (double V, unsigned char b[10])

Purpose:
  Convert a double to an 80-bit IEEE double-extended value

Description:
  This routine generates the (big-endian) 80-bit IEEE double-extended
  representation for an input double value.

Parameters:
   -> double V
      Input double value
  <-  unsigned char b[10]
      Output byte array containing the 80-bit representation.  The first byte
      contains the sign bit and the first part of the exponent.

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.11 $  $Date: 2003/05/09 03:23:41 $

-------------------------------------------------------------------------*/

#include <math.h>		/* frexp, ldexp, floor */

#include <libtsp/nucleus.h>
#include <libtsp/UTtypes.h>

#define EXP_BIAS	16383
#define EXPC_ZERO	0
#define EXPC_DENORM	0
#define EXPC_INF	32767
#define EXPC_NAN	32767

/* IEEE 754 double-extended 80-bit format:
   Standard Apple Numerics Environment (SANE) format
   1-bit    15-bits      64-bits
   sign     exponent     mantissa
   Notes:
   1) The mantissa is a normalized value with no hidden bit.  The implicit
      decimal point is after the first bit of the mantissa.
   2) The exponent code is a biased value.  Consider the exponent code to be
      an unsigned 15-bit quantity, with values from 0 to 32767.  The exponent
      code is the true exponent + 16383.  However, exponent code 32767 has a
      special meaning.  The exponent can take on values in the range
      [emin, emax], where emin = -16383 and emax = +16383.
   3) Special values:
       exponent          mantissa           represents
       e = emin          m == 0      +/- 0            zero
       e = emin          0 < m < 1   +/- m x 2^emin   denormalized
      emin <= e <= emax  1 <= m < 2  +/- m x 2^e      normalized
       e = emax+1        m == 0      +/- infinity     infinity
       e = emax+1        m != 0      NaN              not-a-number

   Notes:
   1. The routine frexp normally returns a mantissa, 0.5 <= dmant < 1.  The
      IEEE standard deals with a normalized mantissa, 1 <= m < 2.  The value
      can be expressed in either form,
        m = 2 * d,   e = t - 1,
	VN = m * 2^e = d * 2^t.
      The value e determines the exponent to be stored.  The 64-bit integer
      representation of the mantissa can be determined from d,
        M = d * 2^64.
      In practice M is composed of two 32-bit components,
        M = MU * 2^32 + ML.
      Then the components can be determined as
        MU = int (d * 2^32)
        ML = 2^32 * (d * 2^32 - MU)
   2. For double precision input values, the exponent range is generally less
      than that for the double-extended IEEE format.  For instance, IEEE
      double format has an exponent range of [ -1022, +1023 ], while the
      double-extended IEEE format has an exponent range of [ -16383, +16383 ].
      Even with denormalized double values, frexp will never return a value
      even near the lower limit of the the double-extended exponent range -
      denormalized double-extended values will never occur.
   3. For frexp, an input of infinity gives a mantissa of infinity.  On some
      machines the exponent is returned as zero, on others it is equal to the
      largest positive integer.
   4. The DEC cc2.1 version of frexp has problems for tiny values.  For
      VN = 4e-308, frexp gives the correct values: dmant = 0.898847 and
      texp = -1021.  But halve VN and the mantissa remains unchanged but texp
      jumps to -1024.
*/


void
UTeIEEE80 (double V, unsigned char b[10])

{
  uint4_t mantH, mantL;
  int texp;
  int2_t expc;
  double VN, dmant, Umant;

/* Get the absolute value of V */
  if (V < 0.0)
    VN = -V;
  else
    VN = V;	/* NaN goes here */

  if (VN == 0.0) {

/* Zero value */
    mantH = 0;
    mantL = 0;
    expc = EXPC_ZERO;
  }

  else if (VN > 0.0) {		/* Not NaN */


    /* Decompose VN as d * 2^t */
    dmant = frexp (VN, &texp);
    /* expc = texp + EXP_BIAS - 1 */

                   /* expc > EXPC_INF */
    if (dmant > 1. || texp > EXPC_INF - EXP_BIAS + 1) {

      /* dmant == infinity (for V = infinity) or large exponent */
      mantH = 0;
      mantL = 0;
      expc = EXPC_INF;
    }
          /* expc < 0 */
    else if (texp < -EXP_BIAS + 1) {

      /* Denormalized value, 0 < m < 1 (normally not encountered)
	 VN = d * 2^t
	    = [ d * 2^(t-tmin) ] * 2^tmin
	 where tmin = emin + 1.
      */
                         /* expc + 32 */
      Umant = ldexp (dmant, texp + EXP_BIAS - 1 + 32);
      mantH = (uint4_t) Umant;
      mantL = (uint4_t) ldexp (Umant - floor (Umant), 32);
      expc = EXPC_DENORM;
    }

    else {

      /* Normalized value */
      Umant = ldexp (dmant, 32);
      mantH = (uint4_t) Umant;
      mantL = (uint4_t) ldexp (Umant - floor (Umant), 32);
      expc = texp + EXP_BIAS - 1;
    }
  }

  else {			/* NaN */

/* For NaN's, there is machine-dependent information in the mantissa value
   which we cannot interpret.  Here we set the most significant bit of the
   exponent to be non-zero.
*/
    mantH = (uint4_t) 0x80000000;
    mantL = 0;
    expc = EXPC_NAN;
  }

/* Set the byte values */
  b[0] = (unsigned char) (expc >> 8);
  if (V < 0.0)
    b[0] = b[0] | 0x80;
  b[1] = (unsigned char) expc;
  b[2] = (unsigned char) (mantH >> 24);
  b[3] = (unsigned char) (mantH >> 16);
  b[4] = (unsigned char) (mantH >> 8);
  b[5] = (unsigned char) mantH;
  b[6] = (unsigned char) (mantL >> 24);
  b[7] = (unsigned char) (mantL >> 16);
  b[8] = (unsigned char) (mantL >> 8);
  b[9] = (unsigned char) mantL;
}
