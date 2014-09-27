#include <cassert>

#include "random.h"

const long long k_init_seed = 377003613;
//long long g_rand_num = k_init_seed;

long long g_rand_grp[] = {
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

namespace uniformrand {

/* Normal
long long rand(const int k_seed_id) {
    //  Formula for generating rand num : X(n-1) * 7^5 % (2^31 - 1)
    g_rand_grp[k_seed_id] = 16807 * g_rand_grp[k_seed_id] % g_k_rand_max;
    return g_rand_grp[k_seed_id]; }
*/
/* Normal
void srand(const int seed) {
    for(int i = 0; i < seed; i++) {
        g_rand_num = rand();
    }
}
*/

long long Rand(const RSC& k_seed_rsc_id) {
    /*  Formula for generating rand num : X(n-1) * 7^5 % (2^31 - 1) */
    g_rand_grp[k_seed_rsc_id] = 16807 * g_rand_grp[k_seed_rsc_id] % g_k_rand_max;
    return g_rand_grp[k_seed_rsc_id];
}

void Srand(const int k_seed_id, const int k_seed) {
    for(int i = 0; i < k_seed; i++) {
        g_rand_grp[k_seed_id] = Rand(RSC(k_seed_id));  // force int cast to struct: RSC
        //g_rand_num = rand();
    }
}

void InitRandSeeds() {
    int interval = 200000;
    int seed = interval;  // ordinal of seed

    for(int seed_id = 0; seed_id < k_num_rseeds; seed_id++) {
        Srand(seed_id, seed);
        interval *= seed_id + 1;
        seed += interval;
    }
}

int Roll(const RSC& k_seed_rsc_id,
         const int k_low,
         const int k_up) {
    assert(k_up > k_low);
    // [low, up]
	int number = (int)(((double)Rand(k_seed_rsc_id) / ((double)g_k_rand_max + 1)) *
			     (k_up - k_low + 1)) + k_low;
    // [low, up)
	/* int number = (int)(((double)uniformdist::rand(k_seed_rsc_id) / ((double)g_k_rand_max + 1)) *
			     (k_up - k_low)) + k_low; */
	return number;
}

float Roll(const RSC& k_seed_rsc_id,
           const float k_low,
           const float k_up) {
    assert(k_up > k_low);
    // [low, up]
	float number = (((float)Rand(k_seed_rsc_id) / ((float)g_k_rand_max + 1)) *
			     (k_up - k_low + 0.02)) + k_low;
	return number;
}

int* DistinctRandNumbers(const RSC& k_seed_rsc_id,
                         const int k_size,
                         const int k_rand_limit) {
    int *arr = new int[k_size];

    for(int m = 0; m < k_size; m++)
    {
        int rand_num = Rand(k_seed_rsc_id) % k_rand_limit + 1;

        int s = m;

        bool duplicate = true;

        while(duplicate && s > 0)
        {
            --s;
            if(rand_num == arr[s])  // rand_num is duplicate
            {
                rand_num = Rand(k_seed_rsc_id) % 50 + 1;
                s = m;
                continue;
            }

            if(rand_num != arr[s] && s == 0)  // rand_num is distinct
            {
                duplicate = false;
            }
        }

        arr[m] = rand_num;
    }
    return arr;
}

} // namespace uniformrand

