#include "RFP_piece_selection.h"

using namespace uniformrand;

namespace btte_piece_selection
{

int RandomFirstPiece::SelectTargetPiece(const int self_pid)
{
    CheckNeighbors(self_pid);

    SetTargetPieces(self_pid);

    const size_t num_targets = targets_set_.size();

    // Randomly select a piece no from set
    iSetIter target_piece_no = targets_set_.begin();
    int rand_offset = Rand(RSC::RFP_PIECESELECT) % num_targets;
    for(; rand_offset != 0; --rand_offset) ++target_piece_no;

    return *target_piece_no;
}

}
