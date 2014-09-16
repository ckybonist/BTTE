#ifndef _NEIGHBOR_H
#define _NEIGHBOR_H

/*
 * @pgdelay: Using range 0 to 100 to represent the propagation delay (pgdelay) (if neighbor list had been
 *           changed, the pgdelay which peer ever had need to accumulate with current one).
 *
 * NOTE: Spliting the range to 4 class-intervals so that it could be use for determining
 *       that peers are in same cluster or not (CLUSTER-BASED PEER SELECTION algorithm).
 * * */
struct Neighbors {
    int id;
    int pgdelay;
};

#endif // for #ifndef _NEIGHBOR_H
