/*
   Test FLreadLine

   $Id: tFLreadLine.c 1.2 1999/06/04 libtsp-v6r3 $
*/

#include <libtsp.h>


int
main (int argc, const char *argv[])

{
  char *p;
  int neof;

  neof = 0;

  while (neof < 2) {
    p = FLreadLine (argv[1]);
    if (p == NULL) {
      ++neof;
      printf ("FLreadLine: end-of-file\n");
    }
    else {
      printf ("FLreadLine: %.40s\n", p);
      neof = 0;
    }
  }
  return 0;
}
