/*
   Test STdecNxxx

   $Id: tSTdecNxxx.c 1.9 1999/06/04 libtsp-v6r3 $
*/

#include <errno.h>

#include <libtsp.h>


int
main (int argc, const char *argv[])

{
  int Nmin, Nmax, N;
  const char *String;
  double Dval[100];
  float Fval[100];
  int Ival[100];
  long int Lval[100];
  int status;

  sscanf (argv[2], "%d", &Nmin);
  sscanf (argv[3], "%d", &Nmax);
  String = argv[4];

  if (argv[1][0] == 'D') {
    status = STdecNdouble (String, Nmin, Nmax, Dval, &N);
    printf (
      "STdecNdouble: input |%s|, min = %d, max = %d -> return value = %d\n",
      String, Nmin, Nmax, status);
    if (N > 0)
      VRdPrint (stdout, "  data values:", Dval, N);
  }
  else if (argv[1][0] == 'F') {
    status = STdecNfloat (String, Nmin, Nmax, Fval, &N);
    printf (
      "STdecNfloat: input |%s|, min = %d, max = %d -> return value = %d\n",
      String, Nmin, Nmax, status);
    if (N > 0)
      VRfPrint (stdout, "  data values:", Fval, N);
  }
  else if (argv[1][0] == 'I') {
    status = STdecNint (String, Nmin, Nmax, Ival, &N);
    printf (
      "STdecNint: input |%s|, min = %d, max = %d -> return value = %d\n",
      String, Nmin, Nmax, status);
    if (N > 0)
      VRiPrint (stdout, "  data values:", Ival, N);
  }
  else if (argv[1][0] == 'L') {
    status = STdecNlong (String, Nmin, Nmax, Lval, &N);
    printf (
      "STdecNlong: input |%s|, min = %d, max = %d -> return value = %d\n",
      String, Nmin, Nmax, status);
    if (N > 0)
      VRlPrint (stdout, "  data values:", Lval, N);
  }

  return 0;
}
