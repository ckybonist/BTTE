#ifndef _RF_PIECE_SELECTION_H
#define _RF_PIECE_SELECTION_H

#include "piece_selection.h"


namespace btte_piece_selection
{

class RarestFirst : public IPieceSelection
{
  public:
    RarestFirst() : IPieceSelection() {};
    ~RarestFirst();

  private:
    IntSet StartSelection(const int client_pid) override;
    void CountNumPeerOwnPiece();
    void SortByPieceCounts();
    bool IsDupDest(const IntSet& dest_peers, const int nid);
    IntSet GetRarestPiecesSet() const;
    void RefreshInfo();

    typedef struct PieceOwnersCount
    {
        int piece_no;
        int count;
    } POC;

    POC* count_info_;

    size_t num_target_;
};

}

#endif // for #ifndef _RF_PIECE_SELECTION_H
