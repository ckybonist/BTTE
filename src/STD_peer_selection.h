#ifndef _STD_PEER_SELECTION_H
#define _STD_PEER_SELECTION_H

#include "peer_selection.h"


namespace btte_peer_selection
{

class Standard : public IPeerSelect
{
public:
    Standard() : IPeerSelect(),
                 pg_delays_(nullptr) {};
    ~Standard();

private:
    //Neighbor* StartSelection(const int client_pid, const IntSet& in_swarm_set) override;
    NeighborMap StartSelection(const int client_pid, const IntSet& in_swarm_set) override;
    void AssignNeighbors(NeighborMap& neighbors, const size_t cand_size);
    void RefreshInfo();

    float* pg_delays_;
};

}

#endif // for #ifndef _STD_PEER_SELECTION_H
