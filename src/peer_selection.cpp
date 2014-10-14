//#include <iostream>
#include "error.h"
#include "random.h"

#include "peer.h"
#include "pg_delay.h"
#include "peer_selection.h"


using namespace uniformrand;


namespace peerselection
{

IPeerSelect::IPeerSelect(Args args)
{
    args_ = args;
    ids_ = nullptr;
    pg_delays_ = nullptr;
}

Standard::~Standard()
{
    if(ids_ != nullptr)
    {
        delete [] ids_;
        ids_ = nullptr;
    }

    delete [] pg_delays_;
    pg_delays_ = nullptr;

    for(size_t pid = args_.NUM_SEED; pid < args_.NUM_PEER; pid++)
    {
        if (g_peers[pid].neighbors != nullptr)
        {
            delete [] g_peers[pid].neighbors;
            g_peers[pid].neighbors = nullptr;
        }
    }
}

//Neighbor* Standard::SelectNeighbors(const int self_pid)
Neighbor* Standard::SelectNeighbors(const int self_pid, const iSet& in_swarm_set)
{
    Neighbor* neighbors = new Neighbor[args_.NUM_PEERLIST];
    if(neighbors == nullptr)
    {
        ExitError("Memory Allocation Fault in \
                   peerselection::Standard::SelectNeighbors()");
    }

    /// Erase self first, then, shuffle the index and
    /// select other peers which in swarm.
    iSet in_swarm(in_swarm_set);
    in_swarm.erase(in_swarm.find(self_pid));

    ids_ = new int[in_swarm.size()];
    if(nullptr == ids_)
        ExitError("Memory Allocation Fault");

    int index = 0;
    for(iSetIter it = in_swarm.begin(); it != in_swarm.end(); it++, index++)
    {
        ids_[index] = *it;
    }

    Shuffle<int>(RSC::STD_PEERSELECT, ids_, in_swarm.size());


    if(pg_delays_ == nullptr)
    {
        pg_delays_ = new int[g_kMaxPGdelay];
        if(nullptr == pg_delays_) ExitError("Memory Allocation Fault");

        for(int i = 0; i < g_kMaxPGdelay; i++)
        {
            pg_delays_[i] = Roll<int>(RSC::PGDELAY,
                                      1,
                                      g_kMaxPGdelay);
        }
    }
    else
    {
        Shuffle<int>(RSC::STD_PEERSELECT, pg_delays_, g_kMaxPGdelay);
    }

    for(size_t i = 0; i < args_.NUM_PEERLIST; i++)
    {
        if (i < in_swarm.size())
        {
            neighbors[i].id = ids_[i];
            neighbors[i].pg_delay = pg_delays_[i];
            neighbors[i].exist = true;
        }
    }

    delete [] ids_;
    ids_ = nullptr;

    return neighbors;
}

//Neighbor* LoadBalancing::SelectNeighbors(const int self_pid)
Neighbor* LoadBalancing::SelectNeighbors(const int self_pid, const iSet& in_swarm_set)
{
    //TODO
    ExitError("\n\nNot Implement Yet!!!");
    return nullptr;
}
//Neighbor* ClusterBased::SelectNeighbors(const int self_pid)
Neighbor* ClusterBased::SelectNeighbors(const int self_pid, const iSet& in_swarm_set)
{
    //TODO
    ExitError("\n\nNot Implement Yet!!!");
    return nullptr;
}

}
