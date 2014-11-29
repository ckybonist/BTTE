#include "error.h"
#include "pg_delay.h"
#include "STD_peer_selection.h"


using namespace uniformrand;


namespace btte_peer_selection
{

Standard::~Standard()
{
    //delete [] pg_delays_;
    //pg_delays_ = nullptr;
}

void Standard::RefreshInfo()
{
    delete [] candidates_;
    candidates_ = nullptr;
}

//void Standard::AssignNeighbors(Neighbor* const neighbors, const size_t cand_size)
void Standard::AssignNeighbors(NeighborMap& neighbors, const size_t cand_size)
{
    const size_t NUM_PEERLIST = g_btte_args.get_num_peerlist();

    for (size_t i = 0; i < NUM_PEERLIST; i++)
    {
        if (i < cand_size)
        {
            const int cand_pid = candidates_[i];
            float pg_delay = Roll(RSC::STD_PEERSELECT, 0.01, 1.0);
            Neighbor nei_info = Neighbor(pg_delay);
            neighbors.insert(std::pair<int, Neighbor>(cand_pid, nei_info));
            //neighbors[i].id = cand_pid;
            //neighbors[i].exist = true;
            //neighbors[i].pg_delay = pg_delay;
            g_peers.at(cand_pid).incr_neighbor_counts(1);
        }
        else
            break;
    }
}

NeighborMap Standard::StartSelection(const int client_pid, const IntSet& in_swarm_set)
{
    g_peers.at(client_pid).clear_neighbors();  // clear previous neighbors

    selector_pid_ = client_pid;

    //Neighbor* neighbors = AllocNeighbors();
    NeighborMap neighbors;

    size_t candidates_size = SetCandidates(in_swarm_set, false);

    // Randomly select peers
    Shuffle<int>(RSC::STD_PEERSELECT, candidates_, (int)candidates_size);

    AssignNeighbors(neighbors, candidates_size);

    DebugInfo(neighbors, client_pid);

    RefreshInfo();

    return neighbors;
}

}
