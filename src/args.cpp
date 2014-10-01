#include "config.h"
#include "peer_selection.h"
#include "piece_selection.h"
#include "args.h"

using namespace peerselection;

Args::Args(const std::string filename)
{
    Config cfg(filename);

    NUM_PEER = cfg.GetValueOfKey<int>("NUM_PEER", 5000);
    NUM_SEED = cfg.GetValueOfKey<int>("NUM_SEED", 100);
    NUM_LEECH = cfg.GetValueOfKey<int>("NUM_LEECH", 100);

    NUM_PIECE = cfg.GetValueOfKey<int>("NUM_PIECE", 256);

    NUM_PEERLIST = cfg.GetValueOfKey<int>("NUM_PEERLIST", 50);

    NUM_CHOKING = cfg.GetValueOfKey<int>("NUM_CHOKING", 4);
    NUM_OU = cfg.GetValueOfKey<int>("NUM_OU", 1);

    TYPE_PEERSELECT = cfg.GetValueOfKey<int>("TYPE_PEERSELECT",
                                             static_cast<int>(PeerSelect_T::STANDARD)
                                            );

    TYPE_PIECESELECT = cfg.GetValueOfKey<int>("TYPE_PIECESELECT",
                                              static_cast<int>(PieceSelect_T::RANDOM_FIRST_PIECE)
                                             );
}
