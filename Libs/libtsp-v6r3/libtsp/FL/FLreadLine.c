/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  char *FLreadLine (const char Prompt[]);

Purpose:
  Print a prompt and read a line of text

Description:
  This routine prints a prompt string and retrieves a line of input from
  standard input.  The return value is a pointer to the text string or NULL if
  end-of-file is detected.  Note that the string is stored in an internal
  buffer that is overwritten with each call.  In addition, other read routines
  may use the same internal buffer.  Strings that need to be kept across read
  operations should be copied to a separate storage area.

Parameters:
  <-  char *FLreadLine
      Pointer to the text string.  The trailing newline character is removed.
      This is NULL if end-of-file is encountered and the line is empty.
      Otherwise, when end-of-file is encountered, the line is treated as if
      it were terminated with a newline.  The text string is in an internally
      allocated storage area; each call to this routine overlays this storage.
   -> const char Prompt[]
      Prompt string.  This string is printed to standard output if both
      standard output and standard input are terminals.  If Prompt is NULL, no
      prompt is printed.

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.10 $  $Date: 2003/05/09 01:36:44 $

-------------------------------------------------------------------------*/

#include <libtsp.h>


char *
FLreadLine (const char Prompt[])

{
/* Print the prompt */
  FLprompt (Prompt);

/* Read a line of text */
  return (FLgetLine (stdin));
}
