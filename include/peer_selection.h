#ifndef _ABS_PEER_SELECTION_H
#define _ABS_PEER_SELECTION_H

#include <vector>

#include "args.h"
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
    IPeerSelect(Args args);
    virtual ~IPeerSelect() {};
    virtual Neighbor* SelectNeighbors(const int self_pid) = 0;

protected:
    Args args_;
    int* ids_;
    int* pg_delays_;
};

class Standard : public IPeerSelect
{
public:
    Standard(Args args) : IPeerSelect(args) {};
    ~Standard();

private:
    Neighbor* SelectNeighbors(const int self_pid) override;
};

class LoadBalancing : public IPeerSelect
{
public:
    LoadBalancing(Args args) : IPeerSelect(args) {};
    ~LoadBalancing() {};

private:
    Neighbor* SelectNeighbors(const int self_pid) override;
};

/*
 * TODO:
 * Method of cluster decision :
 *     1. range 0 ~ 100 will split to 4 segments/clusters (1, 33, 66, 100), each have NUM_PEER/4 peers
 *     2. for (i = 0; i < 4; i++) {
 *          for(j = 0; j < NUM_PEER / 4; j++) {
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
    ClusterBased(Args args) : IPeerSelect(args) {};
    ~ClusterBased() {};

private:
    Neighbor* SelectNeighbors(const int self_pid) override;
    void setCluster(Peer &peers);
};

}
#endif // for _ABS_PEER_SELECTION_H
