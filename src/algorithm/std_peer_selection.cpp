#include "../pg_delay.h"
#include "std_peer_selection.h"


using namespace btte_uniformrand;


namespace btte_peer_selection
{

Standard::~Standard()
{
    //delete [] pg_delays_;
    //pg_delays_ = nullptr;
}

void Standard::RefreshInfo()
{
}

void Standard::AssignNeighbors(NeighborMap& neighbors,
                               const size_t num_candidates)
{
    const size_t NUM_PEERLIST = g_btte_args.get_num_peerlist();

    for (size_t i = 0; i < NUM_PEERLIST; i++)
    {
        if (i < num_candidates)
        {
            const int cand_pid = candidates_[i];
            float pg_delay = Roll(RSC::STD_PEERSELECT, 0.01, 1.0);
            Neighbor nei_info = Neighbor(pg_delay);

            neighbors.insert(std::pair<int, Neighbor>(cand_pid, nei_info));
            g_peers.at(cand_pid).incr_neighbor_counts(1);
        }
        else
            break;
    }
}

NeighborMap Standard::StartSelection(const int client_pid,
                                     const IntSet& in_swarm_set)
{
    g_peers.at(client_pid).clear_neighbors();  // clear previous neighbors

    selector_pid_ = client_pid;

    NeighborMap neighbors;

    size_t num_candidates = SetCandidates(in_swarm_set,
                                          RSC::STD_PEERSELECT);

    AssignNeighbors(neighbors, num_candidates);

    //DebugInfo(neighbors, client_pid);

    RefreshInfo();

    return neighbors;
}

}
