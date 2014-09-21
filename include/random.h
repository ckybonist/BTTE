#ifndef _RANDOM_H
#define _RANDOM_H

const long long g_k_rand_max = 2147483647;  // C++ RAND_MAX Macro

/*
 * Group of Rand Seeds :
 *     0 事件產生
 *     1 peer 的群組
 *     2 亂數抓 peer list
 *     3 load balancing 的方法抓 peer list
 *     4 cluster 的方法抓 peer list
 *     5 piece selection (random first piece, rarest first)
 *     6 optimistic unchokingng7peer leave
 *     8 leecher 有的 piece 的基準值
 *     9 leecher 有該塊 piece 的機率
 *
 * * * * * * * * */
extern long long g_rand_seeds[10];

extern long long g_rand_num;


namespace uniformdist {  // uniform distribution random
long long rand();
void srand(int);
int RangeRand(const int low, const int up);
}

#endif // for #ifndef _RANDOM_H
