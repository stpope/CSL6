/*
   Test FLexpHome

   $Id: tFLexpHome.c 1.8 1999/06/04 libtsp-v6r3 $
*/

#include <string.h>

#include <libtsp.h>


int
main (int argc, const char *argv[])

{
  char Name[FILENAME_MAX];
  int n;

  n = FLexpHome (argv[1], Name);
  printf ("FLexpHome: %s -> %s\n", argv[1], Name);
  if (n != strlen (Name))
    UThalt ("tFLexpName: Invalid returned length");

  return 0;
}
