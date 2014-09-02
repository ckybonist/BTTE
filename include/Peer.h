#ifndef _PEER_H
#define _PEER_H

#include <iostream>
#include <string>

#include "Piece.h"

struct Peer_t
{
    std::string id;
    std::string cid;         // cluster id
    float time_piece;          // time that spends for each piece

    bool seed;
    bool in_swarm;

    std::string *neighbors;  // peer list
    Piece_t *pieces;

    float *pgdelay;             // propagation delay
    float time;                // time for getting all pieces
    int count;               // count of being selected as other peers' candidate of its peer list

    Peer_t()
    {
        id = "";
        cid = "";  // not yet
        time_piece = 0.0;

        seed = false;
        in_swarm = false;

        neighbors = nullptr;
        pieces = nullptr;

        pgdelay = nullptr;
        time = 0.0;
        count = 0;
    }
};

extern Peer_t *peers;

#endif // for #ifndef _PEER_H
