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
    MsgQueue StartSelection(const int client_pid) override;

    void CountNumPeerOwnPiece(const size_t num_targets);
    void SortByPieceCounts(const int num_targets);
    bool IsDupDest(const IntSet& dest_peers, const int nid);
    //PieceMsg CreateReqMsg(const int target_piece_no, const bool is_last_piece);  // for Rarest First
    PieceMsg CreateReqMsg(const int target_piece_no, IntSet& dest_peers);  // for Rarest First
    void RefreshInfo();

    struct PeerOwnCounts
    {
        int piece_no;
        int counts;
    };
    std::map<int, IntSet> piece_owner_set_;
    PeerOwnCounts* counts_info_;
};

}

#endif // for #ifndef _RF_PIECE_SELECTION_H
