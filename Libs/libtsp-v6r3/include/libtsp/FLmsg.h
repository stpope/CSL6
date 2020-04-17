/*------------ Telecommunications & Signal Processing Lab --------------
                         McGill University

Routine:
  FLmsg.h

Description:
  Message texts for the TSP FL routines.

Author / revision:
  P. Kabal  Copyright (C) 1997
  $Revision: 1.2 $  $Date: 1997/10/11 03:39:41 $

----------------------------------------------------------------------*/

#ifndef FLmsg_h_
#define FLmsg_h_

/* I/O errors */
#define FLM_DelBackErr	"Error deleting previous backup file"
#define FLM_FilePosErr	"File positioning error"
#define FLM_NoFilePos	"Cannot determine file position"
#define FLM_OpenRErr	"Error opening input file"
#define FLM_OpenWErr	"Error opening output file"
#define FLM_ReadErr	"Read error"
#define FLM_RenameErr	"Error renaming file"

/* Error messages */
#define FLM_DataErr	"Data format error, line"
#define FLM_LongLine	"Input line too long"
#define FLM_LongName	"File name too long"
#define FLM_ExtraData	"Too many values or extra data"

/* Warning messages */
#define FLM_CWDErr	"Unable to reset current working directory"

/* Informational messages */
#define FLM_Rename	"Renamed existing file to"

#endif	/* FLmsg_h_ */
