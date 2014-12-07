#ifndef _ABS_PEER_SELECTION_H
#define _ABS_PEER_SELECTION_H

#include <set>

#include "../args.h"
#include "../error.h"
#include "../peer.h"
#include "../random.h"
#include "../neighbor.h"


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


class IPeerSelection
{
  public:
    IPeerSelection();
    virtual ~IPeerSelection();  // ensuring destructor of derived class will be invoked
    virtual NeighborMap StartSelection(const int client_pid, const IntSet& in_swarm_set) = 0;

  protected:
    Neighbor* AllocNeighbors();
    void DebugInfo(NeighborMap const& neighbors, const int client_pid) const;

    IntSet ExcludeSelf(const IntSet& in_swarm_set);
    size_t SetCandidates(const IntSet& in_swarm_set, const RSC rsc, bool sort_cid_flag);  // find candidates for neighbors

    int selector_pid_;
    int* candidates_;
};

}
#endif // for _ABS_PEER_SELECTION_H
