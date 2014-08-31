#ifndef _PEERMANAGER_H
#define _PEERMANAGER_H

#include "Peer.h"
#include "Piece.h"

/**
 * Simulation arguments
 *
 * Piece:
 *     @piece_size: 512 (Kbytes)
 *
 * Category of peers:
 *	   @class1 : 10 (Mbps)   // seeder
 *	   @class2 : 128 (Kbps)  // leecher or normal peer
 *	   @class3 : 56 (Kbps)   // leecher or normal peer
 *
 * */

/* Download speed of each class */
const int DOWNSPEED_1 = 10 * 1024 * 1024;
const int DOWNSPEED_2 = 128 * 1024;
const int DOWNSPEED_3 = 56 * 1024;

/* Distributed rate(%) of each class */
const int DRATE[3] = { 30, 50, 20 };

const float TRANS_TIME[3]  // each class' transmission time
{
	(float)PIECE_SIZE / (float)DOWNSPEED_1,
	(float)PIECE_SIZE / (float)DOWNSPEED_2,
	(float)PIECE_SIZE / (float)DOWNSPEED_3,
};


class PeerManager
{
public:
    PeerManager(const int, const int ,const int);
    void createPeers () const;
    Peer_t& selectPeers () const;

private:
    void allotTransTime(Peer_t&, int) const;
    void makeSeeds(Peer_t&, int) const;
    void makeLeechs(Peer_t&, int) const;
    void randSelect();

    int NUM_PEER;
    int NUM_SEED;
    int NUM_LEECH;
};

#endif // for #ifndef _PEERMANAGER_H
