#ifndef _PEER_LEVEL_H
#define _PEER_LEVEL_H

#include "piece.h"

/**
 * Peer Level:
 *	   @level 1 : 10 (Mbps)   30 % of peers
 *	   @level 2 : 128 (Kbps)  50 % of peers
 *	   @level 3 : 56 (Kbps)   20 % of peers
 *
 * */

const int g_kNumLevel = 3;

struct PeerLevel
{
    float dist_rate;
    float bandwidth;
};

extern const PeerLevel g_kPeerLevel[g_kNumLevel] =
{
    // Level 0, fatest
    {
        0.3,
        10 * 1024 * 1024
    },

    // Level 1, average
    {
        0.5,
        128 * 1024
    },

    // Level 2, slowest
    {
        0.2,
        56 * 1024
    },
};

#endif // for #ifndef _PEER_LEVEL_H
