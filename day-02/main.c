#include <err.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define RED_MAX   12
#define GREEN_MAX 13
#define BLUE_MAX  14

enum {
	RED,
	GREEN,
	BLUE,
};

struct color {
	const char *name;
	int max;
	int fewest;
};

struct color colors[] = {
	{ .name = "red",   .max = RED_MAX,   .fewest = 0 },
	{ .name = "green", .max = GREEN_MAX, .fewest = 0 },
	{ .name = "blue",  .max = BLUE_MAX,  .fewest = 0 },
};

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
	long sum = 0;
	long powers = 0;

	while (fgets(buf, sizeof buf, input) != NULL) {
		int id;
		int offset = 0;

		for (size_t i = 0; i < sizeof colors / sizeof *colors; i++) {
			colors[i].fewest = 0;
		}
		if (sscanf(buf, "Game %d:%n", &id, &offset) != 1) {
			err(EXIT_FAILURE, "Bad input format");
		}

		char *current = buf + offset;
		bool valid = true;

		for (;;) {
			struct color *color = NULL;
			char color_name[8];
			int value = 0;

			if (sscanf(current, "%d %7[^,;\n]", &value, color_name) != 2) {
				err(EXIT_FAILURE, "Bad input format");
			}

			for (size_t i = 0; i < sizeof colors / sizeof *colors; i++) {
				if (strcmp(colors[i].name, color_name) == 0) {
					if (colors[i].fewest < value) {
						colors[i].fewest = value;
					}

					color = &colors[i];
					break;
				}
			}

			if (color == NULL) {
				err(EXIT_FAILURE, "Bad input format");
			}
			if (value > color->max) {
				valid = false;
			}

			current = strpbrk(current, ",;");

			if (current == NULL) {
				break;
			}

			current++;
		}

		if (valid) {
			sum += id;
		}

		powers += colors[RED].fewest * colors[GREEN].fewest * colors[BLUE].fewest;
	}

	printf("Part 1: %ld\n", sum);
	printf("Part 2: %ld\n", powers);

	if (ferror(input)) {
		err(EXIT_FAILURE, "fgets");
	}
	if (input != stdin) {
		fclose(input);
	}

	return 0;
}
