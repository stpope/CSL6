/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  int AFfRdMulaw (AFILE *AFp, long int offs, float Dbuff[], int Nreq)

Purpose:
  Read 8-bit mu-law data from an audio file (return float values)

Description:
  This routine reads a specified number of 8-bit mu-law samples from an audio
  file.  The data in the file is converted to float format on output.

Parameters:
  <-  int AFfRdMulaw
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

#define LW		FDL_MULAW8
#define MINV(a, b)	(((a) < (b)) ? (a) : (b))
#define NBBUF		8192

#define FREAD(buf,size,nv,fp)	(int) fread ((char *) buf, (size_t) size, \
					     (size_t) nv, fp)

/* ITU-T Recommendation G.711
   Mu-law data is stored in sign-magnitude bit-complemented format.  The sign
   bit is 0 for positive data.  After complementing the bits, the byte is in
   sign-segment-mantissa format.
   mu-law data  complement  segment step-size       value
   0 000 xxxx   1 111 yyyy     7      256     -8031, ... , -4191
   0 001 xxxx   1 110 yyyy     6      128     -3999, ... , -2079
   0 010 xxxx   1 101 yyyy     5       64     -1983, ... , -1023
   0 011 xxxx   1 100 yyyy     4       32      -975, ... , -495
   0 100 xxxx   1 011 yyyy     3       16      -471, ... , -231
   0 101 xxxx   1 010 yyyy     2        8      -219, ... , -99
   0 110 xxxx   1 001 yyyy     1        4       -93, ... , -33
   0 111 xxxx   1 000 yyyy     0        2       -30, ... ,  0
   1 000 xxxx   0 111 yyyy     7      256      8031, ... , 4191
   1 001 xxxx   0 110 yyyy     6      128      3999, ... , 2079
   1 010 xxxx   0 101 yyyy     5       64      1983, ... , 1023
   1 011 xxxx   0 100 yyyy     4       32       975, ... , 495
   1 100 xxxx   0 011 yyyy     3       16       471, ... , 231
   1 101 xxxx   0 010 yyyy     2        8       219, ... , 99
   1 110 xxxx   0 001 yyyy     1        4        93, ... , 33
   1 111 xxxx   0 000 yyyy     0        2        30, ... , 0

  The following table implements the conversion, with the output data scaled
  by 4, and includes the complementing operation.  This corresponds to output
  values from -32124 to +32124 (for AFp->ScaleF = 1).
*/

static const double Mutab[256] = {
  -32124, -31100, -30076, -29052, -28028, -27004, -25980, -24956,
  -23932, -22908, -21884, -20860, -19836, -18812, -17788, -16764,
  -15996, -15484, -14972, -14460, -13948, -13436, -12924, -12412,
  -11900, -11388, -10876, -10364,  -9852,  -9340,  -8828,  -8316,
   -7932,  -7676,  -7420,  -7164,  -6908,  -6652,  -6396,  -6140,
   -5884,  -5628,  -5372,  -5116,  -4860,  -4604,  -4348,  -4092,
   -3900,  -3772,  -3644,  -3516,  -3388,  -3260,  -3132,  -3004,
   -2876,  -2748,  -2620,  -2492,  -2364,  -2236,  -2108,  -1980,
   -1884,  -1820,  -1756,  -1692,  -1628,  -1564,  -1500,  -1436,
   -1372,  -1308,  -1244,  -1180,  -1116,  -1052,   -988,   -924,
    -876,   -844,   -812,   -780,   -748,   -716,   -684,   -652,
    -620,   -588,   -556,   -524,   -492,   -460,   -428,   -396,
    -372,   -356,   -340,   -324,   -308,   -292,   -276,   -260,
    -244,   -228,   -212,   -196,   -180,   -164,   -148,   -132,
    -120,   -112,   -104,    -96,    -88,    -80,    -72,    -64,
     -56,    -48,    -40,    -32,    -24,    -16,     -8,      0,
   32124,  31100,  30076,  29052,  28028,  27004,  25980,  24956,
   23932,  22908,  21884,  20860,  19836,  18812,  17788,  16764,
   15996,  15484,  14972,  14460,  13948,  13436,  12924,  12412,
   11900,  11388,  10876,  10364,   9852,   9340,   8828,   8316,
    7932,   7676,   7420,   7164,   6908,   6652,   6396,   6140,
    5884,   5628,   5372,   5116,   4860,   4604,   4348,   4092,
    3900,   3772,   3644,   3516,   3388,   3260,   3132,   3004,
    2876,   2748,   2620,   2492,   2364,   2236,   2108,   1980,
    1884,   1820,   1756,   1692,   1628,   1564,   1500,   1436,
    1372,   1308,   1244,   1180,   1116,   1052,    988,    924,
     876,    844,    812,    780,    748,    716,    684,    652,
     620,    588,    556,    524,    492,    460,    428,    396,
     372,    356,    340,    324,    308,    292,    276,    260,
     244,    228,    212,    196,    180,    164,    148,    132,
     120,    112,    104,     96,     88,     80,     72,     64,
      56,     48,     40,     32,     24,     16,      8,      0
};


int
AFfRdMulaw (AFILE *AFp, float Dbuff[], int Nreq)

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
      Dbuff[is] = (float) (g * Mutab[Buf[i]]);
      ++is;
    }

    if (Nr < N)
      break;
  }

  return is;
}
