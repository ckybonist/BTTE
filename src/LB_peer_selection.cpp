#include "LB_peer_selection.h"

using namespace uniformrand;

namespace btte_peer_selection
{

LoadBalancing::~LoadBalancing()
{
    delete [] pg_delays_;
    delete [] nbc_info_;
}

void LoadBalancing::RefreshInfo()
{
    delete [] nbc_info_;
    nbc_info_ = nullptr;

    delete [] candidates_;
    candidates_ = nullptr;
}

void LoadBalancing::AllocNBCInfo(const size_t N)
{
    nbc_info_ = new NBCountsInfo[N];
    if (nbc_info_ == nullptr)
        std::cout << "Memory Allocation Fault!";
}

void LoadBalancing::GatherNeighborCounts(const size_t cand_size)
{
    AllocNBCInfo(cand_size);

    for (int i = 0; (size_t)i < cand_size; i++)
    {
        const int cand_pid = candidates_[i];
        nbc_info_[i].pid = cand_pid;
        nbc_info_[i].counts = g_peers.at(cand_pid).neighbor_counts;
    }
}

void LoadBalancing::SortCountsInfo(const size_t cand_size)
{
    auto func_comp = [] (const void* a, const void* b) {
                             const NBCountsInfo* pa = (NBCountsInfo*)a;
                             const NBCountsInfo* pb = (NBCountsInfo*)b;
                             return pa->counts - pb->counts;
                        };
    qsort(nbc_info_,
          cand_size,
          sizeof(NBCountsInfo),
          func_comp);

    std::cout << "\nNBC Info (pid, neighbor_counts):\n";
    for (int i = 0; (size_t)i < cand_size; i++)
    {
        std::cout << "(" << nbc_info_[i].pid
                  << ",  " << nbc_info_[i].counts << ")\n";
    }
    std::cout << std::endl;
}

void LoadBalancing::AssignNeighbors(Neighbor* const neighbors, const size_t cand_size)
{
    for (int i = 0; (size_t)i < args_.NUM_PEERLIST; i++)
    {
        if ((size_t)i < cand_size)
        {
            const int cand_pid = nbc_info_[i].pid;
            neighbors[i].id = cand_pid;
            neighbors[i].exist = true;

            float pg_delay = Roll(RSC::LB_PEERSELECT, 0.01, 1.0);
            neighbors[i].pg_delay = pg_delay;

            ++g_peers.at(cand_pid).neighbor_counts;  // Important
        }
        else
        {
            break;
        }
    }
}

Neighbor* LoadBalancing::StartSelection(const int client_pid, const IntSet& in_swarm_set)
{
    selector_pid_ = client_pid;

    Neighbor* neighbors = AllocNeighbors();

    size_t candidates_size = SetCandidates(in_swarm_set, false);

    GatherNeighborCounts(candidates_size);

    // Sort neighbor counts (low to high)
    SortCountsInfo(candidates_size);

    AssignNeighbors(neighbors, candidates_size);

    // debug info
    std::cout << "\nNeighbors of Peer #" << client_pid << std::endl;
    std::cout << "Info: (pid, cid, neighbor counts, PG delay)\n";
    for (int n = 0; (size_t)n < args_.NUM_PEERLIST; n++)
    {
        Neighbor nei = neighbors[n];
        if (nei.id == -1)
        {
            std::cout << "None\n";
            continue;
        }
        std::cout << "(" << nei.id << ",  "
                  << g_peers.at(nei.id).cid << ",  "
                  << g_peers.at(nei.id).neighbor_counts << ",  "
                  << nei.pg_delay << ")" << std::endl;
    }
    std::cout << std::endl;

    RefreshInfo();

    return neighbors;
}

}
