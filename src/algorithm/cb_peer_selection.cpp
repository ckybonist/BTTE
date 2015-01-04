#include <list>

#include "../pg_delay.h"
#include "../cluster_info.h"
#include "cb_peer_selection.h"


using namespace btte_uniformrand;


namespace btte_peer_selection
{

float ClusterBased::ComputePGDelayByCluster(const int client_cid,
                                                const int cand_cid)
{
    const float min_delay = 0.01;
    const float same_cluster_bound = 1.00 / static_cast<float>(g_kNumClusters);
    const float diff_cluster_bound = 0.01 + same_cluster_bound;
    const float max_delay = 1.0;

    float pg_delay = 0.0;
    if (cand_cid == client_cid)
    {
        pg_delay = Roll<float>(RSC::CB_PEERSELECT,
                               min_delay,
                               same_cluster_bound);
    }
    else
    {
        pg_delay = Roll<float>(RSC::CB_PEERSELECT,
                               diff_cluster_bound,
                               max_delay);
    }

    return pg_delay;
}

void ClusterBased::AddNeighborData(NeighborMap& neighbors,
                                       const int pid,
                                       const int client_cid)
{
    const int cid = g_peers.at(pid).get_cid();
    float pg_delay = ComputePGDelayByCluster(client_cid, cid);

    Neighbor nei_info = Neighbor(pg_delay);
    neighbors.insert(std::pair<int, Neighbor>(pid, nei_info));
    g_peers.at(pid).incr_neighbor_counts(1);

}

void ClusterBased::AssignNeighbors(NeighborMap& neighbors,
                                   const size_t num_candidates,
                                   const int client_cid)
{
    const size_t NUM_PEERLIST = g_btte_args.get_num_peerlist();

    std::list<int> nearby;   // peer and selector are in same cluster
    std::list<int> faraway;  // in different cluster

    for (size_t i = 0; i < num_candidates; i++)
    {
        //const int pid = candidates_[i];
        const int pid = candidates_.at(i);
        Peer const& client = g_peers.at(selector_pid_);
        Peer const& peer = g_peers.at(pid);

        if (peer.get_cid() == client.get_cid())
        {
            nearby.push_back(pid);
        }
        else
        {
            faraway.push_back(pid);
        }
    }

    int total_neighbors = 0;
    for (const int pid : nearby)
    {
        if (total_neighbors == NUM_PEERLIST) break;

        AddNeighborData(neighbors, pid, client_cid);

        ++total_neighbors;
    }

    if (total_neighbors < NUM_PEERLIST)
    {
        for (const int pid : faraway)
        {
            if (total_neighbors == NUM_PEERLIST) break;
            AddNeighborData(neighbors, pid, client_cid);
            ++total_neighbors;
        }
    }
}

void ClusterBased::RefreshInfo()
{
}

NeighborMap ClusterBased::StartSelection(const int client_pid,
                                             const IntSet& in_swarm_set)
{
    g_peers.at(client_pid).clear_neighbors();  // clear previous neighbors

    selector_pid_ = client_pid;

    NeighborMap neighbors;

    // Set parameter of sort_cid_flag to true, in order to get
    // candidates (most part, not all) which have
    // same cluster id as selector.
    size_t num_candidates = SetCandidates(in_swarm_set,
                                           RSC::CB_PEERSELECT);

    const int client_cid = g_peers.at(client_pid).get_cid();
    AssignNeighbors(neighbors, num_candidates, client_cid);

    //DebugInfo(neighbors, client_pid);

    RefreshInfo();

    return neighbors;
}

}
