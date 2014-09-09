#include "Random.h"

namespace uniformdist
{
const long long RAND_MAX = 2147483647;  // C++ RAND_MAX Macro
long long rand()
{
    /* X(n-1) * 7^5 % (2^31 - 1) */
    randNum = 16807 * randNum % RAND_MAX;
    return randNum;
}

void srand(int seed)
{
    for(int i = 0; i < seed; i++)
        randNum = rand();
}

int range_rand(const int max, const int min)
{
    randNum = rand();
	int number = (int)(((double)randNum / ((double)RAND_MAX + 1)) *
			 (max - min + 1)) + min;
	return number;
}
}
