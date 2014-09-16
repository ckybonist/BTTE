#ifndef _PEER_H
#define _PEER_H

#include "piece.h"

struct Neighbor;  // forward declaration

struct Peer {
    int id;           // different with index of struct array
    int cid;          // cluster id
    bool is_seed;
    bool is_leech;
    bool in_swarm;

    bool *pieces;  // size is in config file, dynamic array
    float time_per_piece; // time that spends for each piece

    Neighbor *neighbors;  // neighbor list size is in config file, dynamic array

    float start_time;    // start time of peer run the routine
    float end_time;      // end time of all pieces have been downloaded

    int counts;     // counts of being selected as an candidate in other peers' neighbor lists

    Peer();
};

extern Peer *g_peers;

#endif // for #ifndef _PEER_H
