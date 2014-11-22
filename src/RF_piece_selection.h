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

    MsgQueue StartSelection(const int self_pid) override;

    void CountNumPeerOwnPiece(const size_t num_targets);
    void SortByPieceCounts(const int num_targets);
    bool IsDupReq(const IntSet& dest_peers, const int nid);
    PieceMsg CreateReqMsg(const int target_piece_no, const bool is_last_piece);  // for Rarest First
    void RefreshInfo();

    PieceCounts* piece_counts_info_;
};

}

#endif // for #ifndef _RF_PIECE_SELECTION_H
