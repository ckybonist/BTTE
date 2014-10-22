//#include <iostream>
#include "error.h"
//#include "peer.h"
//#include "pg_delay.h"
//#include "cluster_info.h"
#include "peer_selection.h"


using namespace uniformrand;


namespace btte_peer_selection
{

IPeerSelect::IPeerSelect(Args args)
{
    args_ = args;
    candidates_ = nullptr;
}

IPeerSelect::~IPeerSelect()
{
    if (candidates_ != nullptr)
    {
        delete [] candidates_;
        candidates_ = nullptr;
    }

    for(int pid = args_.NUM_SEED; (size_t)pid < args_.NUM_PEER; pid++)
    {
        delete [] g_peers[pid].neighbors;
        g_peers[pid].neighbors = nullptr;
    }
}

Neighbor* IPeerSelect::AllocNeighbors()
{
    Neighbor* neighbors = new Neighbor[args_.NUM_PEERLIST];

    if(neighbors == nullptr)
    {
        ExitError("Memory Allocation Fault in \
                   peerselection::Standard::SelectNeighbors()");
    }

    return neighbors;
}

// return size of candidates
// NOTE: if sort_cid_flag is true, then put peers that have
// same cid as g_peers[self_pid] at top of array
size_t IPeerSelect::SetCandidates(const int self_pid,
                                  const iSet& in_swarm_set,
                                  bool sort_cid_flag)
{
    /// Erase self first, then, shuffle the index and
    /// select other peers which in swarm.

    iSet in_swarm(in_swarm_set);
    in_swarm.erase(in_swarm.find(self_pid));

    candidates_ = new int[in_swarm.size()];
    if(nullptr == candidates_) ExitError("\nMemory Allocation Fault in SetCandidates\n");

    // for cluster-based
    if (sort_cid_flag)
    {
        iSet same_cluster_peers;
        iSet diff_cluster_peers;
        const int self_cid = g_peers[self_pid].cid;

        for(iSetIter pid = in_swarm.begin(); pid != in_swarm.end(); pid++)
        {
            if (g_peers[*pid].cid == self_cid)
            {
                same_cluster_peers.insert(*pid);
            }
            else
            {
                diff_cluster_peers.insert(*pid);
            }
        }

        std::cout << std::flush;
        // assign peers with same cid at front of array
        int index = 0;
        for(iSetIter pid = same_cluster_peers.begin(); pid != same_cluster_peers.end(); pid++, index++)
        {
            candidates_[index] = *pid;
        }

        // peers with different cid put on tail of array
        for(iSetIter pid = diff_cluster_peers.begin(); pid != diff_cluster_peers.end(); pid++, index++)
        {
            candidates_[index] = *pid;
        }
    }
    else  // for standard or load-balancing
    {
        int index = 0;
        for(iSetIter it = in_swarm.begin(); it != in_swarm.end(); it++, index++)
        {
            candidates_[index] = *it;
        }
    }

    return in_swarm.size();
}

}
