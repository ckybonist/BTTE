#ifndef _PEER_H
#define _PEER_H

#include <map>

#include "piece.h"
#include "neighbor.h"


struct Peer
{
    Peer() {};
    Peer(const int k_id,
         const float k_time_per_piece,
         Neighbor *neighbors,
         const int NUM_PIECE);  // seed

    Peer(const int k_id,
         const float k_time_per_piece,
         Neighbor *neighbors,
         const int NUM_PIECE,
         const double k_prob_leech);  // leech

    Peer(const int k_id,
         const int k_cid,
         const float k_time_per_piece,
         const float k_start_time,
         const int NUM_PIECE);  // average peer


    int id;
    int cid;              // cluster id :: { 1, 2, 3, 4 }
    bool is_seed;
    bool is_leech;
    bool in_swarm;

    bool* pieces;
    float time_per_piece; // download time of each piece

    Neighbor* neighbors;

    float start_time;     // start time of peer run the routine
    float end_time;       // end time of all pieces have been downloaded

    int counts;           // counts of being selected as an candidate in other peers' neighbor lists

};

extern Peer* g_peers;     // sort by time-order

#endif // for #ifndef _PEER_H
