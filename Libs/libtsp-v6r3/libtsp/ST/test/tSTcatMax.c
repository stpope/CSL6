/*
   Test STcatMax

   $Id: tSTcatMax.c 1.7 1999/06/04 libtsp-v6r3 $
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

  strcpy (string, argv[2]);
  n = STcatMax (argv[3], string, Maxchar);
  printf ("STcatMax: \"%s\"\n", string);
  if (n != strlen (string))
    UThalt ("tSTcatMax: Invalid string length returned");

  return 0;
}
