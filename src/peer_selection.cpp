//#include <iostream>
#include "error.h"
#include "peer.h"
#include "pg_delay.h"
#include "cluster_info.h"
#include "peer_selection.h"


using namespace uniformrand;


namespace peerselection
{

IPeerSelect::IPeerSelect(Args args)
{
    args_ = args;
    candidates_ = nullptr;
    //pg_delays_ = nullptr;
}

IPeerSelect::~IPeerSelect()
{ delete [] candidates_;
    candidates_ = nullptr;

    for(int pid = args_.NUM_SEED; (size_t)pid < args_.NUM_PEER; pid++)
    {
        delete [] g_peers[pid].neighbors;
        g_peers[pid].neighbors = nullptr;
    }
}

Neighbor* IPeerSelect::AllocNeighbors()
{
    Neighbor* neighbors = new Neighbor[args_.NUM_PEERLIST];

    if(neighbors == nullptr)
    {
        ExitError("Memory Allocation Fault in \
                   peerselection::Standard::SelectNeighbors()");
    }

    return neighbors;
}

// return size of candidates
// NOTE: if sort_cid_flag is true, then put peers that have
// same cid as g_peers[self_pid] at top of array
size_t IPeerSelect::SetCandidates(const int self_pid,
                                  const iSet& in_swarm_set,
                                  bool sort_cid_flag)
{
    /// Erase self first, then, shuffle the index and
    /// select other peers which in swarm.
    iSet in_swarm(in_swarm_set);
    in_swarm.erase(in_swarm.find(self_pid));

    candidates_ = new int[in_swarm.size()];
    if(nullptr == candidates_) ExitError("\nMemory Allocation Fault in SetCandidates\n");

    // for cluster-based
    if (sort_cid_flag)
    {
        iSet same_cluster_peers;
        iSet diff_cluster_peers;
        const int self_cid = g_peers[self_pid].cid;

        for(iSetIter pid = in_swarm.begin(); pid != in_swarm.end(); pid++)
        {
            if (g_peers[*pid].cid == self_cid)
            {
                same_cluster_peers.insert(*pid);
            }
            else
            {
                diff_cluster_peers.insert(*pid);
            }
        }

        std::cout << std::flush;
        // assign peers with same cid at front of array
        int index = 0;
        for(iSetIter pid = same_cluster_peers.begin(); pid != same_cluster_peers.end(); pid++, index++)
        {
            candidates_[index] = *pid;
        }

        // peers with different cid put on tail of array
        for(iSetIter pid = diff_cluster_peers.begin(); pid != diff_cluster_peers.end(); pid++, index++)
        {
            candidates_[index] = *pid;
        }
    }
    else  // for standard or load-balancing
    {
        int index = 0;
        for(iSetIter it = in_swarm.begin(); it != in_swarm.end(); it++, index++)
        {
            candidates_[index] = *it;
        }
    }

    return in_swarm.size();
}


Standard::~Standard()
{
    delete [] pg_delays_;
    pg_delays_ = nullptr;
}

Neighbor* Standard::SelectNeighbors(const int self_pid, const iSet& in_swarm_set)
{
    Neighbor* neighbors = AllocNeighbors();

    size_t candidates_size = SetCandidates(self_pid, in_swarm_set, false);

    Shuffle<int>(RSC::STD_PEERSELECT, candidates_, (int)candidates_size);

    // Randomly allot propagation delay values
    if(pg_delays_ == nullptr)
    {
        pg_delays_ = new float[g_kMaxPGdelay];
        if(nullptr == pg_delays_) ExitError("Memory Allocation Fault");

        for(int i = 0; (size_t)i < g_kMaxPGdelay; i++)
        {
            pg_delays_[i] = Roll<float>(RSC::PGDELAY,
                                        0.01,
                                        (float)g_kMaxPGdelay / 100.0);
        }
    }
    else
    {
      Shuffle<float>(RSC::STD_PEERSELECT, pg_delays_, g_kMaxPGdelay);
    }

    for(int i = 0; (size_t)i < args_.NUM_PEERLIST; i++)
    {
        if ((size_t)i < candidates_size)
        {
            neighbors[i].id = candidates_[i];
            neighbors[i].pg_delay = pg_delays_[i];
            neighbors[i].exist = true;
        }
    }

    delete [] candidates_;
    candidates_ = nullptr;

    return neighbors;
}

Neighbor* LoadBalancing::SelectNeighbors(const int self_pid, const iSet& in_swarm_set)
{
    //TODO
    ExitError("\n\nNot Implement Yet!!!");
    return nullptr;
}

Neighbor* ClusterBased::SelectNeighbors(const int self_pid, const iSet& in_swarm_set)
{
    Neighbor* neighbors = AllocNeighbors();

    size_t candidates_size = SetCandidates(self_pid, in_swarm_set, true);

    // TODO: Sort candidates by cluster_id, if its cid same as cid of peers[pid]
    // then move on top
    //Shuffle<int>(RSC::CB_PEERSELECT, candidates_, (int)candidates_size);

    const float min_delay = 0.01;
    const float same_cid_bound = 1.00 / static_cast<float>(g_kNumClusters);
    const float diff_cid_bound = 0.01 + same_cid_bound;
    const float max_delay = 1.0;
    const int self_cid = g_peers[self_pid].cid;

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

    delete [] candidates_;
    candidates_ = nullptr;

    return neighbors;
}

}
