#include <assert.h>
#include <err.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct digit {
	const char *name;
	int digit;
	int value;
};

struct digit digits[] = {
	{ "one",   '1', 1 },
	{ "two",   '2', 2 },
	{ "three", '3', 3 },
	{ "four",  '4', 4 },
	{ "five",  '5', 5 },
	{ "six",   '6', 6 },
	{ "seven", '7', 7 },
	{ "eight", '8', 8 },
	{ "nine",  '9', 9 },
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

	while (fgets(buf, sizeof buf, input) != NULL) {
		size_t len = strlen(buf);
		struct digit *first = NULL, *last = NULL;

		for (size_t i = 0; i < len; i++) {
			for (size_t j = 0; j < sizeof digits / sizeof *digits; j++) {
				if (strncmp(&buf[i], digits[j].name, strlen(digits[j].name)) == 0 || buf[i] == digits[j].digit) {
					if (first == NULL) {
						first = &digits[j];
					}
					else {
						last = &digits[j];
					}
				}
			}
		}

		if (last == NULL) {
			last = first;
		}

		assert(first != NULL && last != NULL);
		sum += first->value * 10 + last->value;
	}

	printf("Answer: %ld\n", sum);

	if (ferror(input)) {
		err(EXIT_FAILURE, "fgets");
	}
	if (input != stdin) {
		fclose(input);
	}

	return 0;
}
