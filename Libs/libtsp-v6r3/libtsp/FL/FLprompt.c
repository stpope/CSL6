/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  void FLprompt (const char Prompt[]);

Purpose:
  Print a prompt

Description:
  This routine prints a prompt string if both standard output and standard
  input are terminals.

Parameters:
   -> const char Prompt[]
      Prompt string.  This string is printed to standard output if both
      standard output and standard input are terminals.  If Prompt is NULL, no
      prompt is printed.

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.13 $  $Date: 2003/05/09 01:36:44 $

-------------------------------------------------------------------------*/

#include <libtsp.h>
#include <libtsp/nucleus.h>


void
FLprompt (const char Prompt[])

{
  if (Prompt != NULL && FLterm (stdout) && FLterm (stdin)) {
    fprintf (stdout, "%s", Prompt);
    fflush (stdout);
  }

  return;
}
