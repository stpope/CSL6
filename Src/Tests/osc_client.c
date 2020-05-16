/* osc_client - stephen@heaveneverywhere.com - 20.05.14
 *		replacement for sendOSC
 *
 *  Copyright (C) 2014 Steve Harris et al. (see AUTHORS)
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as
 *  published by the Free Software Foundation; either version 2.1 of the
 *  License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  $Id$
 */

// Compile this with:
// cc -w -O2 osc_client.c -llo -o osc_client

// osc_client /i1/p
// osc_client /i1/pn fff 0.5 100.0 0.1
// osc_client -h localhost 54321 /i8/pn fff 1.0 100.0 0.0
// osc_client /i17/p
// osc_client /i18/fi s /Content/Code/CSL6/Data/wet.snd

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef WIN32
#include <unistd.h>
#endif
#include "lo/lo.h"
#define CSL_mOSCPort "54321"	// CSL OSC input port

#define argN(z) if(types[z] == 'f')
void usage(char *arg);

int main(int argc, char *argv[]) {
	char host[32];			// host
	char port[8];			// port
	char cmd[32];			// OSC command
	char types[16];			// OSC type string
//	float fl_args[16];		// arg arrays
//	char * str_args[16];
//	int int_args[16];
	unsigned argn = 1;		// lowest argv to consider

	if (argc < 2) {			// if called w no args - exit
		usage(argv[0]);
		exit(0);
	}
	strcpy(host, "localhost");
	strcpy(port, CSL_mOSCPort);
	strcpy(cmd, "/q");
	strcpy(types, "");

	if ((argc > 3) && (argv[1][0] == '-') && (argv[1][1] == 'h')) {
		strcpy(host, argv[2]);
		strcpy(port, argv[3]);
		argn = 4;
	}
//	printf("\tlo_address_new(%s, %s)\n", host, port);
	lo_address addr = lo_address_new(host, port);

	if (argc > argn)
		strcpy(cmd, argv[argn++]);
	if (argc > argn)
		strcpy(types, argv[argn++]);
	else {
		printf("\tlo_send(addr, %s, \"\")\n", cmd);
		lo_send(addr, cmd, "");
	}
	printf("\tlo_send(addr, %s, \"%s\" ...)\n", cmd, types);
	if (strcmp(types, "s") == 0)
		lo_send(addr, cmd, types, argv[argn]);
	else if (strcmp(types, "ff") == 0)
		lo_send(addr, cmd, types, atof(argv[argn++]), atof(argv[argn++]));
	else if (strcmp(types, "fff") == 0)
		lo_send(addr, cmd, types, atof(argv[argn++]), atof(argv[argn++]), atof(argv[argn++]));
	else if (strcmp(types, "ffff") == 0)
		lo_send(addr, cmd, types, atof(argv[argn++]), atof(argv[argn++]),
				atof(argv[argn++]), atof(argv[argn++]));
/*
	unsigned c_args = strlen(types);
	
	for (int i = argn; i < c_args; i++) {
		char ty = ;
		switch (types[i]) {
			case ('s'):
				str_args[i] = argv[argn++];
				break
			case ('f'):
				fl_args[i] = argv[argn++];
				break
			case ('i'):
				int_args[i] = argv[argn++];
				break
		}
	}
	lo_send(addr, cmd, types, ...);
*/
    return 0;
}

void usage(char *arg) {
	printf("\nUsage: %s -h localhost 54321 /i8/pn ff 1.0 1.0\n", arg);
}

