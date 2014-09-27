#include "config.h"
#include "args.h"

Args::Args(const std::string filename) {
    Config cfg(filename);

    NUM_PEER = cfg.GetValueOfKey<int>("NUM_PEER", 5000);  // if num_peer not being read, then set num_peer to 5000
    NUM_SEED = cfg.GetValueOfKey<int>("NUM_SEED", 100);
    NUM_LEECH = cfg.GetValueOfKey<int>("NUM_LEECH", 100);
    NUM_PIECE = cfg.GetValueOfKey<int>("NUM_PIECE", 256);
    TYPE_PEERSELECT = cfg.GetValueOfKey<int>("TYPE_PEERSELECT", 100);
    TYPE_PIECESELECT = cfg.GetValueOfKey<int>("TYPE_PIECESELECT", 100);
}
