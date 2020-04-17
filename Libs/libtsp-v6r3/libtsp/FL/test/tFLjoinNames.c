/*
   Test FLjoinNames

   $Id: tFLjoinNames.c 1.8 1999/06/04 libtsp-v6r3 $
*/

#include <string.h>

#include <libtsp.h>
#include <libtsp/nucleus.h>


int
main (int argc, const char *argv[])

{
  char Name[FILENAME_MAX];
  int n;

  n = FLjoinNames (argv[1], argv[2], Name);
  printf ("FLjoinNames: %s, %s -> %s\n", argv[1], argv[2], Name);
  if (n != strlen (Name))
    UThalt ("tFLjoinNames: Invalid returned length");

  return 0;
}
