#ifndef _PEERMANAGER_H
#define _PEERMANAGER_H

#include "Peer.h"
#include "Piece.h"

class PeerManager
{
public:
    void createPeers () const;
    Peer_t& selectPeers () const;

private:
    void allotTransTime(Peer_t &, int) const;
    void makeSeeds(Peer_t&, int) const;
    void makeLeechs(Peer_t&, int) const;
    void randSelect();
};

#endif // for #ifndef _PEERMANAGER_H
