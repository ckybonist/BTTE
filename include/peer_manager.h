#ifndef _PEERMANAGER_H
#define _PEERMANAGER_H

#include "args.h"
#include "peer_selection.h"


using namespace peerselection;


class PeerManager
{
public:
    PeerManager();
    PeerManager(Args* const args);
    ~PeerManager();

    void Debug();

    // CreatePeers() does two things
    //   1. allocate memory-spaces of all peers
    //   2. allot each peer one level(see peer_level.h) and
    //      init some peers as seeds and leeches
    void CreatePeers();
    void NewPeer(const int id, const int cid, const float start_time) const;  // for peer_join event
    void AllotNeighbors(const int peer_id) const;  // for average peers

    // A rate (0.x) for extracting steady peers stat..
    // We get rid of head and tail's peers (0.x / 2 * NUM_PEERS), and
    // gather stat. info of middle peers.
    const float dummy_peers_rate = 0.1;

private:
    void AllocAllPeersSpaces();
    void AllotAllPeersLevel();
    void InitSeeds() const;
    void InitLeeches() const;

    Args *args_;  // don't use pointer

    float* packet_time_4_peers_;  // tt : transmission time

    IPeerSelect* type_peerselect_;
};

#endif // for #ifndef _PEERMANAGER_H
