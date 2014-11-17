#ifndef _PEER_H
#define _PEER_H

#include <map>
#include <list>
#include <vector>

#include "piece.h"
#include "neighbor.h"
#include "peer_level.h"


enum PeerType
{
    SEED,
    LEECH,
    NORMAL,
};

class Peer
{
public:
    Peer();

    // seed
    Peer(const int pid,
         const int cid,
         const int NUM_PIECE,
         const Bandwidth bw);

    // leech
    Peer(const int pid,
         const int cid,
         const int NUM_PIECE,
         const double prob_leech,
         const Bandwidth bw);

    // average peer
    Peer(const int pid,
         const int cid,
         const int NUM_PIECE,
         const float join_time,
         const Bandwidth bw);

    int pid;
    int cid;  // start from 1

    PeerType type;
    bool in_swarm;

    bool* pieces;

    // 記錄上一次要求的 piece，如果這個要求沒有被 choking 而且尚未收到，
    // 就不要選擇這個 piece 去要求
    std::list<PieceMsg> send_msgs;
    std::list<PieceMsg> recv_msgs;

    const Neighbor* neighbors;

    Bandwidth bw;

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
