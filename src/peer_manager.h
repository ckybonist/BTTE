#ifndef _PEERMANAGER_H
#define _PEERMANAGER_H

#include <set>

#include "peer_selection.h"
#include "piece_selection.h"

using namespace btte_peer_selection;
using namespace btte_piece_selection;

class PeerManager
{
public:
    typedef std::set<int> IntSet;
    typedef IntSet::iterator IntSetIter;
    typedef enum class InSwarmFlag
    {
        LEAVE,
        JOIN
    } ISF;


    PeerManager();
    PeerManager(Args* const args);
    ~PeerManager();

    void NewPeer(const int pid) const;  // for peer_join event

    void UpdateSwarmInfo(const ISF isf, const int pid);
    bool CheckAllPiecesGet(const int pid) const;

    void AllotNeighbors(const int client_pid) const;  // Peer Selection, for average peers
    std::deque<PieceMsg> GetPieceReqMsgs(const int client_pid);     // get the result of Piece Selection

    void CreatePeers();

    // A rate (0.x) for extracting steady peers stat..
    // We get rid of head and tail's peers (0.x / 2 * NUM_PEERS),
    // and gather stat info of middle peers.
    const float dummy_peers_rate = 0.1;

private:
    //void AllocPeersSpace();
    void NewPeerData(Peer::Type type,
                     const int pid,
                     //const float join_time,
                     double prob_leech = 0.1) const;
    void DeployPeersLevel();
    void DeployClusterIDs();  // if use cluster-based
    void InitSeeds() const;
    void InitLeeches();
    void InitAbstractObj();

    Args *args_;

    IPeerSelect* obj_peerselect_;
    IPieceSelect* obj_pieceselect_;

    int* reserved_cids_;  // pre-init, assign to peer after
    int* reserved_peer_levels_;  // pre-init

    IntSet in_swarm_set_;  // for peer selection
};

#endif // for #ifndef _PEERMANAGER_H
