#include <cmath>

#include "random.h"


namespace
{

void RandForInternal(const RSC& rsc)
{
    const int iRSC = static_cast<int>(rsc);

    /*  Formula for generating rand num : X(n-1) * 7^5 % (2^31 - 1) */
    g_rand_grp[iRSC] = 16807 * g_rand_grp[iRSC] % RAND_MAX;
}

}


long long g_rand_grp[] =
{
    0, 0, 0,
    0, 0, 0,
    0, 0, 0,
    0, 0, 0,
    0, 0, 0
};


namespace uniformrand {

const int kInterval = 1000000;

long long Rand(const RSC rsc)
{
    const int iRSC = static_cast<int>(rsc);

    /*  Formula for generating rand num : X(n-1) * 7^5 % (2^31 - 1) */
    g_rand_grp[iRSC] = 16807 * g_rand_grp[iRSC] % RAND_MAX;

    return g_rand_grp[iRSC];
}


void Srand(const int iRSC, const int seed)
{
    for (int i = 0; i < seed; i++)
    {
        RandForInternal(static_cast<RSC>(iRSC));

        //g_rand_grp[k_seed_id] = Rand(static_cast<RSC>(k_seed_id));
        //Rand(static_cast<RSC>(k_seed_id));
        //g_rand_num = rand();
    }
}

void InitRandSeeds(long long seed)
{
    //int seed = kInterval;  // ordinal of seed
    for (int seed_id = 0; seed_id < g_kNumRSeeds; seed_id++)
    {
        g_rand_grp[seed_id] = seed;
    }

    for (int seed_id = 0; seed_id < g_kNumRSeeds; seed_id++)
    {
        Srand(seed_id, seed);
        seed = seed + kInterval * (seed_id + 1);
    }
}

float ExpRand(float rate, long long rand_num)
{
    float rand_exp = 0 - (1.0 / rate) *
                     log(1.0 - (double)rand_num / (double)RAND_MAX);
    return rand_exp;
}

} // namespace uniformrand

