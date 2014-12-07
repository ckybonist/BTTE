#ifndef _LB_PEER_SELECTION_H
#define _LB_PEER_SELECTION_H

#include "peer_selection.h"


namespace btte_peer_selection
{


class LoadBalancingRule : public IPeerSelection
{
  public:
    LoadBalancingRule() : IPeerSelection(),
                      pg_delays_(nullptr) {};
    ~LoadBalancingRule();

  private:
    struct NBCountsInfo  // neighbor counts info
    {
        int pid;
        int counts;
    };

    NeighborMap StartSelection(const int client_pid,
                               const IntSet& in_swarm_set) override;

    void AllocNBCInfo(const size_t N);
    void GatherNeighborCounts(const size_t cand_size);
    void SortCountsInfo(const size_t cand_size);
    void AssignNeighbors(NeighborMap& neighbors, const size_t cand_size);
    void RefreshInfo();

    NBCountsInfo* nbc_info_;
    float* pg_delays_;
};

}

#endif // for #ifndef _LB_PEER_SELECTION_H
