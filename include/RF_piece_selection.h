#ifndef _RF_PIECE_SELECTION_H
#define _RF_PIECE_SELECTION_H

#include "piece_selection.h"


namespace btte_piece_selection
{

class RarestFirst : public IPieceSelect
{
public:
    RarestFirst(Args args) : IPieceSelect(args) {};
    ~RarestFirst();

private:
    struct PieceCounts
    {
        int piece_no;
        int counts;
    };

    void CountNumPeerOwnPiece(const int self_pid, const size_t num_targets);
    int GetNumRarest(const int num_targets);
    int GetTargetPieceNo(const int num_rarest);
    int SelectTargetPiece(const int self_pid) override;

    PieceCounts* piece_counts_info_;
};

}

#endif // for #ifndef _RF_PIECE_SELECTION_H
