#include "peer_selection.h"


using namespace btte_uniformrand;


namespace btte_peer_selection
{

IPeerSelection::IPeerSelection()
{
    //candidates_ = nullptr;
}

IPeerSelection::~IPeerSelection()
{
    //if (candidates_ != nullptr)
    //{
    //    delete [] candidates_;
    //    candidates_ = nullptr;
    //}
}

void IPeerSelection::RefreshCandidates()
{
    //delete [] candidates_;
    //candidates_ = nullptr;
    if (!candidates_.empty())
        candidates_.clear();
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
                                     const RSC rsc)
{
    RefreshCandidates();

    // Erase self first, then, shuffle the index and
    // select other peers which in swarm.
    IntSet cand_pid_set = ExcludeSelf(in_swarm_set);
    const size_t num_candidates = cand_pid_set.size();

    for (const int pid : cand_pid_set)
    {
        candidates_.push_back(pid);
    }

    // Shuffle the set of candidates to ensure the selection is random
    RandomShuffle(candidates_.begin(), candidates_.end());

    return num_candidates;
}

/*  if candidates_ is oridinary array
size_t IPeerSelection::SetCandidates(const IntSet& in_swarm_set,
                                     const RSC rsc)
{
    RefreshCandidates();

    // Erase self first, then, shuffle the index and
    // select other peers which in swarm.
    //int* tmp_arr = new int[num_candidates];
    //if (tmp_arr == nullptr)
    //    ExitError("Memory Allocation Error");

    //const int init_val = -1;
    //for(size_t i = 0; i < num_candidates; i++) { tmp_arr[i] = init_val; }

    //int index = 0;
    //for (auto const& it : cand_pid_set)
    //{
    //    tmp_arr[index] = it;
    //    ++index;
    //}

    // Shuffle the set of candidates to ensure the selection is random
    //Shuffle<int>(rsc, tmp_arr, num_candidates);

    // Copy result
    //candidates_ = new int[num_candidates];
    //if (nullptr == candidates_)
    //    ExitError("\nMemory Allocation Fault\n");

    //for (size_t i = 0; i < num_candidates; i++)
    //{
    //    if (tmp_arr[i] == init_val) { break; }  // the values start here are not,
    //    else { candidates_[i] = tmp_arr[i]; }
    //}

    //delete [] tmp_arr;
    //tmp_arr = nullptr;

    //return num_candidates;
}
*/

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
