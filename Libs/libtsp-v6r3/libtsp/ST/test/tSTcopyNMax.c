/*
   Test STcopyNMax

   $Id: tSTcopyNMax.c 1.6 1999/06/04 libtsp-v6r3 $
*/

#include <string.h>

#include <libtsp.h>


int
main (int argc, const char *argv[])

{
  int n;
  int Maxchar;
  int N;
  char string[1024];

  sscanf (argv[1], "%d", &N);
  sscanf (argv[2], "%d", &Maxchar);

  n = STcopyNMax (argv[3], string, N, Maxchar);
  printf ("STcopyNMax: \"%s\"\n", string);
  if (n != strlen (string))
    UThalt ("tSTcopyNMax: Invalid string length");

  return 0;
}
