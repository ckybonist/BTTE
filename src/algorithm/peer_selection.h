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
    CLUSTER_BASED,
    USER_DEFINED
} PeerSelect_t;


class IPeerSelection
{
  public:
    IPeerSelection();
    virtual ~IPeerSelection();  // ensuring destructor of derived class will be invoked
    virtual NeighborMap StartSelection(const int client_pid,
                                       const IntSet& in_swarm_set) = 0;

  protected:
    Neighbor* AllocNeighbors();
    void DebugInfo(NeighborMap const& neighbors,
                   const int client_pid) const;

    IntSet ExcludeSelf(const IntSet& in_swarm_set);
    size_t SetCandidates(const IntSet& in_swarm_set, const RSC rsc);  // find candidates for neighbors
    void RefreshCandidates();

    int selector_pid_;
    //int* candidates_;
    std::vector<int> candidates_;
    struct TmpPeer
    {
        TmpPeer(int, int, int);
        int pid;
        int cid;
        int nbcount;
    };
    std::vector<TmpPeer> orig_cands_;
};

}
#endif // for _ABS_PEER_SELECTION_H
