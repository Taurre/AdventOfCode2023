#define _XOPEN_SOURCE 500
#include <assert.h>
#include <err.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct row {
	char *springs;
	size_t n_springs;
	int *groups;
	size_t n_groups;
};

static void chomp(char *);
static bool isvalid(struct row *);
static size_t arrange(struct row *, size_t *, size_t, size_t);

static void
chomp(char *s) {
	assert(s != NULL);

	while (*s != '\n' && *s != '\0') {
		s++;
	}

	*s = '\0';
}

static size_t arrange(struct row *row, size_t *stack, size_t index, size_t max) {
	assert(row != NULL);
	size_t count = 0;
	char possible[] = { '#', '.' };

	if (index >= max) {
		return isvalid(row) ? 1 : 0;
	}

	for (size_t j = 0; j < sizeof possible / sizeof *possible; j++) {
		row->springs[stack[index]] = possible[j];
		count += arrange(row, stack, index + 1, max);
	}

	return count;
} 

static bool
isvalid(struct row *row) {
	assert(row != NULL);

	char *springs = row->springs;
	size_t group = 0;	

	for (;;) {
		while (*springs == '.') {
			springs++;
		}
		if (*springs == '\0') {
			break;
		}
		if (group == row->n_groups) {
			break;
		}

		for (size_t i = 0; i < (size_t)row->groups[group]; i++) {
			if (springs[i] != '#') {
				return false;
			}
		}

		springs += row->groups[group];
		group++;

		if (*springs == '#') {
			return false;
		}
	}

	return (group == row->n_groups) ? ((*springs == '\0') ? true : false) : false;
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

	char buf[BUFSIZ];
	struct row *rows = NULL;
	size_t n_rows = 0;

	while (fgets(buf, sizeof buf, input) != NULL) {
		chomp(buf);
		void *tmp = realloc(rows, sizeof *rows * (n_rows + 1));

		if (tmp == NULL) {
			err(EXIT_FAILURE, "realloc");
		}

		rows = tmp;
		struct row *row = &rows[n_rows];
		n_rows++;
		char *spring = strtok(buf, " ");

		if (spring == NULL) {
			err(EXIT_FAILURE, "strtok");
		}

		row->springs = strdup(spring);
		row->n_springs = strlen(spring);

		if (row->springs == NULL) {
			err(EXIT_FAILURE, "strdup");
		}

		row->groups = NULL;
		row->n_groups = 0;

		for (char *group = strtok(NULL, ","); group != NULL; group = strtok(NULL, ",")) {
			int value;

			if (sscanf(group, "%d", &value) != 1) {
				err(EXIT_FAILURE, "sscanf");
			}

			void *tmp = realloc(row->groups, sizeof *row->groups * (row->n_groups + 1));

			if (tmp == NULL) {
				err(EXIT_FAILURE, "realloc");
			}

			row->groups = tmp;
			row->groups[row->n_groups++] = value;
			
		}
	}

	if (ferror(input)) {
		err(EXIT_FAILURE, "fgets");
	}
	if (input != stdin) {
		fclose(input);
	}

	long sum = 0;

	for (size_t i = 0; i < n_rows; i++) {
		size_t stack[rows[i].n_springs];
		size_t *top = stack;

		for (size_t j = 0; j < rows[i].n_springs; j++) {
			if (rows[i].springs[j] == '?') {
				*top++ = j;
			}
		}

		sum += arrange(&rows[i], stack, 0, top - stack);
	}

	printf("Part 1: %ld\n", sum);
	return 0;
}
