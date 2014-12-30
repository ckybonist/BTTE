#include "../src/algorithm/peer_selection.h"


using namespace btte_peer_selection;


class MyPeerSelection : public IPeerSelection
{
    public:
        MyPeerSelection() : IPeerSelection() {};
        ~MyPeerSelection();

    private:
        NeighborMap StartSelection(const int client_pid,
                                   const IntSet& in_swarm_set) override;
};
