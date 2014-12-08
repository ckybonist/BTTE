#include "../pg_delay.h"
#include "../cluster_info.h"
#include "cb_peer_selection.h"


using namespace uniformrand;


namespace btte_peer_selection
{

float ClusterBasedRule::ComputePGDelayByCluster(const int client_cid,
                                                const int cand_cid)
{
    const float min_delay = 0.01;
    const float same_cid_bound = 1.00 / static_cast<float>(g_kNumClusters);
    const float diff_cid_bound = 0.01 + same_cid_bound;
    const float max_delay = 1.0;

    float pg_delay = 0.0;
    if (cand_cid == client_cid)
    {
        pg_delay = Roll<float>(RSC::CB_PEERSELECT,
                               min_delay,
                               same_cid_bound);
    }
    else
    {
        pg_delay = Roll<float>(RSC::CB_PEERSELECT,
                               diff_cid_bound,
                               max_delay);
    }

    return pg_delay;
}

void ClusterBasedRule::AssignNeighbors(NeighborMap& neighbors,
                                   const size_t cand_size,
                                   const int client_cid)
{
    const size_t NUM_PEERLIST = g_btte_args.get_num_peerlist();

    for (size_t i = 0; i < cand_size; i++)
    {
        if (i >= NUM_PEERLIST) break;

        const int pid = candidates_[i];
        const int cid = g_peers.at(pid).get_cid();
        float pg_delay = ComputePGDelayByCluster(client_cid, cid);

        Neighbor nei_info = Neighbor(pg_delay);
        neighbors.insert(std::pair<int, Neighbor>(pid, nei_info));
        g_peers.at(pid).incr_neighbor_counts(1);
    }
}

void ClusterBasedRule::RefreshInfo()
{
    delete [] candidates_;
    candidates_ = nullptr;
}

NeighborMap ClusterBasedRule::StartSelection(const int client_pid,
                                             const IntSet& in_swarm_set)
{
    g_peers.at(client_pid).clear_neighbors();  // clear previous neighbors

    selector_pid_ = client_pid;

    NeighborMap neighbors;

    // Set parameter of sort_cid_flag to true, in order to get
    // candidates (most part, not all) which have
    // same cluster id as selector.
    size_t candidates_size = SetCandidates(in_swarm_set, RSC::CB_PEERSELECT, true);

    const int client_cid = g_peers.at(client_pid).get_cid();
    AssignNeighbors(neighbors, candidates_size, client_cid);

    //DebugInfo(neighbors, client_pid);

    RefreshInfo();

    return neighbors;
}

}