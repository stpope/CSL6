/*
   Test sscanf for invalid assignment
   This problem appears in sscanf in the djgpp port of gcc to the PC

   $Id: tsscanf.c 1.4 1999/06/04 libtsp-v6r3 $
*/

#include <stdio.h>

int
main (int argc, const char *argv[])

{
  int m, iv;
  char c;

  iv = -999;
  c = '\0';
  m = sscanf ("xx", "%d%c", &iv, &c);
  if (m != 0)
    printf ("Error in sscanf: %s ->  m=%d, iv=%d, c=%c\n", "xx", m, iv, c);
  else
    printf ("sscanf OK\n");

  return 0;
}
