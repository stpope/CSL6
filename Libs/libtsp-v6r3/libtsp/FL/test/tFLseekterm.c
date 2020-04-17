/*
   Test FLseekable, FLterm

   $Id: tFLseekterm.c 1.5 2001/10/09 libtsp-v6r3 $
*/

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <libtsp/nucleus.h>


int
main (int argc, const char *argv[])

{
  FILE *fp;
  const char *fname;
  const char *fpn;

  fname = argv[1];
  if (strcmp (fname, "stdin") == 0) {
    fp = stdin;
    fpn = fname;
  }
  else if (strcmp (fname, "stdout") == 0) {
    fp = stdout;
    fpn = fname;
  }
  else {
    fp = fopen (fname, "rb");
    if (fp == NULL) {
      fprintf (stderr, "Error opening file: %s\n", fname);
      perror ("File open error:");
      return 1;
    }
    fpn = "fp";
  }

  fprintf (stderr, "FLseekable (%s): %d\n", fpn, FLseekable (fp));
  fprintf (stderr, "FLterm (%s): %d\n", fpn, FLterm (fp));
 
  fclose (fp);

  return 0;
}
