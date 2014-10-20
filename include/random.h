#ifndef _RANDOM_H
#define _RANDOM_H

#include <iostream>
#include <cstdlib>
#include <cstring>
#include <typeinfo>

#include "error.h"

//const long long g_kRandMax = 2147483647;  // C++ RAND_MAX Macro

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
const int g_kNumRSeeds = 15;

typedef enum class RandSeedCases
{
    PEER_LEVEL = 0,
    PROB_LEECH,
    PROB_PIECE,
    EVENT_TIME,
    PEER_LEAVE,
    PGDELAY,
    STD_PEERSELECT,
    CB_PEERSELECT,
    LB_PEERSELECT,
    PIECESELECT,
    CHOKING,

    // Free usage below
    FREE_2,
    FREE_3,
    FREE_4,
    FREE_5,
} RSC;


extern long long g_rand_grp[g_kNumRSeeds];
//extern long long g_rand_num;


namespace uniformrand
{

long long Rand(const RSC& rsc);

void Srand(const int iRsc, const int seed);

void InitRandSeeds();

float ExpRand(float rate, long long rand_num);

template<typename T>
T Roll(const RSC& rsc,
       const T low,
       const T up);

template <typename T>
T* DistinctRandNum(const RSC& rsc,
                   const size_t size,
                   const T rand_limit);

template<typename T>
void Shuffle(const RSC& rsc, T *arr, size_t N);


/*
 * Generating random numbers except some numbers in
 * exclude-set
 *
 * Args:
 *     @set : the set of numbers trying to exclude
 *     @set_size : the size of set
 *     @min : minimum for range of numbers
 *     @max : maximum for range of numbers
 *
 * Return:
 *	   random nubmer within the specific range but
 *	   except numbers in that exclusive set
 *
 * */
template<typename T, size_t set_size>
T RangeRandNumExceptEx(const RSC& rsc, const T (&exclude_set)[set_size]);

#include "random.tpp"

} // namespace uniformrand

#endif // for #ifndef _RANDOM_H
