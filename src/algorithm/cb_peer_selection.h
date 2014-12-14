#ifndef _CB_PEER_SELECTION_H
#define _CB_PEER_SELECTION_H

#include "peer_selection.h"

namespace btte_peer_selection
{

class ClusterBasedRule : public IPeerSelection
{
  public:
    ClusterBasedRule() : IPeerSelection() {};
    ~ClusterBasedRule() {};

  private:
    NeighborMap StartSelection(const int client_pid,
                               const IntSet& in_swarm_set) override;

    float ComputePGDelayByCluster(const int self_cid,
                                  const int cand_cid);

    void AddNeighborData(NeighborMap& neighbors,
                         const int pid,
                         const int client_cid);

    void AssignNeighbors(NeighborMap& neighbors,
                         const size_t cand_size,
                         const int self_cid);
    void RefreshInfo();
};

}

#endif // for #ifndef _CB_PEER_SELECTION_H
