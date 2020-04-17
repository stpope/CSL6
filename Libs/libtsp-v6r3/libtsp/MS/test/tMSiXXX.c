/*
   Test MSiCeil and MSiFloor

   $Id: tMSiXXX.c 1.3 1999/06/04 libtsp-v6r3 $
*/

#include <libtsp.h>


int
main (int argc, const char *argv[])

{
  int N;
  int M;

  sscanf (argv[1], "%d", &N);
  sscanf (argv[2], "%d", &M);

  printf ("MSiFloor (%d, %d) = %d\n", N, M, MSiFloor (N, M));
  printf ("MSiCeil (%d, %d) = %d\n", N, M, MSiCeil (N, M));

  return 0;
}
