#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "help.h"

int main(int argc, char **argv) {

	long nflags = argc - 1;
	char flags[nflags];

	// initialize all indexes to null
	for (int i = 0; i < nflags; i++)
		flags[i] = 0;

	// get flags
	for (int i = 1, j = 0; i < argc; i++) {
		if (argv[i][0] == '-') {
			if (argv[i][1] == '\0') {
				printf("Invalid flag %s\n", argv[i]);
				return 1;
			} else if (!strcmp(argv[i], "--help")) {
				printf(HELP_TEXT, argv[0]);
				return 0;
			} else {
				printf(HELP_TEXT, argv[0]);
				return 1;
			}
		}
	}
	
	return 0;
}
