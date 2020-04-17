/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  int AFdWrAlaw (AFILE *AFp, const double Dbuff[], int Nval)

Purpose:
  Write 8-bit A-law data to an audio file (double input values)

Description:
  This routine writes a specified number of 8-bit A-law samples to an audio
  file.  The input to this routine is a buffer of double values.

Parameters:
  <-  int AFdWrAlaw
      Number of samples written.  If this value is less than Nval, an error
      has occurred.
   -> AFILE *AFp
      Audio file pointer for an audio file opened by AFopnWrite
   -> const double Dbuff[]
      Array of doubles with the samples to be written
   -> int Nval
      Number of samples to be written

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.2 $  $Date: 2003/05/09 01:11:34 $

-------------------------------------------------------------------------*/

#include <libtsp.h>
#include <libtsp/AFdataio.h>
#include <libtsp/AFpar.h>
#include <libtsp/UTtypes.h>

#define LW		FDL_ALAW8
#define MINV(a, b)	(((a) < (b)) ? (a) : (b))
#define NBBUF		8192
#define NLEV		256
#define AMAX		32768	/* Xq[NLEV-1] + (Xq[NLEV-1] - Xq[NLEV-2]) */

#define FWRITE(buf,size,nv,fp)	(int) fwrite ((const char *) buf, \
					      (size_t) size, (size_t) nv, fp)

/* ITU-T Recommendation G.711
   Conversion to A-law is carried out using a quantization operation.  Given
   an array of (ordered) decision levels, the interval containing the input
   value is determined.  However, there is an ambiguity at the quantizer 
   decision levels themselves.  G.711 allows the output value corresponding to
   the decision levels to go either up or down.  The decision levels themselves
   are symmetric with respect to zero.  The ITU-T Software Tool Library (STL)
   (Recommendation G.191) has a reference implementation for which positive
   values on the decision levels move upward (away from zero) and negative
   values on the decision levels also move upward (towards zero).

   The present implementation uses a direct quantization.  For the quantization
   routine SPfQuantL, the intervals are defined as Xq[i-1] <= x < Xq[i], and
   so input values which lie at the decision level move upward.  For integer
   valued inputs, the results match exactly the results of the STL96 G.711
   reference routine.
   
   A-law data is stored in sign-magnitude alternate-bit-complemented format.
   The sign bit is 1 for positive data.  The uncomplemented byte is in
   sign-segment-mantissa format.

    bin no.   signal value     bit pattern  complement
     0-15      1, ... , 31     1 000 xxxx   1 101 xyxy
    16-31     33, ... , 63     1 001 xxxx   1 100 xyxy
    32-47     66, ... , 126    1 010 xxxx   1 111 xyxy
    48-63    132, ... , 252    1 011 xxxx   1 110 xyxy
    64-79    264, ... , 504    1 100 xxxx   1 001 xyxy
    80-95    528, ... , 1008   1 101 xxxx   1 000 xyxy
    96-111  1056, ... , 2016   1 110 xxxx   1 011 xyxy
   112-127  2112, ... , 4032   1 111 xxxx   1 010 xyxy

   AFquant is used to find a quantization region for the input value.  The
   A-law value is determined from this region index by table lookup.  The
   table includes the effect of complementing the bit pattern.  The decision
   levels are scaled by 8, corresponding to quantized values from -32256 to
   +32256 (for AFp->ScaleF = 1).
*/

static const double Xq[NLEV-1] = {
          -31744, -30720, -29696, -28672, -27648, -26624, -25600,
  -24576, -23552, -22528, -21504, -20480, -19456, -18432, -17408,
  -16384, -15872, -15360, -14848, -14336, -13824, -13312, -12800,
  -12288, -11776, -11264, -10752, -10240,  -9728,  -9216,  -8704,
   -8192,  -7936,  -7680,  -7424,  -7168,  -6912,  -6656,  -6400,
   -6144,  -5888,  -5632,  -5376,  -5120,  -4864,  -4608,  -4352,
   -4096,  -3968,  -3840,  -3712,  -3584,  -3456,  -3328,  -3200,
   -3072,  -2944,  -2816,  -2688,  -2560,  -2432,  -2304,  -2176,
   -2048,  -1984,  -1920,  -1856,  -1792,  -1728,  -1664,  -1600,
   -1536,  -1472,  -1408,  -1344,  -1280,  -1216,  -1152,  -1088,
   -1024,   -992,   -960,   -928,   -896,   -864,   -832,   -800,
    -768,   -736,   -704,   -672,   -640,   -608,   -576,   -544,
    -512,   -496,   -480,   -464,   -448,   -432,   -416,   -400,
    -384,   -368,   -352,   -336,   -320,   -304,   -288,   -272,
    -256,   -240,   -224,   -208,   -192,   -176,   -160,   -144,
    -128,   -112,    -96,    -80,    -64,    -48,    -32,    -16,
       0,
      16,     32,     48,     64,     80,     96,    112,    128,
     144,    160,    176,    192,    208,    224,    240,    256,
     272,    288,    304,    320,    336,    352,    368,    384,
     400,    416,    432,    448,    464,    480,    496,    512,
     544,    576,    608,    640,    672,    704,    736,    768,
     800,    832,    864,    896,    928,    960,    992,   1024,
    1088,   1152,   1216,   1280,   1344,   1408,   1472,   1536,
    1600,   1664,   1728,   1792,   1856,   1920,   1984,   2048,
    2176,   2304,   2432,   2560,   2688,   2816,   2944,   3072,
    3200,   3328,   3456,   3584,   3712,   3840,   3968,   4096,
    4352,   4608,   4864,   5120,   5376,   5632,   5888,   6144,
    6400,   6656,   6912,   7168,   7424,   7680,   7936,   8192,
    8704,   9216,   9728,  10240,  10752,  11264,  11776,  12288,
   12800,  13312,  13824,  14336,  14848,  15360,  15872,  16384,
   17408,  18432,  19456,  20480,  21504,  22528,  23552,  24576,
   25600,  26624,  27648,  28672,  29696,  30720,  31744
};
static const uint1_t Yq[NLEV] = {
  0x2A, 0x2B, 0x28, 0x29, 0x2E, 0x2F, 0x2C, 0x2D,
  0x22, 0x23, 0x20, 0x21, 0x26, 0x27, 0x24, 0x25,
  0x3A, 0x3B, 0x38, 0x39, 0x3E, 0x3F, 0x3C, 0x3D,
  0x32, 0x33, 0x30, 0x31, 0x36, 0x37, 0x34, 0x35,
  0x0A, 0x0B, 0x08, 0x09, 0x0E, 0x0F, 0x0C, 0x0D,
  0x02, 0x03, 0x00, 0x01, 0x06, 0x07, 0x04, 0x05,
  0x1A, 0x1B, 0x18, 0x19, 0x1E, 0x1F, 0x1C, 0x1D,
  0x12, 0x13, 0x10, 0x11, 0x16, 0x17, 0x14, 0x15,
  0x6A, 0x6B, 0x68, 0x69, 0x6E, 0x6F, 0x6C, 0x6D,
  0x62, 0x63, 0x60, 0x61, 0x66, 0x67, 0x64, 0x65,
  0x7A, 0x7B, 0x78, 0x79, 0x7E, 0x7F, 0x7C, 0x7D,
  0x72, 0x73, 0x70, 0x71, 0x76, 0x77, 0x74, 0x75,
  0x4A, 0x4B, 0x48, 0x49, 0x4E, 0x4F, 0x4C, 0x4D,
  0x42, 0x43, 0x40, 0x41, 0x46, 0x47, 0x44, 0x45,
  0x5A, 0x5B, 0x58, 0x59, 0x5E, 0x5F, 0x5C, 0x5D,
  0x52, 0x53, 0x50, 0x51, 0x56, 0x57, 0x54, 0x55,
  0xD5, 0xD4, 0xD7, 0xD6, 0xD1, 0xD0, 0xD3, 0xD2,
  0xDD, 0xDC, 0xDF, 0xDE, 0xD9, 0xD8, 0xDB, 0xDA,
  0xC5, 0xC4, 0xC7, 0xC6, 0xC1, 0xC0, 0xC3, 0xC2,
  0xCD, 0xCC, 0xCF, 0xCE, 0xC9, 0xC8, 0xCB, 0xCA,
  0xF5, 0xF4, 0xF7, 0xF6, 0xF1, 0xF0, 0xF3, 0xF2,
  0xFD, 0xFC, 0xFF, 0xFE, 0xF9, 0xF8, 0xFB, 0xFA,
  0xE5, 0xE4, 0xE7, 0xE6, 0xE1, 0xE0, 0xE3, 0xE2,
  0xED, 0xEC, 0xEF, 0xEE, 0xE9, 0xE8, 0xEB, 0xEA,
  0x95, 0x94, 0x97, 0x96, 0x91, 0x90, 0x93, 0x92,
  0x9D, 0x9C, 0x9F, 0x9E, 0x99, 0x98, 0x9B, 0x9A,
  0x85, 0x84, 0x87, 0x86, 0x81, 0x80, 0x83, 0x82,
  0x8D, 0x8C, 0x8F, 0x8E, 0x89, 0x88, 0x8B, 0x8A,
  0xB5, 0xB4, 0xB7, 0xB6, 0xB1, 0xB0, 0xB3, 0xB2,
  0xBD, 0xBC, 0xBF, 0xBE, 0xB9, 0xB8, 0xBB, 0xBA,
  0xA5, 0xA4, 0xA7, 0xA6, 0xA1, 0xA0, 0xA3, 0xA2,
  0xAD, 0xAC, 0xAF, 0xAE, 0xA9, 0xA8, 0xAB, 0xAA
};


int
AFdWrAlaw (AFILE *AFp, const double Dbuff[], int Nval)

{
  int is, N, Nw, i;
  uint1_t Buf[NBBUF/LW];
  double g, Dv;

/* Write data to the audio file */
  is = 0;
  g = AFp->ScaleF;
  while (is < Nval) {
    N = MINV (NBBUF / LW, Nval - is);
    for (i = 0; i < N; ++i) {
      Dv = g * Dbuff[i+is];
      if (Dv > AMAX || Dv < -AMAX)
	++AFp->Novld;
      Buf[i] = Yq[SPdQuantL (Dv, Xq, NLEV)];
    }
    Nw = FWRITE (Buf, LW, N, AFp->fp);
    is += Nw;
    if (Nw < N)
      break;
  }

  return is;
}
