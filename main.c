#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "help.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

struct details {
	long lines;
	long words;
	long characters;
	long bytes;
	long length;
};

struct details get_file_details(FILE *fp);
struct details get_stdin_details();
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
	
	int show_total = 1;

	if (state < 0)
		return 1;
	
	if (files[0] == 0 && state != 0) {
		struct details stdin_details = get_stdin_details();
		printf("\n");
		report(stdin_details, state, "");
		return 0;
	}

	// gather total details
	struct details total_details = {0, 0, 0, 0, 0};
	char *total_text = "total";

	for (int i = 0; i < nfiles; i++) {
		struct stat path_stat;
		stat(files[i], &path_stat);
		if (!S_ISREG(path_stat.st_mode)) {
			printf("%s is a directory", files[i]);
			continue;
		}
		FILE *fp = fopen(files[i], "r");
		if (fp == 0) {
			printf("no such file or directory: %s\n", files[i]);
			continue;
		}
		struct details file_details = get_file_details(fp);
		add_to_total(file_details, &total_details);
		report(file_details, state, files[i]);
		fclose(fp);
	}

	if ((nfiles > 1) && (state & 32)) {
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
	// 32 show total
	// the > 0 values will be summed to return the final state

	int state = 0;
	int show = 1;

	// parse command line arguments
	for (int i = 1; i < argc; i++) {
		if (argv[i][0] == '-') {
			if (argv[i][1] == '-') {
				if (!strcmp(argv[i] + 2, "lines")) {
					state |= 1;
				} else if (!strcmp(argv[i] + 2, "words")) {
					state |= 2;
				} else if (!strcmp(argv[i] + 2, "chars")) {
					state |= 4;
				} else if (!strcmp(argv[i] + 2, "bytes")) {
					state |= 8;
				} else if (!strcmp(argv[i] + 2, "max-line-length")) {
					state |= 16;
				} else if (!strcmp(argv[i] + 2, "total=yes")) {
					show = 1;
				} else if (!strcmp(argv[i] + 2, "total=no")) {
					show = 0;
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
				int c;
				for (int j = 1; (c = argv[i][j]) != '\0'; j++) {
					switch (c) {
						case 'l':
							state |= 1;
							break;
						case 'w':
							state |= 2;
							break;
						case 'm':
							state |= 4;
							break;
						case 'c':
							state |= 8;
							break;
						case 'L':
							state |= 16;
							break;
						default:
							printf("Invalid flag -%c\nTry: \"%s --help\" for more information.", c, argv[0]);
							return -1;
					}
				}
			}
		} else {
			files[(*nfiles)++] = argv[i];
		}
	}

	if (*nfiles < 1) {
		files[0] = 0;
	}
	// default state is 7
	return (state ? state : 7) + (show * 32);
}

struct details get_file_details(FILE *fp) {
	struct details file_details;
	file_details.lines = 0;
	file_details.words = 0;
	file_details.characters = 0;
	file_details.length = 0;
	int c;
	int is_word = 0;
	long line_length = 0;	

	while ((c = fgetc(fp)) != EOF) {
		if (!((c > 128 && c < 192) || c < 128))
			continue;
		file_details.characters++;
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
				if (line_length > file_details.length)
					file_details.length = line_length;
				line_length = 0;
				continue;
			}

		}

		line_length++;
	}
	if (is_word)
		file_details.words++;
	file_details.bytes = ftell(fp);
	return file_details;
}

struct details get_stdin_details() {
	struct details stdin_details;
	stdin_details.lines = 0;
	stdin_details.words = 0;
	stdin_details.characters = 0;
	stdin_details.bytes = 0;
	stdin_details.length = 0;
	long c;
	int is_word = 0;
	long line_length = 0;

	while ((c = getc(stdin)) != EOF) {
		stdin_details.bytes++;
		if (!(((c & 255) > 128 && (c & 255) < 192) || (c & 255) < 128))
			continue;
		stdin_details.characters++;	
		
		if (!isspace(c)) {
			is_word = 1;
		}
		else {
			if (is_word) {
				stdin_details.words++;
				is_word = 0;
			}

			if (c == '\n') {
				stdin_details.lines++;
				if (line_length > stdin_details.length)
					stdin_details.length = line_length;
				line_length = 0;
				continue;
			}

		}

		line_length++;
	}
	if (line_length > stdin_details.length)
					stdin_details.length = line_length;
				line_length = 0;
	if (is_word)
		stdin_details.words++;

	return stdin_details;
}

void report(struct details file, int state, char *file_name) {
	if (state & 1)
		printf("%4ld ", file.lines);

	if (state & 2)
		printf("%4ld ", file.words);

	if (state & 4)
		printf("%4ld ", file.characters);

	if (state & 8)
		printf("%4ld ", file.bytes);

	if (state & 16)
		printf("%4ld ", file.length);

	printf("%s", file_name);
}

void add_to_total(struct details file, struct details *total) {
	total->lines += file.lines;
	total->words += file.words;
	total->characters += file.characters;
	total->bytes += file.bytes;
	total->length = total->length > file.length ? total->length : file.length;
}
