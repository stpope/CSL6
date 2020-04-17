/*
   Test STstrLC

   $Id: tSTstrLC.c 1.2 2001/10/12 libtsp-v6r3 $
*/

#include <ctype.h>
#include <stdio.h>

#include <libtsp.h>


int
main (int argc, const char *argv[])

{
  int i, k, n;
  unsigned char Table[257];
  unsigned char LC[257];
  char String[1024];

  if (argc <= 1) {

    /* Check all characters table */
    for (i = 0; i < 256; ++i)
      Table[i] = (unsigned char) i;
    Table[256] = '\0';

    n = STstrLC ((const char *) &Table[1], (char *) &LC[1]);
    LC[0] = Table[0];
    ++n;

    printf ("STstrLC:\n");
    k = 0;
    for (i = 0; i < n; ++i) {
      if (Table[i] != LC[i]) {
	if (isprint((char) Table[i]))
	  printf ("    %c  =>  %c  ", Table[i], LC[i]);
	else
	  printf ("  0X%.2X => 0X%.2X", Table[i], LC[i]);
	if ((k % 4) == 3)
	  printf ("\n");
	++k;
      }
    }
    printf ("\n");

  }
  else {
    n = STstrLC (argv[1], String);
    printf ("STstrLC: %s => %s (%d chars)\n", argv[1], String, n);
  }

  return 0;
}
