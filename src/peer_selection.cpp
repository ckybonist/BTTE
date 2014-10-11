#include <iostream>
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

    for(int pid = args_.NUM_SEED; pid < args_.NUM_PEER; pid++)
    {
        delete [] g_peers[pid].neighbors;
        g_peers[pid].neighbors = nullptr;
    }
}

Neighbor* Standard::SelectNeighbors(const int self_pid)
{
    Neighbor* neighbors = new Neighbor[args_.NUM_PEERLIST];
    if(neighbors == nullptr)
    {
        ExitError("Memory Allocation Fault in \
                   peerselection::Standard::SelectNeighbors()");
    }

    iSet in_swarm = g_in_swarm_set;
    in_swarm.erase(in_swarm.find(self_pid));

    ids_ = new int[in_swarm.size()];
    if(nullptr == ids_) ExitError("Memory Allocation Fault");

    int index = 0;
    for(iSetIter iter = in_swarm.begin(); iter != in_swarm.end(); iter++, index++)
    {
        ids_[index] = *iter;
    }

    std::cout << "\nPeers in swarm: ";
    for(size_t i = 0; i < in_swarm.size(); i++)
    {
        std::cout << ids_[i] << " ";
    }
    std::cout << "\n";

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

    //if(nullptr == ids_ && nullptr == pg_delays_)  // create array of random numbers first time
    //{
    //    //ids_ = DistinctRandNumsAndExcludeNum<int>(RSC::STD_PEERSELECT,
    //    //                                          NUM_PEER_,  // size of number set
    //    //                                          self_pid,       // number be excluded
    //    //                                          NUM_PEER_);     // number of max peer

    //    pg_delays_ = new int[g_kMaxPGdelay];

    //    if(ids_ == nullptr || pg_delays_ == nullptr)
    //    {
    //        ExitError("Memory Allocation Fault in \
    //                   peerselection::Standard::SelectNeighbors()");
    //    }

    //    for(int i = 0; i < g_kMaxPGdelay; i++)
    //    {
    //        pg_delays_[i] = Roll<int>(RSC::PGDELAY,
    //                                  1, g_kMaxPGdelay);
    //    }
    //}
    //else  // shuffle array if it had ever been created
    //{
    //    Shuffle<int>(RSC::STD_PEERSELECT, ids_, NUM_PEER_);

    //    Shuffle<int>(RSC::STD_PEERSELECT, pg_delays_, g_kMaxPGdelay);
    //}

    for(int i = 0; i < args_.NUM_PEERLIST; i++)
    {
        if (i < in_swarm.size())
        {
            neighbors[i].id = ids_[i];
            neighbors[i].pg_delay = pg_delays_[i];
            neighbors[i].exist = true;
        }
        else
        {
            continue;
        }
    }

    delete [] ids_;
    ids_ = nullptr;

    return neighbors;
}


Neighbor* LoadBalancing::SelectNeighbors(const int self_pid)
{
    //TODO
    ExitError("\n\nNot Implement Yet!!!");
    return nullptr;
}

Neighbor* ClusterBased::SelectNeighbors(const int self_pid)
{
    //TODO
    ExitError("\n\nNot Implement Yet!!!");
    return nullptr;
}

}
