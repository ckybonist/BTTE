#include "config.h"
#include "peer_selection.h"
#include "piece_selection.h"
#include "args.h"

using namespace btte_peer_selection;
using namespace btte_piece_selection;

Args::Args(const std::string filename)
{
    Config cfg(filename);

    NUM_PEER = cfg.GetValueOfKey<size_t>("NUM_PEER", 5000);
    NUM_SEED = cfg.GetValueOfKey<size_t>("NUM_SEED", 100);
    NUM_LEECH = cfg.GetValueOfKey<size_t>("NUM_LEECH", 100);

    NUM_PIECE = cfg.GetValueOfKey<size_t>("NUM_PIECE", 256);

    NUM_PEERLIST = cfg.GetValueOfKey<size_t>("NUM_PEERLIST", 50);

    NUM_CHOKING = cfg.GetValueOfKey<size_t>("NUM_CHOKING", 4);
    NUM_OU = cfg.GetValueOfKey<size_t>("NUM_OU", 1);

    TYPE_PEERSELECT = cfg.GetValueOfKey<size_t>("TYPE_PEERSELECT",
                                             static_cast<int>(PeerSelect_T::STANDARD));

    TYPE_PIECESELECT = cfg.GetValueOfKey<size_t>("TYPE_PIECESELECT",
                                              static_cast<int>(PieceSelect_T::BUILTIN));

    //ARRIVAL_RATE = cfg.GetValueOfKey<float>("ARRIVAL_RATE", 1);
}
