#include "peer.h"

Peer::Peer() {
    id = 0;
    cid = 0;  // not yet

    is_seed = false;
    in_swarm = false;

    neighbors = nullptr;

    pieces = nullptr;
    time_per_piece = 0.0;

    //pgdelay = nullptr;
    time = 0.0;
    counter = 0;
}

Peer *g_peers = nullptr;
