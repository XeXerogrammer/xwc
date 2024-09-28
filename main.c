#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "help.h"

int parse_command_line_arguments(int argc, char **argv, char **files);

int main(int argc, char **argv) {

	int max_files = argc - 1;	
	char *files[max_files];

	// initialize all indexes to null
	for (int i = 0; i < max_files; i++)
		files[i] = 0;

	int state = parse_command_line_arguments(argc, argv, files);

	if (state >= 0)
		return 0;
	else 
		return 1;
	
	return 0;
}

int parse_command_line_arguments(int argc, char **argv, char **files) {
	
	// parse command line arguments
	// push possible file name pointers to files array
	// return state of program as an int
	// -1 user error
	// 0 print info and exit immididately
	// 1 lines
	// 2 words
	// 4 characters
	// 8 bytes
	// 16 maximum line length
	// the > 0 values will be summed to return the final state

	int state = 0;
	int nfiles = 0;

	// parse command line arguments
	for (int i = 1; i < argc; i++) {
		if (argv[i][0] == '-') {
			if (argv[i][1] == '-') {
				if (!strcmp(argv[i] + 2, "help")) {
					printf(HELP_TEXT, argv[0]);
					return 0;
				} else if (!strcmp(argv[i] + 2, "version")) {
					printf(VERSION_TEXT);
					return 0;
				} else {
					printf("Invalid flag %s\nTry: \"%s --help\" for more information.", argv[i], argv[0]);
					return -1;
				}
			} else {
				printf("Invalid flag %s\nTry: \"%s --help\" for more information.", argv[i], argv[0]);
				return -1;
			}
		} else {
			files[nfiles++] = argv[i];
		}
	}

	if (nfiles < 1) {
		printf(TIP_TEXT, argv[0], argv[0]);
		return -1;
	}
}
