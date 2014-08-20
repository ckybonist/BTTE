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


/*
 * Exclude set of number in the random number set
 *
 * Args:
 *     @set : the set of numbers will be excluded
 *     @size : the size of set
 *     @min : minimum for range of numbers
 *     @max : maximum for range of numbers
 *
 * Return:
 *	   the value within the specific range but
 *	   without that exclusive set
 *
 * */
int Dice::excludeSet(int *set, int size, int min, int max) {
	int target = 0;
	bool flag = true;

	while(flag) {
		target = Dice::roll(min, max);

		for(int i = 0; i < size; i++) {
			//if(target == set[i] && set[i] != 0)
			if(target == set[i])
		   	{
				flag = true;
				break;
			}
		   	else
		   	{
				flag = false;
			}
		}
	}

	return target;
}


/*
 * Exclude specific number in the random number set
 *
 * Args:
 *     @num : the number be excluded
 *     @min : minimum for range of numbers
 *     @max : maximum for range of numbers
 *
 * Return:
 *	   the value within the specific range but
 *	   without exclusive number
 *
 * Note: Currently not using this function. Being reserved.
 *
 * */
int Dice::excludeNum(int num, int min, int max) {
	int target = Dice::roll(min, max);

	if(target == num) {
		Dice::excludeNum(num, min, max);
	} else {
		return num;
	}
}

// dice.c
