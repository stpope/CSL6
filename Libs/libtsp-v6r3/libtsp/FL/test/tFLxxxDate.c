/*
   Test FLdate and FLfileDate

   $Id: tFLxxxDate.c 1.3 1999/06/04 libtsp-v6r3 $
*/

#include <libtsp.h>


int
main (int argc, const char *argv[])

{
  FILE *fp;

  fp = fopen (argv[1], "rb");
  if (fp == NULL) {
    fprintf (stderr, "Error opening file %s\n", argv[1]);
    return 1;
  }
  printf ("FLfileDate: %s: %s\n", argv[1], FLfileDate (fp, 0));
  printf ("FLfileDate: %s: %s\n", argv[1], FLfileDate (fp, 1));
  printf ("FLfileDate: %s: %s\n", argv[1], FLfileDate (fp, 2));
  printf ("FLfileDate: %s: %s\n", argv[1], FLfileDate (fp, 3));

  printf ("FLdate: %s: %s\n", argv[1], FLdate (argv[1], 0));
  printf ("FLdate: %s: %s\n", argv[1], FLdate (argv[1], 1));
  printf ("FLdate: %s: %s\n", argv[1], FLdate (argv[1], 2));
  printf ("FLdate: %s: %s\n", argv[1], FLdate (argv[1], 3));

  return 0;
}
