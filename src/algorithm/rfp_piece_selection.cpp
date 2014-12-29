#include "../args.h"
#include "rfp_piece_selection.h"

using namespace uniformrand;

namespace btte_piece_selection
{

void RandomFirstPiece::RefreshInfo()
{
    no_download_pieces_set_.clear();
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
        const size_t num_target = no_download_pieces_set_.size();
        if (num_target == 0) break;
        const int piece_no = SelectRandPiece(num_target,
                                             no_download_pieces_set_.begin());
        target_pieces.insert(piece_no);
        no_download_pieces_set_.erase(piece_no);
    }

    return target_pieces;
}

IntSet RandomFirstPiece::StartSelection(const int client_pid)
{
    selector_pid_ = client_pid;

    CheckNeighbors();

    CollectNoDownloadPieces();

    IntSet target_pieces = GetRandomPieceSet();

    RefreshInfo();

    return target_pieces;
}

}
