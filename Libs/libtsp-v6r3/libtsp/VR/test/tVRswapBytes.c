/*
   Test VRswapBytes

   $Id: tVRswapBytes.c 1.5 1999/06/05 libtsp-v6r3 $
*/

#include <libtsp.h>
#include <libtsp/nucleus.h>
#include <libtsp/UTtypes.h>

#define NMAX	128


int
main (int argc, const char *argv[])

{
  uint1_t x1[NMAX], y1[NMAX], z1[NMAX];
  uint2_t x2[NMAX], y2[NMAX], z2[NMAX];
  uint4_t x4[NMAX], y4[NMAX], z4[NMAX];
  double8_t x8[NMAX], y8[NMAX], z8[NMAX];
  int size, i, k, N;
  double x[NMAX], y[NMAX], z[NMAX];

  sscanf (argv[1], "%d", &size);
  k = 2;
  N = argc - k;
  for (i = 0; i < N; ++i, ++k)
    sscanf (argv[k], "%lg", &x[i]);

  switch (size) {
  case 1:
    for (i = 0; i < N; ++i)
      x1[i] = (uint1_t) x[i];
    VRswapBytes (x1, y1, size, N);
    VRswapBytes (y1, z1, size, N);
    for (i = 0; i < N; ++i) {
      y[i] = y1[i];
      z[i] = z1[i];
    }
    break;
  case 2:
    for (i = 0; i < N; ++i)
      x2[i] = (uint2_t) x[i];
    VRswapBytes (x2, y2, size, N);
    VRswapBytes (y2, z2, size, N);
    for (i = 0; i < N; ++i) {
      y[i] = y2[i];
      z[i] = z2[i];
    }
    break;
  case 4:
    for (i = 0; i < N; ++i)
      x4[i] = (uint4_t) x[i];
    VRswapBytes (x4, y4, size, N);
    VRswapBytes (y4, z4, size, N);
    for (i = 0; i < N; ++i) {
      y[i] = y4[i];
      z[i] = z4[i];
    }
    break;
  case 8:
    for (i = 0; i < N; ++i)
      x8[i] = (double8_t) x[i];
    VRswapBytes (x8, y8, size, N);
    VRswapBytes (y8, z8, size, N);
    for (i = 0; i < N; ++i) {
      y[i] = y8[i];
      z[i] = z8[i];
    }
    break;
  default:
    VRswapBytes (x1, y1, size, N);
  }

  VRdPrint (stdout, "input:", x, N);
  VRdPrint (stdout, "swapped:", y, N);
  VRdPrint (stdout, "swapped again:", z, N);

  return 0;
}
