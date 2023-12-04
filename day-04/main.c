#include <assert.h>
#include <err.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LINES   211
#define COLUMNS 36

struct array {
	int *data;
	size_t len;
};

static int compare(const void *, const void *);
static long copies_sum(int *);

const int scratchcards[LINES][COLUMNS] = {
#	include "input.h"
};

static int
compare(const void *a, const void *b) {
	const int *x = a;
	const int *y = b;

	return (*x < *y) ? -1 : (*x == *y) ? 0 : 1;
}

static long
copies_sum(int *copies) {
	long sum = 0;

	for (size_t i = 0; i < LINES; i++) {
		sum += copies[i];
	}

	return sum;
}

int
main(int argc, char **argv) {
	FILE *input = NULL;

	if (argc < 2) {
		input = stdin;
	}
	else  {
		input = fopen(argv[1], "rb");

		if (input == NULL) {
			err(EXIT_FAILURE, "fopen(%s)", argv[1]);
		}
	}

	long sum = 0;
	int copies[LINES];

	for (size_t i = 0; i < LINES; i++) {
		copies[i] = 1;
	}

	for (size_t line = 0; line < LINES; line++) {
		struct array wnumbers = { .data = NULL, .len = 0 };
		bool check = false;
		long points = 0;
		long copy = 0;

		for (size_t column = 0; column < COLUMNS; column++) {
			if (scratchcards[line][column] == 0) {
				check = true;
				qsort(wnumbers.data, wnumbers.len, sizeof *wnumbers.data, &compare);
				continue;
			}
			if (!check) {
				void *tmp = realloc(wnumbers.data, sizeof *wnumbers.data * (wnumbers.len+1));

				if (tmp == NULL) {
					err(EXIT_FAILURE, "realloc");
				}

				wnumbers.data = tmp;
				wnumbers.data[wnumbers.len++] = scratchcards[line][column];
			}
			else {
				int *p = bsearch(&scratchcards[line][column], wnumbers.data, wnumbers.len, sizeof *wnumbers.data, &compare);

				if (p != NULL) {
					if (points == 0) {
						points = 1;
					}
					else {
						points <<= 1;
					}

					copy++;
				}
			}
		}

		for (long i = 0; i < copies[line]; i++) {
			for (long j = 0; j < copy; j++) {
				assert(line + j + 1 < LINES);
				copies[line + j + 1]++;
			}
		}

		sum += points;
	}

	printf("Part 1: %ld\n", sum);
	printf("Part 2: %ld\n", copies_sum(copies));

	if (ferror(input)) {
		err(EXIT_FAILURE, "fgets");
	}
	if (input != stdin) {
		fclose(input);
	}

	return 0;
}
