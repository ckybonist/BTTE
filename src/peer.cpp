#include "error.h"
#include "random.h"
#include "piece.h"
#include "peer.h"

Peer *g_peers = nullptr;
//int g_last_join = 0;  // important !

Peer::Peer(const int id, const float time_per_piece, Neighbor *neighbors, const int NUM_PIECE) {
    in_swarm = true;
    is_seed = true;
    is_leech = false;

    this->id = id;
    cid = 0;

    pieces = MakePieces(NUM_PIECE);
    for(int i = 0; i < NUM_PIECE; i++) {
        pieces[i] = true;
    }
    this->time_per_piece = time_per_piece;

    this->neighbors = neighbors;  // TODO:  Until the peer selection have completed

    start_time = 0.0;
    end_time = 0.0;

    counts = 0;
}

Peer::Peer(const int id, const float time_per_piece,
           Neighbor *neighbors,
           const int NUM_PIECE, const double prob_leech) {
    in_swarm = true;
    is_leech = true;
    is_seed = false;

    this->id = id;
    cid = 0;

    pieces = MakePieces(NUM_PIECE);

    for(int i = 0; i < NUM_PIECE; i++) {
        double prob_piece = uniformdist::rand(rsc_prob_piece) / (double)g_k_rand_max;
        //double prob_piece = uniformdist::rand(2) / (double)g_k_rand_max;
        pieces[i] = (prob_piece < prob_leech);
    }
    this->time_per_piece = time_per_piece;

    this->neighbors = neighbors;  // TODO:  Until the peer selection have completed

    start_time = 0.0;
    end_time = 0.0;

    counts = 0;

}

// for normal peer joining
Peer::Peer(const int id, const int cid,
           const float time_per_piece, const float start_time,
           const int NUM_PIECE) {
    in_swarm = true;

    this->id = id;
    this->cid = cid;

    is_seed = false;
    is_leech = false;

    this->time_per_piece = time_per_piece;
    this->pieces = MakePieces(NUM_PIECE);

    this->start_time = start_time;
    end_time = 0.0;

    counts = 0.0;
}
