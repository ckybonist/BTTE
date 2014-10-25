#ifndef _CB_PEER_SELECTION_H
#define _CB_PEER_SELECTION_H

#include "peer_selection.h"

namespace btte_peer_selection
{

class ClusterBased : public IPeerSelect
{
public:
    ClusterBased(Args args) : IPeerSelect(args) {};
    ~ClusterBased() {};

private:
    Neighbor* SelectNeighbors(const int self_pid, const iSet& in_swarm_set) override;

    float ComputePGDelayByCluster(const int self_cid, const int cand_cid);
    void RefreshInfo();
};

}

#endif // for #ifndef _CB_PEER_SELECTION_H
