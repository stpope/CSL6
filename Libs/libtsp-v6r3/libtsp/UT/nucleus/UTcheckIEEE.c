/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  int UTcheckIEEE (void)

Purpose:
  Check if host uses IEEE standard 754 format for float values

Description:
  This routine determines if the current host uses IEEE standard 754 format to
  represent float values.

Parameters:
  <-  int UTcheckIEEE
      Flag, 1 if host uses IEEE float format, 0 otherwise

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.14 $  $Date: 2003/05/09 03:23:41 $

-------------------------------------------------------------------------*/

#include <libtsp/nucleus.h>
#include <libtsp/UTtypes.h>

/* Note:
   On many Unix machines, this routine could instead check the setting of
   _IEEE in values.h.
*/

/* IEEE float value check values */
static const uint4_t IEEEiv = 0xc3268000;
static const float4_t IEEEfv = -166.5;


int
UTcheckIEEE (void)

{
  union {
    float4_t fv;
    uint4_t iv;
  } Floatv;

/* Store a float value and check the bit pattern of the result */
  Floatv.fv = IEEEfv;
  if (Floatv.iv == IEEEiv)
    return 1;
  else
    return 0;
}
