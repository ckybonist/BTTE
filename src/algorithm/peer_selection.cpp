#include "peer_selection.h"


using namespace uniformrand;


namespace btte_peer_selection
{

IPeerSelection::IPeerSelection()
{
    candidates_ = nullptr;
}

IPeerSelection::~IPeerSelection()
{
    if (candidates_ != nullptr)
    {
        delete [] candidates_;
        candidates_ = nullptr;
    }
}

Neighbor* IPeerSelection::AllocNeighbors()
{
    const size_t NUM_PEERLIST = g_btte_args.get_num_peerlist();
    Neighbor* neighbors = new Neighbor[NUM_PEERLIST];

    if (neighbors == nullptr)
        ExitError("Memory Allocation Fault!");

    return neighbors;
}

// The peer-info of swarm which exclude selector itself
IntSet IPeerSelection::ExcludeSelf(const IntSet& in_swarm_set)
{
    IntSet cand_pid_set(in_swarm_set);
    cand_pid_set.erase(selector_pid_);

    return cand_pid_set;
}

// return size of candidates
// NOTE: if sort_cid_flag is true, then put peers that have
// same cid as selector at the prior of array
size_t IPeerSelection::SetCandidates(const IntSet& in_swarm_set,
                                     const RSC rsc,
                                     bool sort_cid_flag)
{
    // Erase self first, then, shuffle the index and
    // select other peers which in swarm.
    IntSet cand_pid_set = ExcludeSelf(in_swarm_set);
    const size_t kCandSize = cand_pid_set.size();

    int* tmp_arr = new int[kCandSize];
    if (tmp_arr == nullptr)
        ExitError("Memory Allocation Error");
    const int kUnvalidVal = -1;
    for(size_t i = 0; i < kCandSize; i++) { tmp_arr[i] = kUnvalidVal; }


    // for cluster-based, FIXME : 太冗長，可再縮減
    if (sort_cid_flag)
    {
        IntSet same_cluster_peers;
        IntSet diff_cluster_peers;
        const int self_cid = g_peers.at(selector_pid_).get_cid();

        for (const int pid : cand_pid_set)
        {
            if (g_peers.at(pid).get_cid() == self_cid)
                same_cluster_peers.insert(pid);
            else
                diff_cluster_peers.insert(pid);
        }

        // assign peers with same cid at front of array
        size_t index = 0;
        for (IntSetIter pid = same_cluster_peers.begin(); pid != same_cluster_peers.end(); pid++, index++)
        {
            if (index >= kCandSize) break;
            tmp_arr[index] = *pid;
        }

        // peers with different cid put on tail of array
        for (IntSetIter pid = diff_cluster_peers.begin(); pid != diff_cluster_peers.end(); pid++, index++)
        {
            if (index >= kCandSize) break;
            tmp_arr[index] = *pid;
        }
    }
    else  // for standard or load-balancing
    {
        int index = 0;
        for (IntSetIter it = cand_pid_set.begin(); it != cand_pid_set.end(); it++, index++)
        {
            tmp_arr[index] = *it;
        }
    }

    // Shuffle the set of candidates to ensure the selection is random
    Shuffle<int>(rsc, tmp_arr, kCandSize);

    // Copy result
    candidates_ = new int[kCandSize];
    if (nullptr == candidates_)
        ExitError("\nMemory Allocation Fault\n");

    for (size_t i = 0; i < kCandSize; i++)
    {
        if (tmp_arr[i] == kUnvalidVal) { break; }
        else { candidates_[i] = tmp_arr[i]; }
    }

    delete [] tmp_arr;
    tmp_arr = nullptr;

    return kCandSize;
}

void IPeerSelection::DebugInfo(NeighborMap const& neighbors,
                               const int client_pid) const
{
    // debug info
    std::cout << "\nNeighbors of Peer #" << client_pid << std::endl;
    std::cout << "Info: (pid, cid, neighbor counts, PG delay)\n";
    for (auto& it : neighbors)
    {
        std::cout << "(" << it.first << ",  "
                  << g_peers.at(it.first).get_cid() << ",  "
                  << g_peers.at(it.first).get_neighbor_counts() << ",  "
                  << it.second.pg_delay << ")" << std::endl;
    }
    std::cout << std::endl;
}

}
