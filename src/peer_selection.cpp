#include "error.h"
#include "peer_selection.h"


using namespace uniformrand;


namespace btte_peer_selection
{

IPeerSelect::IPeerSelect(Args args)
{
    args_ = args;
    candidates_ = nullptr;
}

IPeerSelect::~IPeerSelect()
{
    if (candidates_ != nullptr)
    {
        delete [] candidates_;
        candidates_ = nullptr;
    }

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
        ExitError("Memory Allocation Fault!");
    }

    return neighbors;
}


//bool IPeerSelect::IsNewNeighbor(const int self_pid, const int cand_pid)
//{
//    bool flag = false;
//
//    try  // key found
//    {
//        const Peer& myself = g_peers[self_pid];
//
//        // don't use [] accessor of STL map. It will automatically
//        // insert a new pair(key, value). This operation will cause
//        // original propagation delay become 0
//        const float unused_val = myself.pg_delay_records.at(cand_pid);
//    }
//    catch (const std::out_of_range& oor)  // key not found exception
//    {
//        flag = true;
//    }
//
//    return flag;
//}

//void IPeerSelect::RecordPGDelay(const int self_pid,
//                                const int cand_pid,
//                                const float pg_delay)
//{
//    Peer &myself = g_peers[self_pid];
//    std::pair<int, float> record(cand_pid, pg_delay);
//    //myself.pg_delay_records.insert(std::pair<int, float>(cand_pid, new_pg_delay));
//    myself.pg_delay_records.insert(record);
//}
//
//float IPeerSelect::QueryPGDelay(const int self_pid, const int cand_pid)
//{
//    Peer &myself = g_peers[self_pid];
//    return myself.pg_delay_records[cand_pid];
//}

// The peer-info of swarm which exclude selector itself
iSet IPeerSelect::ExcludeSelf(const int self_pid,
                 const iSet& in_swarm_set)
{
    iSet cand_pid_set(in_swarm_set);
    cand_pid_set.erase(cand_pid_set.find(self_pid));

    return cand_pid_set;
}

// return size of candidates
// NOTE: if sort_cid_flag is true, then put peers that have
// same cid as selector at the prior of array
size_t IPeerSelect::SetCandidates(const int self_pid,
                                  const iSet& in_swarm_set,
                                  bool sort_cid_flag)
{
    // Erase self first, then, shuffle the index and
    // select other peers which in swarm.
    iSet cand_pid_set = ExcludeSelf(self_pid, in_swarm_set);

    candidates_ = new int[cand_pid_set.size()];
    if(nullptr == candidates_)
        ExitError("\nMemory Allocation Fault in SetCandidates\n");

    // for cluster-based
    if (sort_cid_flag)
    {
        iSet same_cluster_peers;
        iSet diff_cluster_peers;
        const int self_cid = g_peers[self_pid].cid;

        for(iSetIter pid = cand_pid_set.begin(); pid != cand_pid_set.end(); pid++)
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
        for(iSetIter it = cand_pid_set.begin(); it != cand_pid_set.end(); it++, index++)
        {
            candidates_[index] = *it;
        }
    }

    return cand_pid_set.size();
}

}
