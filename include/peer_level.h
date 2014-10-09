#ifndef _PEER_LEVEL_H
#define _PEER_LEVEL_H

#include "piece.h"

/**
 * Simulation arguments
 *
 * Piece:
 *     @piece_size: 512 (Kbytes)
 *
 * Category of peers:
 *	   @level 1 : 10 (Mbps)   seeder  30 %
 *	   @level 2 : 128 (Kbps)  leecher or normal peer  50 %
 *	   @level 3 : 56 (Kbps)   leecher or normal peer  20 %
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
