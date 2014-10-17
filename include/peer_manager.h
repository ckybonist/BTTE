#ifndef _PEERMANAGER_H
#define _PEERMANAGER_H

#include <set>

#include "args.h"
#include "peer_selection.h"


using namespace peerselection;


class PeerManager
{
public:
    PeerManager();
    PeerManager(Args* const args);
    ~PeerManager();

    void NewPeer(const int id, const int cid, const float start_time) const;  // for peer_join event

    enum class InSwarmFlag
    {
        LEAVE,
        JOIN
    };
    typedef InSwarmFlag ISF;
    void CheckInSwarm(const ISF isf, const int pid);

    void AllotNeighbors(const int peer_id) const;  // for average peers
    void CreatePeers();

    // A rate (0.x) for extracting steady peers stat..
    // We get rid of head and tail's peers (0.x / 2 * NUM_PEERS), and
    // gather stat. info of middle peers.
    const float dummy_peers_rate = 0.1;

private:
    void AllocAllPeersSpaces();
    void AllotAllPeersLevel();
    void InitSeeds() const;
    void InitLeeches();

    typedef std::set<int> iSet;
    typedef iSet::iterator iSetIter;
    iSet in_swarm_set_;

    Args *args_;  // don't use pointer

    float* packet_time_4_peers_;  // tt : transmission time

    IPeerSelect* type_peerselect_;
};

#endif // for #ifndef _PEERMANAGER_H
