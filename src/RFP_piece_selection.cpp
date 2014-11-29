#include "RFP_piece_selection.h"

using namespace uniformrand;

namespace btte_piece_selection
{

void RandomFirstPiece::RefreshInfo()
{
    no_download_pieces_set_.clear();
}

IntSet RandomFirstPiece::GetTargetPieces(const size_t num_target) const
{
    // Randomly select a piece no from set
    IntSet target_pieces;

    IntSetIter piece_no = no_download_pieces_set_.begin();
    int rand_offset = Rand(RSC::RFP_PIECESELECT) % num_target;
    for (; rand_offset != 0; --rand_offset) ++piece_no;

    target_pieces.insert(*piece_no);

    return target_pieces;
}

IntSet RandomFirstPiece::StartSelection(const int client_pid)
{
    selector_pid_ = client_pid;

    CheckNeighbors();

    CollectNoDownloadPieces();

    const size_t num_target = no_download_pieces_set_.size();

    IntSet target_pieces = GetTargetPieces(num_target);

    RefreshInfo();

    return target_pieces;
}

}
