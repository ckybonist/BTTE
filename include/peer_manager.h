#ifndef _PEERMANAGER_H
#define _PEERMANAGER_H

#include "args.h"
#include "peer_selection.h"


using namespace peerselection;


class PeerManager
{
public:
    PeerManager(const Args& args);
    ~PeerManager();

    // CreatePeers() does two things
    //   1. allocate memory-spaces of all peers
    //   2. allot each peer one level(see peer_level.h) and
    //      init some peers as seeds and leeches
    void CreatePeers();
    void NewPeer(const int k_id, const int k_cid, const float k_start_time) const;  // for peer_join event
    void AllotNeighbors(const int k_peer_id) const;  // for average peers

private:
    void AllocAllPeersSpaces();
    void AllotPeerLevel();
    void InitSeeds() const;
    void InitLeeches() const;

    Args args_;  // don't use pointer
    float* time_per_pieces_;
    IPeerSelect* type_peerselect_;
};

#endif // for #ifndef _PEERMANAGER_H
