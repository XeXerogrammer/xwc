#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "help.h"

struct details {
	long lines;
	long words;
	long characters;
	long bytes;
	long length;
};

struct details get_file_details(FILE *fp);
int parse_command_line_arguments(int argc, char **argv, char **files, int *nfiles);
void report(struct details file, int state, char *file_name);
void add_to_total(struct details file, struct details *total);

int main(int argc, char **argv) {
	
	int nfiles = 0;
	int max_files = argc - 1;	
	char *files[max_files];

	// initialize all indexes to null
	for (int i = 0; i < max_files; i++)
		files[i] = 0;

	int state = parse_command_line_arguments(argc, argv, files, &nfiles);
	
	if (state < 0)
		return 1;

	// gather total details
	struct details total_details = {0, 0, 0, 0, 0};
	char *total_text = "total";

	for (int i = 0; i < nfiles; i++) {
		FILE *fp = fopen(files[i], "r");
		struct details file_details = get_file_details(fp);
		add_to_total(file_details, &total_details);
		report(file_details, state, files[i]);
	}

	if (nfiles > 1) {
		report(total_details, state, total_text);
	}
	return 0;


}

int parse_command_line_arguments(int argc, char **argv, char **files, int *nfiles) {
	
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

	// parse command line arguments
	for (int i = 1; i < argc; i++) {
		if (argv[i][0] == '-') {
			if (argv[i][1] == '-') {
				if (!strcmp(argv[i] + 2, "lines")) {
					state += 1;
				} else if (!strcmp(argv[i] + 2, "words")) {
					state += 2;
				} else if (!strcmp(argv[i] + 2, "help")) {
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
				if (!strcmp(argv[i] + 1, "l")) {
					state += 1;
				} else if (!strcmp(argv[i] + 1, "w")) {
					state += 2;
				}
			}
		} else {
			files[(*nfiles)++] = argv[i];
		}
	}

	if (*nfiles < 1) {
		printf(TIP_TEXT, argv[0], argv[0]);
		return -1;
	}
	return state;
}

struct details get_file_details(FILE *fp) {
	struct details file_details;
	file_details.lines = 0;
	file_details.words = 0;
	int c;
	int is_word = 0;
	while ((c = fgetc(fp)) != EOF) {
		if (!isspace(c)) {
			is_word = 1;
		}
		else {
			if (is_word) {
				file_details.words++;
				is_word = 0;
			}

			if (c == '\n') {
				file_details.lines++;	
			}

		}
	}
	return file_details;
}

void report(struct details file, int state, char *file_name) {
	if (state & 1)
		printf(" %4ld", file.lines);

	if (state & 2)
		printf(" %4ld", file.words);

	printf(" %s", file_name);
	printf("\n");
}

void add_to_total(struct details file, struct details *total) {
	total->lines += file.lines;
	total->words += file.words;
	total->characters += file.characters;
	total->bytes += file.bytes;
	total->length = total->length > file.length ? total->length : file.length;
}
