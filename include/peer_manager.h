#ifndef _PEERMANAGER_H
#define _PEERMANAGER_H

#include "args.h"
#include "algorithm/abs_peer_selection.h"

class IPeerSelection;

class PeerManager
{
public:
    PeerManager(const Args& args);
    ~PeerManager();
    void CreatePeers();  // 1. create peers memory-spaces  2. init seeds, leeches and allot each peer a level
    void NewPeer(const int k_id, const int k_cid, const float k_start_time) const;  // for peer_join event
    void SelectNeighbors(IPeerSelection& ips, const int k_peer_id) const;
    void DestroyPeers();

private:
    Args args_;
    float *peers_bandwidth;
    void AllotPeerLevel_();
    void InitSeeds_() const;
    void InitLeeches_() const;
};

#endif // for #ifndef _PEERMANAGER_H
