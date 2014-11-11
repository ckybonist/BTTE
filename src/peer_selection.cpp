#include "error.h"
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
        delete [] g_peers.at(pid).neighbors;
        g_peers.at(pid).neighbors = nullptr;
    }
}

Neighbor* IPeerSelect::AllocNeighbors()
{
    Neighbor* neighbors = new Neighbor[args_.NUM_PEERLIST];

    if(neighbors == nullptr)
    {
        ExitError("Memory Allocation Fault!");
    }

    return neighbors;
}

// The peer-info of swarm which exclude selector itself
IntSet IPeerSelect::ExcludeSelf(const IntSet& in_swarm_set)
{
    IntSet cand_pid_set(in_swarm_set);
    cand_pid_set.erase(cand_pid_set.find(selector_pid_));

    return cand_pid_set;
}

// return size of candidates
// NOTE: if sort_cid_flag is true, then put peers that have
// same cid as selector at the prior of array
size_t IPeerSelect::SetCandidates(const IntSet& in_swarm_set, bool sort_cid_flag)
{
    // Erase self first, then, shuffle the index and
    // select other peers which in swarm.
    IntSet cand_pid_set = ExcludeSelf(in_swarm_set);

    candidates_ = new int[cand_pid_set.size()];
    if(nullptr == candidates_)
        ExitError("\nMemory Allocation Fault in SetCandidates\n");

    // for cluster-based
    if (sort_cid_flag)
    {
        IntSet same_cluster_peers;
        IntSet diff_cluster_peers;
        const int self_cid = g_peers.at(selector_pid_).cid;

        for(IntSetIter pid = cand_pid_set.begin(); pid != cand_pid_set.end(); pid++)
        {
            if (g_peers.at(*pid).cid == self_cid)
            {
                same_cluster_peers.insert(*pid);
            }
            else
            {
                diff_cluster_peers.insert(*pid);
            }
        }

        // assign peers with same cid at front of array
        int index = 0;
        for(IntSetIter pid = same_cluster_peers.begin(); pid != same_cluster_peers.end(); pid++, index++)
        {
            candidates_[index] = *pid;
        }

        // peers with different cid put on tail of array
        for(IntSetIter pid = diff_cluster_peers.begin(); pid != diff_cluster_peers.end(); pid++, index++)
        {
            candidates_[index] = *pid;
        }
    }
    else  // for standard or load-balancing
    {
        int index = 0;
        for(IntSetIter it = cand_pid_set.begin(); it != cand_pid_set.end(); it++, index++)
        {
            candidates_[index] = *it;
        }
    }

    return cand_pid_set.size();
}

}
