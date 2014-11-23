#ifndef _ABS_PEER_SELECTION_H
#define _ABS_PEER_SELECTION_H

#include <set>

#include "args.h"
#include "peer.h"
#include "random.h"
#include "neighbor.h"


namespace btte_peer_selection
{

typedef std::set<int> IntSet;
typedef IntSet::iterator IntSetIter;


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
    virtual Neighbor* StartSelection(const int client_pid, const IntSet& in_swarm_set) = 0;

protected:
    Neighbor* AllocNeighbors();

    //bool IsNewNeighbor(const int cand_pid);
    //void RecordPGDelay(const int cand_pid, const float pg_delay);
    //float QueryPGDelay(const int cand_pid);

    IntSet ExcludeSelf(const IntSet& in_swarm_set);
    size_t SetCandidates(const IntSet& in_swarm_set, bool sort_cid_flag);  // find candidates for neighbors

    Args args_;
    int selector_pid_;
    int* candidates_;
};

}
#endif // for _ABS_PEER_SELECTION_H
