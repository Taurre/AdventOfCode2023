#include <assert.h>
#include <ctype.h>
#include <err.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct tile {
	int symbol;
	bool adjacent;
	int value;
};

struct coord {
	int x;
	int y;
};

void
schema_mark_adjacents(struct tile **schema, size_t lines, size_t columns) {
	assert(schema != NULL);
	assert(lines > 0);
	assert(columns > 0);

	struct coord moves[] = {
		{ -1, -1 },
		{ -1,  0 },
		{ -1,  1 },
		{  0, -1 },
		{  0,  1 },
		{  1, -1 },
		{  1,  0 },
		{  1,  1 },
	};

	for (int i = 0; i < (int)lines; i++) {
		for (int j = 0; j < (int)columns; j++) {
			if (isdigit(schema[i][j].symbol) || schema[i][j].symbol == '.') {
				continue;
			}

			for (size_t move = 0; move < sizeof moves / sizeof *moves; move++) {
				struct coord adj = { .x = i + moves[move].x, .y = j + moves[move].y };

				if (adj.x < 0 || adj.x >= (int)lines || adj.y < 0 || adj.y >= (int)columns) {
					continue;
				}

				schema[adj.x][adj.y].adjacent = true;
			}
		}
	}
}

long
schema_compute_sum(struct tile **schema, size_t lines, size_t columns) {
	assert(schema != NULL);
	assert(lines > 0);
	assert(columns > 0);

	long sum = 0;

	for (size_t i = 0; i < lines; i++) {
		int current = 0;

		for (size_t j = 0; j < columns; j++) {
			if (isdigit(schema[i][j].symbol)) {
				if (schema[i][j].adjacent) {
					if (schema[i][j].value != current) {
						current = schema[i][j].value;
						sum += current;
					}
				}
			}
			else {
				current = 0;
			}
		}
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

	char buf[255];
	struct tile **schema = NULL;
	size_t lines = 0;
	size_t columns = 0;

	while (fgets(buf, sizeof buf, input) != NULL) {
		struct tile **tmp = realloc(schema, sizeof *schema * (lines+1));
		int value = 0;

		if (tmp == NULL) {
			err(EXIT_FAILURE, "realloc");
		}

		schema = tmp;

		if (!columns) {
			columns = strlen(buf) - 1;
		}
		else if (columns != strlen(buf) - 1) {
			errx(EXIT_FAILURE, "Bad line length");
		}

		schema[lines] = malloc(sizeof **schema * columns);

		if (schema[lines] == NULL) {
			err(EXIT_FAILURE, "malloc");
		}

		for (size_t i = 0; i < columns; i++) {
			if (isdigit(buf[i])) {
				if (!value) {
					if (sscanf(&buf[i], "%d", &value) != 1) {
						err(EXIT_FAILURE, "sscanf");
					}
				}
			}
			else {
				value = 0;
			}

			schema[lines][i].symbol = buf[i];
			schema[lines][i].adjacent = false;
			schema[lines][i].value = value;
		}

		lines++;
	}

	schema_mark_adjacents(schema, lines, columns);
	printf("Part 1: %ld\n", schema_compute_sum(schema, lines, columns));
#if 0
	for (size_t i = 0; i < lines; i++) {
		for (size_t j = 0; j < columns; j++) {
			printf("%c (%03d), ", schema[i][j].symbol, schema[i][j].value);
		}

		printf("\n");
	}
#endif

	if (ferror(input)) {
		err(EXIT_FAILURE, "fgets");
	}
	if (input != stdin) {
		fclose(input);
	}

	return 0;
}
