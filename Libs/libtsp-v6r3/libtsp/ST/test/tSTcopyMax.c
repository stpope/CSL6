/*
   Test STcopyMax

   $Id: tSTcopyMax.c 1.5 1999/06/04 libtsp-v6r3 $
*/

#include <string.h>

#include <libtsp.h>


int
main (int argc, const char *argv[])

{
  int n;
  int Maxchar;
  char string[1024];

  sscanf (argv[1], "%d", &Maxchar);

  n = STcopyMax (argv[2], string, Maxchar);
  printf ("STcopyMax: \"%s\"\n", string);
  if (n != strlen (string))
    UThalt ("tSTcopyMax: Invalid string length");

  return 0;
}
