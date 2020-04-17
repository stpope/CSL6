/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  int UTgetOption (int *Index, int argc, const char *argv[],
                   const char *OptTable[], const char *OptArg[])

Purpose:
  Decode command line options and arguments

Description:
  This routine decodes command line options, returning option values and other
  (non-option) argument strings.  Typically this routine is used to decode Unix
  style command options and to return argument strings.  An example of a Unix
  command line is as follows.
    command -x -f name --delay=30 argA argB.
  Each of the items in the command line, including the command itself, appears
  in an array of strings.  This routine compares the elements of the command
  line after the command name for a match to a table of option keywords.
  Options normally have a leading "-" character.

  The table of keywords contains entries which can specify the minimum length
  needed for a match with an optional "*" character.
  -d
      This form specifies that an exact match to "-d" is required.  The "-"
      character is just treated as an ordinary character for the match.
  --d*elay
      This form specifies that the option must match the first three characters
      and optionally match additional characters.  For instance, "--d" matches
      as does "--del", but not "--dx".

  The need for an option value is signalled in two ways.
  -d#
      The presence of the trailing "#" indicates that the option can either
      directly follow the option as in "-d30" or in the next argument as in
      "-d" followed by "30" in the next argument.
  --d*elay=
      The presence of the trailing "=" indicates that the option can either
      follow the option, separated by a '=' character as in "--del=30" or in
      the next argument as in "--del" followed by "30" in the next argument.

  The keywords are searched in order.  Keywords can contain a second asterisk
  can be used to indicate that characters after this point need not match.  

  Errors detected by this routine result in an error message being printed and
  an error code being returned.
  Invalid option:
      If all keywords in the list of option keywords start with a "-"
      character, then any argument with a leading "-", followed by one or
      more characters and not matching any of the options causes an error
      condition.  This means that ordinary arguments (non-option) cannot start
      with a "-" if the first option keyword starts with a "-".
  Missing option value:
      If an option requires a value, and none is supplied, an error is
      reported.  Such an error can only happen if the option is the last item
      in the list of arguments, since in other cases the item after the option
      will be picked up as the option value.

  The results of the option scan are communicated to the calling program via
  the pointer OptArg.  OptArg is a pointer to a pointer to a null terminated
  strings (substrings of the strings in argv).  The interpretation of these
  strings is given by the return code.  When this routine is called in a
  loop, the OptArg strings are returned in the same order as the options and
  arguments appeared in argv.  The OptArg pointer can be NULL.  This is the
  case for an option with no option value.  Each call to UTgetOption returns
  at most one option or argument.

  Return codes: 
    -2 - Error detected, invalid option or missing value
    -1 - Normal end of processing
     0 - Argument string, not an option.
     n - Option specified by the n'th keyword, n >= 1.
  For the non-negative return codes, the argument string or the option value
  string are pointed to by OptArg.  OptArg will be NULL for an option for which
  no option value is allowed.

  The use of this routine is illustrated in the following program fragment.
    static const char *OptTable[] = {
      "-d#", "--d*elay=",
      "-p", "--p*rint",
      NULL};
    
    void main (int argc, const char *argv[])
    {
      const char *OptArg;
      int Index;
      int n;
    
      Index = 1;
      while (Index < argc) {
        n = UTgetOption (&Index, argc, argv, OptTable, &OptArg);
        switch (n) {
        case 0:
          ... process non-option OptArg
          break;
        case 1:
        case 2:
          ... process option 1 (-d or --delay)
          break;
        case 3:
       	case 4:
          ... process option 2 (-p or --print)
          break;
        default:
          ... error
        }
      }
    }

Parameters:
  <-  int UTgetOptions
      Status code,
         -2 - Error, invalid option or missing value
         -1 - End of arguments/options
          0 - Argument value
        >=1 - Option code
  <-> int *Index
      Integer used internally for saving the state of the search.  It is the
      index of the next element of argv to be examined.  This value should be
      saved from call to call.  Note that the index may be incremented by zero
      (error detected), one or two (option followed by a value in the next
      element of argv).  The initial value of Index should be one.
   -> int argc
      Number of argument strings in argv
   -> const char *argv[]
      Pointers to option/argument strings.  Note that with ANSI C, if the
      actual parameter is not declared to have the const attribute, an explicit
      cast to (const char **) is required.
   -> const char *OptTable[]
      Pointers to the option keywords.  The end of the keyword table is
      signalled by a NULL pointer.  Note that with ANSI C, if the actual
      parameter is not declared to have the const attribute, an explicit cast
      to (const char **) is required.
  <-  const char *OptArg[]
      Argument/option string.  For an argument or an option taking a value,
      this is a pointer to a null terminated substring string in argv.  If the
      decoded option does not take a value, this pointer will be NULL.  Note
      that with ANSI C, if the actual parameter is not declared to have the
      const attribute, an explict cast to (const char **) is required.

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.31 $  $Date: 2003/05/09 03:20:37 $

-------------------------------------------------------------------------*/

#include <libtsp.h>
#include <libtsp/nucleus.h>
#include <libtsp/UTmsg.h>

#define NOMATCH		0
#define MATCH		1
#define MVAL		-2

#define RET_ERROR	-2
#define RET_END		-1
#define RET_ARG		0


int
UTgetOption (int *Index, const int argc, const char *argv[],
	     const char *OptTable[], const char **OptArg)

{
  int n, status, ip, rv, allminus;

  if (*Index <= 0)
    ip = 1;
  else
    ip = *Index;

/* Check the index, return if finished processing arguments from argv */
  if (ip >= argc) {
    /* Finished processing */
    *OptArg = NULL;
    return RET_END;
  }

/* Decode the option */
  rv = NOMATCH;
  allminus = (OptTable[0] != NULL);
  for (n = 0; OptTable[n] != NULL; ++n) {
    if (OptTable[n][0] != '-')
      allminus = 0;
    rv = UTdecOption (argv[ip], OptTable[n], OptArg);
    if (rv != NOMATCH)
      goto Match;
  }

  /* Not an option */
  if (allminus && argv[ip][0] == '-' && argv[ip][1] != '\0') {
    /* The argument has a leading '-' */
    UTwarn ("UTgetOption - %s: \"%s\"", UTM_BadOption, argv[ip]);
    *OptArg = NULL;
    status = RET_ERROR;
  }
  else {
    /* Ordinary argument */
    *OptArg = argv[ip];
    status = RET_ARG;
  }

  *Index = ip + 1;
  return status;

/* ===================== */
/* Found an option match */
 Match:
  if (rv == MATCH)
    /* Found an option match */
    status = n+1;

  else {
    /* Error - missing value */
    /* Try to pick up the option from the next argument */
    if (ip+1 < argc) {
      ++ip;		/* extra increment */
      *OptArg = argv[ip];
      status = n+1;
    }
    else {
      UTwarn ("UTgetOption - %s \"%s\"", UTM_ExpectValue, argv[ip]);
      status = RET_ERROR;
    }
  }

  /* Update for next call */
  *Index = ip + 1;
  return status;
}
