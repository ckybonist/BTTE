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

const int g_k_num_level = 3;

struct PeerLevel {
    float dist_rate;
    float trans_time;
};

extern const PeerLevel g_k_peer_level[g_k_num_level] = {
    // Level 0, fatest
    {
        0.3,
        (g_k_piece_size / static_cast<float>(10 * 1024 * 1024))
    },

    // Level 1, average
    {
        0.5,
        (g_k_piece_size / static_cast<float>(128 * 1024))
    },

    // Level 2, slowest
    {
        0.2,
        (g_k_piece_size / static_cast<float>(56 * 1024))
    },
};

#endif // for #ifndef _PEER_LEVEL_H
