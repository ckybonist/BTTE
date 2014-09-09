#ifndef _UNIFORMRANDOM_H
#define _UNIFORMRANDOM_H

const long long INIT_NUM = 1491701989;
long long randNum = INIT_NUM;

namespace uniformdist  // uniform distribution random
{
long long rand();
void srand(int);
int range_rand(const int, const int);
}

#endif // for #ifndef _RANDOMGENERATOR_H
