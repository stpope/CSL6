/*
   Test UTerror

   $Id: tUTerror.c 1.2 1999/06/04 libtsp-v6r3 $
*/

#include <libtsp.h>


int
main (int argc, const char *argv[])

{
  FILE *fp;

  UTsetProg ("PRogRAM");

  fp = fopen ("XxXx.Yy", "rb");
  UTerror ("tUTerror: Cannot open file \"%s\"", "XxXx.Yy");

  return 0;
}
