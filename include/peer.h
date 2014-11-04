#ifndef _PEER_H
#define _PEER_H

#include <map>
#include <vector>

#include "piece.h"
#include "neighbor.h"


class Peer
{
public:
    Peer();

    // seed
    Peer(const int pid,
         const int cid,
         const float time_packet,
         const int NUM_PIECE);

    // leech
    Peer(const int pid,
         const int cid,
         const float time_packet,
         const int NUM_PIECE,
         const double prob_leech);

    // average peer
    Peer(const int pid,
         const int cid,
         const float join_time,
         const float time_packet,
         const int NUM_PIECE);

    int pid;
    int cid;  // start from 1

    bool is_seed;
    bool is_leech;
    bool in_swarm;


    bool* pieces;

    std::vector<PieceReqMsg> msg_queue;

    const Neighbor* neighbors;

    float time_packet; // download time of each piece

    // If pg_delay is steady (but need a large amount of space)
    //typedef std::map<int, float> Map_i2f;
    //Map_i2f pg_delay_records;

    float join_time;     // start time of peer run the routine
    float end_time;      // end time of all pieces have been downloaded

    // counts of being selected as an neighbor, this variable will use
    // in Load Balancing Peer Selection
    int neighbor_counts;  // counts of peer being served(selected) as neighbor

private:
    // TODO
};

//extern Peer* g_peers;
extern std::vector<Peer> g_peers;
extern bool* g_in_swarm_set;

#endif // for #ifndef _PEER_H
