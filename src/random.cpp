#include "random.h"

static const long long k_init_seed = 377003613;
//long long g_rand_num = k_init_seed;

long long g_rand_grp[] =
{
    k_init_seed,
    k_init_seed,
    k_init_seed,
    k_init_seed,
    k_init_seed,
    k_init_seed,
    k_init_seed,
    k_init_seed,
    k_init_seed,
    k_init_seed,
    k_init_seed,
    k_init_seed,
    k_init_seed,
    k_init_seed,
    k_init_seed
};

static void RandForInternal(const RSC& k_seed_rsc_id)
{
    /*  Formula for generating rand num : X(n-1) * 7^5 % (2^31 - 1) */
    g_rand_grp[k_seed_rsc_id] = 16807 * g_rand_grp[k_seed_rsc_id] % g_k_rand_max;
}

namespace uniformrand {

const int k_interval = 1000000;


long long Rand(const RSC& k_seed_rsc_id)
{
    /*  Formula for generating rand num : X(n-1) * 7^5 % (2^31 - 1) */
    g_rand_grp[k_seed_rsc_id] = 16807 * g_rand_grp[k_seed_rsc_id] % g_k_rand_max;
    return g_rand_grp[k_seed_rsc_id];
}


void Srand(const int k_seed_id, const int k_seed)
{
    for(int i = 0; i < k_seed; i++)
    {
        //g_rand_grp[k_seed_id] = Rand(static_cast<RSC>(k_seed_id));  // force int cast to struct: RSC
        //Rand(static_cast<RSC>(k_seed_id));
        RandForInternal(static_cast<RSC>(k_seed_id));
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

} // namespace uniformrand

