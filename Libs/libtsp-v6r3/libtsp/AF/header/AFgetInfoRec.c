/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  const char *AFgetInfoRec (const char name[], const struct AF_info *InfoS)

Purpose:
  Find a named record in an AFsp information structure

Description:
  This routine searches through records in an AFsp information structure,
  looking for a match to a record with a given name.  A pointer to the data
  value for that record is returned.

  The records are separated by nulls.  If the information string does not have
  a terminating null, the last unterminated record will not be checked.

  AFsp Information Records:
    The header information records are separated by null characters.  The name
    field is the first part of the record and the value field is the remainder.
    Standard names are terminated by a ':' character, which effectively
    delimits the name field from the value field.

Parameters:
  <-  const char *AFgetInfoRec
      Pointer to a null terminated value string in the information string
   -> const char name[]
      Record name to be matched.  An empty name will match the first record.
   -> const struct AF_info *InfoS
      AFsp information structure

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.26 $  $Date: 2003/11/04 12:42:08 $

-------------------------------------------------------------------------*/

#include <string.h>	/* memchr definition */

#include <libtsp/AFheader.h>
#include <libtsp/AFpar.h>


const char *
AFgetInfoRec (const char name[], const struct AF_info *InfoS)

{
  const char *pst;
  const char *h_end;
  const char *r_term;
  int lenn;

  if (InfoS == NULL || InfoS->Info == NULL || InfoS->N <= 0)
    return NULL;

  h_end = &InfoS->Info[InfoS->N];
  pst = InfoS->Info;
  lenn = strlen (name);
  while (pst < h_end) {

    /* Find the record terminator */
    r_term = (char *) memchr (pst, '\0', h_end - pst);
    if (r_term == NULL)
      break;

    /* Check for a name match */
    if (r_term - pst >= lenn && memcmp (pst, name, lenn) == 0 ) {
      return (pst + lenn);
    }
    pst = r_term + 1;
  }

/* No match to the name */
  return NULL;
}
