#include "utils/config.h"
#include "algorithm/peer_selection.h"
#include "algorithm/piece_selection.h"
#include "args.h"


using namespace btte_peer_selection;
using namespace btte_piece_selection;


Args g_btte_args;


void Args::InitArgs(const std::string filename)
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

/* getter */
size_t Args::get_num_peer()      const { return NUM_PEER; }
size_t Args::get_num_seed()      const { return NUM_SEED; }
size_t Args::get_num_leech()     const { return NUM_LEECH; }
size_t Args::get_num_peerlist()  const { return NUM_PEERLIST; }
size_t Args::get_num_choking()   const { return NUM_CHOKING; }
size_t Args::get_num_ou()        const { return NUM_OU; };
size_t Args::get_num_piece()     const { return NUM_PIECE; }
int Args::get_type_peerselect()  const { return TYPE_PEERSELECT; }
int Args::get_type_pieceselect() const { return TYPE_PIECESELECT; }
float Args::get_arrival_rate()   const { return ARRIVAL_RATE; }
