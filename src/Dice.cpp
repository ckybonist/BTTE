#include "Dice.h"

/*
 * Get random number within [min, max]
 *
 * Args:
 *    @min : minimum
 *    @max : maxmimum
 *
 * */
int Dice::roll(int min, int max) {
	int number;

	number = (int)((rand() / ((double)RAND_MAX + 1)) *
			 (max - min + 1)) + min;

	return number;
}

// dice.c
