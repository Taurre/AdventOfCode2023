#include <assert.h>
#include <err.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct race {
	long time;
	long distance;
};

#if 0
static struct race races[] = {
	{ 7, 9 },
	{ 15, 40},
	{ 30, 200},
};
#endif
static struct race races[] = {
	{ 40, 215 },
	{ 92, 1064 },
	{ 97, 1505 },
	{ 90, 1100 },
};

static long
ways_to_win(long time, long distance) {
	long ways = 0;

	for (long i = 1; i < time; i++) {
		if (i * (time - i) > distance) {
			ways++;
		}
	}

	return ways;
}

int
main(void) {
	long margin = 1;

	for (size_t i = 0; i < sizeof races / sizeof *races; i++) {
		margin *= ways_to_win(races[i].time, races[i].distance);
	}

	printf("Part 1: %ld\n", margin);
	printf("Part 2: %ld\n", ways_to_win(40929790L, 215106415051100L));
	return 0;
}
