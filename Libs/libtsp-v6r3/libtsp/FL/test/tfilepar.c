/*
   Test file parameters

   $Id: tfilepar.c 1.2 2001/10/12 libtsp-v6r3 $
*/

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#ifndef S_IFIFO
#  define S_IFIFO _S_IFIFO
#endif


int
main (int argc, const char *argv[])

{
  FILE *fp;
  struct stat Fstat;
  int status;
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

  fprintf (stderr, "ftell (%s): %ld\n", fpn, ftell (fp));
  fprintf (stderr, "fseek (%s, 0L, SEEK_CUR): %d\n",
	   fpn, fseek (fp, 0L, SEEK_CUR));
  fprintf (stderr, "fseek (%s, 0L, SEEK_END): %d\n",
	   fpn, fseek (fp, 0L, SEEK_END));
  status = fstat (fileno (fp), &Fstat);
  if (status != 0)
    perror ("File stat error");
  else {
    fprintf (stderr, "fstat (fileno (%s), &Fstat): mode: %.3lo\n",
	     fpn, (unsigned long int) (Fstat.st_mode & (~S_IFMT)));
    if ((Fstat.st_mode & S_IFMT) == S_IFREG)
      fprintf (stderr, "fstat (fileno (%s), &Fstat): type: %s\n",
	       fpn, "regular");
    else if ((Fstat.st_mode & S_IFMT) == S_IFCHR)
      fprintf (stderr, "fstat (fileno (%s), &Fstat): type: %s\n",
	       fpn, "character special");
    else if ((Fstat.st_mode & S_IFMT) == S_IFIFO)
      fprintf (stderr, "fstat (fileno (%s), &Fstat): type: %s\n",
	       fpn, "pipe");
    else
      fprintf (stderr, "fstat (fileno (%s), &Fstat): type: %.3lo\n",
	       fpn, (unsigned long int) (Fstat.st_mode & S_IFMT));
  }
 
  fclose (fp);

  return 0;
}
