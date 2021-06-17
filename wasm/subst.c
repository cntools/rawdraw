/*
 * Copyright (c) 2021 zNoctum
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this
 * software and associated documentation files (the "Software"), to deal in the Software
 * without restriction, including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons
 * to whom the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED *AS IS*, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
 * PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE
 * FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct table_entry {
	size_t keysize;
	char *key;
	char *value;
};

#define SWALLOW 1

char *readfile(const char *filename, int flags)
{
	size_t size;
	char *buffer;
	FILE *file = fopen(filename, "r");

	if (file == NULL) {
		printf("Failed to open '%s'!\n", filename);
		exit(1);
	}

	fseek(file, 0, SEEK_END);
	size = ftell(file);
	rewind(file);
	
	buffer = malloc(size);

	fread(buffer, size, 1, file);
	
	if ((flags & SWALLOW) != 0) {
		if (buffer[size - 1] == '\n')
			buffer[size - 1] = 0x00;
	}

	fclose(file);
	return buffer;
}

void print_help()
{
	printf(
		"Usage: subst [TEMPLATE-FILE] [VARIABLES]\n"
		"\n"
		"Options:\n"
		"  -s: Swallow following newline in files\n"
		"  -o: Define output file\n"
		"  -f: Define variable to replace '-f VARNAME FILEPATH'\n"
		"  -d: Define variable to replace '-f VARNAME VARCONTENT'\n"
		"  -h: Print this message\n"
	);
}


char *outfile_name = NULL;
char *outfile = NULL;
size_t outfile_size;
size_t outfile_current_size;
int outfile_set = 0;
void putchar_in_output(char c)
{
	if (outfile_set) {
		if (outfile == NULL) {
			outfile = malloc(sizeof(char) * 1024);
			outfile_size = 1024;
		}
		if (outfile_current_size == outfile_size) {
			outfile_size *= 2;
			outfile = realloc(outfile, outfile_size);
		}
		outfile[outfile_current_size++] = c;
	} else {
		putchar(c);
		fflush(stdout);
	}
}


int main(int argc, char *argv[])
{
	int flags = 0;
	size_t i;
	size_t table_counter = 0;
	struct table_entry *table = malloc((argc - 2) / 3 * sizeof(struct table_entry));
	char *infile;
	char *ident_begin;
	char *table_str;
	size_t ident_len;

	if (argc < 2) {
		printf("No input file was specified\n");
		exit(1);
	}
	
	if (argv[1][0] == '-' && argv[1][1] == 'h') {
		print_help();
		return 0;
	}
		
	
	infile = readfile(argv[1], 0);

read_args:
	for (i = 2; i < argc; i++) {
		if (argv[i][0] == '-') {
			switch(argv[i][1]) {
			case 's':
				if ((flags & SWALLOW) == 0) {
					flags |= SWALLOW;
					goto read_args;
				}
				break;
			case 'o':
				outfile_name = argv[++i];
				outfile_set = 1;
				break;
			case 'd':
				if (i++ == argc) {
					printf("supplied not enough arguments\n");
				}
				table[table_counter].keysize = strlen(argv[i]);
				table[table_counter].key = argv[i];
				i++;
				table[table_counter].value = argv[i];
				table_counter++;
				break;
			case 'f':
				if (i++ == argc) {
					printf("supplied not enough arguments\n");
				}
				table[table_counter].keysize = strlen(argv[i]);
				table[table_counter].key = argv[i];
				i++;
				table[table_counter].value = readfile(argv[i], flags);
				table_counter++;
				break;
			case 'h':
				print_help();
				return 0;
			}
		}

	}

	while (*infile) {
		if (!(*infile == '$' && infile[1] == '{')) {
			putchar_in_output(*infile);
			infile++;
			continue;
		}
		infile += 2;
		ident_begin = infile;
		ident_len = 0;
		while (*infile != '}') {
			if (*infile == 0x00) {
				printf("Unclosed variable refrence!\n");
				exit(1);
			}
			infile++;
			ident_len++;
		}
		for (i = 0; i < table_counter; i++) {
			if (table[i].keysize != ident_len)
				continue;
			if (strncmp(table[i].key, ident_begin, ident_len) != 0)
				continue;
			table_str = table[i].value;
			while (*table_str)
				putchar_in_output(*table_str++);
		}

		infile++;
	}

	if (outfile_set) {
		FILE* file = fopen(outfile_name, "w");
		if (file == NULL) {
			printf("Failed to open %s for output!\n", outfile_name);
			exit(1);
		}
		fwrite(outfile, outfile_current_size - 3, 1, file);
		fclose(file);
	}
	return 0;
}
