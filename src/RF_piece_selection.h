#ifndef _RF_PIECE_SELECTION_H
#define _RF_PIECE_SELECTION_H

#include <deque>
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

    void CountNumPeerOwnPiece(const size_t num_target);
    void SortByPieceCounts(const int num_target);
    bool IsDupDest(const IntSet& dest_peers, const int nid);
    IntSet GetFinalTargetPieces(const int num_target) const;
    //void PushReqMsg(MsgQueue& req_msgs, IntSet& dest_peers, const int target_piece_no);  // for Rarest First
    MsgQueue CreateReqMsgQ(IntSet const& final_target_pieces);
                           //IntSet& dest_peers,
                           //const int target_piece_no);  // for Rarest First
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
