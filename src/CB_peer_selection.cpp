#include "error.h"
#include "pg_delay.h"
#include "cluster_info.h"
#include "CB_peer_selection.h"


using namespace uniformrand;


namespace btte_peer_selection
{

bool ClusterBased::IsNewNeighbor(const int self_pid, const int cand_pid)
{
    bool flag = false;

    try  // key found
    {
        const Peer& myself = g_peers[self_pid];

        // don't use [] accessor of STL map. It will automatically
        // insert a new pair(key, value). This operation will cause
        // original propagation delay become 0
        const float unused_val = myself.pg_delay_pairs.at(cand_pid);
    }
    catch (const std::out_of_range& oor)  // key not found exception
    {
        flag = true;
    }

    return flag;
}

float ClusterBased::ComputePGDelay(const int self_cid, const int cand_cid)
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

Neighbor* ClusterBased::SelectNeighbors(const int self_pid, const iSet& in_swarm_set)
{
    Neighbor* neighbors = AllocNeighbors();

    const int self_cid = g_peers[self_pid].cid;
    size_t candidates_size = SetCandidates(self_pid, in_swarm_set, true);

    Peer& myself = g_peers[self_pid];

    for (int i = 0; (size_t)i < args_.NUM_PEERLIST; i++)
    {
        if ((size_t)i < candidates_size)
        {
            const int candidate_pid = candidates_[i];
            const int candidate_cid = myself.cid;
            neighbors[i].id = candidate_pid;
            neighbors[i].exist = true;

            if (IsNewNeighbor(self_pid, candidate_pid))
            {
                const float new_pg_delay = ComputePGDelay(self_cid, candidate_cid);
                myself.pg_delay_pairs.insert(std::pair<int, float>(candidate_pid, new_pg_delay));
                neighbors[i].pg_delay = new_pg_delay;
            }
            else
            {
                const float pg_delay = myself.pg_delay_pairs[candidate_pid];
                neighbors[i].pg_delay = pg_delay;
            }
        }
        else
        {
            break;
        }
    }

    // refresh info
    delete [] candidates_;
    candidates_ = nullptr;

    return neighbors;
}

}
