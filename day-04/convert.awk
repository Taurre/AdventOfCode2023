BEGIN {
	FS = "[: ]+"
}

{
	printf("{ ")

	for (i = 3; i <= NF; i++) {
		if ($i ~ /\|/) {
			printf("%2d, ", 0)
		}
		else {
			printf("%2d, ", $i)
		}
	}

	printf("},\n")
}
