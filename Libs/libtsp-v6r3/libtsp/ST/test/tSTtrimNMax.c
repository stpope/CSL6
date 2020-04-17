/*
   Test STtrimNMax

   $Id: tSTtrimNMax.c 1.4 1999/06/04 libtsp-v6r3 $
*/

#include <string.h>

#include <libtsp.h>
#include <libtsp/nucleus.h>


int
main (int argc, const char *argv[])

{
  int n;
  int Maxchar;
  int N;
  char string[1024];

  sscanf (argv[1], "%d", &N);
  sscanf (argv[2], "%d", &Maxchar);

  n = STtrimNMax (argv[3], string, N, Maxchar);
  printf ("STtrimNMax: \"%s\"\n", string);
  if (n != strlen (string))
    UThalt ("tSTtrimNMax: Invalid string length");

  return 0;
}
