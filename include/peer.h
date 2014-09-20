#ifndef _PEER_H
#define _PEER_H

#include <map>

#include "piece.h"

struct Neighbor;  // forward declaration

struct Peer {
    Peer() {};
    Peer(const int id, float time_per_piece, Neighbor *neighbors, const int NUM_PIECE);  // seed

    Peer(const int id, float time_per_piece, Neighbor *neighbors,
         const int NUM_PIECE, double prob_leech);  // leech

    Peer(const int id, const int cid,
         const float time_per_piece, const float start_time,
         const int NUM_PIECE);  // average peer


    int id;
    int cid;              // cluster id :: { 1, 2, 3, 4 }
    bool is_seed;
    bool is_leech;
    bool in_swarm;

    bool *pieces;         // size is in config file, dynamic array
    float time_per_piece; // time that spends for each piece

    //TODO: must be destroyed somewhere
    Neighbor *neighbors;  // neighbor list size is in config file, dynamic array

    float start_time;     // start time of peer run the routine
    float end_time;       // end time of all pieces have been downloaded

    int counts;           // counts of being selected as an candidate in other peers' neighbor lists

};

extern Peer *g_peers;     // sort by time-order
//extern int g_last_join; // index for acceding next peer into the swarm

#endif // for #ifndef _PEER_H
