#ifndef _PEER_H
#define _PEER_H

#include <iostream>
#include <string>

struct Piece_t
{
    std::string no;
    bool received;
};

struct Peer_t
{
    std::string id;
    std::string cid;         // cluster id
    int time_piece;          // time that spends for each piece

    bool is_seeder;
    bool in_swarm;

    std::string *neighbors;  // peer list
    Piece_t *pieces;

    int *pdelay;             // propagation delay
    int time;                // time for getting all pieces
    int count;               // count of being selected as other peers' candidate of its peer list
};

extern Peer_t *peers;

#endif // for #ifndef _PEER_H
