#include "error.h"
#include "random.h"
#include "piece.h"
#include "peer.h"


//Peer *g_peers = nullptr;
std::vector<Peer> g_peers;
bool* g_in_swarm_set = nullptr;

Peer::Peer()
{
    is_seed = false;
    is_leech = false;
    in_swarm = false;
    pid = -1;
    cid = -1;

    pieces = nullptr;

    neighbors = nullptr;

    time_packet = 0.0;
    join_time = 0.0;
    end_time = 0.0;
    counts = 0;
}

// seed
Peer::Peer(const int pid,
           const int cid,
           const float time_packet,
           const int NUM_PIECE)
{
    in_swarm = true;
    is_seed = true;
    is_leech = false;

    this->pid = pid;
    this->cid = cid;

    pieces = MakePieces(NUM_PIECE);
    for (int i = 0; i < NUM_PIECE; i++)
    {
        pieces[i] = true;
    }
    neighbors = nullptr;

    this->time_packet = time_packet;
    join_time = 0.0;
    end_time = 0.0;

    counts = 0;
}

// leech
Peer::Peer(const int pid,
           const int cid,
           const float time_packet,
           const int NUM_PIECE,
           const double prob_leech)
{
    in_swarm = true;
    is_leech = true;
    is_seed = false;

    this->pid = pid;
    this->cid = cid;

    pieces = MakePieces(NUM_PIECE);

    for (int i = 0; i < NUM_PIECE; i++)
    {
        double prob_piece = uniformrand::Rand(RSC::PROB_PIECE) / (double)RAND_MAX;
        pieces[i] = (prob_piece < prob_leech);
    }

    neighbors = nullptr;

    this->time_packet = time_packet;
    join_time = 0.0;
    end_time = 0.0;

    counts = 0;
}


// average peer
Peer::Peer(const int pid,
           const int cid,
           const float join_time,
           const float time_packet,
           const int NUM_PIECE)
{
    in_swarm = true;
    is_seed = false;
    is_leech = false;

    this->pid = pid;
    this->cid = cid;

    pieces = MakePieces(NUM_PIECE);

    neighbors = nullptr;

    this->time_packet = time_packet;
    this->join_time = join_time;
    end_time = 0.0;
    counts = 0;
}
