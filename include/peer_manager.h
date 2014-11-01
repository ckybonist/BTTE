#ifndef _PEERMANAGER_H
#define _PEERMANAGER_H

#include <set>

#include "args.h"
#include "piece.h"
#include "peer_level.h"
#include "peer_selection.h"
#include "piece_selection.h"


using namespace btte_peer_selection;
using namespace btte_piece_selection;


class PeerManager
{
public:
    typedef std::set<int> IntSet;
    typedef IntSet::iterator IntSetIter;
    typedef std::vector<PieceReqMsg> PRMVec;

    PeerManager();
    PeerManager(Args* const args);
    ~PeerManager();

    //void NewPeer(const int id, const int cid, const float start_time) const;  // for peer_join event
    void NewPeer(const int id, const float start_time) const;  // for peer_join event

    typedef enum class InSwarmFlag
    {
        LEAVE,
        JOIN
    } ISF;

    void CheckInSwarm(const ISF isf, const int pid);

    void AllotNeighbors(const int self_pid) const;  // Peer Selection, for average peers
    PRMVec GetPieceReqMsgs(const int self_pid);
    //int GetReqPiece(const int self_pid) const;      // Piece Selection , for average peers

    void CreatePeers();

    // A rate (0.x) for extracting steady peers stat..
    // We get rid of head and tail's peers (0.x / 2 * NUM_PEERS), and
    // gather stat. info of middle peers.
    const float dummy_peers_rate = 0.1;

private:
    //void AllocPeersSpace();
    void DeployPeersLevel();
    void DeployClusterIDs();  // if use cluster-based
    void InitSeeds() const;
    void InitLeeches();
    void InitAbstractObj();

    Args *args_;  // don't use pointer

    IPeerSelect* obj_peerselect_;
    IPieceSelect* obj_pieceselect_;

    int* cluster_ids_;  // pre-init, assign to peer after

    float* packet_time_4_peers_;  // pre-init, assign to peer after

    IntSet in_swarm_set_;  // for peer selection
};

#endif // for #ifndef _PEERMANAGER_H
