/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  double FNevChebP (double x, const float c[], int N)

Purpose:
  Evaluate a series expansion in Chebyshev polynomials

Description:
  The series expansion in Chebyshev polynomials is defined as

              N-1
       Y(x) = SUM c(i) T(i,x) ,
              i=0

  where N is the number of terms in the expansion, c(i) is the coefficient for
  the i'th Chebyshev polynomial, and T(i,x) is the i'th order Chebyshev
  polynomial evaluated at x.

  The Chebyshev polynomials satisfy the recursion
    T(i,x) = 2x T(i-1,x) - T(i-2,x),
  with the initial conditions T(0,x)=1 and T(1,x)=x.  This routine evaluates
  the expansion using a backward recursion.

Parameters:
  <-  double FNevChebP
      Resultant value
   -> double x
      Input value
   -> const float c[]
      Array of coefficient values.  c[i] is the coefficient of the i'th order
      Chebyshev polynomial.
   -> int N
      Number of coefficients

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.16 $  $Date: 2003/05/09 01:49:35 $

-------------------------------------------------------------------------*/

#include <assert.h>

#include <libtsp.h>


double
FNevChebP (double x, const float c[], int N)

{
  int i;
  float X;
  float b0, b1, b2;

/*
   Consider the backward recursion
     b(i,x) = 2xb(i+1,x) - b(i+2,x) + c(i),
   with initial conditions b(N,x)=0 and b(N+1,x)=0.  Then dropping the
   dependence on x,
     c(i) = b(i) - 2xb(i+1) + b(i+2).

          N-1
   Y(x) = SUM c(i) T(i)
          i=0

          N-1
        = SUM [b(i)-2xb(i+1)+b(i+2)] T(i)
          i=0
                                             N-1
        = b(0)T(0) + b(1)T(1) - 2xb(1)T(0) + SUM b(i) [T(i)-2xT(i-1)+T(i-2)]
                                             i=2
 The term inside the sum is zero because of the recursive relationship
 satisfied by the Chebyshev polynomials.  Then substituting the values T(0)=1
 and T(1)=x, Y(x) is expressed in terms of the difference between b(0) and b(2)
 (errors in b(0) and b(2) tend to cancel),
   Y(x) = b(0)-xb(1) = [b(0)-b(2)+c(0)] / 2

*/
  X = (float) x;
  b1 = 0.0F;
  b0 = 0.0F;
  b2 = 0.0F;
  assert (N > 0);	/* guarantee 1 pass through loop */

  for (i = N - 1; i >= 0; --i) {
    b2 = b1;
    b1 = b0;
    b0 = 2.0F * X * b1 - b2 + c[i];
  }

  return (0.5F * (b0 - b2 + c[0]));
}
