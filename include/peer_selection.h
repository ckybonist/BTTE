#ifndef _ABS_PEER_SELECTION_H
#define _ABS_PEER_SELECTION_H

#include "neighbor.h"


struct Peer;

namespace peerselection
{

typedef enum class TypePeerSelect
{
    STANDARD = 0,
    LOAD_BALANCING,
    CLUSTER_BASED
} PeerSelect_T;


class IPeerSelect
{
public:
    IPeerSelect(const int NUM_PEERLIST, const int NUM_PEER);
    virtual ~IPeerSelect() {};
    virtual Neighbor* SelectNeighbors() = 0;

protected:
    int _NUM_PEERLIST;
    int _NUM_PEER;
    int* _ids;
    int* _pg_delays;
};

class Standard : public IPeerSelect
{
public:
    Standard(const int NUM_PEERLIST,
             const int NUM_PEER) : IPeerSelect(NUM_PEERLIST, NUM_PEER) {};
    ~Standard();

private:
    Neighbor* SelectNeighbors() override;
};

class LoadBalancing : public IPeerSelect
{
public:
    LoadBalancing(const int NUM_PEERLIST,
                  const int NUM_PEER) : IPeerSelect(NUM_PEERLIST, NUM_PEER) {};
    ~LoadBalancing() {};

private:
    Neighbor* SelectNeighbors() override;
};

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

class ClusterBased : public IPeerSelect
{
public:
    ClusterBased(const int NUM_PEERLIST,
                 const int NUM_PEER) : IPeerSelect(NUM_PEERLIST, NUM_PEER) {};

    ~ClusterBased() {};

private:
    Neighbor* SelectNeighbors() override;
    void _setCluster(Peer &peers);
};

}
#endif // for _ABS_PEER_SELECTION_H
