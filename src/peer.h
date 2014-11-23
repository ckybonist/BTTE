#ifndef _PEER_H
#define _PEER_H

#include <set>
#include <deque>
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

public:
    bool in_swarm;
    int pid;
    int cid;  // start from 1
    PeerType type;

    bool* pieces;
    Neighbor* neighbors;
    int neighbor_counts;  // counts of peer being served(selected) as neighbor
    int unchoke_counts;

    // 記錄上一次要求的 piece，如果這個要求沒有被 choking 而且尚未收到，
    // 就不要選擇這個 piece 去要求
    std::set<int> pieces_on_req;
    std::deque<PieceMsg> recv_msg_buf;

    Bandwidth bandwidth;

    float join_time;     // start time of peer run the routine
    float leave_time;      // end time of all pieces have been downloaded
};

//extern Peer* g_peers;
extern std::vector<Peer> g_peers;
extern bool* g_in_swarm_set;

#endif // for #ifndef _PEER_H
