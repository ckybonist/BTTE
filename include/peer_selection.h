#ifndef _ABS_PEER_SELECTION_H
#define _ABS_PEER_SELECTION_H

#include <set>

#include "args.h"
#include "peer.h"
#include "random.h"
#include "neighbor.h"


namespace btte_peer_selection
{

typedef std::set<int> iSet;
typedef iSet::iterator iSetIter;


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
    virtual ~IPeerSelect();  // ensuring destructor of derived class will be invoked
    virtual Neighbor* SelectNeighbors(const int self_pid, const iSet& in_swarm_set) = 0;

protected:
    Neighbor* AllocNeighbors();
    size_t SetCandidates(const int self_pid,
                         const iSet& in_swarm_set,
                         bool sort_cid_flag);  // find candidates for neighbors
    Args args_;
    int* candidates_;
};

class Standard;

class LoadBalancing;

class ClusterBased;

}
#endif // for _ABS_PEER_SELECTION_H
