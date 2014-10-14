#ifndef _PEER_H
#define _PEER_H

#include <vector>

#include "piece.h"
#include "neighbor.h"


class Peer
{
public:
    Peer();

    // seed
    Peer(const int pid,
         const int cid,
         const float time_packet,
         const int NUM_PIECE);

    // leech
    Peer(const int pid,
         const float time_packet,
         const int NUM_PIECE,
         const double prob_leech);

    // average peer
    Peer(const int pid,
         const int cid,
         const float join_time,
         const float time_packet,
         const int NUM_PIECE);

    int pid;
    int cid;              // cluster id :: { 1, 2, 3, 4 }
    bool is_seed;
    bool is_leech;
    bool in_swarm;

    bool* pieces;
    double time_packet; // download time of each piece

    const Neighbor* neighbors;

    float join_time;     // start time of peer run the routine
    float end_time;       // end time of all pieces have been downloaded

    int counts;           // counts of being selected as an candidate in other peers' neighbor lists
};

//extern Peer* g_peers;     // sort by time-order
extern std::vector<Peer> g_peers;
extern bool* g_in_swarm_set;

#endif // for #ifndef _PEER_H
