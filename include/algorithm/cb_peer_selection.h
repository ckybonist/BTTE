#ifndef _CB_PEER_SELECTION_H
#define _CB_PEER_SELECTION_H

#include "abs_peer_selection.h"

struct Peer;

/*
 * TODO:
 * Method of cluster decision :
 *     1. range 0 ~ 100 will split to 4 segments/clusters (1, 33, 66, 100), each have NUM_PEER/4 peers
 *     2. for (i = 0; i < 4; i++) {
 *          for(j = 0; j < NUM_PEER/4; j++) {
 *               peer_id = RangeRand((NUM_SEED + NUM_LEECH), (NUM_PEER - 1);
 *               if(0 == g_peers[peer_id].cid) {
 *                   pgdelay = rand();
 *                   if(pgdelay < 33) {
 *                      g_peers[peer_id].cid = i;
 *                   } else {
 *                     tmp_cid = "rand number 0 ~ 4 exclude current i"
 *                     g_peers[peer_id].cid = tmp_cid;
 *                   }
 *
 *               }
 *          }
 *     }
 *     if pgdelay of the peer was within first segment (1 ~ 32), then means
 *
 * * * * * * * */

class ClusterBased : protected IPeerSelection
{
public:
    Neighbor* ChoosePeers() override;

private:
    void setCluster_(Peer &peers);
};

#endif // for #ifndef _CB_PEER_SELECTION_H
