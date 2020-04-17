/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  void AFsetInfo (const char Info[])

Purpose:
  Set the audio file information string

Description:
  This routine sets the information string to be written to an audio file
  header or trailer.  By default the information records specify the date and
  user name.  This routine allows the user to specify information records
  that are used in addition to, or in place of, the standard information
  records.  This routine must be called before the audio file to be created
  is opened using AFopnWrite. After the file has been opened (and the
  information records have been created), there is an implicit call
  AFsetInfo(NULL) to reset the information string to the default condition.
  To override the default condition, this routine must be called each time
  before a file is opened for writing.

  Standard Information: 
    date: 1994-01-25 19:19:39 UTC    date
    sample_rate: 8012.5              sampling frequency (if non-integer)
    program: CopyAudio               program name (set by UTsetProg)

  Additional structured information records should follow this format; a named
  field terminated by a colon followed by numeric data or a character string.
  Comments can follow as unstructured information.
    Record delimiter: Newline character or the two character escape
        sequence "\" + "n".  The last record need not have a delimiter.
    Line delimiter: Within records, lines are delimited by a carriage
        control character, the two character escape sequence "\" + "r",
        or the two character sequence "\" + newline.
  If the information string starts with a record delimiter, the information
  string is appended to the standard information string.  If not, the user
  supplied information string appears alone.

  If the input information string is a NULL pointer, the standard information
  is used.  If the information string starts with a record delimiter, the input
  information string is added to the existing information records (the standard
  information and/or any previously specified records).  Otherwise the input
  information string replaces the standard information.  The following examples
  illustrate the behaviour.

  - AFsetInfo (NULL)
      Reset, use only the standard information records
  - AFsetInfo ("")
      No information records
  - AFsetInfo ("<info>")
      Replace existing information with <info>; do not use the standard
      information records
  - AFsetInfo ("\n<info>")
      Add <info> to the existing information string.

Parameters:
   -> const char Info[]
      String containing the user supplied information

Author / revision:
  P. Kabal  Copyright (C) 2006
  $Revision: 1.45 $  $Date: 2006/06/06 13:53:55 $

-------------------------------------------------------------------------*/

#include <string.h>

#include <libtsp.h>
#include <libtsp/AFpar.h>

void
AFsetInfo (const char Info[])

{
  struct AF_opt *AFopt;
  int NU, Ninfo, Nt;

  AFopt = AFoptions ();

  if (Info == NULL) {
    UTfree ((void *) AFopt->Uinfo);
    AFopt->Uinfo = NULL;
  }

  /* Copy the information string to an allocated buffer */
  else {

    if (AFopt->Uinfo == NULL)
      NU = 0;
    else if (Info[0] == '\n' || (Info[0] == '\\' && Info[1] == 'n'))
      NU = strlen (AFopt->Uinfo);
    else
      NU = 0;

    Ninfo = strlen (Info);
    Nt = NU + Ninfo;

    /* Allocate or reallocate the buffer */
    if (NU == 0) {
      UTfree ((void *) AFopt->Uinfo);
      AFopt->Uinfo = (char *) UTmalloc (Nt+1);
    }
    else
      AFopt->Uinfo = (char *) UTrealloc (AFopt->Uinfo, Nt+1);

    /* Copy / Append the string */
    strcpy (&(AFopt->Uinfo[NU]), Info);
  }

  return;
}
