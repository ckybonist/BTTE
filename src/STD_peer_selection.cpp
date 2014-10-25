#include "error.h"
#include "pg_delay.h"
#include "STD_peer_selection.h"


using namespace uniformrand;


namespace btte_peer_selection
{

Standard::~Standard()
{
    delete [] pg_delays_;
    pg_delays_ = nullptr;
}

void Standard::RefreshInfo()
{
    delete [] candidates_;
    candidates_ = nullptr;
}

void Standard::AssignNeighbors(Neighbor* const neighbors,
                              const size_t cand_size,
                              const int self_pid)
{
    Peer& myself = g_peers[self_pid];
    for(int i = 0; (size_t)i < args_.NUM_PEERLIST; i++)
    {
        if ((size_t)i < cand_size)
        {
            const int cand_pid = candidates_[i];
            neighbors[i].id = cand_pid;
            neighbors[i].exist = true;

            // assign propagation delay
            float pg_delay = 0.0;
            if (IsNewNeighbor(self_pid, cand_pid))
            {
                pg_delay = Roll(RSC::LB_PEERSELECT, 0.01, 1.0);
                RecordPGDelay(self_pid, cand_pid, pg_delay);
            }
            else
            {
                pg_delay = QueryPGDelay(self_pid, cand_pid);
            }
            neighbors[i].pg_delay = pg_delay;

            ++g_peers[cand_pid].neighbor_counts;
        }
    }
}

Neighbor* Standard::SelectNeighbors(const int self_pid, const iSet& in_swarm_set)
{
    Neighbor* neighbors = AllocNeighbors();

    size_t candidates_size = SetCandidates(self_pid, in_swarm_set, false);

    // randomly selected peers
    Shuffle<int>(RSC::STD_PEERSELECT, candidates_, (int)candidates_size);

    AssignNeighbors(neighbors, candidates_size, self_pid);

    // debug info
    std::cout << "\nNeighbors of Peer #" << self_pid << std::endl;
    std::cout << "Info: (pid, cid, PG delay)\n";
    for (int n = 0; (size_t)n < args_.NUM_PEERLIST; n++)
    {
        Neighbor nei = neighbors[n];
        if (nei.id == -1)
        {
            std::cout << "None\n";
            continue;
        }
        std::cout << "(" << nei.id << ",  "
                  << g_peers[nei.id].cid << ",  "
                  << nei.pg_delay << ")" << std::endl;
    }

    RefreshInfo();

    return neighbors;
}

}
