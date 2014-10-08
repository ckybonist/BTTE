#include "error.h"
#include "random.h"

#include "peer.h"
#include "pg_delay.h"
#include "peer_selection.h"


using namespace uniformrand;


namespace peerselection
{

IPeerSelect::IPeerSelect(const int NUM_PEERLIST, const int NUM_PEER)
{
   NUM_PEERLIST_ = NUM_PEERLIST;
   NUM_PEER_ = NUM_PEER;
   ids_ = nullptr;
   pg_delays_ = nullptr;
}

Standard::~Standard()
{
    delete [] ids_;
    delete [] pg_delays_;

    for(int pid = 0; pid < NUM_PEER_; pid++)
    {
        delete [] g_peers[pid].neighbors;
    }
}

Neighbor* Standard::SelectNeighbors()
{
    Neighbor* neighbors = new Neighbor[NUM_PEERLIST_];
    if(neighbors == nullptr)
    {
        ExitError("Memory Allocation Fault in \
                   peerselection::Standard::SelectNeighbors()");
    }

    if(nullptr == ids_ && nullptr == pg_delays_)  // create array of random numbers first time
    {
        ids_ = DistinctRandNumbers<int>(RSC::STD_PEERSELECT,
                                        NUM_PEER_,  // size
                                        NUM_PEER_); // max number

        pg_delays_ = new int[g_k_max_pgdelay];

        if(ids_ == nullptr || pg_delays_ == nullptr)
        {
            ExitError("Memory Allocation Fault in \
                       peerselection::Standard::SelectNeighbors()");
        }

        for(int i = 0; i < g_k_max_pgdelay; i++)
        {
            pg_delays_[i] = Roll<int>(RSC::PGDELAY,
                                      1, g_k_max_pgdelay);
        }
    }
    else  // shuffle array if it had ever been created
    {
        Shuffle<int>(RSC::STD_PEERSELECT, ids_, NUM_PEER_);

        Shuffle<int>(RSC::STD_PEERSELECT, pg_delays_, g_k_max_pgdelay);
    }

    for(int i = 0; i < NUM_PEERLIST_; i++)
    {
        neighbors[i].id = ids_[i];
        neighbors[i].pg_delay = pg_delays_[i];
    }

    return neighbors;
}


Neighbor* LoadBalancing::SelectNeighbors()
{
    //TODO
    return nullptr;
}

Neighbor* ClusterBased::SelectNeighbors()
{
    //TODO
    return nullptr;
}

}
