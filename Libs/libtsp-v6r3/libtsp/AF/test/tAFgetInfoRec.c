/*
   Test AFgetInfoRec

   $Id: tAFgetInfoRec.c 1.9 2003/11/04 libtsp-v6r3 $
*/

#include <string.h>

#include <libtsp/AFheader.h>
#include <libtsp/AFpar.h>


int
main (int argc, const char *argv[])

{
  int N;
  char Info[1000];
  int i;
  int len;
  const char *p;
  struct AF_info InfoS;

  InfoS.Info = Info;
  N = 0;
  for (i = 2; i < argc; ++i) {
    len = strlen (argv[i]);
    strcpy (&InfoS.Info[N], argv[i]);
    N = N + len + 1;
  }
  InfoS.N = N;

  p = AFgetInfoRec (argv[1], &InfoS);
  if (p == NULL)
    printf ("AFgetInfoRec: Error status\n");
  else
    printf ("Returned arg |%s|\n", p);

  return 0;
}
