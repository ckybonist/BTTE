#ifndef _PEER_H
#define _PEER_H

#include <iostream>
#include <string>

#include "Piece.h"

struct Peer {
    int id;
    int cid;          // cluster id
    float time_per_piece; // time that spends for each piece

    bool is_seed;
    bool in_swarm;

    Piece *pieces;

    int *neighbors;  // peer list

    float *pgdelay;  // propagation delay
    float time;      // time for getting all pieces
    int counter;     // count of being selected as candidate in other peers' peer lists

    Peer() {
        id = 0;
        cid = 0;  // not yet
        time_per_piece = 0.0;

        is_seed = false;
        in_swarm = false;

        neighbors = nullptr;
        pieces = nullptr;

        pgdelay = nullptr;
        time = 0.0;
        counter = 0;
    }
};

extern Peer *g_peers;

#endif // for #ifndef _PEER_H
