#include "iostream"
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
};  // for extern definition

namespace uniformdist {

/* Normal
long long rand(const int k_seed_id) {
    //  Formula for generating rand num : X(n-1) * 7^5 % (2^31 - 1)
    g_rand_grp[k_seed_id] = 16807 * g_rand_grp[k_seed_id] % g_k_rand_max;
    return g_rand_grp[k_seed_id];
}
*/
/* Normal
void srand(const int seed) {
    for(int i = 0; i < seed; i++) {
        g_rand_num = rand();
    }
}
*/

long long rand(const RSC &k_seed_id) {
    /*  Formula for generating rand num : X(n-1) * 7^5 % (2^31 - 1) */
    g_rand_grp[k_seed_id] = 16807 * g_rand_grp[k_seed_id] % g_k_rand_max;
    return g_rand_grp[k_seed_id];
}

void srand(const int k_seed_id, const int k_ordinal) {
    for(int i = 0; i < k_ordinal; i++) {
        g_rand_grp[k_seed_id] = rand(RSC(k_seed_id));  // force int cast to struct: RSC
        //g_rand_num = rand();
    }
}

void InitRandSeeds() {
    const int k_interval = 10000;
    int ordinal = k_interval;  // ordinal of seed

    /*
    for(int i = 0; i < k_num_rseeds; i++) {
        g_rand_grp[i] = k_init_seed;
    }
    */

    for(int seed_id = 0; seed_id < k_num_rseeds; seed_id++) {
        srand(seed_id, ordinal);
        ordinal += k_interval;
    }
}

int Roll(const RSC &k_seed_id, const int low, const int up) {
    // [low, up]
	int number = (int)(((double)uniformdist::rand(k_seed_id) / ((double)g_k_rand_max + 1)) *
			     (up - low + 1)) + low;
    // [low, up)
	/* int number = (int)(((double)uniformdist::rand(k_seed_id) / ((double)g_k_rand_max + 1)) *
			     (up - low)) + low; */
	return number;
}

} // namespace uniformdist

