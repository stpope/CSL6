/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  int AFfRdAlaw (AFILE *AFp, float Dbuff[], int Nreq)

Purpose:
  Read 8-bit A-law data from an audio file (return float values)

Description:
  This routine reads a specified number of 8-bit A-law samples from an audio
  file.  The data in the file is converted to float format on output.

Parameters:
  <-  int AFfRdAlaw
      Number of data values transferred from the file.  On reaching the end of
      the file, this value may be less than Nreq.
   -> AFILE *AFp
      Audio file pointer for an audio file opened by AFopnRead
  <-  float Dbuff[]
      Array of floats to receive the samples
   -> int Nreq
      Number of samples requested.  Nreq may be zero.

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.2 $  $Date: 2003/05/09 01:11:34 $

-------------------------------------------------------------------------*/

#include <libtsp/AFdataio.h>
#include <libtsp/AFpar.h>
#include <libtsp/UTtypes.h>

#define LW		FDL_ALAW8
#define MINV(a, b)	(((a) < (b)) ? (a) : (b))
#define NBBUF		8192

#define FREAD(buf,size,nv,fp)	(int) fread ((char *) buf, (size_t) size, \
					     (size_t) nv, fp)

/* ITU-T Recommendation G.711
   A-law data is stored in sign-magnitude alternate-bit-complemented format.
   The sign bit is 1 for positive data.  After complementing the  bits, the
   byte is in sign-segment-mantissa format.
   A-law data   complement  segment step-size       value
   0 000 xxxx   0 101 xyxy     5       32     -1008, ... , -528
   0 001 xxxx   0 100 xyxy     4       16      -504, ... , -264
   0 010 xxxx   0 111 xyxy     7      128     -4032, ... , -2112
   0 011 xxxx   0 110 xyxy     6       64     -2016, ... , -1056
   0 100 xxxx   0 001 xyxy     1        2       -63, ... , -33
   0 101 xxxx   0 000 xyxy     1        2       -31, ... , -1
   0 110 xxxx   0 011 xyxy     3        8      -252, ... , -132
   0 111 xxxx   0 010 xyxy     2        4      -126, ... , -66
   1 000 xxxx   1 101 xyxy     5       32       528, ... , 1008
   1 001 xxxx   1 100 xyxy     4       16       264, ... , 504
   1 010 xxxx   1 111 xyxy     7      128      2112, ... , 4032
   1 011 xxxx   1 110 xyxy     6       64      1056, ... , 2016
   1 100 xxxx   1 001 xyxy     1        2        33, ... , 63
   1 101 xxxx   1 000 xyxy     1        2         1, ... , 31
   1 110 xxxx   1 011 xyxy     3        8       132, ... , 252
   1 111 xxxx   1 010 xyxy     2        4        66, ... , 126

  The following table implements the conversion, with the output data scaled
  by 8, and includes the complementing operation.  This corresponds to output
  values from -32256 to +32256 (for AFp->ScaleF = 1).
*/

static const double Atab[256] = {
   -5504,  -5248,  -6016,  -5760,  -4480,  -4224,  -4992,  -4736, 
   -7552,  -7296,  -8064,  -7808,  -6528,  -6272,  -7040,  -6784, 
   -2752,  -2624,  -3008,  -2880,  -2240,  -2112,  -2496,  -2368, 
   -3776,  -3648,  -4032,  -3904,  -3264,  -3136,  -3520,  -3392, 
  -22016, -20992, -24064, -23040, -17920, -16896, -19968, -18944, 
  -30208, -29184, -32256, -31232, -26112, -25088, -28160, -27136, 
  -11008, -10496, -12032, -11520,  -8960,  -8448,  -9984,  -9472, 
  -15104, -14592, -16128, -15616, -13056, -12544, -14080, -13568, 
    -344,   -328,   -376,   -360,   -280,   -264,   -312,   -296, 
    -472,   -456,   -504,   -488,   -408,   -392,   -440,   -424, 
     -88,    -72,   -120,   -104,    -24,     -8,    -56,    -40, 
    -216,   -200,   -248,   -232,   -152,   -136,   -184,   -168, 
   -1376,  -1312,  -1504,  -1440,  -1120,  -1056,  -1248,  -1184, 
   -1888,  -1824,  -2016,  -1952,  -1632,  -1568,  -1760,  -1696, 
    -688,   -656,   -752,   -720,   -560,   -528,   -624,   -592, 
    -944,   -912,  -1008,   -976,   -816,   -784,   -880,   -848, 
    5504,   5248,   6016,   5760,   4480,   4224,   4992,   4736, 
    7552,   7296,   8064,   7808,   6528,   6272,   7040,   6784, 
    2752,   2624,   3008,   2880,   2240,   2112,   2496,   2368, 
    3776,   3648,   4032,   3904,   3264,   3136,   3520,   3392, 
   22016,  20992,  24064,  23040,  17920,  16896,  19968,  18944, 
   30208,  29184,  32256,  31232,  26112,  25088,  28160,  27136, 
   11008,  10496,  12032,  11520,   8960,   8448,   9984,   9472, 
   15104,  14592,  16128,  15616,  13056,  12544,  14080,  13568, 
     344,    328,    376,    360,    280,    264,    312,    296, 
     472,    456,    504,    488,    408,    392,    440,    424, 
      88,     72,    120,    104,     24,      8,     56,     40, 
     216,    200,    248,    232,    152,    136,    184,    168, 
    1376,   1312,   1504,   1440,   1120,   1056,   1248,   1184, 
    1888,   1824,   2016,   1952,   1632,   1568,   1760,   1696, 
     688,    656,    752,    720,    560,    528,    624,    592, 
     944,    912,   1008,    976,    816,    784,    880,    848
};


int
AFfRdAlaw (AFILE *AFp, float Dbuff[], int Nreq)

{
  int is, N, i, Nr;
  uint1_t Buf[NBBUF/LW];
  double g;

  for (is = 0; is < Nreq; ) {

    /* Read data from the audio file */
    N = MINV (NBBUF / LW, Nreq - is);
    Nr = FREAD (Buf, LW, N, AFp->fp);

    /* Convert to float */
    g = AFp->ScaleF;
    for (i = 0; i < Nr; ++i) {
      Dbuff[is] = (float) (g * Atab[Buf[i]]);
      ++is;
    }

    if (Nr < N)
      break;
  }

  return is;
}
