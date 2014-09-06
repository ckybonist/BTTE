#ifndef _RANDOMGENERATOR_H
#define _RANDOMGENERATOR_H

#include <cstdlib>

//const long long RANDMAX = 2147483647;  // C++ RAND_MAX Macro
const long long INIT_NUM = 1491701989;
long long rand_num = INIT_NUM;

namespace uniformdist  // uniform distribution random
{
long long rand()
{
    /* X(n-1) * 7^5 % (2^31 - 1) */
    rand_num = 16807 * rand_num % RAND_MAX;
    return rand_num;
}

void srand(int seed)
{
    for(int i = 0; i < seed; i++)
        rand_num = rand();
}
}

int range_rand(const int max, const int min)
{
	int number = (int)((rand() / ((double)RAND_MAX + 1)) *
			 (max - min + 1)) + min;
	return number;
}

#endif // for #ifndef _RANDOMGENERATOR_H
