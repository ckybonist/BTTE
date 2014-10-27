#ifndef _LB_PEER_SELECTION_H
#define _LB_PEER_SELECTION_H

#include "peer_selection.h"


namespace btte_peer_selection
{


class LoadBalancing : public IPeerSelect
{
public:
    LoadBalancing(Args args) : IPeerSelect(args),
                               pg_delays_(nullptr) {};
    ~LoadBalancing();

private:
    struct NBCountsInfo  // neighbor counts info
    {
        int pid;
        int counts;
    };

    Neighbor* SelectNeighbors(const int self_pid,
                              const iSet& in_swarm_set) override;

    void AllocNBCInfo(const size_t N);
    void GatherNeighborCounts(const size_t cand_size);
    void SortCountsInfo(const size_t cand_size);

    void AssignNeighbors(Neighbor* const neighbors,
                         const size_t cand_size,
                         const int self_pid);
    void RefreshInfo();

    NBCountsInfo* nbc_info_;
    float* pg_delays_;
};

}

#endif // for #ifndef _LB_PEER_SELECTION_H
