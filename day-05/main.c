#include <assert.h>
#include <err.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct seeds {
	long *seeds;
	size_t n_seeds;
};

struct range {
	long dst_start;
	long dst_end;
	long src_start;
	long src_end;
	size_t len;
};

static struct seeds *read_seeds(char *);
static struct range *read_range(char *);
static int compare(const void *, const void *); 

static struct seeds *
read_seeds(char *line) {
	assert(line != NULL);
	struct seeds *seeds = malloc(sizeof *seeds);

	if (seeds == NULL) {
		err(EXIT_FAILURE, "malloc");
	}

	seeds->seeds = NULL;
	seeds->n_seeds = 0;
	char *s = strchr(line, ':');

	if (s == NULL) {
		err(EXIT_FAILURE, "Bad format for seeds");
	}

	for (s = strtok(s + 1, " "); s != NULL; s = strtok(NULL, " ")) {
		long seed;

		if (sscanf(s, "%ld", &seed) != 1) {
			err(EXIT_FAILURE, "sscanf");
		}

		void *tmp = realloc(seeds->seeds, sizeof *seeds->seeds * (seeds->n_seeds + 1));

		if (tmp == NULL) {
			err(EXIT_FAILURE, "realloc");
		}

		seeds->seeds = tmp;
		seeds->seeds[seeds->n_seeds++] = seed;
	}

	return seeds;
}

static struct range *
read_range(char *line) {
	assert(line != NULL);
	long src, dst;
	size_t len;
	struct range *range = malloc(sizeof *range);

	if (range == NULL) {
		err(EXIT_FAILURE, "malloc");
	}
	if (sscanf(line, "%ld %ld %zu", &dst, &src, &len) != 3) {
		err(EXIT_FAILURE, "sscanf");
	}

	range->dst_start = dst;
	range->dst_end = dst + len - 1;
	range->src_start = src;
	range->src_end = src + len - 1;
	range->len = len;
	return range;
}

static int
compare(const void *a, const void *b) {
	const long *c = a;
	const long *d = b;

	return (*c < *d) ? -1 : (*c == *d) ? 0 : 1;
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

	if (fgets(buf, sizeof buf, input) == NULL) {
		err(EXIT_FAILURE, "fgets");
	}

	struct seeds *seeds = read_seeds(buf);
	char changes[seeds->n_seeds];
	memset(changes, 0, seeds->n_seeds);

	while (fgets(buf, sizeof buf, input) != NULL) {
		if (!isdigit(*buf)) {
			memset(changes, 0, seeds->n_seeds);
			continue;
		}

		struct range *range = read_range(buf);

		for (size_t i = 0; i < seeds->n_seeds; i++) {
			if (seeds->seeds[i] < range->src_start || seeds->seeds[i] > range->src_end) {
				continue;
			}
			if (!changes[i]) {
				seeds->seeds[i] -= range->src_start;
				seeds->seeds[i] += range->dst_start;
				changes[i] = 1;
			}
		}
	}

	qsort(seeds->seeds, seeds->n_seeds, sizeof *seeds->seeds, &compare);
	printf("Part 1: %ld\n", seeds->seeds[0]);

	if (ferror(input)) {
		err(EXIT_FAILURE, "fgets");
	}
	if (input != stdin) {
		fclose(input);
	}

	return 0;
}
