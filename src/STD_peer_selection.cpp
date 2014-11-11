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

void Standard::AssignNeighbors(Neighbor* const neighbors, const size_t cand_size)
{
    //Peer& myself = g_peers[selector_pid_];
    for(int i = 0; (size_t)i < args_.NUM_PEERLIST; i++)
    {
        if ((size_t)i < cand_size)
        {
            const int cand_pid = candidates_[i];
            neighbors[i].id = cand_pid;
            neighbors[i].connected = true;

            float pg_delay = Roll(RSC::STD_PEERSELECT, 0.01, 1.0);
            neighbors[i].pg_delay = pg_delay;

            ++g_peers.at(cand_pid).neighbor_counts;
        }
        else
        {
            break;
        }
    }
}

Neighbor* Standard::StartSelection(const int self_pid, const IntSet& in_swarm_set)
{
    selector_pid_ = self_pid;

    Neighbor* neighbors = AllocNeighbors();

    size_t candidates_size = SetCandidates(in_swarm_set, false);

    // Randomly select peers
    Shuffle<int>(RSC::STD_PEERSELECT, candidates_, (int)candidates_size);

    AssignNeighbors(neighbors, candidates_size);

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
                  << g_peers.at(nei.id).cid << ",  "
                  << nei.pg_delay << ")" << std::endl;
    }

    RefreshInfo();

    return neighbors;
}

}
