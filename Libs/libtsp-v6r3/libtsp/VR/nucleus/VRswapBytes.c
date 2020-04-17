/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  void VRswapBytes (const void *BuffI, void *BuffO, int Size, int Nelem)

Purpose:
  Swap bytes in an array of values

Description:
  This routine does a swap of the bytes making up each word in an array of
  values.  Swapping bytes involves reversing the order of the constituent bytes
  in each word.

Parameters:
   -> const void *BuffI
      Array of values, each element having wordsize Size bytes
  <-  void *BuffO
      Output array of byte swapped values.  The output buffer can occupy the
      same memory as the input buffer.
   -> int Size
      Size of each element in bytes.  This value must be 1 (no-op), 2, 4 or 8.
   -> int Nelem
      Number of values to be byte-swapped

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.21 $  $Date: 2003/05/09 03:33:52 $

-------------------------------------------------------------------------*/

#include <stdlib.h>
#include <string.h>

#include <libtsp.h>
#include <libtsp/nucleus.h>
#include <libtsp/UTtypes.h>
#include <libtsp/VRmsg.h>


void
VRswapBytes (const void *BuffI, void *BuffO, int Size, int Nelem)

{
  const unsigned char *cp;
  unsigned char *sp;
  const int2_t *B2;
  int2_t *S2;
  const int4_t *B4;
  int4_t *S4;
  const double8_t *B8;
  double8_t *S8;
  unsigned char t;

  switch (Size) {
  case (1):
    memcpy (BuffO, BuffI, (size_t) ((long int) Size * Nelem));
    break;

  case (2):
    B2 = (const int2_t *) BuffI;
    S2 = (int2_t *) BuffO;
    while (Nelem-- > 0) {
      cp = (const unsigned char *) B2;
      sp = (unsigned char *) S2;
      t = cp[1];  sp[1] = cp[0];  sp[0] = t;
      ++B2;
      ++S2;
    }
    break;

  case (4):
    B4 = (const int4_t *) BuffI;
    S4 = (int4_t *) BuffO;
    while (Nelem-- > 0) {
      cp = (const unsigned char *) B4;
      sp = (unsigned char *) S4;
      t = cp[3];  sp[3] = cp[0];  sp[0] = t;
      t = cp[2];  sp[2] = cp[1];  sp[1] = t;
      ++B4;
      ++S4;
    }
    break;

  case (8):
    B8 = (const double8_t *) BuffI;
    S8 = (double8_t *) BuffO;
    while (Nelem-- > 0) {
      cp = (const unsigned char *) B8;
      sp = (unsigned char *) S8;
      t = cp[7];  sp[7] = cp[0];  sp[0] = t;
      t = cp[6];  sp[6] = cp[1];  sp[1] = t;
      t = cp[5];  sp[5] = cp[2];  sp[2] = t;
      t = cp[4];  sp[4] = cp[3];  sp[3] = t;
      ++B8;
      ++S8;
    }
    break;

  default:
    UThalt ("VRswapBytes: %s", VRM_BadSize);
    break;
  }
  return;
}
