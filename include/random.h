#ifndef _UNIFORMRANDOM_H
#define _UNIFORMRANDOM_H

const long long RANDMAX = 2147483647;  // C++ RAND_MAX Macro

extern long long g_rand_num;

namespace uniformdist {  // uniform distribution random
long long rand();
void srand(int);
int RangeRand(const int low, const int up);
}

#endif // for #ifndef _RANDOMGENERATOR_H
