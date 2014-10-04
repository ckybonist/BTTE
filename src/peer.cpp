#include "error.h"
#include "random.h"
#include "piece.h"
#include "peer.h"

Peer *g_peers = nullptr;

Peer::Peer(const int k_id,
           const float k_time_per_piece,
           Neighbor* neighbors,
           const int NUM_PIECE)
{
    in_swarm = true;
    is_seed = true;
    is_leech = false;

    this->id = k_id;
    cid = 0;

    pieces = MakePieces(NUM_PIECE);
    for(int i = 0; i < NUM_PIECE; i++) {
        pieces[i] = true;
    }
    this->time_per_piece = k_time_per_piece;

    this->neighbors = neighbors;  // TODO:  Until the peer selection have completed

    start_time = 0.0;
    end_time = 0.0;

    counts = 0;
}

Peer::Peer(const int k_id,
           const float k_time_per_piece,
           Neighbor* neighbors,
           const int NUM_PIECE,
           const double k_prob_leech)
{
    in_swarm = true;
    is_leech = true;
    is_seed = false;

    this->id = k_id;
    cid = 0;

    pieces = MakePieces(NUM_PIECE);

    for(int i = 0; i < NUM_PIECE; i++) {
        double prob_piece = uniformrand::Rand(RSC::PROB_PIECE) / (double)g_k_rand_max;
        //double prob_piece = uniformrand::Rand(2) / (double)g_k_rand_max;
        pieces[i] = (prob_piece < k_prob_leech);
    }
    this->time_per_piece = k_time_per_piece;

    this->neighbors = neighbors;  // TODO:  Until the peer selection have completed

    start_time = 0.0;
    end_time = 0.0;

    counts = 0;

}

// for normal peer joining
Peer::Peer(const int k_id,
           const int k_cid,
           const float k_time_per_piece,
           const float k_start_time,
           const int NUM_PIECE)
{
    in_swarm = true;

    this->id = k_id;
    this->cid = k_cid;

    is_seed = false;
    is_leech = false;

    this->time_per_piece = k_time_per_piece;
    this->pieces = MakePieces(NUM_PIECE);

    this->start_time = k_start_time;
    end_time = 0.0;

    counts = 0.0;
}
