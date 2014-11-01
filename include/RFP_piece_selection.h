#ifndef _RFP_PIECE_SELECTION_H
#define _RFP_PIECE_SELECTION_H

#include "piece_selection.h"


namespace btte_piece_selection
{

class RandomFirstPiece : public IPieceSelect
{
public:
    RandomFirstPiece(Args args) : IPieceSelect(args) {};

private:
    PRMVec StartSelection(const int self_pid) override;
    PieceReqMsg CreateReqMsg(const int target_piece_no);  // for Random First Piece
    void RefreshInfo();
};

}

#endif // for #ifndef _RFP_PIECE_SELECTION_H
