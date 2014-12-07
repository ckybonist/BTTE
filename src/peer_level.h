#ifndef _PEER_LEVEL_H
#define _PEER_LEVEL_H

#include <cstring>
#include "bandwidth.h"

/**
 * Peer Level:
 *	   @level 1 : 10 (Mbps)(bi-directinal)         30% of peers
 *	   @level 2 : Up: 128 (Kbps), Down: 1.5(Mbps)  50% of peers
 *	   @level 3 : 56 (Kbps)(bi-directinal)         20% of peers
 *
 * */

struct PeerLevel
{
    float dist_rate;
    Bandwidth bandwidth;
};

const int g_kNumLevel = 3;
const PeerLevel g_kPeerLevel[g_kNumLevel] =
{
    // fatest
    {
        0.3,
        {
            10 * 1024 * 1024,
            10 * 1024 * 1024
        }
    },

    // average
    {
        0.5,
        {
            128 * 1024,
            1.5 * 1024 * 1024
        }
    },

    // slowest
    {
        0.2,
        {
            56 * 1024,
            56 * 1024
        }
    },
};

#endif // for #ifndef _PEER_LEVEL_H
