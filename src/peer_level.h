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

extern const size_t g_kNumLevel;  // equal to 3
extern const float lv1_ratio;
extern const float lv2_ratio;
extern const float lv3_ratio;

extern const PeerLevel g_kPeerLevel[];

#endif // for #ifndef _PEER_LEVEL_H
