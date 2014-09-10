#ifndef _PEERMANAGER_H
#define _PEERMANAGER_H

#include "peer_manager.h"
#include "algorithm/peer_selection.h"

class IPeerSelection;

class PeerManager
{
public:
    PeerManager() {};
    //PeerManager(const int, const int ,const int, const int);
    void InitArgs(const int, const int ,const int, const int);
    void CreatePeers() const;  // init usage
    void NewPeer(int id) const;  // for peer_join event
    void SetNeighbors(IPeerSelection&) const;
    void DestroyPeers();

private:
    void AllotTransTime() const;
    void InitSeeds() const;
    void InitLeeches() const;
    int NUM_PEER;
    int NUM_SEED;
    int NUM_LEECH;
    int NUM_PIECE;
};

#endif // for #ifndef _PEERMANAGER_H
