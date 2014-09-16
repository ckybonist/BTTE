#include <map>

#include "error.h"
#include "piece.h"
#include "peer.h"

Peer::Peer() {
    id = 0;
    cid = 0;

    is_seed = false;
    is_leech = false;
    in_swarm = false;

    pieces = nullptr;
    time_per_piece = 0.0;

    neighbors = nullptr;

    start_time = 0.0;
    end_time = 0.0;

    counts = 0.0;
}

// for normal peer joining
Peer::Peer(const int id, const int NUM_PIECE, const float start_time) {
    this->id = id;
    this->pieces = MakePieces(NUM_PIECE);
    this->start_time = start_time;
    in_swarm = true;
}

// for cluster based
Peer::Peer(const int id, const int cid, const int NUM_PIECE, const float start_time) {
    this->id = id;
    this->cid = cid;
    this->pieces = MakePieces(NUM_PIECE);
    this->start_time = start_time;
    in_swarm = true;
}
