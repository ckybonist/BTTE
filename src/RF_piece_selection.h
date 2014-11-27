#ifndef _RF_PIECE_SELECTION_H
#define _RF_PIECE_SELECTION_H

#include <deque>
#include "piece_selection.h"


namespace btte_piece_selection
{

class RarestFirst : public IPieceSelect
{
public:
    RarestFirst() : IPieceSelect() {};
    ~RarestFirst();

private:
    IntSet StartSelection(const int client_pid) override;
    void CountNumPeerOwnPiece(const size_t num_target);
    void SortByPieceCounts(const size_t num_target);
    bool IsDupDest(const IntSet& dest_peers, const int nid);
    IntSet GetRarestPiecesSet(const size_t num_target) const;
    void RefreshInfo();

    struct PeerOwnCounts
    {
        int piece_no;
        int counts;
    };
    PeerOwnCounts* counts_info_;
};

}

#endif // for #ifndef _RF_PIECE_SELECTION_H
