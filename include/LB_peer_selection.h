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
    ~LoadBalancing() {};

private:
    Neighbor* SelectNeighbors(const int self_pid, const iSet& in_swarm_set) override;
    float* pg_delays_;
};

}

#endif // for #ifndef _LB_PEER_SELECTION_H
