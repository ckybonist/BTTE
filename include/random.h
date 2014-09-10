#ifndef _UNIFORMRANDOM_H
#define _UNIFORMRANDOM_H

extern long long g_rand_num;

namespace uniformdist {  // uniform distribution random
long long rand();
void srand(int);
int RangeRand(const int, const int);
}

#endif // for #ifndef _RANDOMGENERATOR_H
