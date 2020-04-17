/*
   Test STunQuote

   $Id: tSTunQuote.c 1.2 1999/06/04 libtsp-v6r3 $
*/

#include <string.h>

#include <libtsp.h>


int
main (int argc, const char *argv[])

{
  int n;
  char string[1024];

  n = STunQuote (argv[1], argv[2], string);
  printf ("STunQuote: \"%s\"\n", string);
  if (n != strlen (string))
    UThalt ("tSTunQuote: Invalid string length");

  return 0;
}
