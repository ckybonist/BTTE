#ifndef _PEERMANAGER_H
#define _PEERMANAGER_H

#include "args.h"
#include "peer_manager.h"
#include "algorithm/peer_selection.h"

class IPeerSelection;

class PeerManager
{
public:
    PeerManager(const Args&);
    ~PeerManager();
    void CreatePeers();  // 1. create peers memory-spaces  2. init seeds and leeches
    void NewPeer(const int id, const int cid, const float start_time) const;  // for peer_join event
    void SelectNeighbors(IPeerSelection&, const int) const;  // FIXME: argument too less
    void DestroyPeers();

private:
    Args args_;
    float *peers_bandwidth;
    void AllotPeerLevel_();
    void InitSeeds_() const;
    void InitLeeches_() const;
};

#endif // for #ifndef _PEERMANAGER_H
