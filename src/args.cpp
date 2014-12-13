#include <cmath>

#include "utils/config.h"
#include "peer_level.h"
#include "cluster_info.h"
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

    CheckArgs();
}

void Args::CheckArgs()
{
    auto not_integer = [=] (const float num) {
        return num != std::floor(num);
    };

    bool exist_err = false;

    if (not_integer(NUM_PEER * lv1_ratio) ||
        not_integer(NUM_PEER * lv2_ratio) ||
        not_integer(NUM_PEER * lv3_ratio))
    {
        std::cout << "ValueError: NUM_PEER (value multiply by ratio of peer-level is not integer)";
        exist_err = true;
    }
    else if (NUM_PEER % g_kNumClusters != 0)
    {
        std::cout << "ValueError: NUM_PEER (value divided by number of clusters is not divisible)";
        exist_err = true;
    }
    else if (NUM_SEED + NUM_LEECH >= NUM_PEER)
    {
        std::cout << "ValueError: NUM_SEED or NUM_LEECH (sum of NUM_PEER and NUM_LEECH must smaller than NUM_PEER)";
        exist_err = true;
    }
    else if (NUM_PEERLIST > 50)
    {
        std::cout << "ValueError: NUM_PEERLIST (must smaller or equal 50)";
        exist_err = true;
    }
    else if (TYPE_PEERSELECT < (int)PeerSelect_T::STANDARD ||
             TYPE_PEERSELECT > (int)PeerSelect_T::CLUSTER_BASED ||
             TYPE_PIECESELECT < (int)PieceSelect_T::BUILTIN ||
             TYPE_PIECESELECT > (int)PieceSelect_T::USER_DEFINED_2)
    {
        std::cout << "ValueError: Type of Algorithm";
        exist_err = true;
    }

    if (exist_err) exit(1);
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
