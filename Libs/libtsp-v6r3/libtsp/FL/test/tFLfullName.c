/*
   Test FLfullName

   $Id: tFLfullName.c 1.7 1999/06/04 libtsp-v6r3 $
*/

#include <string.h>

#include <libtsp.h>


int
main (int argc, const char *argv[])

{
  char Name[FILENAME_MAX];
  int n;

  n = FLfullName (argv[1], Name);
  printf ("FLfullName: %s -> %s\n", argv[1], Name);
  if (n != strlen (Name))
    UThalt ("tFLfullName: Invalid returned length");

  return 0;
}
