#ifndef _LB_PEER_SELECTION_H
#define _LB_PEER_SELECTION_H

#include "abs_peer_selection.h"

class LoadBalance : protected IPeerSelection
{
public:
    Neighbor* ChoosePeers() override;
};

#endif // for #ifndef _LB_PEER_SELECTION_H
