#include "peers_dict.h"

PeersDict g_pdict = PeersDict();

PeersDict::PeersDict(const int NUM_SEED, const int NUM_LEECH) {
    const int k_size = NUM_SEED + NUM_LEECH;
    for(int i = 0; i < k_size; i++) {
        g_pdict.tids[i] = i;
    }
}
