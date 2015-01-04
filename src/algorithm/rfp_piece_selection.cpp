#include <fstream>
#include "../args.h"
#include "rfp_piece_selection.h"

using namespace btte_uniformrand;

namespace btte_piece_selection
{

void RandomFirstPiece::RefreshInfo()
{
    candidates_.clear();
}

IntSet RandomFirstPiece::GetRandomPieceSet()
{
    // Randomly select a piece no from set
    IntSet target_pieces;

    auto SelectRandPiece = [] (const int num_target,
                               IntSetIter first_piece_no)
    {
        int rand_offset = Rand(RSC::RFP_PIECESELECT) % num_target;
        for (; rand_offset != 0; --rand_offset) ++first_piece_no;
        return *first_piece_no;
    };

    const size_t NUM_PEERLIST = g_btte_args.get_num_peerlist();

    for (size_t i = 0; i < NUM_PEERLIST; i++)
    {
        const size_t num_target = candidates_.size();
        if (num_target == 0) break;
        const int piece_no = SelectRandPiece(num_target,
                                             candidates_.begin());
        target_pieces.insert(piece_no);
        candidates_.erase(piece_no);
    }

    return target_pieces;
}

IntSet RandomFirstPiece::StartSelection(const int client_pid)
{
    selector_pid_ = client_pid;

    CheckNeighbors();

    SetCandidates();

    IntSet tmp_cands = candidates_; // for debug

    IntSet target_pieces = candidates_;

    //IntSet target_pieces = GetRandomPieceSet();

    DebugInfo(tmp_cands, target_pieces);

    RefreshInfo();

    return target_pieces;
}

void RandomFirstPiece::DebugInfo(IntSet const& tmp_cands, IntSet const& result) const
{
    std::string prefix = " ";
    switch (g_btte_args.get_type_pieceselect())
    {
        case 0:
            prefix = "rand_";
            break;
        case 1:
            prefix = "rarefst_";
            break;
        case 2:
            prefix = "user_";
            break;
        default:
            ExitError("wrong algo ID");
            break;
    }

    std::ofstream ofs;
    ofs.open(prefix + "piecesel_log.txt", std::fstream::app);

    // pieces haven't downloaded yet
    ofs << "執行者("<< selector_pid_ <<  ") 還未取得的 pieces :\n";
    for (const int no : tmp_cands) ofs << no << ' ';

    ofs << std::endl << std::endl;

    // piece ready to download
    ofs << "執行者("<< selector_pid_ <<  ") 準備下載的 pieces :\n";
    for (const int no : result) ofs << no << ' ';

    ofs << "\n----------------------\n\n\n";
    ofs.close();
}

}
