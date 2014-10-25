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
    int SelectTargetPiece(const int self_pid) override;
};

}

#endif // for #ifndef _RFP_PIECE_SELECTION_H
