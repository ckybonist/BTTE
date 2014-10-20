#include "error.h"
#include "pg_delay.h"
#include "STD_peer_selection.h"


using namespace uniformrand;


namespace btte_peer_selection
{

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

}
