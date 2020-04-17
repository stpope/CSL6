/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  int AFfWrMulaw (AFILE *AFp, const float Dbuff[], int Nval)

Purpose:
  Write 8-bit mu-law data to an audio file (float input values)

Description:
  This routine writes a specified number of 8-bit mu-law samples to an audio
  file.  The input to this routine is a buffer of float values.

Parameters:
  <-  int AFfWrMulaw
      Number of samples written.  If this value is less than Nval, an error
      has occurred.
   -> AFILE *AFp
      Audio file pointer for an audio file opened by AFopnWrite
   -> const float Dbuff[]
      Array of floats with the samples to be written
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

#define LW		FDL_MULAW8
#define MINV(a, b)	(((a) < (b)) ? (a) : (b))
#define NBBUF		8192
#define NLEV		256
#define AMAX		32636	/* Xq[NLEV-1] + (Xq[NLEV-1] - Xq[NLEV-2]) */

#define FWRITE(buf,size,nv,fp)	(int) fwrite ((const char *) buf, \
					      (size_t) size, (size_t) nv, fp)

/* ITU-T Recommendation G.711
   Conversion to mu-law is carried out using a quantization operation.  Given
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
   
   Mu-law data is stored in sign-magnitude bit-complemented format.  The sign
   bit is 0 for positive data.  The uncomplemented byte is in sign-segment-
   mantissa format.

    bin no.   signal value     bit pattern  complement
     0-15      0, ... , 30     0 000 xxxx   1 111 yyyy
    16-31     33, ... , 93     0 001 xxxx   1 110 yyyy
    32-47     99, ... , 217    0 010 xxxx   1 101 yyyy
    48-63    231, ... , 471    0 011 xxxx   1 100 yyyy
    64-79    495, ... , 975    0 100 xxxx   1 011 yyyy
    80-95   1023, ... , 1983   0 101 xxxx   1 010 yyyy
    96-111  2079, ... , 3999   0 110 xxxx   1 001 yyyy
   112-127  4191, ... , 8031   0 111 xxxx   1 000 yyyy

   AFquant is used to find a quantization region for the input value.  The
   mu-law value is determined from this region index by table lookup.  The
   table includes the effect of complementing the bit pattern.  G.711 allows
   for the optional suppression of the all zero code.  In this implementation,
   the zero code is NOT suppressed.  The decision levels are scaled by 4,
   corresponding to quantized values from -32124 to +32124 (for
   AFp->ScaleF = 1).
*/

static const double Xq[NLEV-1] = {
          -31612, -30588, -29564, -28540, -27516, -26492, -25468,
  -24444, -23420, -22396, -21372, -20348, -19324, -18300, -17276,
  -16252, -15740, -15228, -14716, -14204, -13692, -13180, -12668,
  -12156, -11644, -11132, -10620, -10108,  -9596,  -9084,  -8572,
   -8060,  -7804,  -7548,  -7292,  -7036,  -6780,  -6524,  -6268,
   -6012,  -5756,  -5500,  -5244,  -4988,  -4732,  -4476,  -4220,
   -3964,  -3836,  -3708,  -3580,  -3452,  -3324,  -3196,  -3068,
   -2940,  -2812,  -2684,  -2556,  -2428,  -2300,  -2172,  -2044,
   -1916,  -1852,  -1788,  -1724,  -1660,  -1596,  -1532,  -1468,
   -1404,  -1340,  -1276,  -1212,  -1148,  -1084,  -1020,   -956,
    -892,   -860,   -828,   -796,   -764,   -732,   -700,   -668,
    -636,   -604,   -572,   -540,   -508,   -476,   -444,   -412,
    -380,   -364,   -348,   -332,   -316,   -300,   -284,   -268,
    -252,   -236,   -220,   -204,   -188,   -172,   -156,   -140,
    -124,   -116,   -108,   -100,    -92,    -84,    -76,    -68,
     -60,    -52,    -44,    -36,    -28,    -20,    -12,     -4,
       0,
       4,     12,     20,     28,     36,     44,     52,     60,
      68,     76,     84,     92,    100,    108,    116,    124,
     140,    156,    172,    188,    204,    220,    236,    252,
     268,    284,    300,    316,    332,    348,    364,    380,
     412,    444,    476,    508,    540,    572,    604,    636,
     668,    700,    732,    764,    796,    828,    860,    892,
     956,   1020,   1084,   1148,   1212,   1276,   1340,   1404,
    1468,   1532,   1596,   1660,   1724,   1788,   1852,   1916,
    2044,   2172,   2300,   2428,   2556,   2684,   2812,   2940,
    3068,   3196,   3324,   3452,   3580,   3708,   3836,   3964,
    4220,   4476,   4732,   4988,   5244,   5500,   5756,   6012,
    6268,   6524,   6780,   7036,   7292,   7548,   7804,   8060,
    8572,   9084,   9596,  10108,  10620,  11132,  11644,  12156,
   12668,  13180,  13692,  14204,  14716,  15228,  15740,  16252,
   17276,  18300,  19324,  20348,  21372,  22396,  23420,  24444,
   25468,  26492,  27516,  28540,  29564,  30588,  31612
};
static const uint1_t Yq[NLEV] = {
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
  0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 
  0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
  0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 
  0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
  0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, 
  0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
  0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F, 
  0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
  0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 
  0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57,
  0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F, 
  0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67,
  0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F, 
  0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77,
  0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F, 
  0xFF, 0xFE, 0xFD, 0xFC, 0xFB, 0xFA, 0xF9, 0xF8,
  0xF7, 0xF6, 0xF5, 0xF4, 0xF3, 0xF2, 0xF1, 0xF0,
  0xEF, 0xEE, 0xED, 0xEC, 0xEB, 0xEA, 0xE9, 0xE8,
  0xE7, 0xE6, 0xE5, 0xE4, 0xE3, 0xE2, 0xE1, 0xE0,
  0xDF, 0xDE, 0xDD, 0xDC, 0xDB, 0xDA, 0xD9, 0xD8,
  0xD7, 0xD6, 0xD5, 0xD4, 0xD3, 0xD2, 0xD1, 0xD0,
  0xCF, 0xCE, 0xCD, 0xCC, 0xCB, 0xCA, 0xC9, 0xC8,
  0xC7, 0xC6, 0xC5, 0xC4, 0xC3, 0xC2, 0xC1, 0xC0,
  0xBF, 0xBE, 0xBD, 0xBC, 0xBB, 0xBA, 0xB9, 0xB8,
  0xB7, 0xB6, 0xB5, 0xB4, 0xB3, 0xB2, 0xB1, 0xB0,
  0xAF, 0xAE, 0xAD, 0xAC, 0xAB, 0xAA, 0xA9, 0xA8,
  0xA7, 0xA6, 0xA5, 0xA4, 0xA3, 0xA2, 0xA1, 0xA0,
  0x9F, 0x9E, 0x9D, 0x9C, 0x9B, 0x9A, 0x99, 0x98,
  0x97, 0x96, 0x95, 0x94, 0x93, 0x92, 0x91, 0x90,
  0x8F, 0x8E, 0x8D, 0x8C, 0x8B, 0x8A, 0x89, 0x88,
  0x87, 0x86, 0x85, 0x84, 0x83, 0x82, 0x81, 0x80
};


int
AFfWrMulaw (AFILE *AFp, const float Dbuff[], int Nval)

{
  int is, N, Nw, i;
  uint1_t Buf[NBBUF/LW];
  double Dv;

/* Write data to the audio file */
  is = 0;
  while (is < Nval) {
    N = MINV (NBBUF / LW, Nval - is);
    for (i = 0; i < N; ++i) {
      Dv = AFp->ScaleF * Dbuff[i+is];
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
