#ifndef _PEER_SELECTION_H
#define _PEER_SELECTION_H

#include "abs_peer_selection.h"

struct Peer;
struct Neighbor;

class Standard : protected IPeerSelection
{
public:
    Neighbor* ChoosePeers() override;
};

#endif // for #ifndef _PEER_SELECTION_H
