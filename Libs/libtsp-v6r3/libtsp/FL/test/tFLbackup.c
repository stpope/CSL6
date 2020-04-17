/*
   Test FLbackup

   $Id: tFLbackup.c 1.3 1999/06/04 libtsp-v6r3 $
*/

#include <libtsp.h>


int
main (int argc, const char *argv[])

{
  UTsetProg ("tFLbackup");

  FLbackup (argv[1]);
  return 0;
}
