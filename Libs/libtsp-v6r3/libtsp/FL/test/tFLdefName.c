/*
   Test FLdefName

   $Id: tFLdefName.c 1.7 1999/06/04 libtsp-v6r3 $
*/

#include <string.h>

#include <libtsp.h>


int
main (int argc, const char *argv[])

{
  char Name[FILENAME_MAX];
  int n;

  n = FLdefName (argv[1], argv[2], Name);
  printf ("FLdefName: %s, %s -> %s\n", argv[1], argv[2], Name);
  if (n != strlen (Name))
    UThalt ("tFLdefName: Invalid returned length");

  return 0;
}
