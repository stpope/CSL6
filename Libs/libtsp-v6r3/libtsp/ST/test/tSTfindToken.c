/*
   Test STfindToken

   $Id: tSTfindToken.c 1.5 2001/10/12 libtsp-v6r3 $
*/

#include <stdio.h>
#include <string.h>

#include <libtsp.h>


int
main (int argc, char *argv[])

{
  char *p, *pp;
  int WSFlag;
  int n, nc;

  n = sscanf (argv[4], "%d", &WSFlag);
  if (n != 1)
    UThalt ("Invalid WSFlag");

  printf ("string: |%s|\n", argv[1]);
  printf ("  Delims: |%s|, Quotes: |%s|, WSFlag: %d\n",
	  argv[2], argv[3], WSFlag);
  p = argv[1];
  do
    {
      pp = p;		/* argv[1] gets modified */
      nc = strlen (pp);
      p = STfindToken (pp, argv[2], argv[3], pp, WSFlag, nc);
      printf ("  Token: |%s|\n", pp);
    } while (p != NULL);

  return 0;
}
