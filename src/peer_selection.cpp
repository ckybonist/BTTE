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
   _NUM_PEERLIST = NUM_PEERLIST;
   _NUM_PEER = NUM_PEER;
   _ids = nullptr;
   _pg_delays = nullptr;
}

Standard::~Standard()
{
    delete [] _ids;
    delete [] _pg_delays;

    for(int pid = 0; pid < _NUM_PEER; pid++)
    {
        delete [] g_peers[pid].neighbors;
    }
}

Neighbor* Standard::SelectNeighbors()
{
    Neighbor* neighbors = new Neighbor[_NUM_PEERLIST];
    if(neighbors == nullptr)
    {
        ExitError("Memory Allocation Fault in \
                   peerselection::Standard::SelectNeighbors()");
    }

    if(nullptr == _ids && nullptr == _pg_delays)  // create array of random numbers first time
    {
        _ids = DistinctRandNumbers<int>(rsc_std_peerselect,
                                        _NUM_PEER,  // size
                                        _NUM_PEER); // max number

        _pg_delays = new int[g_k_max_pgdelay];

        if(_ids == nullptr || _pg_delays == nullptr)
        {
            ExitError("Memory Allocation Fault in \
                       peerselection::Standard::SelectNeighbors()");
        }

        for(int i = 0; i < g_k_max_pgdelay; i++)
        {
            _pg_delays[i] = Roll<int>(rsc_pgdelay,
                                      1, g_k_max_pgdelay);
        }
    }
    else  // shuffle array if it had ever been created
    {
        Shuffle<int>(rsc_std_peerselect, _ids, _NUM_PEER);

        Shuffle<int>(rsc_std_peerselect, _pg_delays, g_k_max_pgdelay);
    }

    for(int i = 0; i < _NUM_PEERLIST; i++)
    {
        neighbors[i].id = _ids[i];
        neighbors[i].pg_delay = _pg_delays[i];
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
