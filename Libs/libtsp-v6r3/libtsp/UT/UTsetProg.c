/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  void UTsetProg (const char Program[]);
  char *UTgetProg (void)

Purpose:
  Set the program name for messages
  Get the program name for messages

Description:
  This routine sets a program name string.  This string is used by error
  reporting routines as an identifier in error message strings.

Parameters:
  UTsetProg:
   -> const char Program[]
      Program name, normally without a path
  UTgetProg:
  <-  char *UTgetProg
      Pointer to the program name.  If the program name has not been set, this
      is a pointer to an empty string.

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.12 $  $Date: 2003/05/09 03:20:37 $

-------------------------------------------------------------------------*/

#include <string.h>

#include <libtsp.h>

static char *Pgm = NULL;


void
UTsetProg (const char Program[])

{
  int nc;

  UTfree ((void *) Pgm);
  nc = strlen (Program);
  if (nc > 0) {
    Pgm = (char *) UTmalloc (nc + 1);
    strcpy (Pgm, Program);
  }
  else
    Pgm = NULL;
}



char *
UTgetProg (void)

{
  static char Empty[] = "";

  if (Pgm == NULL)
    return Empty;
  else
    return Pgm;
}
