#include "my_peer_selection.h"


MyPeerSelection::~MyPeerSelection()
{

}


NeighborMap MyPeerSelection::StartSelection(const int client_pid,
                                     const IntSet& in_swarm_set)
{
    g_peers.at(client_pid).clear_neighbors();  // clear previous neighbors

    selector_pid_ = client_pid;

    NeighborMap neighbors;

    /* Collect all peers which in swarm into candidates_*/
    size_t num_candidates = SetCandidates(in_swarm_set,
                                          RSC::FREE_1);

    /*
     * Use your algorithm to select possible
     * neighbor<peer-id, propagation-delay> in candidates_
     * and insert to "NeighborMap"
     * * * * * */

    return neighbors;
}
