#include <cmath>

#include "random.h"

static const long long k_init_seed = 377003613;
//long long g_rand_num = k_init_seed;

long long g_rand_grp[] =
{
    k_init_seed, k_init_seed, k_init_seed,
    k_init_seed, k_init_seed, k_init_seed,
    k_init_seed, k_init_seed, k_init_seed,
    k_init_seed, k_init_seed, k_init_seed,
    k_init_seed, k_init_seed, k_init_seed
};

static void RandForInternal(const RSC& k_seed_id)
{
    const int k_sid = static_cast<int>(k_seed_id);
    /*  Formula for generating rand num : X(n-1) * 7^5 % (2^31 - 1) */
    g_rand_grp[k_sid] = 16807 * g_rand_grp[k_sid] % RAND_MAX;
}

namespace uniformrand {

const int k_interval = 1000000;


long long Rand(const RSC& k_seed_id)
{
    const int k_sid = static_cast<int>(k_seed_id);

    /*  Formula for generating rand num : X(n-1) * 7^5 % (2^31 - 1) */
    //g_rand_grp[k_seed_rsc_id] = 16807 * g_rand_grp[k_seed_rsc_id] % g_k_rand_max;
    g_rand_grp[k_sid] = 16807 * g_rand_grp[k_sid] % RAND_MAX;
    return g_rand_grp[k_sid];
}


void Srand(const int k_seed_id, const int k_seed)
{
    for(int i = 0; i < k_seed; i++)
    {
        RandForInternal(static_cast<RSC>(k_seed_id));

        //g_rand_grp[k_seed_id] = Rand(static_cast<RSC>(k_seed_id));
        //Rand(static_cast<RSC>(k_seed_id));
        //g_rand_num = rand();
    }
}

void InitRandSeeds()
{
    int seed = k_interval;  // ordinal of seed

    for(int seed_id = 0; seed_id < k_num_rseeds; seed_id++)
    {
        Srand(seed_id, seed);
        seed += k_interval * (seed_id + 1);
    }
}

float ExpRand(float rate, long long rand_num)
{
    float rand_exp = 0 - (1.0 / rate) *
                       log(1.0 - (double)rand_num / RAND_MAX);
    return rand_exp;
}

} // namespace uniformrand

