#include "error.h"
#include "pg_delay.h"
#include "cluster_info.h"
#include "CB_peer_selection.h"


using namespace uniformrand;


namespace btte_peer_selection
{

float ClusterBased::ComputePGDelayByCluster(const int self_cid, const int cand_cid)
{
    const float min_delay = 0.01;
    const float same_cid_bound = 1.00 / static_cast<float>(g_kNumClusters);
    const float diff_cid_bound = 0.01 + same_cid_bound;
    const float max_delay = 1.0;

    float pg_delay = 0.0;
    if (cand_cid == self_cid)
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

void ClusterBased::RefreshInfo()
{
    delete [] candidates_;
    candidates_ = nullptr;
}

Neighbor* ClusterBased::SelectNeighbors(const int self_pid, const iSet& in_swarm_set)
{
    Neighbor* neighbors = AllocNeighbors();

    const int self_cid = g_peers[self_pid].cid;

    // Set parameter of sort_cid_flag to true, in order to get
    // candidates (most part, not all) which have
    // same cluster id as selector.
    size_t candidates_size = SetCandidates(self_pid, in_swarm_set, true);

    for (int i = 0; (size_t)i < args_.NUM_PEERLIST; i++)
    {
        if ((size_t)i < candidates_size)
        {
            const int cand_pid = candidates_[i];
            const int cand_cid = g_peers[self_pid].cid;
            neighbors[i].id = cand_pid;
            neighbors[i].exist = true;

            // assign propagation delay
            float pg_delay = 0.0;
            if (IsNewNeighbor(self_pid, cand_pid))
            {
                pg_delay = ComputePGDelayByCluster(self_cid, cand_cid);
                RecordPGDelay(self_pid, cand_pid, pg_delay);
            }
            else
            {
                pg_delay = QueryPGDelay(self_pid, cand_pid);
            }
            neighbors[i].pg_delay = pg_delay;

            ++g_peers[cand_pid].neighbor_counts;
        }
        else
        {
            break;
        }
    }

    // debug info
    std::cout << "\nNeighbors of Peer #" << self_pid << std::endl;
    std::cout << "Cluster ID of Peer #" << self_pid
              << " : " << g_peers[self_pid].cid << std::endl;
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
