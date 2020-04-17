/*
   Test FLbaseName and FLdirName

   $Id: tFLxxxName.c 1.7 1999/06/04 libtsp-v6r3 $
*/

#include <string.h>

#include <libtsp.h>


int
main (int argc, const char *argv[])

{
  char Name[FILENAME_MAX];
  int n;

  n = FLdirName (argv[1], Name);
  printf ("FLdirName:  %s -> %s\n", argv[1], Name);
  if (n != strlen (Name))
    UThalt ("tFLxxxName: Invalid returned length");

  n = FLbaseName (argv[1], Name);
  printf ("FLbaseName: %s -> %s\n", argv[1], Name);
  if (n != strlen (Name))
    UThalt ("tFLxxxName: Invalid returned length");

  n = FLpreName (argv[1], Name);
  printf ("FLpreName:  %s -> %s\n", argv[1], Name);
  if (n != strlen (Name))
    UThalt ("tFLxxxName: Invalid returned length");

  n = FLextName (argv[1], Name);
  printf ("FLextName:  %s -> %s\n", argv[1], Name);
  if (n != strlen (Name))
    UThalt ("tFLxxxName: Invalid returned length");

  return 0;
}
