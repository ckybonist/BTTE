#include <fstream>
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
    orig_cands_.clear();
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

IPeerSelection::TmpPeer::TmpPeer(int pid, int cid, int nbcount)
{
    this->pid = pid;
    this->cid = cid;
    this->nbcount = nbcount;
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
    //RandomShuffle(candidates_.begin(), candidates_.end());
    RandomShuffle(rsc, candidates_.begin(), candidates_.end());

    for (int i = 0; i < num_candidates; i++)
    {
        const int pid = candidates_[i];
        const int cid = g_peers.at(pid).get_cid();
        const int nbcount = g_peers.at(pid).get_neighbor_counts();
        orig_cands_.push_back(TmpPeer(pid, cid, nbcount));
    }

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
    std::string prefix = " ";
    switch (g_btte_args.get_type_peerselect())
    {
        case 0:
            prefix = "std_";
            break;
        case 1:
            prefix = "lb_";
            break;
        case 2:
            prefix = "cb_";
            break;
        case 3:
            prefix = "user_";
            break;
        default:
            ExitError("wrong algo ID");
            break;
    }

    std::ofstream ofs;
    ofs.open(prefix + "peersel_log.txt", std::fstream::app);

    if (g_btte_args.get_type_peerselect() == 2)
    {
        ofs << "執行者(" << client_pid << ") 所屬的 cluster : "
            << g_peers.at(client_pid).get_cid() << std::endl;
    }
    // all peers in swarm
    ofs << "目前在 swarm 裡面的節點：\n";
    ofs << "<pid>  <cid>  <neighbor counts>\n";
    for (auto const& it : orig_cands_)
    {
        ofs << "(" << it.pid << ",  "
                   << it.cid << ",  "
                   << it.nbcount << ")"
                   << std::endl;
    }

    // result
    ofs << "\n執行者(" << client_pid << ") 的 neighbors\n";
    ofs << "<pid>  <cid>  <neighbor counts>  <propagation delay>\n";
    for (auto& it : neighbors)
    {
        const int pid = it.first;
        const float pg_delay = it.second.pg_delay;
        ofs << "(" << pid << ",  "
                  << g_peers.at(pid).get_cid() << ",  "
                  << g_peers.at(pid).get_neighbor_counts() << ",  "
                  << pg_delay << ")" << std::endl;
    }
    ofs << std::endl;
    ofs.close();
}

}
