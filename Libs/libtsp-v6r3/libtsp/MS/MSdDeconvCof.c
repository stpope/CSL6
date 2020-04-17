/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  int MSdDeconvCof (const double u[], int Nu, const double v[], int Nv,
                    double q[], double r[])

Purpose:
  Polyomial divide (deconvolve the coefficients of two vectors)

Description:
  This routine divides one polynomial by another.  Given polynomials U(D)
  and V(D), the polynomial division U(D)/V(D) gives quotient Q(D) and
  remainder R(D), where
    U(D) = Q(D) V(D) + R(D).
  Polynomial U(D) is represented as
    U(D) = u[0] + u[1] D + u[2] D^2 + ... + u[Nu-1] D^(Nu-1).
  The quotient Q(D) normally has Nu-Nv+1 terms.  The remainder R(C) normally
  has Nv-1 terms.

Parameters:
  <-  int MSdDeconvCof
      Number of quotient coefficients.  This is normally Nu-Nv+1.  If some of
      the higher order terms in v are zero, this may be as large as Nu.  If
      the last non-zero term in v is v[k], then the number of quotient
      coefficients is returned as Nu-k.
   -> double u[]
      Input vector of coefficients (Nu values)
   -> int Nu
      Number of coefficients in u
   -> double v[]
      Input vector of coefficients (Nv values)
   -> int Nv
      Number of coefficients in v.  If Nu < Nv, the quotient is empty and the
      remainder is equal to u.
  <-  double q[]
      Output vector of Nu coefficients representing the quotient of the
      polynomial division.  The quotient vector can be the same as the vector
      u.  If v[k] is the last non-zero coefficient of v, the last k
      coefficients of q will be zero.
  <-  double r[]
      Output vector of Nu coefficient representing the remainder of the
      polynomial division.  If r is NULL, the remainder is not calculated.
      The remainder satisfies R(D) = U(D) - Q(D) U(D).  If v[k] is the last
      nonzero coefficient of v (k can be ascertained from the function return
      value which is Nu-k), the last Nu-k terms of R(D) will be zero.

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.3 $  $Date: 2003/05/09 02:29:37 $

-------------------------------------------------------------------------*/

#include <libtsp.h>
#include <libtsp/MSmsg.h>

#define MINV(a, b)	(((a) < (b)) ? (a) : (b))
#define MAXV(a, b)	(((a) > (b)) ? (a) : (b))

#define NB		1024


int
MSdDeconvCof (const double u[], int Nu, const double v[], int Nv,
	      double q[], double r[])

{
  int i, k;
  double buffer [NB];
  double *tr;

/*
  Reference:
    D. E. Knuth, "The art of computer programming", vol. 2 / Seminumerical
    algorithms", 3rd Ed., Addison-Wesley, 1997.

  Given U(D) with m+1 terms and V(D) with n+1 terms, Q(D) with m-n+1 terms
  and R(D) with n terms are given by,
    for (k = m-n; k >= 0; --k) {
      q[k] = u[n+k] / v[n];
      for (i = n+k-1; i >= k; --i)
        u[i] = u[i] - q[k] * v[i-k];
    }
    for (i = 0; i < n; ++i)
      r[i] = u[i];
  This algorithm overwrites the input coefficients.  We will modify it
  to avoid this.

*/

  /* Use the internal buffer if possible */
  tr = buffer;
  if (Nu > NB)
    tr = (double *) UTmalloc (Nu * sizeof (double));

  /* Look for the last non-zero coefficient in v[] */
  for (i = Nv-1; i >= 0; --i) {
    if (v[i] != 0.0)
      break;
  }
  Nv = i+1;
  if (Nv <=0)
    UTerror ("MSdDeconvCof: %s", MSM_ZeroDiv);

  /* Initial remainder */
  for (i = 0; i < Nu; ++i)
    tr[i] = u[i];

  /* Long division */
  for (k = Nu - Nv; k >= 0; --k) {
    q[k] = tr[Nv+k-1] / v[Nv-1];
    tr[Nv+k-1] = 0.0;		/* Zero the leading part of the remainder */
    for (i = Nv+k-2; i >= k; --i)
      tr[i] = tr[i] - q[k] * v[i-k];
  }

  if (r != NULL) {
    for (i = 0; i < Nu; ++i)
      r[i] = tr[i];
  }

  /* Deallocate the temporary buffer */
  if (Nu > NB)
    UTfree (tr);

  return (Nu - Nv + 1);
}
