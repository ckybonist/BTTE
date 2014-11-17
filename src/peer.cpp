#include "error.h"
#include "random.h"
#include "piece.h"
#include "peer.h"


//Peer *g_peers = nullptr;
std::vector<Peer> g_peers;

bool* g_in_swarm_set = nullptr;



Peer::Peer()
{
    in_swarm = false;
    pid = -1;
    cid = -1;
    pieces = nullptr;
    neighbors = nullptr;
    join_time = 0.0;
    end_time = 0.0;
    neighbor_counts = 0;
}

// seed
Peer::Peer(const int pid,
           const int cid,
           const int NUM_PIECE,
           const Bandwidth bw)
{
    in_swarm = true;
    type = SEED;
    this->pid = pid;
    this->cid = cid;

    pieces = MakePieces(NUM_PIECE);
    for (int i = 0; i < NUM_PIECE; i++)
        pieces[i] = true;

    neighbors = nullptr;

    this->bw = bw;

    join_time = 0.0;
    end_time = 0.0;

    neighbor_counts = 0;
}

// leech
Peer::Peer(const int pid,
           const int cid,
           const int NUM_PIECE,
           const double prob_leech,
           const Bandwidth bw)
{
    in_swarm = true;
    type = LEECH;
    this->pid = pid;
    this->cid = cid;

    pieces = MakePieces(NUM_PIECE);
    for (int i = 0; i < NUM_PIECE; i++)
    {
        double prob_piece = uniformrand::Rand(RSC::PROB_PIECE) / (double)RAND_MAX;
        pieces[i] = (prob_piece < prob_leech);
    }

    neighbors = nullptr;

    this->bw = bw;

    join_time = 0.0;
    end_time = 0.0;

    neighbor_counts = 0;
}


// average peer
Peer::Peer(const int pid,
           const int cid,
           const int NUM_PIECE,
           const float join_time,
           const Bandwidth bw)
{
    in_swarm = true;
    type = NORMAL;
    this->pid = pid;
    this->cid = cid;

    pieces = MakePieces(NUM_PIECE);
    neighbors = nullptr;

    this->bw = bw;

    this->join_time = join_time;
    end_time = 0.0;
    neighbor_counts = 0;
}
