#include <pwd.h>	/* struct passwd, getpwnam prototype */
#include <unistd.h>	/* getuid definitions */

#include <stdio.h>

int
main (int argc, const char *argv[])

{
  struct passwd *pwd;
  char *h;

  pwd = getpwnam (argv[1]);
  if (pwd == NULL)
    printf ("search name: %s, pwd NULL\n", argv[1]);
  else {
    h = pwd->pw_name;
    printf ("search name: %s, user name: %s\n", argv[1], h);
  }

  return 0;
}
