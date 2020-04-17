/*
  Test STdecPair

  $Id: tSTdecPair.c 1.3 1999/06/05 libtsp-v6r3 $
*/

#include <string.h>

#include <libtsp.h>


int
main (int argc, const char *argv[])

{
  double Dval1, Dval2;
  int Ival1, Ival2;
  long int Lval1, Lval2;
  const char *Ident;
  const char *String;
  int status;

  Ident = argv[1];
  String = argv[2];

  if (strcmp (Ident, "Irange") == 0) {
    Ival1 = 888;
    Ival2 = 999;
    status = STdecIrange (String, &Ival1, &Ival2);
    printf ("STdecIrange: status = %d, Values: %d : %d\n",
	    status, Ival1, Ival2);
  }
  else if (strcmp (Ident, "Lrange") == 0) {
    Lval1 = 888;
    Lval2 = 999;
    status = STdecLrange (String, &Lval1, &Lval2);
    printf ("STdecLrange: status = %d, Values: %ld : %ld\n",
	    status, Lval1, Lval2);
  }
  else if (strcmp (Ident, "Dfrac") == 0) {
    Dval1 = 888;
    Dval2 = 999;
    status = STdecDfrac (String, &Dval1, &Dval2);
    printf ("STdecDfrac: status = %d, Values: %g / %g\n",
	    status, Dval1, Dval2);
  }
  else if (strcmp (Ident, "Ifrac") == 0) {
    Ival1 = 888;
    Ival2 = 999;
    status = STdecIfrac (String, &Ival1, &Ival2);
    printf ("STdecIfrac: Status = %d, Values: %d / %d\n",
	    status, Ival1, Ival2);
  }

  return 0;
}
