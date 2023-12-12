#include <assert.h>
#include <err.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct array {
	long *data;
	size_t len;
};

static void chomp(char *);
static void read_history(struct array *, char *);
static void array_fill_with_diff(struct array *, struct array *);
static void extrapolate_backward(struct array *, size_t);
static void extrapolate_forward(struct array *, size_t);
static bool iszero(struct array *);

static void
chomp(char *s) {
	assert(s != NULL);

	while (*s != '\n' && *s != '\0') {
		s++;
	}

	*s = '\0';
}

static void
read_history(struct array *array, char *history) {
	assert(array != NULL);
	assert(history != NULL);

	array->data = NULL;
	array->len = 0;

	for (char *s = strtok(history, " "); s != NULL; s = strtok(NULL, " ")) {
		long value;

		if (sscanf(s, "%ld", &value) != 1) {
			err(EXIT_FAILURE, "sscanf");
		}

		void *tmp = realloc(array->data, sizeof *array->data * (array->len + 1));

		if (tmp == NULL) {
			err(EXIT_FAILURE, "realloc");
		}

		array->data = tmp;
		array->data[array->len++] = value;
	}
}

static void
array_fill_with_diff(struct array *dst, struct array *src) {
	assert(src != NULL);
	assert(dst != NULL);

	dst->data = NULL;
	dst->len = 0;

	for (size_t i = 0; i < src->len; i++) {
		if (i + 1 >= src->len) {
			break;
		}

		void *tmp = realloc(dst->data, sizeof *dst->data * (dst->len + 1));

		if (tmp == NULL) {
			err(EXIT_FAILURE, "realloc");
		}

		dst->data = tmp;
		dst->data[dst->len++] = src->data[i + 1] - src->data[i];
	}
}

static void
extrapolate_backward(struct array *sequences, size_t n_sequences) {
	assert(sequences != NULL);
	assert(n_sequences > 0);

	for (size_t i = 0; i < n_sequences; i++) {
		struct array *current = &sequences[i]; 
		void *tmp = realloc(current->data, sizeof *current->data * (current->len + 1));

		if (tmp == NULL) {
			err(EXIT_FAILURE, "realloc");
		}

		current->data = tmp;
		memmove(&current->data[1], &current->data[0], sizeof *current->data * current->len);
		current->data[0] = 0;
		current->len++;
	}

	for (long i = (long)n_sequences - 2; i >= 0; i--) {
		struct array *next = &sequences[i + 1];
		struct array *current = &sequences[i];

		if (current->len >= 2) {
			current->data[0] = current->data[1] - next->data[0];
		}
	}
}

static void
extrapolate_forward(struct array *sequences, size_t n_sequences) {
	assert(sequences != NULL);
	assert(n_sequences > 0);

	for (size_t i = 0; i < n_sequences; i++) {
		struct array *current = &sequences[i]; 
		void *tmp = realloc(current->data, sizeof *current->data * (current->len + 1));

		if (tmp == NULL) {
			err(EXIT_FAILURE, "realloc");
		}

		current->data = tmp;
		current->data[current->len++] = 0;
	}

	for (long i = (long)n_sequences - 2; i >= 0; i--) {
		struct array *next = &sequences[i + 1];
		struct array *current = &sequences[i];

		if (current->len >= 2) {
			current->data[current->len - 1] = next->data[next->len - 1] + current->data[current->len - 2];
		}
	}
}

static bool
iszero(struct array *array) {
	assert(array != NULL);

	for (size_t i = 0; i < array->len; i++) {
		if (array->data[i] != 0) {
			return false;
		}
	}

	return true;
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
	long part_1 = 0;
	long part_2 = 0;

	while (fgets(buf, sizeof buf, input) != NULL) {
		chomp(buf);
		struct array *sequences = malloc(sizeof *sequences);
		size_t n_sequences = 0;

		if (sequences == NULL) {
			err(EXIT_FAILURE, "malloc");
		}

		read_history(&sequences[n_sequences], buf);
	next:
		n_sequences++;
		void *tmp = realloc(sequences, sizeof *sequences * (n_sequences + 1));

		if (tmp == NULL) {
			err(EXIT_FAILURE, "realloc");
		}

		sequences = tmp;
		array_fill_with_diff(&sequences[n_sequences], &sequences[n_sequences - 1]);

		if (!iszero(&sequences[n_sequences])) {
			goto next;
		}

		n_sequences++;
		extrapolate_forward(sequences, n_sequences);
		extrapolate_backward(sequences, n_sequences);
		part_1 += sequences[0].data[sequences[0].len - 1];
		part_2 += sequences[0].data[0];

		for (size_t i = 0; i < n_sequences; i++) {
			free(sequences[i].data);
		}

		free(sequences);
	}

	printf("Part 1: %ld\n", part_1);
	printf("Part 2: %ld\n", part_2);

	if (ferror(input)) {
		err(EXIT_FAILURE, "fgets");
	}
	if (input != stdin) {
		fclose(input);
	}

	return 0;
}
