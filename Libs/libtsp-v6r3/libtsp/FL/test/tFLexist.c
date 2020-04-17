/*
   Test FLexist

   $Id: tFLexist.c 1.4 1999/06/04 libtsp-v6r3 $
*/

#include <libtsp/nucleus.h>


int
main (int argc, const char *argv[])

{
  int status;

  status = FLexist (argv[1]);
  printf ("FLexist: %s, status = %d\n", argv[1], status);

  return 0;
}
