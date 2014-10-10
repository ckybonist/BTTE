#include "error.h"
#include "random.h"
#include "piece.h"
#include "peer.h"

Peer *g_peers = nullptr;

Peer::Peer(const int id,
           const float time_packet,
           Neighbor* neighbors,
           const int NUM_PIECE)
{
    in_swarm = true;
    is_seed = true;
    is_leech = false;

    this->id = id;
    cid = 0;

    pieces = MakePieces(NUM_PIECE);
    for(int i = 0; i < NUM_PIECE; i++) {
        pieces[i] = true;
    }

    this->time_packet = time_packet;

    this->neighbors = neighbors;  // TODO:  Until the peer selection have completed

    join_time = 0.0;
    end_time = 0.0;

    counts = 0;
}

Peer::Peer(const int id,
           const float time_packet,
           Neighbor* neighbors,
           const int NUM_PIECE,
           const double prob_leech)
{
    in_swarm = true;
    is_leech = true;
    is_seed = false;

    this->id = id;
    cid = 0;

    pieces = MakePieces(NUM_PIECE);

    for(int i = 0; i < NUM_PIECE; i++) {
        double prob_piece = uniformrand::Rand(RSC::PROB_PIECE) / (double)RAND_MAX;
        pieces[i] = (prob_piece < prob_leech);
    }

    this->time_packet = time_packet;

    this->neighbors = neighbors;  // TODO:  Until the peer selection have completed

    join_time = 0.0;
    end_time = 0.0;

    counts = 0;

}

// for normal peer joining
Peer::Peer(const int id,
           const int cid,
           const float time_packet,
           const float join_time,
           const int NUM_PIECE)
{
    in_swarm = true;

    this->id = id;
    this->cid = cid;

    is_seed = false;
    is_leech = false;

    this->time_packet = time_packet;
    this->pieces = MakePieces(NUM_PIECE);

    this->join_time = join_time;
    end_time = 0.0;

    counts = 0.0;
}
