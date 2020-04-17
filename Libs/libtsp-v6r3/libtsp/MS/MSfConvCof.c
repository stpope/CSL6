/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  int MSfConvCof (float x[], int Nx, float y[], int Ny, float z[])

Purpose:
  Convolve the coefficients of two vectors

Description:
  This routine convolves the coefficients of two vectors.  If the coefficients
  are considered to be polynomial coefficients, this operation is equivalent to
  polynomial multiplication.  The operation can be summarized as follows.
            Nx
    z[i] = SUM x[k] y[k-i],  0 <= i <= Nx+Ny-1.
           k=0
  This form of the sum assumes that y[i] is zero for i < 0 and i >= Ny.  The
  number of coefficients in the vector z is Nx+Ny-1.  The computation is
  arranged to allow the input vector x and the output vector z to be the same,
  i.e. the output values can overlay the input values.

Parameters:
  <-  int MSdConvCof
      Number of output coefficients (Nx + Ny - 1)
   -> float x[]
      Input vector of coefficients (Nx values)
   -> int Nx
      Number of coefficients in x
   -> double y[]
      Input vector of coefficients (Ny values)
   -> int Ny
      Number of coefficients in y
  <-  float z[]
      Output vector of Nx+Ny-1 coefficients representing the convolution of the
      coefficients of x and y.  The vector x and the vector z can be the same.

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.9 $  $Date: 2003/05/09 02:29:38 $

-------------------------------------------------------------------------*/

#include <libtsp.h>

#define MINV(a, b)	(((a) < (b)) ? (a) : (b))
#define MAXV(a, b)	(((a) > (b)) ? (a) : (b))


int
MSfConvCof (const float x[], int Nx, const float y[], int Ny, float z[])

{
  int m, k, kl, ku, Nz;
  double sum;

  if (Nx <= 0 || Ny <= 0)
    Nz = 0;
  else
    Nz = Nx + Ny - 1;

  for (m = Nz - 1; m >= 0; --m) {
    sum = 0.0;
    kl = MAXV (0, m - Nx + 1);
    ku = MINV (Ny - 1, m);
    for (k = kl; k <= ku; ++k)
      sum = sum + y[k] * x[m-k];
    z[m] = (float) sum;
  }
  return Nz;
}
