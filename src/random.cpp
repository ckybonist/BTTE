#include "random.h"

const long long INIT_NUM = 1491701989;
long long g_rand_num = INIT_NUM;

namespace uniformdist {
long long rand() {
    /* X(n-1) * 7^5 % (2^31 - 1) */
    g_rand_num = 16807 * g_rand_num % RANDMAX;
    return g_rand_num;
}

void srand(int seed) {
    for(int i = 0; i < seed; i++)
        g_rand_num = rand();
}

int RangeRand(const int low, const int up) {
    // [low, up]
	int number = (int)(((double)uniformdist::rand() / ((double)RANDMAX + 1)) *
			     (up - low + 1)) + low;
    // [low, up)
	/* int number = (int)(((double)uniformdist::rand() / ((double)RANDMAX + 1)) *
			     (up - low)) + low; */
	return number;
}
}

