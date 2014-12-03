#include "error.h"
#include "peer_selection.h"


using namespace uniformrand;


namespace btte_peer_selection
{

IPeerSelect::IPeerSelect()
{
    candidates_ = nullptr;
}

IPeerSelect::~IPeerSelect()
{
    if (candidates_ != nullptr)
    {
        delete [] candidates_;
        candidates_ = nullptr;
    }

    //for(int pid = args_.NUM_SEED; (size_t)pid < args_.NUM_PEER; pid++)
    //{
    //    delete [] g_peers.at(pid).neighbors;
    //    g_peers.at(pid).neighbors = nullptr;
    //}
}

Neighbor* IPeerSelect::AllocNeighbors()
{
    const size_t NUM_PEERLIST = g_btte_args.get_num_peerlist();
    Neighbor* neighbors = new Neighbor[NUM_PEERLIST];

    if (neighbors == nullptr)
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
size_t IPeerSelect::SetCandidates(const IntSet& in_swarm_set, const RSC rsc, bool sort_cid_flag)
{
    // Erase self first, then, shuffle the index and
    // select other peers which in swarm.
    IntSet cand_pid_set = ExcludeSelf(in_swarm_set);
    const int kCandSize = cand_pid_set.size();

    candidates_ = new int[cand_pid_set.size()];
    if (nullptr == candidates_)
        ExitError("\nMemory Allocation Fault in SetCandidates\n");

    // Shuffle the set of candidates to ensure the selection is random
    Shuffle<int>(rsc, candidates_, kCandSize);

    // for cluster-based
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
            if (index >= cand_pid_set.size()) break;
            candidates_[index] = *pid;
        }

        // peers with different cid put on tail of array
        for (IntSetIter pid = diff_cluster_peers.begin(); pid != diff_cluster_peers.end(); pid++, index++)
        {
            if (index >= cand_pid_set.size()) break;
            candidates_[index] = *pid;
        }
    }
    else  // for standard or load-balancing
    {
        int index = 0;
        for (IntSetIter it = cand_pid_set.begin(); it != cand_pid_set.end(); it++, index++)
        {
            candidates_[index] = *it;
        }
    }

    return cand_pid_set.size();
}

void IPeerSelect::DebugInfo(NeighborMap const& neighbors, const int client_pid) const
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

    //for (int n = 0; (size_t)n < args_.NUM_PEERLIST; n++)
    {
        //Neighbor nei = neighbors[n];
        //if (nei.id == -1)
        //{
        //    std::cout << "None\n";
        //    continue;
        //}
        //std::cout << "(" << nei.id << ",  "
        //          << g_peers.at(nei.id).cid << ",  "
        //          << g_peers.at(nei.id).neighbor_counts << ",  "
        //          << nei.pg_delay << ")" << std::endl;
    }
    std::cout << std::endl;
}

}
