/*
   Test STstrstrNM

   $Id: tSTstrstrNM.c 1.4 1999/06/04 libtsp-v6r3 $
*/

#include <libtsp/nucleus.h>


int
main (int argc, const char *argv[])

{
  int M;
  int N;
  char *p;

  sscanf (argv[1], "%d", &N);
  sscanf (argv[2], "%d", &M);

  p = STstrstrNM (argv[3], argv[4], N, M);
  if (p != NULL)
    printf ("STstrstrNM: match at offset %d\n", p-argv[3]);
  else
    printf ("STstrstrNM: no match\n");

  return 0;
}
