#ifndef _RANDOM_H
#define _RANDOM_H

const long long g_k_rand_max = 2147483647;  // C++ RAND_MAX Macro

extern long long g_rand_num;

namespace uniformdist {  // uniform distribution random
long long rand();
void srand(int);
int RangeRand(const int low, const int up);
}

#endif // for #ifndef _RANDOM_H
