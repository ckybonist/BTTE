#include "LB_peer_selection.h"

using namespace uniformrand;

namespace btte_peer_selection
{

LoadBalancing::~LoadBalancing()
{
    //delete [] pg_delays_;
    //delete [] nbc_info_;
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

    for (size_t i = 0; i < cand_size; i++)
    {
        const int cand_pid = candidates_[i];
        nbc_info_[i].pid = cand_pid;
        nbc_info_[i].counts = g_peers.at(cand_pid).get_neighbor_counts();
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
    for (size_t i = 0; i < cand_size; i++)
    {
        std::cout << "(" << nbc_info_[i].pid
                  << ",  " << nbc_info_[i].counts << ")\n";
    }
    std::cout << std::endl;
}

//void LoadBalancing::AssignNeighbors(Neighbor* const neighbors, const size_t cand_size)
void LoadBalancing::AssignNeighbors(NeighborMap& neighbors, const size_t cand_size)
{
    for (size_t i = 0; i < args_.NUM_PEERLIST; i++)
    {
        if (i < cand_size)
        {
            const int cand_pid = nbc_info_[i].pid;
            float pg_delay = Roll(RSC::STD_PEERSELECT, 0.01, 1.0);
            Neighbor nei_info = Neighbor(pg_delay);
            neighbors.insert(std::pair<int, Neighbor>(cand_pid, nei_info));
            //const int cand_pid = nbc_info_[i].pid;
            //neighbors[i].id = cand_pid;
            //neighbors[i].exist = true;

            //float pg_delay = Roll(RSC::LB_PEERSELECT, 0.01, 1.0);
            //neighbors[i].pg_delay = pg_delay;

            g_peers.at(cand_pid).incr_neighbor_counts(1);  // Important
        }
        else
            break;
    }
}

//Neighbor* LoadBalancing::StartSelection(const int client_pid, const IntSet& in_swarm_set)
NeighborMap LoadBalancing::StartSelection(const int client_pid, const IntSet& in_swarm_set)
{
    g_peers.at(client_pid).clear_neighbors();  // clear previous neighbors

    selector_pid_ = client_pid;

    //Neighbor* neighbors = AllocNeighbors();
    NeighborMap neighbors;

    size_t candidates_size = SetCandidates(in_swarm_set, false);

    GatherNeighborCounts(candidates_size);

    // Sort neighbor counts (low to high)
    SortCountsInfo(candidates_size);

    AssignNeighbors(neighbors, candidates_size);

    DebugInfo(neighbors, client_pid);

    RefreshInfo();

    return neighbors;
}

}
