/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  double UTdIEEE80 (const unsigned char b[10])

Purpose:
  Convert an 80-bit IEEE double-extended value to a double

Description:
  This routine takes as input a 10 byte array containing the (big-endian)
  representation for an 80-bit IEEE double-extended value.

Parameters:
  <-  double UTdIEEE80
      Returned double value
   -> const unsigned char b[10]
      Input byte array containing the 80-bit representation.  The first byte
      contains the sign bit and the first part of the exponent.

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.7 $  $Date: 2003/05/09 03:23:41 $

-------------------------------------------------------------------------*/

#include <math.h>		/* ld_exp, sqrt */

#include <libtsp/nucleus.h>
#include <libtsp/UTtypes.h>

#define INFINITY	(sqrt(1.)/0.0)
#define NAN		(sqrt(-1.))

#define EXP_BIAS	16383
#define EXPC_ZERO	0
#define EXPC_NAN	32767
#define EXPC_INF	32767

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
*/


double UTdIEEE80 (const unsigned char b[10])

{
  double dv;
  int2_t expc;
  int exp;
  uint4_t mantH, mantL;

  expc = ((int2_t) (b[0] & 0x7F) << 8) | (int2_t) b[1];
  exp = expc - EXP_BIAS;

  mantH = ((uint4_t) b[2] << 24) | ((uint4_t) b[3] << 16) |
          ((uint4_t) b[4] << 8)  | ((uint4_t) b[5]);
  mantL = ((uint4_t) b[6] << 24) | ((uint4_t) b[7] << 16) |
          ((uint4_t) b[8] << 8)  | ((uint4_t) b[9]);

  if (expc == EXPC_ZERO && mantH == 0 && mantL == 0) 	/* Zero */

/* Not really a special case, since the general code below can handle it */
    dv = 0.0;

   else if (expc == EXPC_INF)		/* Infinity or NaN */

/* We can distinguish infinity (mantH == 0 && mantL == 0) from NaN.  However,
   for NaN, there is machine-dependent information in the mantissa value
   which we cannot interpret.
*/
     if (mantH == 0 && mantL == 0)
       dv = INFINITY;
     else
       dv = NAN;

  else					/* Normalized  mantissa */

/* The decoded value is
     dv = m * 2^exp, where m is the mantissa, m < 2.
   m is calculated from the integer components as
     m = 2^(-31) * (mantH + 2^(-32) * manL)
*/
    dv = ldexp ((double) mantH, exp - 31) +
         ldexp ((double) mantL, exp - 63);

/* Set the sign */
  if (b[0] & 0x80)
    dv = -dv;

  return dv;
}
