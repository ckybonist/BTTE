#ifndef _PEERMANAGER_H
#define _PEERMANAGER_H

#include <set>

#include "algorithm/peer_selection.h"
#include "algorithm/piece_selection.h"

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
    ~PeerManager();

    bool CheckAllPiecesGet(const int pid) const;
    void UpdateSwarmInfo(const ISF isf, const int pid);
    void NewPeer(const int pid);  // for peer_join event
    void AllotNeighbors(const int client_pid) const;      // Peer Selection
    MsgList GenrAllPieceReqs(const int client_pid);       // Piece Selection
    PieceMsg ReGenrPieceReq(const int piece_no,
                            const int client_pid);        // for req-timeout
    void CreatePeers();

  private:
    void NewPeerData(Peer::Type type,
                     const int pid,
                     double prob_leech = 0.1);
    void DeployPeersLevel();
    void DeployClusterIDs();  //cluster-based usage
    void InitSeeds();
    void InitLeeches();
    void InitAbstractObj();

    IPeerSelection* obj_peerselect_;
    IPieceSelection* obj_pieceselect_;

    int* reserved_cids_;         // pre-init
    int* reserved_peer_levels_;  // pre-init

    // A rate (0.x) for extracting steady peers stat..
    // We get rid of head and tail's peers (0.x / 2 * NUM_PEERS),
    // and gather stat info of middle peers.
    const float dummy_peers_rate = 0.1;

};

#endif // for #ifndef _PEERMANAGER_H
