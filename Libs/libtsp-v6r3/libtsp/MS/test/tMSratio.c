/*
   Test MSratio

   $Id: tMSratio.c 1.4 1999/06/04 libtsp-v6r3 $
*/

#include <libtsp.h>


int
main (int argc, const char *argv[])

{
  double tol, err;
  double VN, VD, Val;
  long int N, D, MaxN, MaxD;

  STdecDfrac (argv[1], &VN, &VD);
  Val = VN / VD;
  sscanf (argv[2], "%lg", &tol);
  sscanf (argv[3], "%ld", &MaxN);
  sscanf (argv[4], "%ld", &MaxD);

  MSratio (Val, &N, &D, tol, MaxN, MaxD);

  /* There is some ambiguity as to how to calculate the error.
     If we calculate "Val - N/D", we get different results depending on the
     compiler.  Some compilers keep the division to extra precision; others
     round the result to double.  For instance when VN = 8001 and VD = 8000,
     MSratio returns N = 8001 and D = 8000.  Some compilers will compute
     "Val - N/D" as zero, others as a small non-zero value.

     Here we calculate the error using long double (if available), hoping for
     better precision.
  */
  err = Val - (long double) N / D;
  printf ("MSratio: %g ~= %ld / %ld, err = %g\n", Val, N, D, err);

  return 0;
}
