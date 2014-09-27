#include "random.h"
#include "abs_peer_selection.h"

using namespace uniformrand;

int* IPeerSelection::NewPGDelay(const int NUM_PEERLIST) {
    return DistinctRandNumbers(NUM_PEERLIST);
}
