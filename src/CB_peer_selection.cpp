#include "error.h"
#include "pg_delay.h"
#include "cluster_info.h"
#include "CB_peer_selection.h"


using namespace uniformrand;


namespace btte_peer_selection
{

Neighbor* ClusterBased::SelectNeighbors(const int self_pid, const iSet& in_swarm_set)
{
    const float min_delay = 0.01;
    const float same_cid_bound = 1.00 / static_cast<float>(g_kNumClusters);
    const float diff_cid_bound = 0.01 + same_cid_bound;
    const float max_delay = 1.0;
    const int self_cid = g_peers[self_pid].cid;

    Neighbor* neighbors = AllocNeighbors();

    size_t candidates_size = SetCandidates(self_pid, in_swarm_set, true);


    for (int i = 0; (size_t)i < args_.NUM_PEERLIST; i++)
    {
        if ((size_t)i < candidates_size)
        {
            const int candidate_cid = g_peers[candidates_[i]].cid;
            neighbors[i].id = candidates_[i];
            neighbors[i].exist = true;
            if (candidate_cid == self_cid)
            {
                neighbors[i].pg_delay = Roll<float>(RSC::CB_PEERSELECT,
                                                    min_delay,
                                                    same_cid_bound);
            }
            else
            {
                neighbors[i].pg_delay = Roll<float>(RSC::CB_PEERSELECT,
                                                    diff_cid_bound,
                                                    max_delay);
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
