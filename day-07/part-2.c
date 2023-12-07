#include <assert.h>
#include <err.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum hand_type {
	HT_UNKNOWN = 0,
	HT_HIGH_CARD,
	HT_ONE_PAIR,
	HT_TWO_PAIRS,
	HT_THREE_OF_A_KIND,
	HT_FULL_HOUSE,
	HT_FOUR_OF_A_KIND,
	HT_FIVE_OF_A_KIND,
};

struct hand {
	char cards[6];
	long bid;
	enum hand_type type;
};

struct card {
	int symbol;
	size_t count;
};

int card_strenght[] = {
	['J'] =  1,
	['2'] =  2,
	['3'] =  3,
	['4'] =  4,
	['5'] =  5,
	['6'] =  6,
	['7'] =  7,
	['8'] =  8,
	['9'] =  9,
	['T'] = 10,
	['Q'] = 11,
	['K'] = 12,
	['A'] = 13,
};

static enum hand_type hand_type(struct hand *);
static int compare_count(const void *, const void *);
static int compare_hands(const void *, const void *);

static enum hand_type
hand_type(struct hand *hand) {
	assert(hand != NULL);
	struct card cards[] = {
		{ 'J', 0 },
		{ '2', 0 },
		{ '3', 0 },
		{ '4', 0 },
		{ '5', 0 },
		{ '6', 0 },
		{ '7', 0 },
		{ '8', 0 },
		{ '9', 0 },
		{ 'T', 0 },
		{ 'Q', 0 },
		{ 'K', 0 },
		{ 'A', 0 },
	};

	for (size_t i = 0; i < sizeof hand->cards - 1; i++) {
		for (size_t j = 0; j < sizeof cards / sizeof *cards; j++) {
			if (hand->cards[i] == cards[j].symbol) {
				cards[j].count++;
				break;
			}
		}
	}

	qsort(cards, sizeof cards / sizeof *cards, sizeof *cards, &compare_count);
	struct card *jokers = NULL;

	for (size_t i = 0; i < sizeof cards / sizeof *cards; i++) {
		if (cards[i].symbol == 'J') {
			jokers = &cards[i];
			break;
		}
	}

	assert(jokers != NULL);

	switch (cards[sizeof cards / sizeof *cards - 1].count) {
	case 5:
		return HT_FIVE_OF_A_KIND;
	case 4:
		if (jokers->count == 1 || jokers->count == 4) {
			return HT_FIVE_OF_A_KIND;
		}

		return HT_FOUR_OF_A_KIND;
	case 3:
		if (jokers->count == 3) {
			if (cards[sizeof cards / sizeof *cards - 2].count == 2) {
				return HT_FIVE_OF_A_KIND;
			}

			return HT_FOUR_OF_A_KIND;
		}
		else if (jokers->count == 2) {
			return HT_FIVE_OF_A_KIND;
		}
		else if (jokers->count == 1) {
			return HT_FOUR_OF_A_KIND;
		}
		else if (cards[sizeof cards / sizeof *cards - 2].count == 2) {
			return HT_FULL_HOUSE;
		}
		else {
			return HT_THREE_OF_A_KIND;
		}

		break;
	case 2:
		if (jokers->count == 2) {
			if (cards[sizeof cards / sizeof *cards - 2].count == 2) {
				return HT_FOUR_OF_A_KIND;
			}

			return HT_THREE_OF_A_KIND;
		}
		else if (jokers->count == 1) {
			if (cards[sizeof cards / sizeof *cards - 2].count == 2) {
				return HT_FULL_HOUSE;
			}

			return HT_THREE_OF_A_KIND;
		}
		else if (cards[sizeof cards / sizeof *cards - 2].count == 2) {
			return HT_TWO_PAIRS;
		}
		else {
			return HT_ONE_PAIR;
		}

		break;

	case 1:
		if (jokers->count == 1) {
			return HT_ONE_PAIR;
		}
		else {
			return HT_HIGH_CARD;
		}
	default:
		err(EXIT_FAILURE, "Bad hand");
		return HT_UNKNOWN;
	}
}

static int
compare_count(const void *a, const void *b) {
	const struct card *ca = a;
	const struct card *cb = b;

	return (ca->count < cb->count) ? -1 : (ca->count == cb->count) ? 0 : 1;
}

static int
compare_hands(const void *a, const void *b) {
	const struct hand *ha = a;
	const struct hand *hb = b;

	if (ha->type != hb->type) {
		return (ha->type < hb->type) ? -1 : 1;
	}

	for (size_t i = 0; i < sizeof ha->cards - 1; i++) {
		int sa = card_strenght[(int)ha->cards[i]];
		int sb = card_strenght[(int)hb->cards[i]];

		if (sa != sb) {
			return (sa < sb) ? -1 : 1;
		}
	}

	return 0;
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
	struct hand *hands = NULL;
	size_t n_hands = 0;

	while (fgets(buf, sizeof buf, input) != NULL) {
		char hand[6];
		long bid;

		if (sscanf(buf, "%5s %ld", hand, &bid) != 2) {
			err(EXIT_FAILURE, "sscanf");
		}

		void *tmp = realloc(hands, sizeof *hands * (n_hands + 1));

		if (tmp == NULL) {
			err(EXIT_FAILURE, "realloc");
		}

		hands = tmp;
		strcpy(hands[n_hands].cards, hand);
		hands[n_hands].bid = bid;
		hands[n_hands].type = hand_type(&hands[n_hands]);
		n_hands++;
	}

	qsort(hands, n_hands, sizeof *hands, &compare_hands);
	long total = 0;

	for (size_t i = 0; i < n_hands; i++) {
		total += hands[i].bid * (i + 1);
	}

	printf("Part 2: %ld\n", total);

	if (ferror(input)) {
		err(EXIT_FAILURE, "fgets");
	}
	if (input != stdin) {
		fclose(input);
	}

	return 0;
}
