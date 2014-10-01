#ifndef _RANDOM_H
#define _RANDOM_H

#include <cstring>

#include "error.h"

const long long g_k_rand_max = 2147483647;  // C++ RAND_MAX Macro

/*
 * Group of Rand Seeds (size 15) :
 *       0 : peer level
 *       1 : prob_leech
 *       2 : prob_piece
 *       3 : Event Time
 *       4 : peer leave (random time)
 *       5 : standard piece selection
 *       6 : cluster-based peer selection
 *       7 : load balancing peer selection
 *       8 : piece selection (random-first-piece, rarest-first)
 *       9 : choking & optimistic unchoking
 *       10 : free
 *       11 : free
 *       12 : free
 *       13 : free
 *       14 : free
 *
 * * * * * * * * */
const int k_num_rseeds = 15;

/*
typedef enum RandSeedCases
{
    rsc_peer_level = 0,
    rsc_prob_leech,
    rsc_prob_piece,
    rsc_event_time,
    rsc_peer_leave,
    rsc_pgdelay,
    rsc_std_peerselect,
    rsc_cb_peerselect,
    rsc_lb_peerselect,
    rsc_pieceselect,
    rsc_choking,

    // Free usage below
    rsc_free_2,
    rsc_free_3,
    rsc_free_4,
    rsc_free_5,
} RSC;
*/

typedef enum class RandSeedCases
{
    peer_level = 0,
    prob_leech,
    prob_piece,
    event_time,
    peer_leave,
    pgdelay,
    std_peerselect,
    cb_peerselect,
    lb_peerselect,
    pieceselect,
    choking,

    // Free usage below
    free_2,
    free_3,
    free_4,
    free_5,
} RSC;


extern long long g_rand_grp[k_num_rseeds];
//extern long long g_rand_num;


namespace uniformrand
{

long long Rand(const RSC& k_seed_id);

void Srand(const int k_seed_id, const int k_seed);

void InitRandSeeds();

template<typename T>
T Roll(const RSC& k_seed_rsc_id,
       const T k_low,
       const T k_up);

template <typename T>
T* DistinctRandNumbers(const RSC& k_seed_id,
                       const size_t k_size,
                       const T k_rand_limit);

template<typename T>
void Shuffle(const RSC& k_seed_rsc_id, T *arr, size_t N);

#include "random.tpp"

} // namespace uniformrand

#endif // for #ifndef _RANDOM_H
