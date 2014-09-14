#ifndef _PEER_H
#define _PEER_H

#include "piece.h"

struct PeerList;  // forward declaration

struct Peer {
    int id;
    int cid;          // cluster id
    bool is_seed;
    bool is_leech;
    bool in_swarm;

    Piece *pieces;
    float time_per_piece; // time that spends for each piece

    //int *neighbors;  // peer list
    PeerList *neighbors;  // peer list

    //float *pgdelay;  // propagation delay
    float time;      // time for getting all pieces
    int counter;     // count of being selected as candidate in other peers' peer lists

    Peer();
};

extern Peer *g_peers;

#endif // for #ifndef _PEER_H
