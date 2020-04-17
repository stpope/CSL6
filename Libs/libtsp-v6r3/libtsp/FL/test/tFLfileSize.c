/*
   Test FLfileSize

   $Id: tFLfileSize.c 1.4 1999/06/04 libtsp-v6r3 $
*/

#include <libtsp.h>
#include <libtsp/nucleus.h>


int
main (int argc, const char *argv[])

{
  FILE *fp;
  long int size;

  fp = fopen (argv[1], "rb");

  size = FLfileSize (fp);
  printf ("File: %s, Size = %ld\n", argv[1], size);

  fclose (fp);

  return 0;
}
