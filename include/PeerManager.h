#ifndef _PEERMANAGER_H
#define _PEERMANAGER_H

#include "Peer.h"
#include "algorithm/PeerSelection.h"

class IPeerSelection;

class PeerManager
{
public:
    PeerManager(const int, const int ,const int, const int);
    void createPeers() const;  // init usage
    void newPeer(Peer_t &peer, int id) const;  // for peer_join event
    void setNeighbors(IPeerSelection&) const;
    void destroyPeers(Peer_t*);

private:
    void allotTransTime() const;
    void initSeeds() const;
    void initLeeches() const;
    int NUM_PEER;
    int NUM_SEED;
    int NUM_LEECH;
    int NUM_PIECE;
};

#endif // for #ifndef _PEERMANAGER_H
