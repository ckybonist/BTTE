#include "peer_level.h"


const size_t g_kNumLevel = 3;
const float lv1_ratio = 0.3;
const float lv2_ratio = 0.5;
const float lv3_ratio = 0.2;

const PeerLevel g_kPeerLevel[3] =
{
    // fatest
    {
        lv1_ratio,
        {
            10 * 1024 * 1024,
            10 * 1024 * 1024
        }
    },

    // average
    {
        lv2_ratio,
        {
            128 * 1024,
            1.5 * 1024 * 1024
        }
    },

    // slowest
    {
        lv3_ratio,
        {
            56 * 1024,
            56 * 1024
        }
    },
};
